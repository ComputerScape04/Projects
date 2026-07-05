/****
 *
 * Mongoose - Embedded web server for C: https://github.com/cesanta/mongoose
 * Documentation - https://mongoose.ws/docs/getting-started/introduction/
 * SQLite Database - https://sqlite.org/download.html 
 * Documentation - https://sqlite.org/c3ref/funclist.html
 * Making a server - https://devlogs.xyz/blog/building-http-servers-with-mongoose
 * https://raspapi.hackclub.com/guides/building/c-mongoose
 * https://mongoose.ws/documentation/
 *
 *****/

#include "mongoose.h"
#include "urlshortner.h"
#include <sqlite3.h>
#include <errno.h>

// globally defined DB
sqlite3* DB;

static int print_callback(void* data, int argc, char** argv, char** azColName) {
  int i;
  fprintf(stderr, "%s: ", (const char*)data);

  for (int i = 0; i < argc; i++) {
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }

  printf("\n");
  return 0;
}

// event handler for any activity on localhost:8000
static void ev_handler(struct mg_connection *c, int ev, void* ev_data) {
  // HTTP Event occurs
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    printf("HTTP Request: %.*s %.*s\n", (int)hm->method.len, hm->method.buf, (int)hm->uri.len, hm->uri.buf);

    /* GET / ---> Health Check */
    if (mg_match(hm->uri, mg_str("/"), NULL)) {
      mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{\"status\" : \"alive\"}\n");
      return;
    }

    /* /api/hello ---> says hello   */
    else if (mg_match(hm->uri, mg_str("/api/hello"), NULL)) {
      mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{\"message\" : \"Hello!\"}\n");
      return;
    }

    /* POST /api/short_url ---> generate a shortened URL */
    else if (mg_match(hm->uri, mg_str("/api/short_url"), NULL) && mg_match(hm->method, mg_str("POST"), NULL)) {
      // send the main URL in the post request
      char* url = mg_json_get_str(mg_str(hm->body.buf), "$.url");

      // get the shortened url
      char* base64e = hash_func(url);
      char* shortened_url = shortenedUrl(url, base64e);

      // store the {shortened url : url} pair in DB
      const char* insert_stmt = "INSERT INTO hash_table (SHORT_URL, URL) VALUES (?, ?);";
      sqlite3_stmt *stmt;

      // prepare the statement
      if (sqlite3_prepare_v2(DB, insert_stmt, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Failed to prepare a SQL statement: %s\n", sqlite3_errmsg(DB));
        mg_free(url);
        return;
      }

      // bind the parameters
      if (sqlite3_bind_text(stmt, 1, shortened_url, -1, SQLITE_TRANSIENT) != SQLITE_OK 
          || sqlite3_bind_text(stmt, 2, url, -1, SQLITE_TRANSIENT) != SQLITE_OK) {

        sqlite3_finalize(stmt);
        printf("Failed to bind the parameters to the SQL statement: %s\n", sqlite3_errmsg(DB));
        mg_free(url);
        return;
      }

      // execute the sql statement
      if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        printf("Failed to execute the SQL Statement: %s\n", sqlite3_errmsg(DB));
        mg_free(url);
        return;
      }

      printf("Successfully inserted the URL: %s\n", shortened_url);
      sqlite3_finalize(stmt);

      // clear the mem allocation on the json string
      mg_free(url);

      mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{\"status\" : \"OK\"}");
      return;
    }

    /* GET /api/get_urls --> get all the shortened URLs */
    else if (mg_match(hm->uri, mg_str("/api/get_urls"), NULL)) {
      const char* sql = "SELECT SHORT_URL, URL FROM hash_table;";
      int exit = 0;

      char* errmsg;
      exit = sqlite3_exec(DB, sql, print_callback, 0, &errmsg);

      if (exit != SQLITE_OK) {
        perror("Failed to retrieve the URLs\n");
        sqlite3_free(errmsg); // Fix SQLite free function
      }

      mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{\"message\" : \"Retrieved rows\"}");
      return;
    }

    /* GET /{short_code}  --> use the shortened URL and get redirected to the appropriate destination  */
    else if (mg_match(hm->method, mg_str("GET"), NULL)) {
      char short_code[64];

      snprintf(
        short_code,
        sizeof(short_code),
        "%.*s",
        (int)(hm->uri.len - 1),
        hm->uri.buf + 1
      );

      // perform lookup
      const char* sql = "SELECT URL FROM hash_table WHERE SHORT_URL = ?;";
      sqlite3_stmt* stmt;

      // prepare the statement
      if (sqlite3_prepare_v2(DB, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Failed to prepare the statement: %s\n", sqlite3_errmsg(DB));
        return;
      }

      // bind the parameter
      if (sqlite3_bind_text(stmt, 1, short_code, -1, SQLITE_STATIC) != SQLITE_OK) {
        printf("Failed to bind the parameter: %s\n", sqlite3_errmsg(DB));
        sqlite3_finalize(stmt); // Fix resource leak
        return;
      }

      // execute the statement
      if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* originalUrl = (const char*)sqlite3_column_text(stmt, 0);
        
        printf("Original URL: %s\n", originalUrl);
        mg_printf(c,
          "HTTP/1.1 302 Found\r\n"
          "Location: %s\r\n"
          "Content-Length: 0\r\n"
          "\r\n",
          originalUrl
        );
        sqlite3_finalize(stmt); // Fix resource leak
        return;
      }
      
      sqlite3_finalize(stmt); // Fix resource leak
      mg_http_reply(c, 404, "", "%s", "URL Not found");
      return;
    }
  }
}

/*****
 *
 * Creating a table { ID, SHORT_URL, URL } 
 *
 *****/

void initDB(sqlite3* DB) {
  const char* sql = 
    "CREATE TABLE IF NOT EXISTS hash_table ("
        "ID         INTEGER  PRIMARY KEY AUTOINCREMENT NOT NULL, "
        "SHORT_URL  TEXT             NOT NULL, "
        "URL        TEXT             NOT NULL);";

  int exit = 0;
  char* messageError;
  exit = sqlite3_exec(DB, sql, NULL, 0, &messageError);

  if (exit != SQLITE_OK) {
    perror("Error creating table\n");
    sqlite3_free(messageError);
  }
  else {
    printf("Successfully created table!\n");
  }
}

int main() {
  // Disable buffering for standard output and error so logs appear immediately in Docker
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

  // event manager
  struct mg_mgr mgr; 

  // init the event manager
  mg_mgr_init(&mgr);

  /******** set up the sqlite database ********/
 
  int exit = 0;
  exit = sqlite3_open("example.db", &DB);

  if (exit) {
    perror("Error creating the database.");
    return -1;
  }
  else {
    printf("Successfully opened the database\n");
  }

  initDB(DB);

  /*******************************************/

  // listen on 0.0.0.0:8000 for Docker/external accessibility
  mg_http_listen(&mgr, "http://0.0.0.0:8000", ev_handler, NULL);

  // continuously poll for any activity
  while(1) {
    mg_mgr_poll(&mgr, 1000);
  }

  // close the DB
  sqlite3_close(DB);
  return 0;

}

