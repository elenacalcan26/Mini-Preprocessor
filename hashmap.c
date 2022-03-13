#include "hashmap.h"

unsigned int hash(unsigned char *str) {
    /* https://stackoverflow.com/questions/7666509/hash-function-for-string */
    unsigned int hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

void init_hm(struct hashmap_t *hm, int hmax) {
    hm->hmax = hmax;
    hm->size = 0;
    hm->buckets = (struct pair**)malloc(hmax * sizeof(struct pair*));

    if (hm->buckets == NULL) {
        free(hm);
        printf("Malloc feiled - hashmap buckets");
        exit(12);
    }

    for (int i = 0; i < hm->hmax; i++) {
        hm->buckets[i] = NULL;
    }


}

void put(struct hashmap_t *hm, char* key, char* value) {
    int index = hash(key) % hm->hmax;
    struct pair *new_pair = (struct pair*)malloc(sizeof(struct pair));

    if (new_pair == NULL) {
        printf("Malloc failed - new pair");
        exit(12);
    }

    new_pair->key = calloc(strlen(key) + 1, sizeof(char));
    
    if (new_pair->key == NULL) {
        printf("Malloc failed - key");
        exit(12);
    }

    new_pair->value = calloc(strlen(value) + 1, sizeof(char));

    if (new_pair->value == NULL) {
        printf("Malloc failed - value");
        exit(12);
    }

    strcpy(new_pair->key, key);
    strcpy(new_pair->value, value);

    if (hm->buckets[index] == NULL) {
        hm->buckets[index] = new_pair;
    } else {

        // verific daca au aceeasi cheie
        if (strcmp(key, hm->buckets[index]->key) == 0) {
            // se inlocuieste vechea valoare cu cea noua

            strcpy(hm->buckets[index]->value, value); 
        } else {

            int inserted = 0;

            // se cauta primul bucket gol in care se insereaza perechea

            for (int i = index; i < hm->hmax; i++) {
                if (hm->buckets[i] == NULL) {
                    hm->buckets[i] = new_pair;
                    inserted = 1;
                    break;
                }
            }

            if (inserted == 0) {
                for (int i = 0; i < index; i++) {
                    if (hm->buckets[i] == NULL) {
                        hm->buckets[i] = new_pair;
                        inserted = 1;
                        break;
                    }
                }
            }

            if (inserted == 0 && hm->size == hm->hmax) {
                // dublez capacitatea bucket-urilor
                hm->buckets = (struct pair**)realloc(hm->buckets, hm->hmax * 2);

                if (hm->buckets == NULL) {
                    printf("Realloc failed - buckets");
                    exit(12);
                }
                hm->hmax *= 2;

                for (int i = hm->size; i < hm->hmax; i++) {
                    if (hm->buckets[i] == NULL) {
                        hm->buckets[i] = new_pair;
                        inserted = 1;
                        break;
                    }
                }
            }

        }

    }

    hm->size++;
}

int has_key(struct hashmap_t *hm, char* key) {

    int index = hash(key) % hm->hmax;

    for (int i = index; i < hm->hmax; i++) {
        if (hm->buckets[i] != NULL && strcmp(key, hm->buckets[i]->key) == 0) {
            return 1;
        }
    }

    for (int i = 0; i < index; i++) {
        if (hm->buckets[i] != NULL && strcmp(key, hm->buckets[i]->key) == 0) {
            return 1;
        }
    }

    return 0;
}

char* get(struct hashmap_t *hm, char* key) {

    int index = hash(key) % hm->hmax;

    for (int i = index; i < hm->hmax; i++) {
        if (hm->buckets[i] != NULL && strcmp(key, hm->buckets[i]->key) == 0) {
            return hm->buckets[i]->value;
        }
    }

    for (int i = 0; i < index; i++) {
        if (hm->buckets[i] != NULL && strcmp(key, hm->buckets[i]->key) == 0) {
            return hm->buckets[i]->value;
        }
    }

    return NULL;
}

void remove_pair(struct hashmap_t *hm, char *key) {
    int index = hash(key) % hm->hmax;

    for (int i = index; i < hm->hmax; i++) {
        if (hm->buckets[i] != NULL && strcmp(key, hm->buckets[i]->key) == 0) {
            free(hm->buckets[i]->key);
            free(hm->buckets[i]->value);
            free(hm->buckets[i]);
            hm->buckets[i] = NULL;
            return;
        }
    }

    for (int i = 0; i < index; i++) {
        if (hm->buckets[i] != NULL && strcmp(key, hm->buckets[i]->key) == 0) {
            free(hm->buckets[i]->key);
            free(hm->buckets[i]->value);
            free(hm->buckets[i]);
            hm->buckets[i] = NULL;
            return;
        }
    }
}

void free_hm(struct hashmap_t *hm) {
    for (int i = 0; i < hm->hmax; i++) {
        if (hm->buckets[i] != NULL) {
            free(hm->buckets[i]->key);
            free(hm->buckets[i]->value);
            free(hm->buckets[i]);
        }
    }

    free(hm->buckets);
    free(hm);
}