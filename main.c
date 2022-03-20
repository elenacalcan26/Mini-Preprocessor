#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"

#define HMAX 200
#define MAX_LEN 256
#define ENOMEM 12

/**
 * @brief defineste simbolul dat ca argument cu valoarea acestuia
 *
 * @param hm hashmap ce stocheaza define-uri
 * @param arg argument specificat de flag-ul -D
 */
void D_flag(struct hashmap_t *hm, char *arg)
{
	char *key, *value;
	char *ch;

	// se cauta aparitia caracterului '='
	ch = strchr(arg, '=');
	if (ch == NULL) {
		// simbolului ii lipseste maparea
		key = arg;
		value = "";
	} else {
		// se asociaza simbolului valoarea dat ca argument
		key = strtok(arg, "=");
		value = strtok(NULL, "");
	}

	put(hm, key, value);
}

/**
 * @brief Se aduaga intr-un array de directoare, directorul dat ca argument
 *
 * @param arg argumentul specificat de flag-ul -I
 * @param directories array-ul de directoare
 * @param n_dirs numarul de directoare
 */
void I_flag(char *arg, char **directories, int *n_dirs)
{
	directories[*n_dirs] = (char *)calloc(strlen(arg) + 1, sizeof(char));

	if (directories[*n_dirs] == NULL)
		exit(ENOMEM);

	strcpy(directories[*n_dirs], arg);
	*n_dirs = *n_dirs + 1;
}

/**
 * @brief Verifica daca conditia directivei #if se evalueaza la 0 sau la o
 * valoare diferita de 0
 *
 * @param hm hashmap-ul cu define-uri
 * @param cond conditia care se evalueaza
 * @return int valoarea conditiei, 0 daca se evalueaza la 0, 1 in caz contrar
 */
int check_if_cond(struct hashmap_t *hm, char *cond)
{
	if (strcmp(cond, "0") == 0)
		return 0;

	if (has_key(hm, cond) == 1 && strcmp(get(hm, cond), "0") == 0)
		return 0;

	return 1;
}

/**
 * @brief Este implementata directiva #define.
 * Se stocheaza in hashmap un simbol definit cu valoarea asociata acestuia
 *
 * @param hm hashmap-ul ce contine define-uri
 * @param fin fisierul de intrare
 * @param line linia pe care s-a gasit un #define
 */
void define_directive(struct hashmap_t *hm, FILE *fin, char *line)
{
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

	// se verifica daca #define -ul este de tip multiline
	if (line[strlen(line) - 2] != '\\') {
		// nu este define pe mai multe linii
		value = strtok(NULL, "\n");

		if (value != NULL) {

			// se ia valoarea simbolului caracter cu caracter
			for (i = 0; i < strlen(value); i++) {
				if (value[i] != ' ') {
					find_val[n] = value[i];
					n++;
					find_val[n] = '\0';
				}

				/**
				 * se verifica daca valoarea simbolului
				 * contine un simbol deja definit inainte
				 */
				if (has_key(hm, find_val) == 1 &&
					i < strlen(value - 1) &&
					value[i + 1] == ' ') {

					/**
					 * se inlocuieste simbolul gasit cu
					 * valoarea sa salvat in hashmap
					 */
					strcat(buff_value, get(hm, find_val));

					memset(find_val, 0, MAX_LEN);
					n = 0;
					continue;
				}

				/**
				 * s-a ajuns la un spatiu sau la ultimul caracter
				 * din valoarea simbolului si se adauga string-ul
				 * construit la valoarea simbolului definti
				 */
				if (value[i] == ' ' || i == strlen(value) - 1) {
					/**
					 * se verifica daca s-a gasit un simbol
					 * defintit anterior
					 */
					if (has_key(hm, find_val) == 1)
						strcat(buff_value,
						get(hm, find_val));

					else
						strcat(buff_value, find_val);

					memset(find_val, 0, MAX_LEN);
					n = 0;
					continue;
				}
			}
		} else
			// lipseste valoarea simbolului
			value = "";

	} else {
		// #define tip multilinie

		value = strtok(NULL, "\\\n");
		value[strlen(value) - 1] = '\0';
		strcpy(buff_value, value);

		/**
		 * se citeste linie cu linie valorile define-ului
		 * si se concateneaza la un string ce va fi asociat
		 * simbolului
		 */
		while (fgets(line, sizeof(line), fin) != NULL) {
			memset(find_val, 0, MAX_LEN);
			n = 0;

			for (i = 0; i < strlen(line); i++) {

				if (line[i] != ' ' && line[i] != '\t' &&
					line[i] != '\\' && line[i] != '\n') {

					find_val[n] = line[i];
					n++;
				}
			}
			find_val[n] = '\0';

			strcat(buff_value, find_val);

			if (line[strlen(line) - 2] != '\\' &&
				strlen(find_val) > 1)

				break;
		}
	}
	put(hm, key, buff_value);
}

/**
 * @brief Rezolva directivele de tip #include.
 * Atunci cand s-a gasit fisierul indicat de header se preproceseaza
 * liniile de cod ale acestuia in fisierul de iesire.
 *
 * @param line linia pe care s-a gasit directiva #inlcude
 * @param hm hashmap-ul cu define-uri
 * @param fout fisierul de output
 * @param in_file numele fisierului de intrare
 * @param directories array de directoare
 * @param n_dirs numarul de directoare
 * @return int 1 s-a inclus cu succes header-ul, -1 in caz contrar
 */
int include_directive(char *line, struct hashmap_t *hm, FILE *fout,
					  char *in_file, char **directories,
					  int n_dirs)
{
	char *header, path[MAX_LEN], *ret;
	FILE *f_header;
	int r, found, i;

	header = strtok(line, "\"");
	header = strtok(NULL, "\"");

	// se ia fisierul inclus
	ret = strrchr(in_file, '/');

	// se ia directorul curent si se creeaza un path
	in_file[strlen(in_file) - strlen(ret) + 1] = '\0';
	strcat(in_file, header);

	// se deschide fisierul
	f_header = fopen(in_file, "r");
	if (f_header != NULL) {

		// se preproceseaza fisierul
		r = data_preprocessing(hm, f_header, fout, in_file,
			directories, n_dirs);

		// se verifica daca preprocesarea s-a realizat cu succes
		if (r != 1)
			return -1;

	} else {
		/**
		 * fisierul de header nu se afla in directorul curent.
		 * se ia fiecare director cunoscut si se creeaza path-uri
		 * catre fisierul indicat de header.
		 */

		found = 0; // indica daca s-a gasit un path

		for (i = 0; i < n_dirs; i++) {
			memset(path, 0, MAX_LEN);
			strcpy(path, directories[i]);
			strcat(path, ret);

			f_header = fopen(path, "r");
			if (f_header == NULL)
				return -1;

			// se preproceseaza fisierul
			r = data_preprocessing(hm, f_header, fout, path,
				directories, n_dirs);

			// se verifica daca preprocesarea s-a realizat cu succes
			if (r != 1)
				return -1;

			found = 1;
			break;

		}

		/**
		 * se verifica daca s-a gasit un path catre
		 * fisierul indicat de header
		 */
		if (found == 0)
			return -1;
	}

	fclose(f_header);

	return 1;
}

/**
 * @brief Se preproceseaza o linie care nu contine directive.
 *
 * @param line linia preprocesata
 * @param hm hashmap-ul cu define-uri
 * @param fout fisierul de output
 */
void process_line(char *line, struct hashmap_t *hm, FILE *fout)
{
	char *token;
	char buffer[MAX_LEN], str[MAX_LEN];
	int n, i;

	// se imparte linia dupa spatii
	token = strtok(line, "\n ");
	memset(buffer, 0, MAX_LEN);

	while (token != NULL) {
		memset(str, 0, MAX_LEN);
		n = 0;

		// se parcurge un cuvant caracter cu caracter
		for (i = 0; i < strlen(token); i++) {
			if (token[i] != ' ' && token[i] != '\t') {
				str[n] += token[i];
				n++;

				// se verifica daca s-a gasit un simbol definit
				if (has_key(hm, str) == 1) {
					strcat(buffer, get(hm, str));
					memset(str, 0, MAX_LEN);
					n = 0;
				}
			}
		}

		strcat(buffer, str);
		strcat(buffer, " ");
		token = strtok(NULL, " ");
	}

	fprintf(fout, "%s\n", buffer);
}

/**
 * @brief Se preproceseaza fisierul primit la intrare
 *
 * @param hm hashmap-ul de define-uri
 * @param fin fisierul preprocesat
 * @param fout fisierul de input
 * @param in_file numele fisierului de input
 * @param directories array de directoare
 * @param n_dirs nuamrul de directoare
 * @return int 1 succes, -1 fail
 */
int data_preprocessing(struct hashmap_t *hm, FILE *fin, FILE *fout,
					   char *in_file, char **directories,
					   int n_dirs)
{

	char line[MAX_LEN];
	int line_len = 0, i;
	char *token;
	char buffer[MAX_LEN];
	int skip = 1; // se poate scrie

	memset(line, 0, MAX_LEN);
	memset(buffer, 0, MAX_LEN);

	/**
	 * se citeste linie cu linie fisierul preprocesat
	 * si se verifica pe fiecare linie ce fel de directiva
	 * se afla
	 */
	while (fgets(line, MAX_LEN, fin) != NULL) {

		if (strstr(line, "#define ") && skip == 1) {

			define_directive(hm, fin, line);
		} else if (strstr(line, "#undef ")) {
			token = strtok(line, " ");
			token = strtok(NULL, "\n");
			remove_pair(hm, token);
		} else if (strstr(line, "#if ")) {

			token = strtok(line, " ");
			token = strtok(NULL, "\n");

			/**
			 * in functie de evaluarea conditiei se scrie
			 * blocul din interiorul acestuia sau nu
			 */
			skip = check_if_cond(hm, token);
		} else if (strstr(line, "#elif ")) {

			token = strtok(line, " ");
			token = strtok(NULL, "\n");

			/**
			 * in functie de evaluarea conditiei se scrie
			 * blocul din interiorul acestuia sau nu
			 */
			skip = check_if_cond(hm, token);
		} else if (strstr(line, "#ifdef ")) {
			token = strtok(line, " ");
			token = strtok(NULL, "\n ");

			/**
			 * in functie de existenta cheii data
			 * de directiva se scrie blocul de cod
			 */
			skip = has_key(hm, token);
		} else if (strstr(line, "#ifndef ")) {

			token = strtok(line, " ");
			token = strtok(NULL, "\n");

			/**
			 * in functie de existenta cheii (aici negata)
			 * data de directiva se scrie blocul
			 */
			skip = ~has_key(hm, token) * (-1);
		} else if (strstr(line, "#else")) {

			/**
			 * se neaga variabila si in functie de rezultat
			 * se scrie blocul de cod sau nu
			 */
			skip = ~skip * (-1);
		} else if (strstr(line, "#endif")) {
			// se resteaza variabila
			skip = 1;
			strcat(buffer, "\n");
		} else if (strstr(line, "#include")) {

			/**
			 * se verifica daca se poate include header-ul
			 */
			if (include_directive(
				line, hm, fout, in_file, directories, n_dirs) !=
				1)

				return -1; // fail

		} else {
			/**
			 * caz in care nu exista directive pe linia curenta.
			 * linia se poate scrie doar daca este in afara
			 * directivelor #if, #elif, #ifdef, #ifndef etc.
			 * sau daca sunt linii dintr-un bloc a carei conditie
			 * este evaluata la o valoare diferita de 0.
			 */

			if (skip == 1)
				process_line(line, hm, fout);

		}
	}

	return 1;
}

int main(int argc, char *argv[])
{

	FILE *fin;
	FILE *fout;

	char input_file[MAX_LEN];
	char output_file[MAX_LEN];
	char **directories;

	int n_dirs = 0;
	int cnt_in = 0; // numara fisierele de intrare primite la intrare
	int cnt_out = 0; // numara fisierele de ouptu  primite la intrare
	int i, r;

	struct hashmap_t *hm = malloc(sizeof(struct hashmap_t));

	if (hm == NULL) {
		printf("Malloc failed - hashmap!\n");
		exit(ENOMEM);
	}

	init_hm(hm, HMAX);

	directories = (char **)malloc(MAX_LEN * sizeof(char *));
	if (directories == NULL) {
		printf("Malloc failed\n");
		exit(ENOMEM);
	}

	memset(input_file, 0, MAX_LEN);
	memset(output_file, 0, MAX_LEN);

	// se parcurg argumentele din linia de comanda si se verifica flag-urile
	for (i = 1; i < argc; i++) {

		//-D <SYMBOL>[=<MAPPING>] sau -D<SYMBOL>[=<MAPPING>]
		if (strncmp(argv[i], "-D", 2) == 0) {
			if (strlen(argv[i]) > 2) {
				memmove(argv[i], argv[i] + 2, strlen(argv[i]));
				D_flag(hm, argv[i]);
			} else {
				D_flag(hm, argv[i + 1]);
				i++;
			}
		} else if (strncmp(argv[i], "-I", 2) == 0) {
			// -I <DIR> sau -I<DIR>

			if (strlen(argv[i]) > 2) {

				memmove(argv[i], argv[i] + 2, strlen(argv[i]));
				I_flag(argv[i], directories, &n_dirs);
			} else {

				I_flag(argv[i + 1], directories, &n_dirs);
				i++;
			}
		} else if (strncmp(argv[i], "-o", 2) == 0) {
			// -o <OUTFILE>/ -o<OUTFILE>

			if (strlen(argv[i]) > 2) {
				memmove(argv[i], argv[i] + 2, strlen(argv[i]));
				strcpy(output_file, argv[i]);
			} else {

				strcpy(output_file, argv[i + 1]);
				i++;
			}
			cnt_out++;
		} else {
			/**
			 * nu s-a gasit nici un flag,
			 * atunci s-au gasit fisiere pentru I/O
			 * primul fisier este considerat de intrare,
			 * iar ultimul de iesire
			 */

			if (strlen(input_file) > 1) {
				strcpy(output_file, argv[i]);
				cnt_out++;
			} else {
				strcpy(input_file, argv[i]);
				cnt_in++;
			}
		}
	}

	// se verifica numarul de fisiere in/out primite ca argumente
	if (cnt_in > 1 || cnt_out > 1)
		return ENOMEM;

	// se verifica daca s-a primit  un fisier intrare / iesire
	if (strlen(input_file) > 1)
		fin = fopen(input_file, "r");
	else
		fin = stdin;

	if (fin == NULL) {
		printf("Can't open file - input\n");
		exit(ENOMEM);
	}

	if (strlen(output_file) > 1)
		fout = fopen(output_file, "w");
	else
		fout = stdout;

	if (fout == NULL) {
		printf("Can't open file - ouput\n");
		exit(ENOMEM);
	}

	r = data_preprocessing(hm, fin, fout, input_file, directories, n_dirs);

	if (r != 1)
		return -1;

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
