
/*********
 *
 * *** HTTP Server in C ***
 *
 *********/

#include <stdio.h>
#include <sys/socket.h> // creating socket endpoint
#include <sys/types.h> 
#include <netdb.h> 
#include <arpa/inet.h> // byte order conversion (HBO -> NBO) 
#include <unistd.h> // sleep()
#include <regex.h>
#include <string.h>

#define PORT 8080
#define BACKLOG 1

int main(void) {

    /* REGEX Search Pattern */
    regex_t regex;
    int reti;
    char msgbuf[100];

    reti = regcomp(&regex, "GET /([^ ]*) HTTP/1.1", REG_EXTENDED);
    if (reti) {
        perror("Could not compile regex\n");
        return 1;
    }

    // Create a TCP socket connection
    // AF_INET -> IPv4 ; SOCK_STREAM -> TCP Protocol
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    int yes = 1;

    // allows the address to be reused (rids off the "addr already is use" error
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

    if (sockfd < 0) {
        perror("Could not create socket!\n");
        return 1;
    }

    /**
        struct sockaddr_in {
               sa_family_t     sin_family;      AF_INET 
               in_port_t       sin_port;       Port number
               struct in_addr  sin_addr;       IPv4 address
           };
     **/

    /**
           struct in_addr {
               in_addr_t s_addr;
           };
     **/

    struct sockaddr_in sockaddr_;

    sockaddr_.sin_family = AF_INET; // ipv4
    sockaddr_.sin_port = htons(PORT); // port we listen on
    sockaddr_.sin_addr.s_addr = htonl(INADDR_ANY); // from hbo to nbo
    
    /*
     *  INADDR_ANY: allows requests on every local interface
     **/

    /*
     * NOTE to self: We do htons() to convert unsigned short int from host-byte-order to network-byte-order
     **/

    // assigning an interface to the socket
    if (bind(sockfd, (struct sockaddr*) &sockaddr_, sizeof(sockaddr_)) < 0) {
        perror("Failed to bind an interface to the socket...");
        return 1;
    }

    // fetch requests
    while(1) {
        printf("\nServer listening on PORT %d\nWaiting for a request...\n", PORT); 
        
        // client socket address
        struct sockaddr_storage their_addr;
        socklen_t their_addr_size;

        // listen for requests?
        if (listen(sockfd, BACKLOG) < 0) {
            perror("Listen error...\n");
            return 1;
        }

        their_addr_size = sizeof(their_addr);

        // accept incoming requests
        int new_sockfd = accept(sockfd, (struct sockaddr*) &their_addr, &their_addr_size);

        if (new_sockfd < 0) {
            perror("Failed to accept connection...\n");
            return 1;
        }

        // cast client address to struct sockaddr_in*
        // then access the s_addr field and cast it to an unsigned char*
        struct sockaddr_in *sin = (struct sockaddr_in*)&their_addr;
        unsigned char* ip = (unsigned char*)&sin->sin_addr.s_addr;

        printf("\nSuccessfully created a connection with %d %d %d %d\n", ip[0], ip[1], ip[2], ip[3]); 

        // handle the http request (GET)
        // 1. receive the request from the client (browser)
        char request[1024];
        recv(new_sockfd, request, 1024, 0);
    
        printf("Request: %s\n", request);

        // regex-based searching
        reti = regexec(&regex, request, 0, NULL, 0);
        if (!reti) {
            // identify the request
            if (strncmp(request, "Request: GET /", 14)) {            
                // serve the request
                char* statusline = "HTTP/1.1 200 OK\r\n";
                char* headers = "Content-Type: text/html\r\n\r\n";

                send(new_sockfd, statusline, strlen(statusline), 0);
                send(new_sockfd, headers, strlen(headers), 0);

                // open a file (index.html)
                FILE *fp;
                fp = fopen("index.html", "r");
                
                char buf[1024];

                while(fgets(buf, 1024, fp)) {
                    
                }

                send(new_sockfd, response, strlen(response), 0);
                close(new_sockfd);
            } 

        }
        else if (reti == REG_NOMATCH) {
            printf("Invalid Request\n");
        }
        else {
            regerror(reti, &regex, msgbuf, sizeof(msgbuf));
            printf("RegError: %s\n", msgbuf);
        }

        sleep(2);
    
    }

    // free regex
    regfree(&regex);

    return 0;
}
