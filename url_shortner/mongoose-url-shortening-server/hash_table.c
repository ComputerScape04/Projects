/***** Sources:
 * https://www.geeksforgeeks.org/dsa/implementation-of-hash-table-in-c-using-separate-chaining/
 * https://www.geeksforgeeks.org/dsa/separate-chaining-collision-handling-technique-in-hashing/
 * https://www.geeksforgeeks.org/dsa/implementation-of-hash-table-in-c-using-separate-chaining/
 ***************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// linked list node
struct node {
	char* key;
	char* value;

	struct node* next;
};

// hash map
struct hashMap {
	int numberOfElements, capacity;

	// base address of the linked list node
	struct node** arr;
};

// init the hash map
void initialize_hashmap(struct hashMap* mp) {
	// init the number of elements ; init the cap
	mp->numberOfElements = 0;
	mp->capacity = 64;

	// allocate the memory of each linked list in the array
	mp->arr = (struct node**)calloc(mp->capacity, sizeof(struct node*));

	if (mp->arr == NULL) {
		printf("Failed to allocate memory!");
	}

	printf("Successfully initialized the hashmap!\n");
}

// set the value of the node
void setNode(struct node* node, char* key, char* val) {
	node->key = key;
	node->value = val;
	node->next = NULL;

	return;
}

// hash function for the key
unsigned long hash(struct hashMap* mp, unsigned char* str) {
	// polynomial rolling hash technique
	unsigned long bucketIndex;
	unsigned long sum = 0, factor = 31;

	for (int i = 0; str[i] != '\0'; i++) {
		sum = sum * 31 + (int)str[i];
	}

	bucketIndex = sum % mp->capacity;
	return bucketIndex;
}

// insert key-value function
void insert(char* key, char* value, struct hashMap* map) {
	// get the bucket index
	unsigned long bucketIndex = hash(map, key);
	struct node* newNode = (struct node*)malloc(sizeof(struct node));

	setNode(newNode, key, value);

	// insert into the map
	if (map->arr[bucketIndex] == NULL) {
		map->arr[bucketIndex] = newNode;
	}
	else {
		// collision occured
		// perform chaining

		newNode->next = map->arr[bucketIndex];
		map->arr[bucketIndex] = newNode;
	}

}


// delete key-value function
void delete(char* key, struct hashMap* map) {
	// get bucket index for key
	unsigned long bucketIndex = hash(map, key);

	struct node* prevNode = NULL;

	struct node* currNode = map->arr[bucketIndex];

	while(currNode != NULL) {
		if (strcmp(key, currNode->key) == 0) {
			// head node deletion
			if (currNode == map->arr[bucketIndex]) {
				map->arr[bucketIndex] = currNode->next;
			}

			// last node or middle node deletion
			else {
				prevNode->next = currNode->next;
			}
			free(currNode);
			break;
		}

		prevNode = currNode;
		currNode = currNode->next;
	}

	return;
}

// search function
char* search(struct hashMap* map, char* key) {
	unsigned long bucketIndex = hash(map, key);
	//printf("Bucket Index: %d\n", bucketIndex);

	struct node* bucketHead = map->arr[bucketIndex];
 // printf("%p\n", bucketHead);	
	while(bucketHead != NULL) {
		if (strcmp(bucketHead->key, key) == 0) {
			return bucketHead->value;
		}
		bucketHead = bucketHead->next;
	}

	char* errorMsg = (char*)malloc(sizeof(char) * 25);
	errorMsg = "No data found!\n";

	return errorMsg;
}
