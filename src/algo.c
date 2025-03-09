/*
 * Projet : Méta-heuristique sur SADM
 * Date   : 08/03/2025
 * Description : algo.c Algorithme principale permettant le fonctionnement du programme
 */

#include "algo.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include "io.h"

/*
 * Fonction : argGestion
 * ----------------
 * Fonction permettant de gérer les arguments de la commande
 *
 * Paramètres :
 *   - int argc : nombre argument
 *   - char *argv[] : argument
 *   - char *input_file : fichier d'instance
 *   - int *exec_time : temps d'execution
 *   - char *algo : type d'algo (VNS|VND) par défault VNS
 *   - char *output_file : fichier de sortie
 *
 * Retour :
 *   - /
 */

void argGestion(int argc, char *argv[], char *input_file, int *exec_time, char *algo, char *output_file)
{
  if (argc < 3)
  {
    fprintf(stderr, "Usage: %s <input_file> <exec_time> [algo]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // Récupérer les arguments
  strncpy(input_file, argv[1], 255);
  input_file[255] = '\0';

  *exec_time = atoi(argv[2]);

  // Algo par défaut = "VNS"
  if (argc >= 4)
  {
    strncpy(algo, argv[3], 9);
    algo[9] = '\0';
  }
  else
  {
    strcpy(algo, "VNS");
  }

  char *file_name = basename(input_file);

  char file_name_copy[256];
  strncpy(file_name_copy, file_name, 255);
  file_name_copy[255] = '\0';

  char *dot = strrchr(file_name_copy, '.');
  if (dot != NULL && strcmp(dot, ".txt") == 0)
  {
    *dot = '\0'; // Tronquer l'extension .txt
  }

  // Générer le nom du fichier de sortie avec la date
  time_t t = time(NULL);
  struct tm tm_info;

  // Utiliser localtime_r si disponible, sinon localtime
#if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 1
  localtime_r(&t, &tm_info);
#else
  struct tm *tm_ptr = localtime(&t);
  if (tm_ptr == NULL)
  {
    fprintf(stderr, "Erreur lors de la récupération de l'heure locale.\n");
    exit(EXIT_FAILURE);
  }
  tm_info = *tm_ptr;
#endif

  snprintf(output_file, 255, "resultat/%s_%s_%04d_%02d_%02d_%02dh_%02dm_%02ds",
           algo,
           file_name_copy,
           tm_info.tm_year + 1900, tm_info.tm_mon + 1, tm_info.tm_mday,
           tm_info.tm_hour, tm_info.tm_min, tm_info.tm_sec);
}

/*
 * Fonction : calculer_profit
 * ----------------
 * Fonction permettant le calcul du profit d'une solution
 *
 * Paramètres :
 *   - int * solution : Tableau de la solution trouvé
 *
 * Retour :
 *   - int : le profit
 */

int calculer_profit(int *solution)
{
  int profit = 0;
  for (int i = 0; i < n; i++)
  {
    if (solution[i])
    {
      profit += tab_coef[i];
    }
  }
  return profit;
}

/*
 * Fonction : comparer
 * ----------------
 * Fonction permettant de comparer deux objets pour le ratio
 *
 * Paramètres :
 *   - const void * a : Ici des objets
 *   - const void * b : Ici des objets
 *
 * Retour :
 *   - int : le résultat de la comparaison (0,1,-1)
 */

int comparer(const void *a, const void *b)
{
  Objet *objA = (Objet *)a;
  Objet *objB = (Objet *)b;
  return (objB->ratio - objA->ratio > 0) - (objB->ratio - objA->ratio < 0);
}

/*
 * Fonction : verif_gloutonne
 * ----------------
 * Fonction de vérification de la gloutonne
 *
 * Paramètres :
 *   - int * solution : Tableau de la solution
 *
 * Retour :
 *   - int : Le status de la vérification (1,-1)
 */

int verif_gloutonne(int *solution)
{
  for (int i = 0; i < m; i++)
  {
    int totalPoids = 0;
    for (int j = 0; j < n; j++)
    {
      if (solution[j] == 1)
      {
        totalPoids += tab_m_poid[i][j];
      }
    }
    // Vérification si le total ne dépasse pas la contrainte
    if (totalPoids > tab_m[i])
    {
      return -1;
    }
  }
  return 1;
}

/*
 * Fonction : ls_echange
 * ----------------
 * Fonction de recherche local (LS-ECHANGE)
 *
 * Paramètres :
 *   - int * solution : Tableau de la solution
 * Retour :
 *   - /
 */

void ls_echange(int *solution)
{

  int best_profit = calculer_profit(solution);
  int *best_solution = (int *)malloc(n * sizeof(int));

  if (!best_solution)
  {
    printf("Erreur d'allocation mémoire pour best_solution\n");
    return;
  }
  memcpy(best_solution, solution, n * sizeof(int));

  for (int i = 0; i < n; i++)
  {
    // Si objet sélectionné
    if (solution[i] == 1)
    {
      for (int j = 0; j < n; j++)
      {
        // Echange avec un objet non sélectionné
        if (solution[j] == 0)
        {
          solution[i] = 0;
          solution[j] = 1;

          // Vérifier contrainte
          int valid = 1;
          for (int k = 0; k < m; k++)
          {
            int totalPoids = 0;
            for (int l = 0; l < n; l++)
            {
              if (solution[l] == 1)
              {
                totalPoids += tab_m_poid[k][l];
              }
            }
            if (totalPoids > tab_m[k])
            {
              valid = 0;
              break;
            }
          }

          // Calcul du nouveau profit et le comparer à l'ancien profit
          if (valid)
          {
            int new_profit = calculer_profit(solution);
            if (new_profit > best_profit)
            {
              best_profit = new_profit;
              memcpy(best_solution, solution, n * sizeof(int));
            }
          }

          solution[i] = 1;
          solution[j] = 0;
        }
      }
    }
  }

  memcpy(solution, best_solution, n * sizeof(int));
  free(best_solution);
}

/*
 * Fonction : ls_1flip
 * ----------------
 * Fonction de recherche local (LS-1FLIP)
 *
 * Paramètres :
 *   - int * solution : Tableau de la solution
 *
 * Retour :
 *   - /
 */

void ls_1flip(int *solution)
{

  int best_profit = 0;
  best_profit = calculer_profit(solution);

  int *best_solution = (int *)malloc(n * sizeof(int));
  if (!best_solution)
  {
    printf("Erreur d'allocation mémoire pour best_solution\n");
    return;
  }
  memcpy(best_solution, solution, n * sizeof(int));

  for (int j = 0; j < n; j++)
  {
    solution[j] = 1 - solution[j]; // Flip l'objet

    // Vérifier si les contraintes
    int valid = 1;
    for (int i = 0; i < m; i++)
    {
      int totalPoids = 0;
      for (int k = 0; k < n; k++)
      {
        if (solution[k] == 1)
        {
          totalPoids += tab_m_poid[i][k];
        }
      }
      if (totalPoids > tab_m[i])
      {
        valid = 0;
        break;
      }
    }

    // Si validé alors calcul du nouveau profit et le comparer a l'ancien
    if (valid)
    {
      int new_profit = 0;
      for (int k = 0; k < n; k++)
      {
        if (solution[k] == 1)
        {
          new_profit += tab_coef[k];
        }
      }

      if (new_profit > best_profit)
      {
        best_profit = new_profit;
        memcpy(best_solution, solution, n * sizeof(int));
      }
    }
    solution[j] = 1 - solution[j];
  }

  memcpy(solution, best_solution, n * sizeof(int));
  free(best_solution);
}

/*
 * Fonction : vnd
 * ----------------
 * Fonction de descente de voisinage variable (VND)
 *
 * Paramètres :
 *   - int * solution : Tableau de la solution
 *   - double temps_max : Le temps du programme
 *
 * Retour :
 *   - /
 */

void vnd(int *solution, double temps_max)
{
  clock_t start_time = clock();
  int best_profit = calculer_profit(solution);
  int *best_solution = (int *)malloc(n * sizeof(int));
  if (!best_solution)
  {
    printf("Erreur d'allocation mémoire.\n");
    return;
  }
  memcpy(best_solution, solution, n * sizeof(int));

  // Boucle principale
  do
  {
    // Vérifier le temps écoulé si temps_max != 0
    if (temps_max != 0)
    {
      double elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
      if (elapsed_time >= temps_max)
      {
        printf("VND : Temps limite atteint (%.2f s), stop.\n", elapsed_time);
        break;
      }
    }

    // Appliquer 1-Flip
    ls_1flip(solution);
    int profit_1flip = calculer_profit(solution);

    if (profit_1flip <= best_profit)
    {
      // Si 1-Flip n'améliore pas, on tente Échange
      ls_echange(solution);
      int profit_echange = calculer_profit(solution);

      // Si Échange améliore, on met à jour la solution
      if (profit_echange > best_profit)
      {
        best_profit = profit_echange;
        memcpy(best_solution, solution, n * sizeof(int));
      }
    }
    else
    {
      // Si 1-Flip améliore, on met à jour directement
      best_profit = profit_1flip;
      memcpy(best_solution, solution, n * sizeof(int));
    }

  } while (temps_max != 0);

  memcpy(solution, best_solution, n * sizeof(int)); // Mise à jour finale
  free(best_solution);
}

/*
 * Fonction : perturber_solution
 * ----------------
 * Fonction permettant la pertubation d'une solution ici juste un random inversement de la présence
 *
 * Paramètres :
 *   - int *solution : Tableau de la solution
 *
 * Retour :
 *   - /
 */

void perturber_solution(int *solution)
{
  for (int i = 0; i < 3; i++)
  {
    int index = rand() % n;
    solution[index] = 1 - solution[index];
  }
}

/*
 * Fonction : vns
 * ----------------
 * Fonction de descente de voisinage variable (VNS)
 *
 * Paramètres :
 *   - int *solution : Tableau de la solution
 *   - double temps_max : Le temps du programme
 *
 * Retour :
 *   - /
 */

void vns(int *solution, double temps_max)
{
  clock_t start_time = clock();
  int best_profit = calculer_profit(solution);
  int *best_solution = (int *)malloc(n * sizeof(int));
  if (!best_solution)
  {
    printf("Erreur d'allocation mémoire.\n");
    return;
  }
  memcpy(best_solution, solution, n * sizeof(int));

  while (1)
  {
    double elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    if (elapsed_time >= temps_max)
    {
      printf("VNS : Temps limite atteint (%.2f s), arrêt.\n", elapsed_time);
      break;
    }

    // Perturbation de la solution actuelle
    perturber_solution(solution);

    // Vérification de faisabilité
    if (verif_gloutonne(solution) == 1)
    {

      vnd(solution, 0);
      // Vérifier si la solution améliorée est meilleure
      int new_profit = calculer_profit(solution);
      if (new_profit > best_profit)
      {
        best_profit = new_profit;
        memcpy(best_solution, solution, n * sizeof(int));
        // printf("VNS : Meilleure solution  (profit = %d)\n", best_profit);
      }
      else
      {
        memcpy(solution, best_solution, n * sizeof(int)); // Retour à la meilleure solution
      }
    }
    else
    {
      memcpy(solution, best_solution, n * sizeof(int)); // Retour à la meilleure solution
    }
  }

  memcpy(solution, best_solution, n * sizeof(int)); // Mise à jour finale
  free(best_solution);
}

/*
 * Fonction : gloutonneV1
 * ----------------
 * Fonction principal de la gloutonne
 *
 * Paramètres :
 *   -
 *
 * Retour :
 *   - /
 */

void gloutonneV1(char *output_file, char *algo, int exec_time)
{
  Objet objets[n];
  int *solution = (int *)malloc(n * sizeof(int));

  if (solution == NULL)
  {
    printf("Échec de l'allocation mémoire pour solution\n");
    return;
  }

  for (int i = 0; i < n; i++)
  {
    solution[i] = 0;
  }

  // Calcul du ratio + triage par coef / total poids
  for (int j = 0; j < n; j++)
  {
    double totalPoids = 0;
    for (int i = 0; i < m; i++)
    {
      totalPoids += tab_m_poid[i][j];
    }
    objets[j].index = j;
    objets[j].ratio = (double)tab_coef[j] / totalPoids;
  }
  qsort(objets, n, sizeof(Objet), comparer);

  // Calcul de la gloutonne en parcourant nos variable globale et vérifiant les contraintes
  for (int j = 0; j < n; j++)
  {
    int peutsolutionner = 1;
    for (int i = 0; i < m; i++)
    {
      int totalPoids = 0;
      for (int k = 0; k < n; k++)
      {
        if (solution[k] == 1)
        {
          totalPoids += tab_m_poid[i][k];
        }
      }
      if (totalPoids + tab_m_poid[i][objets[j].index] > tab_m[i])
      {
        peutsolutionner = 0; // impossible
        break;
      }
    }

    if (peutsolutionner)
    {
      solution[objets[j].index] = 1; // possible
    }
  }
  if (!(verif_gloutonne(solution)))
  {
    printf("Erreur de contrainte dans le sac à dos la gloutonne n'est pas bonne !");
    return;
  }

  if (strcmp(algo, "VNS") == 0)
  {
    vns(solution, exec_time);
  }
  else if (strcmp(algo, "VND") == 0)
  {
    vnd(solution, exec_time);
  }
  else
  {
    printf("Algorithme Inconnu erreur \n");
    return;
  }

  if (!(verif_gloutonne(solution)))
  {
    printf("Erreur de contrainte dans le sac à dos la gloutonne n'est pas bonne !");
    return;
  }
  write_data(output_file, solution);
  free(solution);
}

/*
 * Fonction : aleatoire
 * ----------------
 * Fonction aléatoire pour la création initial de la solution
 *
 * Paramètres :
 *   - /
 *
 * Retour :
 *   - /
 */

void aleatoire()
{

  Objet objets[n];
  int *solution = (int *)malloc(n * sizeof(int));

  if (solution == NULL)
  {
    printf("Échec de l'allocation mémoire pour solution\n");
    return;
  }
  for (int i = 0; i < n; i++)
  {
    solution[i] = 0;
  }

  // Calcul du ratio + triage par coef / total poids
  for (int j = 0; j < n; j++)
  {
    double totalPoids = 0;
    for (int i = 0; i < m; i++)
    {
      totalPoids += tab_m_poid[i][j];
    }
    objets[j].index = j;
    objets[j].ratio = (double)tab_coef[j] / totalPoids;
  }
  qsort(objets, n, sizeof(Objet), comparer);

  // Calcul de la gloutonne en parcourant nos variable globale et vérifiant les contraintes
  for (int j = 0; j < n; j++)
  {
    int tirage = rand() % 2; // 0 ou 1
    if (tirage == 1)
    {
      int peutsolutionner = 1;
      for (int i = 0; i < m; i++)
      {
        int totalPoids = 0;
        for (int k = 0; k < n; k++)
        {
          if (solution[k] == 1)
          {
            totalPoids += tab_m_poid[i][k];
          }
        }

        if (totalPoids + tab_m_poid[i][j] > tab_m[i])
        {
          peutsolutionner = 0;
          break;
        }
      }

      if (peutsolutionner)
      {
        solution[j] = 1;
      }
    }
  }
  if (!(verif_gloutonne(solution)))
  {
    printf("Erreur de contrainte dans le sac à dos la solution n'est pas bonne !");
    return;
  }
  vns(solution, 20);
  if (!(verif_gloutonne(solution)))
  {
    printf("Erreur de contrainte dans le sac à dos la solution n'est pas bonne !");
    return;
  }
  write_data("result.txt", solution);
  free(solution);
}