# LinkShortener

## Project Task:
create a url shortener that will have these two routes:

1. POST /shorten
   - Accepts a JSON payload with a long URL.
   - Returns a shortened URL.

2. GET /:shortId
   - Redirects to the original long URL based on the provided shortId.

## Technologies Used:

- [CWebStudio](https://github.com/OUIsolutions/CWebStudio) 
  our web  framework for building web applications in C.

- [doTheWorld](https://github.com/OUIsolutions/DoTheWorld)
our lib to  handle IO operations, such as write files,create directories and list files


## Folders style:
you must save each short url into  a diferent file and the name of that file will be the short id

example:
```txt 
./database/52654/
./database/23232/
./database/65454/
```
where , the internal content of that file, must be the original long url
example:
```txt
https://www.example.com/some/very/long/url
```
