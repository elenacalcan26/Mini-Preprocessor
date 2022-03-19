#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct pair {
	char *key;
	char *value;
};

struct hashmap_t {
	struct pair **buckets;
	int size /* numar total de noduri existente */;
	int hmax; /* numar de bucket-uri*/
};

unsigned int hash_function_string(char *str);

void init_hm(struct hashmap_t *hm, int hmax);

void put(struct hashmap_t *hm, char *key, char *value);

int has_key(struct hashmap_t *hm, char *key);

char *get(struct hashmap_t *hm, char *key);

void remove_pair(struct hashmap_t *hm, char *key);

void free_hm(struct hashmap_t *hm);

#endif /* HASHMAP_H */
