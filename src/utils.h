#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

extern int n, m;
extern int *tab_coef;
extern int *tab_m;
extern int **tab_m_poid;

void free_tab();
void malloc_tab_m_poid();
void trim_spaces(char *str);
void insert_tab(char *myString, int count, int *tab);

#endif // UTILS_H