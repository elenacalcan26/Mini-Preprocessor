#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"

#define HMAX 200
#define ERROR_CODE 12
#define MAX_LEN 256

void D_flag(struct hashmap_t *hm, char *arg) {
    char *key, *value; 
    char *ch;

    ch = strchr(arg, '=');
    if (ch == NULL) {
        key = arg;
        value = "";
        
    } else {
        key = strtok(arg, "=");
        value = strtok(NULL, "");
    }

    put(hm, key, value);
}

int check_if_cond(struct hashmap_t *hm, char* cond) {

    if (strcmp(cond, "0") == 0) {
        return 0;
    }

    if (has_key(hm, cond) == 1 && strcmp(get(hm, cond), "0") == 0) {
        return 0;
    }

    return 1;
}

void define_directive(struct hashmap_t *hm, FILE *fin, char *line) {
    char *key, *value, *token;
    char tmp_line[MAX_LEN] = {0};
    char buff_value[MAX_LEN] = {0};
    char find_val[MAX_LEN] = {0};
    int n = 0;
    strcpy(tmp_line, line);
    token = strtok(tmp_line, " ");
    key = strtok(NULL, " ");

    memset(buff_value, 0, MAX_LEN);
    memset(find_val, 0, MAX_LEN);

    if (line[strlen(line) - 2] != '\\') {
        // nu este define pe mai multe linii
        value = strtok(NULL, "\n");

        for (int i = 0; i < strlen(value); i++) {
            find_val[n] = value[i];
            n++;
            find_val[n] = '\0';
        
            if (has_key(hm, find_val) == 1) {
                strcat(buff_value, get(hm, find_val));
                
                memset(find_val, 0, MAX_LEN);
                n = 0;
                continue;
            }

            if (value[i] == ' ' || i == strlen(value) - 1) {
                strcat(buff_value, find_val);
                
                memset(find_val, 0, MAX_LEN);
                n = 0;
                continue;
            }
        }

    } else {

        value = strtok(NULL, "\\\n");
        value[strlen(value) - 1] = '\0';
        strcpy(buff_value, value);

        while (fgets(line, sizeof(line), fin) != NULL) {
            memset(find_val, 0, MAX_LEN);
            n = 0;
            
            for (int i = 0; i < strlen(line); i++) {
                if (line[i] != ' ' && line[i] != '\t' && line[i] != '\\' && line[i] != '\n') {
                    
                    find_val[n] = line[i];
                    n++;
                    
                }
                
            }
            find_val[n] = '\0';
            
            strcat(buff_value, find_val);

            if (line[strlen(line) - 2] != '\\' && strlen(find_val) > 1) {
               break;    
            } 
        }
    }
        
    put(hm, key, buff_value); 

}

int data_preprocessing(struct hashmap_t *hm, FILE *fin, FILE *fout) {
    
    char line[MAX_LEN];
    int line_len = 0;
    char *token;
    char buffer[MAX_LEN];
    int can_write = 1; // se poate scrie

    memset(line, 0, MAX_LEN);
    memset(buffer, 0, MAX_LEN);

    while (fgets(line, MAX_LEN, fin) != NULL) {

        // verific daca pe linie se afla o directiva
        if (strstr(line, "#define ") && can_write == 1) {
            define_directive(hm, fin, line);
        
        } else if (strstr(line, "#undef ")) {
            token = strtok(line, " ");
            token = strtok(NULL, "\n");
            remove_pair(hm, token);


        } else if (strstr(line, "#if ")) {

            token = strtok(line, " ");
            token = strtok(NULL, "\n");

            can_write = check_if_cond(hm, token);

        } else if (strstr(line, "#elif ")) {

            token = strtok(line, " ");
            token = strtok(NULL, "\n");

            can_write = check_if_cond(hm, token);

        } else if (strstr(line, "#ifdef ")) {
            // strcat(buffer, "\n");
            token = strtok(line, " ");
            token = strtok(NULL, "\n ");

            can_write = has_key(hm, token);
            

        } else if (strstr(line, "#ifndef ")) {
            // strcat(buffer, "\n");
            token = strtok(line, " ");
            token = strtok(NULL, "\n");

            can_write = ~has_key(hm, token) * (-1);


        } else if (strstr(line, "#else")) {
            
            can_write = ~can_write * (-1); 
            

        } else if (strstr(line, "#endif")) {
            can_write = 1;
            strcat(buffer, "\n");

        } else {
            // caz in care nu am directive
            token = strtok(line, "\n ");
            

            while (token != NULL && can_write == 1) {
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

                strcat(buffer, " ");
                token = strtok(NULL, " ");
            }
            
        }        
    }
    fprintf(fout, "%s\n", buffer);

    return 1;
}

int main(int argc, char* argv[]) {

    FILE *fin;
    FILE *fout;

    char input_file[MAX_LEN];
    char output_file[MAX_LEN];

    struct hashmap_t *hm = malloc(sizeof(struct hashmap_t));

    if (hm == NULL) {
        printf("Malloc failed - hashmap!\n");
        exit(12);
    }
    
    init_hm(hm, HMAX);

    memset(input_file, 0, MAX_LEN);
    memset(output_file, 0, MAX_LEN);
		
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-D", 2) == 0) {

            if (strlen(argv[i]) > 2) {
                memmove(argv[i], argv[i] + 2, strlen(argv[i]));
                D_flag(hm, argv[i]);
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


    int r = data_preprocessing(hm, fin, fout);

    if (r != 1) {
        return ERROR_CODE;
    }


    if (fin != NULL) {
        fclose(fin);
    }

    if (fout != NULL) {
        fclose(fout);
    }

    free_hm(hm);

    return 0;
}