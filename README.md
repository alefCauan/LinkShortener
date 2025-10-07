# LinkShortener - Technical Assessment

## Objective

LinkShortener is a URL shortening service implemented in C, providing a REST API to convert long URLs into short, manageable links. This project demonstrates proficiency in C programming, file I/O operations, and web framework usage, specifically with CWebStudio and doTheWorld libraries.

## Functionalities

- **POST /shorten**: Accepts a long URL via JSON and returns a shortened URL with a unique `shortId`.
- **GET /:shortId**: Redirects the user to the original URL associated with the provided `shortId`.

## Technical Requirements

- **Language**: C
- **Web Framework**: [CWebStudio](https://github.com/OUIsolutions/CWebStudio) for handling HTTP requests and responses
- **I/O Library**: [doTheWorld](https://github.com/OUIsolutions/DoTheWorld) for file and directory operations

## Storage Structure

Shortened URLs are stored in the `./database/` directory, where:

- **Filename**: A 6-character alphanumeric `shortId` (e.g., `abc123`)
- **File Content**: The original long URL in plain text

Example:
```
./database/
├── abc123
├── dEf456
└── GhI789
```

## Build and Execution

### Prerequisites
- GCC compiler
- CWebStudio and doTheWorld libraries (included as `CWebStudioOne.c` and `doTheWorldOne.c`)

### Build
Use the provided `Makefile` to compile:

```bash
make
```

This generates the `linkshortener` executable.
## Execution
Run the server:
```./linkshortener```

The server starts on http://localhost:8080.

# API Endpoints
### 1. POST /shorten

### Purpose: Converts a long URL into a shortened URL
- Request: JSON payload with the original URL
```bash
{
  "url": "https://www.example.com/some/very/long/url"
}
```
- Response (201 Created):
```bash
{   
  "shortUrl": "http://localhost:8080/abc123",
  "shortId": "abc123"
}
```
### Validations:
- URL must start with http:// or https://
- Maximum URL length: 4095 characters
- shortId is a 6-character alphanumeric string generated randomly
- Up to 8 attempts to generate a unique shortId to avoid collisions

### 2. GET /:shortId

- Purpose: Redirects to the original URL
- Parameters: shortId (e.g., abc123)
- Response: HTTP 302 redirect with Location header set to the original URL

### Example:

```bash
curl -v http://localhost:8080/abc123
```
- Response: Redirects to https://www.example.com/some/very/long/url

# Testing

Create a short URL:
```bash
curl -X POST http://localhost:8080/shorten \
  -H "Content-Type: application/json" \
  -d '{"url": "https://www.google.com"}'
```
Expected response:
```bash
{
  "shortUrl": "http://localhost:8080/cQaaE2",
  "shortId": "cQaaE2"
}
```

## Automated Test Script

Change the URL in `url_shortener_test.sh` or just

Run `url_shortener_test.sh`:
```bash
chmod +x url_shortener_test.sh
```
```bash 
./url_shortener_test.sh
```

# Getting Started

- Clone the repository or copy the source files.
- Ensure CWebStudioOne.c and doTheWorldOne.c are in the project directory.
- Run make to compile.
- Execute ./linkshortener to start the server.
- Test using curl or the provided test script.

# Author
Alef Cauan 
