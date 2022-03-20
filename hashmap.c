#include "hashmap.h"

#define ENOMEM 12

/**
 * @brief Functie de hash folosita pentru a calcula index-ul perechii
 * din array-ul de bucket-uri
 *
 * @param str cheia perechii
 * @return unsigned int valoarea hashuita
 */
unsigned int hash(unsigned char *str)
{
	unsigned int hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}

/**
 * @brief Functie apelata dupa alocarea unui hashmap pentru a il initializa
 *
 * @param hm hashmap alocat in main
 * @param hmax dimensiunea maxima initiala a array-ului de bucket-uri
 */
void init_hm(struct hashmap_t *hm, int hmax)
{
	int i;

	hm->hmax = hmax;
	hm->size = 0;
	hm->buckets = (struct pair **)malloc(hmax * sizeof(struct pair *));

	if (hm->buckets == NULL) {
		free(hm);
		printf("Malloc feiled - hashmap buckets");
		exit(ENOMEM);
	}

	for (i = 0; i < hm->hmax; i++)
		hm->buckets[i] = NULL;
}

/**
 * @brief Adauga o pereche cheie - valoare in hashmap
 *
 * @param hm hashmap folosit
 * @param key cheia perechii
 * @param value valoarea asociata cheii
 */
void put(struct hashmap_t *hm, char *key, char *value)
{
	int i;
	int index;
	struct pair *new_pair;

	index = hash(key) % hm->hmax;

	// se aloca memorie pentru perechea ce va fi inserata
	new_pair = (struct pair *)malloc(sizeof(struct pair));
	if (new_pair == NULL) {
		printf("Malloc failed - new pair");
		exit(ENOMEM);
	}

	new_pair->key = calloc(strlen(key) + 1, sizeof(char));

	if (new_pair->key == NULL) {
		printf("Calloc failed - key");
		exit(ENOMEM);
	}

	new_pair->value = calloc(strlen(value) + 1, sizeof(char));

	if (new_pair->value == NULL) {
		printf("Calloc failed - value");
		exit(ENOMEM);
	}

	strcpy(new_pair->key, key);
	strcpy(new_pair->value, value);

	// se verifica daca a aparut o coliziune
	if (hm->buckets[index] == NULL) {
		// se insereaza perechea in index-ul calculat
		hm->buckets[index] = new_pair;
	} else {

		// verific daca au aceeasi cheie
		if (strcmp(key, hm->buckets[index]->key) == 0) {
			// se inlocuieste vechea valoare cu cea noua

			strcpy(hm->buckets[index]->value, value);
		} else {

			int inserted = 0;

			// se cauta primul bucket gol in care se insereaza perechea

			for (i = index; i < hm->hmax; i++) {
				if (hm->buckets[i] == NULL) {
					hm->buckets[i] = new_pair;
					inserted = 1;
					break;
				}
			}

			if (inserted == 0) {
				for (i = 0; i < index; i++) {
					if (hm->buckets[i] == NULL) {
						hm->buckets[i] = new_pair;
						inserted = 1;
						break;
					}
				}
			}

			if (inserted == 0 && hm->size == hm->hmax) {
				// se dubleaza capacitatea bucket-urilor
				hm->buckets = (struct pair **)realloc(
					hm->buckets,
					hm->hmax * 2 * sizeof(struct pair));

				if (hm->buckets == NULL) {
					printf("Realloc failed - buckets");
					exit(ENOMEM);
				}

				hm->hmax *= 2;

				for (i = hm->size; i < hm->hmax; i++)
					hm->buckets[i] = NULL;

				// se insereaza perechea in primul bucket gol gasit
				for (i = hm->size; i < hm->hmax; i++) {
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

/**
 * @brief Verifca daca chiea data ca parametru exista sau nu
 * in hashmap
 *
 * @param hm hashmap-ul in care se face verificarea
 * @param key cheia verificata
 * @return int 1 cheia exista in hashmap, 0 in caz contrar
 */
int has_key(struct hashmap_t *hm, char *key)
{
	int i;

	int index = hash(key) % hm->hmax;

	for (i = index; i < hm->hmax; i++)
		if (hm->buckets[i] != NULL &&
			strcmp(key, hm->buckets[i]->key) == 0)
			return 1;

	for (i = 0; i < index; i++)
		if (hm->buckets[i] != NULL &&
			strcmp(key, hm->buckets[i]->key) == 0)
			return 1;

	return 0;
}

/**
 * @brief Returneaza valoarea asociata unei chei din hashmap
 *
 * @param hm hashmap-ul in care se realizeaza cautarea
 * @param key cheia dupa care se face cautarea
 * @return char* valoarea asociata cheii, NULL nu s-a gasit cheia in hashmap
 */
char *get(struct hashmap_t *hm, char *key)
{
	int i;
	int index;

	index = hash(key) % hm->hmax;

	for (i = index; i < hm->hmax; i++)
		if (hm->buckets[i] != NULL &&
			strcmp(key, hm->buckets[i]->key) == 0)
			return hm->buckets[i]->value;

	for (i = 0; i < index; i++)
		if (hm->buckets[i] != NULL &&
			strcmp(key, hm->buckets[i]->key) == 0)
			return hm->buckets[i]->value;

	return NULL;
}

/**
 * @brief Sterge o pereche cheie - valoare din hashmap
 *
 * @param hm hashmap-ul din care se sterge perechea
 * @param key chia perechii sterse
 */
void remove_pair(struct hashmap_t *hm, char *key)
{
	int i;
	int index;

	index = hash(key) % hm->hmax;

	for (i = index; i < hm->hmax; i++) {
		if (hm->buckets[i] != NULL &&
			strcmp(key, hm->buckets[i]->key) == 0) {

			free(hm->buckets[i]->key);
			free(hm->buckets[i]->value);
			free(hm->buckets[i]);
			hm->buckets[i] = NULL;
			return;
		}
	}

	for (i = 0; i < index; i++) {
		if (hm->buckets[i] != NULL &&
			strcmp(key, hm->buckets[i]->key) == 0) {

			free(hm->buckets[i]->key);
			free(hm->buckets[i]->value);
			free(hm->buckets[i]);
			hm->buckets[i] = NULL;
			return;
		}
	}
}

/**
 * @brief Elibereaza memoria alocata hashmap-ului
 *
 * @param hm hashmap-ul dealocat
 */
void free_hm(struct hashmap_t *hm)
{
	int i;

	for (i = 0; i < hm->hmax; i++) {
		if (hm->buckets[i] != NULL)	{
			free(hm->buckets[i]->key);
			free(hm->buckets[i]->value);
			free(hm->buckets[i]);
		}
	}

	free(hm->buckets);
	free(hm);
}
