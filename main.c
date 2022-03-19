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

void I_flag(char *arg, char **directories, int *n_dirs) {
  directories[*n_dirs] = (char *)calloc(strlen(arg) + 1, sizeof(char));

  if (directories[*n_dirs] == NULL) 
	exit(12);
  
  strcpy(directories[*n_dirs], arg);
  *n_dirs = *n_dirs + 1;
}

int check_if_cond(struct hashmap_t *hm, char *cond) {

  if (strcmp(cond, "0") == 0)
	return 0;

  if (has_key(hm, cond) == 1 && strcmp(get(hm, cond), "0") == 0)
	return 0;
  
  return 1;
}

void define_directive(struct hashmap_t *hm, FILE *fin, char *line) {
  char *key, *value, *token;
  char tmp_line[MAX_LEN] = {0};
  char buff_value[MAX_LEN] = {0};
  char find_val[MAX_LEN] = {0};
  int n = 0;
  int n_dirs = 0;
  int i;
  strcpy(tmp_line, line);
  token = strtok(tmp_line, " ");
  key = strtok(NULL, " ");

  memset(buff_value, 0, MAX_LEN);
  memset(find_val, 0, MAX_LEN);

  if (line[strlen(line) - 2] != '\\') {
	// nu este define pe mai multe linii
	value = strtok(NULL, "\n");

	if (value != NULL) {

	  for (i = 0; i < strlen(value); i++) {
		if (value[i] != ' ') {
		  find_val[n] = value[i];
		  n++;
		  find_val[n] = '\0';
		}

		if (has_key(hm, find_val) == 1 && i < strlen(value - 1) &&
			value[i + 1] == ' ') {
		  strcat(buff_value, get(hm, find_val));

		  memset(find_val, 0, MAX_LEN);
		  n = 0;
		  continue;
		}

		if (value[i] == ' ' || i == strlen(value) - 1) {

		  if (has_key(hm, find_val) == 1) {
			strcat(buff_value, get(hm, find_val));
		  } else {
			strcat(buff_value, find_val);
		  }

		  memset(find_val, 0, MAX_LEN);
		  n = 0;
		  continue;
		}
	  }
	} else {
	  value = "";
	}
  } else {
	value = strtok(NULL, "\\\n");
	value[strlen(value) - 1] = '\0';
	strcpy(buff_value, value);

	while (fgets(line, sizeof(line), fin) != NULL) {
	  memset(find_val, 0, MAX_LEN);
	  n = 0;

	  for (i = 0; i < strlen(line); i++) {
		if (line[i] != ' ' && line[i] != '\t' && line[i] != '\\' &&
			line[i] != '\n') {

		  find_val[n] = line[i];
		  n++;
		}
	  }
	  find_val[n] = '\0';

	  strcat(buff_value, find_val);

	  if (line[strlen(line) - 2] != '\\' && strlen(find_val) > 1)
		break;
	  
	}
  }
  put(hm, key, buff_value);
}

int include_directive(char *line, struct hashmap_t *hm, FILE *fout,
					  char *in_file, char **directories, int n_dirs) {
  char *header, path[MAX_LEN], *ret;
  FILE *f_header;
  int r, found, i;
  header = strtok(line, "\"");
  header = strtok(NULL, "\"");

  // iau fisierul .h
  ret = strrchr(in_file, '/');

  // iau directorul curent
  in_file[strlen(in_file) - strlen(ret) + 1] = '\0';
  strcat(in_file, header);

  f_header = fopen(in_file, "r");
  if (f_header != NULL) {
	r = data_preprocessing(hm, f_header, fout, in_file, directories, n_dirs);

	if (r != 1)
	  return 12;
	
  } else {
	found = 0;

	for (i = 0; i < n_dirs; i++) {
	  memset(path, 0, MAX_LEN);
	  strcpy(path, directories[i]);
	  strcat(path, ret);

	  f_header = fopen(path, "r");
	  if (f_header == NULL) 
		return ERROR_CODE;
	  

	  r = data_preprocessing(hm, f_header, fout, path, directories, n_dirs);

	  if (r != 1) {
		return ERROR_CODE;
	  } else {
		found = 1;
		break;
	  }
	}

	if (found == 0) 
	  return ERROR_CODE;
	
  }

  fclose(f_header);

  return 1;
}

int data_preprocessing(struct hashmap_t *hm, FILE *fin, FILE *fout,
					   char *in_file, char **directories, int n_dirs) {

  char line[MAX_LEN];
  int line_len = 0, i;
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
	  token = strtok(line, " ");
	  token = strtok(NULL, "\n ");
	  can_write = has_key(hm, token);
	} else if (strstr(line, "#ifndef ")) {

	  token = strtok(line, " ");
	  token = strtok(NULL, "\n");

	  can_write = ~has_key(hm, token) * (-1);
	} else if (strstr(line, "#else")) {

	  can_write = ~can_write * (-1);
	} else if (strstr(line, "#endif")) {
	  can_write = 1;
	  strcat(buffer, "\n");
	} else if (strstr(line, "#include")) {

	  if (include_directive(line, hm, fout, in_file, directories, n_dirs) !=
		  1) 
		return 12;
	  
	} else {
	  // caz in care nu am directive
	  token = strtok(line, "\n ");
	  memset(buffer, 0, MAX_LEN);

	  while (token != NULL && can_write == 1) {
		char str[MAX_LEN];
		int k = 0;
		memset(str, 0, MAX_LEN);

		for (i = 0; i < strlen(token); i++) {
		  if (token[i] != ' ' && token[i] != '\t') {
			str[k] += token[i];
			k++;
			if (has_key(hm, str) == 1) {
			  strcpy(str, get(hm, str));
			  strcat(buffer, str);
			  memset(str, 0, MAX_LEN);
			  k = 0;
			}
		  }
		}

		strcat(buffer, str);

		strcat(buffer, " ");
		token = strtok(NULL, " ");
	  }

	  fprintf(fout, "%s\n", buffer);
	}
  }

  return 1;
}

int main(int argc, char *argv[]) {

  FILE *fin;
  FILE *fout;

  char input_file[MAX_LEN];
  char output_file[MAX_LEN];
  char **directories;

  int n_dirs = 0;
  int cnt_in = 0;
  int cnt_out = 0;
  int i, r;

  struct hashmap_t *hm = malloc(sizeof(struct hashmap_t));

  if (hm == NULL) {
	printf("Malloc failed - hashmap!\n");
	exit(12);
  }

  init_hm(hm, HMAX);
  directories = (char **)malloc(MAX_LEN * sizeof(char *));
  if (directories == NULL) {
	printf("Malloc failed\n");
	exit(12);
  }

  memset(input_file, 0, MAX_LEN);
  memset(output_file, 0, MAX_LEN);

  for (i = 1; i < argc; i++) {
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

		memmove(argv[i], argv[i] + 2, strlen(argv[i]));
		I_flag(argv[i], directories, &n_dirs);
	  } else {

		I_flag(argv[i + 1], directories, &n_dirs);

		i++;
	  }
	} else if (strncmp(argv[i], "-o", 2) == 0) {

	  if (strlen(argv[i]) > 2) {
		memmove(argv[i], argv[i] + 2, strlen(argv[i]));
		strcpy(output_file, argv[i]);
	  } else {

		strcpy(output_file, argv[i + 1]);
		i++;
	  }
	  cnt_out++;
	} else {

	  if (strlen(input_file) > 1) {
		strcpy(output_file, argv[i]);
		cnt_out++;
	  } else {
		strcpy(input_file, argv[i]);
		cnt_in++;
	  }
	}
  }

  if (cnt_in > 2 || cnt_out > 1) 
	return 12;
  
  if (strlen(input_file) > 1) {
	fin = fopen(input_file, "r");
  } else {
	fin = stdin;
  }

  if (fin == NULL) {
	printf("Can't open file - input\n");
	return 12;
  }

  if (strlen(output_file) > 1) {
	fout = fopen(output_file, "w");
  } else {
	fout = stdout;
  }

  if (fout == NULL) {
	printf("Can't open file - ouput\n");
	return 12;
  }

  r = data_preprocessing(hm, fin, fout, input_file, directories, n_dirs);

  if (r != 1)
	return ERROR_CODE;

  if (fin != NULL) 
	fclose(fin);

  if (fout != NULL) 
	fclose(fout);
  
  for (i = 0; i < n_dirs; i++)
	free(directories[i]);
  
  free(directories);

  free_hm(hm);

  return 0;
}
