#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"

#define HMAX 200
#define ERROR_CODE 12



int main(int argc, char* argv[]) {

    FILE *fin;

    struct hashmap_t *hm = malloc(sizeof(struct hashmap_t));

    if (hm == NULL) {
        printf("Malloc failed - hashmap!");
        exit(12);
    }
    
    init_hm(hm, HMAX);

    for (int i = 0; i < argc; i++) {
        if (strncmp(argv[i], "-D", 2) == 0) {

        }

        if (strncmp(argv[i], "-I", 2) == 0) {

        }
    }


    free_hm(hm);

    return 0;
}