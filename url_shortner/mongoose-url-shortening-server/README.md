# URL SHORTENER 
Built using **C**, **Mongoose (Networking Library used to create a HTTP server in C)**, **SQLite (for persistence)**

In order to run the service, run the following command in the terminal
```
gcc *.c -o http_server -lws2_32 -l sqlite3
```

Next, use the executable generated and run the following command,
```
.\http_server.exe
```

This will start the server.

Following are the routes available,
```
GET - /api/v1 -- Test route
POST - /api/short_url -- Generate the shortened URL and save it in DB
GET - /api/get_urls -- Get all the shortened
```

To use the Shortened URL, 
```
http://localhost:8000/{short_url} --> Run this in your browser and you should be redirected to the main destination
```
