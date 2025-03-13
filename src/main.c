/*
 * Projet : Méta-heuristique sur SADM
 * Date   : 08/03/2025
 * Description : main.c programme principal
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"
#include "io.h"
#include "algo.h"
#include "test.h"

/* Définition des variables globale */
int n, m;
int *tab_coef;
int *tab_m;
int **tab_m_poid;

int main(int argc, char *argv[])
{
    char input_file[256];
    int exec_time;
    char algo[10];
    char output_file[256];
    srand(time(NULL));

    argGestion(argc, argv, input_file, &exec_time, algo, output_file);

    if (!read_file(input_file))
    {
        exit(EXIT_FAILURE);
    }
    if (strcmp(algo, "STAT") == 0) {
        gloutonneV1(input_file, algo, exec_time);
    } else {
        gloutonneV1(output_file, algo, exec_time);
    }

    // aleatoire();
    // test("../Instances_MKP/100M5_1.txt","result.txt");

    free_tab();
    return 0;
}