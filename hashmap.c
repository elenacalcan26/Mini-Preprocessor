#include "hashmap.h"

void init_hm(struct hashmap_t *hm, int hmax) {
    hm->hmax = hmax;
    hm->size = 0;
    hm->buckets = (struct pair**)malloc(hmax * sizeof(struct pair*));

    if (hm->buckets == NULL) {
        free(hm);
        printf("Malloc feiled - hashmap buckets");
        exit(-1);
    }
    
    for (int i = 0; i < hm->hmax; i++) {
        hm->buckets[i] = NULL;
    }


}

void free_hm(struct hashmap_t *hm) {
    for (int i = 0; i < hm->size; i++) {
        if (hm->buckets[i] != NULL) {
            free(hm->buckets[i]);
        }
    }

    free(hm->buckets);
    free(hm);
}