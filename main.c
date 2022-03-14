#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"

#define HMAX 200
#define ERROR_CODE 12
#define MAX_LEN 256

void D_flag(struct hashmap_t *hm, char *arg) {
    // nu trebuie sa fac strtok dupa =
    char *key = strtok(arg, "=");
    char *value = strtok(NULL, "");

    put(hm, key, value);
}

void define_directive(struct hashmap_t *hm, FILE *fin, char *line) {
    char *key, *value;
    char *token = strtok(line, " ");
    key = strtok(NULL, " ");
    value = strtok(NULL, "\n");
    
    // TODO verific daca #define e pe mai multe linii

    put(hm, key, value); 

}

int data_preprocessing(struct hashmap_t *hm, FILE *fin, FILE *fout) {
    
    char line[MAX_LEN];
    int line_len = 0;
    char *token;
    char buffer[MAX_LEN];

    while (fgets(line, sizeof(line), fin) != NULL) {
        
        // verific daca pe linie se afla o directiva
        if (strstr(line, "#define ")) {
            define_directive(hm, fin, line);
        } else if (strstr(line, "#undef ")) {
            token = strtok(line, " ");
            token = strtok(NULL, "\n");
            remove_pair(hm, token);


        } else {
            // caz in care nu am directive
            token = strtok(line, "\n ");
            memset(buffer, 0, MAX_LEN);

            while (token != NULL) {
                
                if (token[0] != '"') {
                    char str[MAX_LEN];
                    int k = 0;
                    memset(str, 0, MAX_LEN);

                    for (int i = 0; i < strlen(token); i++) {
                        str[k] += token[i];
                        k++;
                        if (has_key(hm, str) == 1) {
                            strcpy(str, get(hm, str));
                            strcat(buffer, str);
                            memset(str, 0, MAX_LEN);
                            k = 0;
                        }  
                    }

                    strcat(buffer, str);
                }

                strcat(buffer, " ");
                token = strtok(NULL, " ");
            }
            strcat(buffer, "\0\n");

            fputs(buffer, fout);
        }

        
    }

    fputs("\n", fout);

    return 1;
}

int main(int argc, char* argv[]) {

    FILE *fin;
    FILE *fout;

    char input_file[MAX_LEN];
    char output_file[MAX_LEN];

    struct hashmap_t *hm = malloc(sizeof(struct hashmap_t));

    if (hm == NULL) {
        printf("Malloc failed - hashmap!");
        exit(12);
    }
    
    init_hm(hm, HMAX);

    memset(input_file, 0, MAX_LEN);
    memset(output_file, 0, MAX_LEN);

   // printf("%d\n", strlen(input_file));

    for (int i = 1; i < argc; i++) {
       // printf("%d = %s\n", i, argv[i]);
        if (strncmp(argv[i], "-D", 2) == 0) {

            if (strlen(argv[i]) > 2) {
                D_flag(hm, argv[i] + 2);
            } else {
                
                D_flag(hm, argv[i + 1]);
                i++;
            }

        } else if (strncmp(argv[i], "-I", 2) == 0) {

            if (strlen(argv[i]) > 2) {
               // D_flag(hm, argv[i] + 2);
            } else {
                //D_flag(hm, argv[i + 1]);
                i++;
            }

        } else if (strncmp(argv[i], "-o", 2) == 0) {

            if (strlen(argv[i]) > 2) {
               // D_flag(hm, argv[i] + 2);
            } else {
               // D_flag(hm, argv[i + 1]);
                i++;
            }
        } else {

            if (strlen(input_file) > 1) {
                return 1;
            } else {
                strcpy(input_file, argv[i]);
            }
       }
    }

    if (strlen(input_file) > 1) {
        fin = fopen(input_file, "r");
    } else {
        fin = stdin;
    }

    if (fin == NULL) {
        printf("Can't open file - input\n");
        free_hm(hm);
        return 1;
    }

    if (strlen(output_file) > 1) {
        fout = fopen(output_file, "w");
    } else {
        fout = stdout;
    }

    if (fout == NULL) {
        printf("Can't open file - ouput\n");
        free_hm(hm);
        return 1;
    }

    // printf("AAAAAAA\n");

    // for (int i = 0; i < hm->hmax; i++) {
    //     if (hm->buckets[i] != NULL) {
    //         printf("%s - %s\n", hm->buckets[i]->key, hm->buckets[i]->value);
    //     }
    // }

    int r = data_preprocessing(hm, fin, fout);

    if (r != 1) {
        return ERROR_CODE;
    }

    // for (int i = 0; i < hm->hmax; i++) {
    //     if (hm->buckets[i] != NULL) {
    //         printf("%s - %s\n", hm->buckets[i]->key, hm->buckets[i]->value);
    //     }
    // }

    if (fin != stdin) {
        fclose(fin);
    }

    if (fout != stdout) {
        fclose(fout);
    }

    free_hm(hm);

    return 0;
}