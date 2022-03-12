#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>
#include <stdlib.h>

struct pair {
    char *key;
    char *value;
};

struct hashmap_t
{
    struct pair **buckets;
    int size;
    int hmax;
};

void init_hm(struct hashmap_t *hm, int hmax);

void put(struct hashmap_t *hm, char* key, char *value);

void free_hm(struct hashmap_t *hm);


#endif /* HASHMAP_H */