#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>
#include "CWebStudioOne.c"
#include "doTheWorldOne.c"

#define SERVER_PORT 8080
#define DATABASE_DIR "./database"
#define SHORT_ID_LEN 6 // IDs must be exactly 6 characters
#define BASE_URL "http://localhost:8080"

const char *ALPHABET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

/**
 * @brief Generates a random alphanumeric short ID of length SHORT_ID_LEN.
 * @param out Buffer to store the generated ID (must be size SHORT_ID_LEN + 1).
 */
void generate_short_id(char *out)
{
    for (int i = 0; i < SHORT_ID_LEN; i++)
    {
        int r = rand() % 62;
        out[i] = ALPHABET[r];
    }

    out[SHORT_ID_LEN] = '\0';
}

/**
 * @brief Ensures the database directory exists, creating it if necessary.
 * @return 0 on success, -1 on failure.
 */
static int ensure_database_dir(void) 
{
    struct stat st;

    if (stat(DATABASE_DIR, &st) == 0) 
    {
        if (S_ISDIR(st.st_mode)) return 0;
        return -1; 
    }
    // Attempt to create the directory
    if (mkdir(DATABASE_DIR, 0755) != 0) 
    {
        return -1;
    }
    return 0;
}

/**
 * @brief Simple check to ensure the URL starts with http:// or https://.
 * @param url The string to check.
 * @return 1 if valid, 0 otherwise.
 */
static int is_valid_url(const char *url) 
{
    if (!url) return 0;

    if (strncmp(url, "http://", 7) == 0) return 1;
    if (strncmp(url, "https://", 8) == 0) return 1;

    return 0;
}

/**
 * @brief Stores the mapping from shortId to longUrl in a file.
 * @param shortId The unique ID (filename).
 * @param longUrl The target URL (file content).
 * @return 0 on success, -1 on failure.
 */
static int store_mapping(const char *shortId, const char *longUrl) 
{
    char path[4096];
    snprintf(path, sizeof(path), "%s/%s", DATABASE_DIR, shortId);

    // Use doTheWorld file writing if available
#ifdef DOWORLD_HAS_WRITE_FILE
    if (doTheWorld_write_file) 
        if (doTheWorld_write_file(path, longUrl) == 0) return 0;
#endif

    // Fallback to standard C file writing
    FILE *f = fopen(path, "w");
    if (!f) 
        return -1;

    size_t len = strlen(longUrl);

    if (fwrite(longUrl, 1, len, f) != len) 
    {
        fclose(f);
        return -1;
    }

    fclose(f);

    return 0;
}

/**
 * @brief Helper function to send an HTTP redirect response.
 * @param location The URL to redirect to.
 * @param status The HTTP status code (e.g., 302 Found).
 * @return The CwebHttpResponse object.
 */
CwebHttpResponse *cweb_send_redirect(const char *location, int status) 
{
    CwebHttpResponse *response = cweb_send_text("", status);
    CwebHttpResponse_add_header(response, "Location", location);
    return response;
}


/**
 * @brief Loads the long URL associated with the short ID from the database directory.
 * @param shortId The short ID (filename).
 * @return A dynamically allocated string containing the long URL, or NULL if not found/error.
 */
static char *load_mapping(const char *shortId) 
{
    char path[4096];
    snprintf(path, sizeof(path), "%s/%s", DATABASE_DIR, shortId);

    // Use doTheWorld file loading if available
#ifdef DOWORLD_HAS_LOAD_FILE
    if (doTheWorld_load_file) 
    {
        char *content = doTheWorld_load_file(path);
        if (content) return content; 
    }
#endif

    // Fallback to standard C file reading
    FILE *f = fopen(path, "r");
    if (!f) 
        return NULL;

    // Determine file size
    if (fseek(f, 0, SEEK_END) != 0) 
    {
        fclose(f);
        return NULL;
    }

    long sz = ftell(f);
    rewind(f);

    if (sz <= 0) // Treat 0-sized files as errors/empty
    {
        fclose(f);
        return NULL;
    }

    // Allocate buffer and read content
    char *buf = malloc((size_t)sz + 1);
    if (!buf) 
    {
        fclose(f);
        return NULL;
    }

    size_t read = fread(buf, 1, (size_t)sz, f);
    fclose(f);

    buf[read] = '\0';

    return buf;
}

/* Handler for POST /shorten */
CwebHttpResponse *shorten_handler(CwebHttpRequest *request) 
{
    if (ensure_database_dir() != 0) 
    {
        return cweb_send_text("Internal server error: cannot create database directory", 500);
    }

    int one_mega_byte = 1048576;
    unsigned char *request_body = CwebHttpRequest_read_content(request, one_mega_byte);

    if (!request_body || strlen((char *)request_body) == 0)
    {
        if (request_body) free(request_body);
        return cweb_send_text("Bad Request: empty body", 400);
    }

    // --- Start of robust JSON parsing using cJSON ---
    cJSON *json = cJSON_Parse((char *)request_body);
    free(request_body); // Free the raw request body immediately after parsing it

    if (!json)
    {
        return cweb_send_text("Bad Request: Invalid JSON format", 400);
    }

    cJSON *url_item = cJSON_GetObjectItemCaseSensitive(json, "url");

    if (!url_item || !cJSON_IsString(url_item))
    {
        cJSON_Delete(json);
        return cweb_send_text("Bad Request: JSON must contain a string value for \"url\"", 400);
    }
    
    const char *raw_url = url_item->valuestring;
    size_t url_len = strlen(raw_url);
    
    if (url_len == 0) 
    {
        cJSON_Delete(json);
        return cweb_send_text("Bad Request: url is empty", 400);
    }
    else if (url_len > 4095) 
    {
        cJSON_Delete(json);
        return cweb_send_text("Bad Request: url too long (max 4095 chars)", 400);
    }

    char longUrl[4096];
    // Copy URL safely, ensuring null termination
    strncpy(longUrl, raw_url, sizeof(longUrl) - 1);
    longUrl[sizeof(longUrl) - 1] = '\0'; 

    cJSON_Delete(json); // Clean up the parsed JSON object
    // --- End of robust JSON parsing using cJSON ---

    if (!is_valid_url(longUrl)) 
    {
        return cweb_send_text("Bad Request: url must start with http:// or https://", 400);
    }

    char shortId[SHORT_ID_LEN + 1];
    int attempts = 0;
    int stored = -1;

    // Attempt to generate a unique short ID (max 8 attempts)
    while (attempts < 8 && stored != 0) 
    {
        generate_short_id(shortId);

        // Check if the file already exists
        char path[4096];
        snprintf(path, sizeof(path), "%s/%s", DATABASE_DIR, shortId);
        
        FILE *f = fopen(path, "r");
        if (f) { 
            fclose(f); 
            attempts++; 
            continue; // ID is already taken
        } 

        stored = store_mapping(shortId, longUrl);
        attempts++;
    }

    if (stored != 0) 
    {
        return cweb_send_text("Internal Server Error: could not store mapping after 8 attempts", 500);
    }

    // Build JSON response 
    char shortUrl[4096];
    snprintf(shortUrl, sizeof(shortUrl), "%s/%s", BASE_URL, shortId);

    // Minimal JSON output using cJSON
    cJSON *response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "shortUrl", shortUrl);
    cJSON_AddStringToObject(response, "shortId", shortId);

    return cweb_send_cJSON(response, 201); /* 201 Created */
}


/**
 * @brief Handler for GET /:shortId, performs the redirect.
 * @param full_path The full requested path (e.g., "/Ax7yR2").
 * @return CwebHttpResponse for redirect (302) or 404 Not Found.
 */
CwebHttpResponse *redirect_handler(const char *full_path) 
{
    if (!full_path || strlen(full_path) <= 1) 
        return cweb_send_text("Not Found", 404);  

    // Skip the leading slash
    const char *path_segment = full_path + 1;
    size_t len = strlen(path_segment);

    // *** CRITICAL FIX: Enforce the generated ID length (SHORT_ID_LEN) ***
    // This rejects multi-segment paths (like the user's "/redirect/shortid")
    if (len != SHORT_ID_LEN) 
    {
        return cweb_send_text("Not Found", 404);
    }
    
    // Create a mutable copy of the ID (guaranteed to be 6 chars + NULL)
    char shortId[SHORT_ID_LEN + 1];
    strncpy(shortId, path_segment, SHORT_ID_LEN);
    shortId[SHORT_ID_LEN] = '\0';

    // Sanitize: Check if all characters are valid base62
    for (size_t i = 0; i < SHORT_ID_LEN; ++i) 
    {
        char ch = shortId[i];

        if (!( (ch >= '0' && ch <= '9')    || 
               (ch >= 'a' && ch <= 'z')    || 
               (ch >= 'A' && ch <= 'Z')    )) 
        {
            // Invalid character found
            return cweb_send_text("Not Found", 404);
        }
    }

    char *longUrl = load_mapping(shortId);

    if (!longUrl) 
        return cweb_send_text("Not Found", 404);

    /* redirect (302 Found) */
    CwebHttpResponse *resp = cweb_send_redirect(longUrl, 302);

    free(longUrl);

    return resp;
}

/**
 * @brief The main request router function.
 */
CwebHttpResponse *main_server(CwebHttpRequest *request) 
{
    // Retrieve the full path. CwebHttpRequest_get_param returns an allocated string.
    char *full_path = request->url ? strdup(request->url) : NULL;
    CwebHttpResponse *response = NULL;

    if (!request || !request->method || !full_path)
    {
        if (full_path) 
            free(full_path);
    
        return cweb_send_text("Bad Request", 400);
    }

    if (strcmp(request->method, "POST") == 0 && 
        strcmp(full_path, "/shorten")   == 0) 
    {
        response = shorten_handler(request);
    }
    else if (strcmp(request->method, "GET") == 0) 
    {
        if (strcmp(full_path, "/") == 0) 
            response = cweb_send_text("LinkShortener - send POST /shorten {\"url\":\"...\"}", 200);
        else 
        {
            // Treat all other GET requests as short IDs
            response = redirect_handler(full_path);
        }
    }
    else 
    {
        response = cweb_send_text("Not Found", 404);
    }

    // *** FIX: Free the allocated path parameter to prevent a memory leak ***
    free(full_path); 
    
    return response;
}

int main(int argc, char *argv[]) 
{
    // Simplified randomization seed
    srand((unsigned)time(NULL));

    CwebServer server = newCwebSever(SERVER_PORT, main_server);
    printf("Starting LinkShortener on port %d, database = %s\n", SERVER_PORT, DATABASE_DIR);

    CwebServer_start(&server);

    return 0;
}
