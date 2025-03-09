/*
 * Projet : Méta-heuristique sur SADM
 * Date   : 08/03/2025
 * Description : utils.c quelques fonctions utiles pour le bien du fonctionnement
 */

#include "utils.h"

/*
 * Fonction : free_tab
 * ----------------
 * Free les tableaux globale
 *
 * Paramètres :
 *   - /
 *
 * Retour :
 *   - /
 */

void free_tab() {
  free(tab_coef);
  free(tab_m);

  if (tab_m_poid) {
    for (int i = 0; i < m; i++) {
      free(tab_m_poid[i]);
    }
    free(tab_m_poid);
  }
}

/*
 * Fonction : malloc_tab_m_poid
 * ----------------
 * Allocation de la mémoire d'un tableau de tableau
 *
 * Paramètres :
 *   - /
 *
 * Retour :
 *   - /
 */

void malloc_tab_m_poid() {
  tab_m_poid = malloc(m * sizeof(int * ));
  if (!tab_m_poid) {
    printf("Erreur d'allocation mémoire pour tab_m_poid\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < m; i++) {
    tab_m_poid[i] = malloc(n * sizeof(int));
    if (!tab_m_poid[i]) {
      printf("Erreur d'allocation mémoire ligne %d\n", i);
      exit(EXIT_FAILURE);
    }
  }
}

/*
 * Fonction : trim_spaces
 * ----------------
 * Permet d'enlever les espaces inutiles
 *
 * Paramètres :
 *   - char * str : une chaîne de caractère
 *
 * Retour :
 *   - /
 */

void trim_spaces(char * str) {
  int i = 0, j = 0;
  int in_space = 0;

  while (str[i]) {
    if (!isspace((unsigned char) str[i]) || (j > 0 && !in_space)) {
      str[j++] = str[i];
    }
    in_space = isspace((unsigned char) str[i]);
    i++;
  }
  str[j] = '\0';
}

/*
 * Fonction : insert_tab
 * ----------------
 * Permet d'insérer dans un tableau une chaine de caractère
 *
 * Paramètres :
 *   - char * myString : une chaîne de caractère
 *   - int count : le nombre d'itération
 *   - int *tab : le tableau
 * Retour :
 *   - /
 */

void insert_tab(char * myString, int count, int * tab) {
  char * ptr = myString;
  for (int i = 0; i < count; i++) {
    sscanf(ptr, "%d", & tab[i]);
    while ( * ptr && * ptr != ' ')
      ptr++;
    while ( * ptr == ' ')
      ptr++;
  }
}