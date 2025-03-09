/*
 * Projet : Méta-heuristique sur SADM
 * Date   : 08/03/2025
 * Description : test.c programme permettant de tester le fichier result pour confirmer la validation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "io.h"
#include "algo.h"

/*
 * Fonction : test
 * ----------------
 * Fonction permettant de re-vérifier la solution d'un fichier resultat
 *
 * Paramètres :
 *   - /
 *
 * Retour :
 *   - /
 */

void test(char *instance, char *result) {
    read_file(instance);
    FILE *file = fopen(result, "r");

    int profit_attendu = 0;
    int nb_objet = 0;
    int *solution_attendu;

    if (!file) {
        printf("Erreur ouverture fichier result.txt\n");
        return;
    }

    fscanf(file, "%d %d", &profit_attendu, &nb_objet);

    solution_attendu = (int *)calloc(n, sizeof(int));
    if (!solution_attendu) {
        printf("Erreur allocation mémoire\n");
        fclose(file);
        return;
    }

    char ligne[4096];
    fgets(ligne, sizeof(ligne), file);
    fgets(ligne, sizeof(ligne), file); // Lire la deuxième ligne

    // Extraire les indices de la chaîne
    char *token = strtok(ligne, " ");
    while (token != NULL) {
        int index = atoi(token);
        if (index >= 0 && index < n) { 
            solution_attendu[index] = 1;
        }
        token = strtok(NULL, " ");
    }

    if (!(verif_gloutonne(solution_attendu))) {
        printf("Erreur de contrainte dans le sac à dos la gloutonne n'est pas bonne !");
        return;
    }
    printf("Pas de probleme ! La solution est bien correct le profit est bien de [%d]", calculer_profit(solution_attendu));

    free(solution_attendu); 
    fclose(file);
}
