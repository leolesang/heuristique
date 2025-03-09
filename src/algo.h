#ifndef ALGO_H
#define ALGO_H

#include "utils.h"

typedef struct
{
    int index;
    double ratio;
} Objet;

void argGestion(int argc, char *argv[], char *input_file, int *exec_time, char *algo, char *output_file);
int comparer(const void *a, const void *b);
int calculer_profit(int *solution);
int verif_gloutonne(int *selection);
void ls_1flip(int *selection);
void ls_echange(int *selection);
void perturber_solution(int *solution);
void vnd(int *solution, double temps_max);
void vns(int *solution, double temps_max);
void gloutonneV1(char *output_file, char *algo, int exec_time);
void aleatoire();

#endif // ALGO_H