# LinkShortener - Technical Assessment

## Objective

Create a URL shortening service that provides a simple REST API for converting long URLs into short, manageable links. This assessment will evaluate your ability to work with C programming, web frameworks, and file I/O operations.

## Project Requirements

You are required to implement a URL shortener application with the following specifications:

### API Endpoints

#### 1. POST `/shorten`
- **Purpose**: Accept a long URL and return a shortened version
- **Request**: JSON payload containing the original URL
- **Response**: JSON containing the shortened URL and short ID

**Example Request:**
```json
{
  "url": "https://www.example.com/some/very/long/url"
}
```

**Expected Response:**
```json
{
  "shortUrl": "http://localhost:8080/abc123",
  "shortId": "abc123"
}
```

#### 2. GET `/:shortId`
- **Purpose**: Redirect users to the original long URL
- **Parameters**: `shortId` - the unique identifier for the shortened URL
- **Response**: HTTP redirect (301 or 302) to the original URL

### Technical Requirements

#### Mandatory Technologies

- **[CWebStudio](https://github.com/OUIsolutions/CWebStudio)**: Web framework for building the REST API in C
- **[doTheWorld](https://github.com/OUIsolutions/DoTheWorld)**: Library for handling I/O operations (file creation, directory management, etc.)

#### Storage Implementation

You must implement a **file-based storage system** with the following structure:

```
./database/
├── 52654
├── 23232
├── 65454
└── [shortId]
```

**Requirements:**
- Each shortened URL must be stored in a separate file
- The filename must be the `shortId`
- File content must contain only the original long URL as plain text

**Example:**
- **File**: `./database/abc123`
- **Content**: `https://www.example.com/some/very/long/url`

## Assessment Criteria

Your solution will be evaluated based on:

1. **Functionality**: Both endpoints work correctly
2. **Code Quality**: Clean, readable, and well-structured C code
3. **Error Handling**: Proper handling of edge cases and invalid inputs
4. **File Management**: Correct implementation of the specified storage structure
5. **API Design**: Proper HTTP status codes and response formats
6. **Documentation**: Clear code comments and basic usage instructions

## Deliverables

1. Complete source code implementation
2. Build instructions (Makefile or compilation commands)
3. Basic documentation on how to run and test the application
4. Example usage demonstrating both endpoints

## Getting Started

1. Set up your development environment with the required libraries
2. Initialize the project structure
3. Implement the API endpoints according to specifications
4. Test your implementation thoroughly
5. Document your solution

## Notes

- Focus on implementing a working solution first, then optimize
- Ensure your code handles concurrent requests appropriately
- Consider URL validation and error scenarios
- The short ID generation algorithm is up to your discretion

Good luck with your implementation!
