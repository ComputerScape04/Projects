#ifndef HASH_TABLE_H
#define HASH_TABLE_H

struct node {
	char* key;
	char* value;

	struct node* next;
};

struct hashMap {
	int numberOfElements, capacity;

	struct node** arr;
};

// Function Prototypes
void initialize_hashmap(struct hashMap* mp);

void setNode(struct node* node, char* key, char* val);

unsigned long hash(struct hashMap* mp, unsigned char* str);

void insert(char* key, char* value, struct hashMap* map);

void delete(char* key, struct hashMap* map);

char* search(struct hashMap* map, char* key);

#endif
