/**** GNU C MANUAL: https://www.gnu.org/software/gnu-c-manual/gnu-c-manual.html ****/
/**** Beej's Guide to C: https://beej.us/guide/bgc/html/split/index.html ****/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "hash_table.h"
#include "md5.h"
#include "base64.h"

/**** things to consider

	1. Base 36 Character Set (26 letters + 10 numbers [0-9]) 
	2. Base 62 Character Set (26 + 26 + 10)
	3. Hash Function that takes in a string and generates a hash value (key)
	4. Must allow the HTTP & HTTPS URI Scheme to be shortened

	Hash Function:
	- Generate an MD5 Hash (128 bits -- 16 bytes) 
	- Base64 encode this md5 hash -- 21 bytes)
	- Extract the first n chars to represent the shortened url path

 ****/

// get the nth occurance of a char in str
char* strnchr(char* str, char ch, int n) {
	char* p = str;
	for (int i = 0; i < n; i++) {
		p = strchr(p, ch);
		if (!p) return NULL;
		if (i < n-1) p++; // skips current match
	}
	return p;
}

// preprocess function 
/****    
 *
 * extract the first 6 characters of the base64 encode
 * clear the path from the URL
 * append the extracted text to the URL
 * 
 ******/

char* shortenedUrl(char* url, char* base64encoded) {
	char* extractedUrl = malloc(22);

	//if (!extractedUrl)
	//  return NULL;
	//strcpy(extractedUrl, "https://short.rl/");

	// extract first 6 characters from base64 encode
	char path[7];
	strncpy(path, base64encoded, 6); 

	strcpy(extractedUrl, path);

	return extractedUrl;
}

// Hash Function
char* hash_func(char* url) {
	// generate a MD5 Hash (128 bits)
	uint8_t* result = (uint8_t*)malloc(16);
	md5String(url, result);

	//for (int i = 0; i < 16; i++) {
	//  printf("%02x", result[i]);
	//}
	//printf("\n");

	// Base 64 encode the hash
	char* base64EncodedURL = base64_encode(result, 16);
	//printf("Base 64 Encoded URL: %s\n", base64EncodedURL);

	return base64EncodedURL;
}

/*
// Main Function
int main(int argc, char** argv) {

	// Global Hash Table
	struct hashMap map;

	// initialize the hashmap
	initialize_hashmap(&map);

	// EXAMPLE URL
	char* url = "https://www.github.com";
	
	// Pass the URL to a hash function
	char* hash = hash_func(url);

	// URL Format: protocol://domain:port/path
  char* shorturl = shortenedUrl(url, hash);

	printf("Shortened URL: %s\n", shorturl);

	// Store the key : url in a hash map
	insert(shorturl, url, &map);

	char tinyurl[100];

	printf("Enter a bo.ta link: ");
	scanf("%s", tinyurl);

  char* requrl = search(&map, tinyurl);

	printf("Invoking web search with the request URL: %s\n", requrl);

  ShellExecute(NULL, "open", requrl, NULL, NULL, SW_SHOWNORMAL);
	
	return 0;
}
*/
