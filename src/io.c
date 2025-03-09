/*
 * Projet : Méta-heuristique sur SADM
 * Date   : 08/03/2025
 * Description : io.c programme gérant les input/output des fichiers
 */

#include "io.h"

/*
 * Fonction : write_data
 * ----------------
 * Fonction permettant d'écrire le résultat de la solution dans un fichier
 *
 * Paramètres :
 *   - char * file : Nom du fichier
 *   - int * solution : Tableau de la solution
 *
 * Retour :
 *   - /
 */

void write_data(char *file, int *solution)
{
  int profit = 0;
  int occurs = 0;
  int index[n];

  for (int i = 0; i < n; i++)
  {
    if (solution[i] == 1)
    {
      profit += tab_coef[i];
      index[occurs] = i;
      occurs++;
    }
  }

  char file_name_copy[256];
  strncpy(file_name_copy, file, 255);
  file_name_copy[255] = '\0';

  snprintf(file, 512, "%s_%d.txt", file_name_copy, profit);

  FILE *fptr = fopen(file, "w");
  if (fptr == NULL)
  {
    perror("Erreur lors de l'ouverture du fichier");
    return;
  }

  fprintf(fptr, "%d %d\n", profit, occurs);
  for (int i = 0; i < occurs; i++)
  {
    fprintf(fptr, "%d ", index[i]);
  }
  fclose(fptr);
}

/*
 * Fonction : read_file
 * ----------------
 * Fonction permettant de lire le fichier d'instance et d'assigner à des variables globale
 *
 * Paramètres :
 *   - char * file : Nom du fichier d'instance
 *
 * Retour :
 *   - /
 */

int read_file(char *file)
{
  FILE *fptr = fopen(file, "r");
  if (!fptr)
  {
    printf("Impossible d'ouvrir le fichier d'instance [%s] introuvable \n", file);
    return -1;
  }

  char myString[256];

  // Lire la première ligne (n et m)
  if (fgets(myString, sizeof(myString), fptr))
  {
    trim_spaces(myString);
    sscanf(myString, "%d %d", &n, &m);

    // Allocation memoire pour tab_coef, tab_m, et tab_m_poid
    tab_coef = malloc(n * sizeof(int));
    tab_m = malloc(m * sizeof(int));
    malloc_tab_m_poid();

    if (!tab_coef || !tab_m)
    {
      printf("Erreur d'allocation mémoire\n");
      return -1;
    }
  }

  // Lire les poids des objets (n valeurs)
  int poids_index = 0;
  while (poids_index < n && fgets(myString, sizeof(myString), fptr))
  {
    trim_spaces(myString);
    int count = (poids_index + 10 <= n) ? 10 : (n - poids_index);
    insert_tab(myString, count, tab_coef + poids_index);
    poids_index += count;
  }
  fgets(myString, sizeof(myString), fptr);
  //  Lire les capacites des contraintes (m valeurs)
  int m_index = 0;
  while (m_index < m && fgets(myString, sizeof(myString), fptr))
  {
    trim_spaces(myString);
    int count = (m_index + 10 <= m) ? 10 : (m - m_index);
    insert_tab(myString, count, tab_m + m_index);
    m_index += count;
  }
  fgets(myString, sizeof(myString), fptr);

  // Lire les poids des objets pour chaque contrainte (m * n valeurs)
  for (int i = 0; i < m; i++)
  {
    int obj_index = 0;
    while (obj_index < n && fgets(myString, sizeof(myString), fptr))
    {
      trim_spaces(myString);
      // regarder si ligne vide ou non
      if (strlen(myString) != 0)
      {
        int count = (obj_index + 10 <= n) ? 10 : (n - obj_index);
        insert_tab(myString, count, tab_m_poid[i] + obj_index);
        obj_index += count;
      }
    }
  }

  fclose(fptr);
  return 1;
}

/*
 * Fonction : print_data
 * ----------------
 * Fonction d'affichage pour le debuggage
 *
 * Paramètres :
 *   - /
 *
 * Retour :
 *   - /
 */

void print_data()
{
  printf("Nombre d objets: %d, Nombre de contraintes: %d\n", n, m);

  printf("\nCoefficients des objets :\n");
  for (int i = 0; i < n; i++)
  {
    printf("%d ", tab_coef[i]);
  }
  printf("\n");

  printf("\nCapacites des contraintes :\n");
  for (int i = 0; i < m; i++)
  {
    printf("%d ", tab_m[i]);
  }
  printf("\n");

  printf("\nPoids des objets par contrainte :\n");
  for (int i = 0; i < m; i++)
  {
    printf("Contrainte %d: ", i + 1);
    for (int j = 0; j < n; j++)
    {
      printf("%d ", tab_m_poid[i][j]);
    }
    printf("\n");
  }
}