#include <stdio.h>
#include <stdlib.h>
#include "hashmap.h"

#define HMAX 100

int main(int argc, char* argv[]) {

    struct hashmap_t *hm = malloc(sizeof(struct hashmap_t));

    if (hm == NULL) {
        printf("Malloc failed - hashmap!");
        exit(-1);
    }
    
    init_hm(hm, HMAX);

    free_hm(hm);

    return 0;
}