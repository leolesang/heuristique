#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

int n, m;
int *tab_coef;
int *tab_m;
int **tab_m_poid;

typedef struct
{
    int index;
    double ratio;
} Objet;

void free_tab()
{
    free(tab_coef);
    free(tab_m);

    if (tab_m_poid)
    {
        for (int i = 0; i < m; i++)
        {
            free(tab_m_poid[i]);
        }
        free(tab_m_poid);
    }
}

void malloc_tab_m_poid()
{
    tab_m_poid = malloc(m * sizeof(int *));
    if (!tab_m_poid)
    {
        printf("Erreur d'allocation mémoire pour tab_m_poid\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < m; i++)
    {
        tab_m_poid[i] = malloc(n * sizeof(int));
        if (!tab_m_poid[i])
        {
            printf("Erreur d'allocation mémoire ligne %d\n", i);
            exit(EXIT_FAILURE);
        }
    }
}

void trim_spaces(char *str)
{
    int i = 0, j = 0;
    int in_space = 0;

    while (str[i])
    {
        if (!isspace((unsigned char)str[i]) || (j > 0 && !in_space))
        {
            str[j++] = str[i];
        }
        in_space = isspace((unsigned char)str[i]);
        i++;
    }
    str[j] = '\0';
}

void insert_tab(char *myString, int count, int *tab)
{
    char *ptr = myString;
    for (int i = 0; i < count; i++)
    {
        sscanf(ptr, "%d", &tab[i]);
        while (*ptr && *ptr != ' ')
            ptr++;
        while (*ptr == ' ')
            ptr++;
    }
}

void read_file(char *file)
{
    FILE *fptr = fopen(file, "r");
    if (!fptr)
    {
        printf("Impossible d'ouvrir le fichier.\n");
        return;
    }

    char myString[256];
    int ligne = 0;

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
            exit(EXIT_FAILURE);
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
}

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

void print_objet(Objet o[])
{
    for (int i = 0; i < n; i++)
    {
        printf("\nObjet %d:\n", i);
        printf("InDEX: %d\n", o[i].index);
        printf("Ratio: %f\n", o[i].ratio);
    }
}

int comparer(const void *a, const void *b)
{
    Objet *objA = (Objet *)a;
    Objet *objB = (Objet *)b;
    return (objB->ratio - objA->ratio > 0) - (objB->ratio - objA->ratio < 0);
}

void verif_gloutonne(int *selection)
{

    printf("Objets sélectionnés (indice): ");
    int profitTotal = 0;
    for (int j = 0; j < n; j++)
    {
        if (selection[j] == 1)
        {
            printf("%d ", j);
            profitTotal += tab_coef[j];
        }
    }
    printf("\n");

    // verif des contraintes et du poids total
    for (int i = 0; i < m; i++)
    {
        int totalPoids = 0;
        for (int j = 0; j < n; j++)
        {
            if (selection[j] == 1)
            {
                totalPoids += tab_m_poid[i][j];
            }
        }
        printf("Poids total pour la contrainte %d: %d / %d\n", i, totalPoids, tab_m[i]);
        if (totalPoids > tab_m[i])
        {
            printf("Erreur: probleme de contrainte !\n", i);
        }
    }

    printf("Profit total: %d\n", profitTotal);
}

void gloutonneV1()
{
    Objet objets[n];
    int *selection = (int *)malloc(n * sizeof(int));

    if (selection == NULL)
    {
        printf("Échec de l'allocation mémoire pour selection\n");
        return;
    }

    for (int i = 0; i < n; i++)
    {
        selection[i] = 0;
    }

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

    for (int j = 0; j < n; j++)
    {
        int peutSelectionner = 1;
        for (int i = 0; i < m; i++)
        {
            int totalPoids = 0;
            for (int k = 0; k < n; k++)
            {
                if (selection[k] == 1)
                {
                    totalPoids += tab_m_poid[i][k];
                }
            }
            if (totalPoids + tab_m_poid[i][objets[j].index] > tab_m[i])
            {
                peutSelectionner = 0; // impossible
                break;
            }
        }

        if (peutSelectionner)
        {
            selection[objets[j].index] = 1; // possible
        }
    }
    verif_gloutonne(selection);
    free(selection);
}

void aleatoire()
{
    Objet objets[n];
    int *selection = (int *)malloc(n * sizeof(int));

    if (selection == NULL)
    {
        printf("Échec de l'allocation mémoire pour selection\n");
        return;
    }
    for (int i = 0; i < n; i++)
    {
        selection[i] = 0;
    }

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

    for (int j = 0; j < n; j++)
    {
        int tirage = rand() % 2; // 0 ou 1
        if (tirage == 1)
        {
            int peutSelectionner = 1;
            for (int i = 0; i < m; i++)
            {
                int totalPoids = 0;
                for (int k = 0; k < n; k++)
                {
                    if (selection[k] == 1)
                    {
                        totalPoids += tab_m_poid[i][k];
                    }
                }

                if (totalPoids + tab_m_poid[i][j] > tab_m[i])
                {
                    peutSelectionner = 0;
                    break;
                }
            }

            if (peutSelectionner)
            {
                selection[j] = 1;
            }
        }
    }
    verif_gloutonne(selection);
    free(selection);
}

int main()
{
    srand(time(NULL));
    read_file("Instances_MKP/100M5_21.txt");
    //  read_file("Instances_MKP/500M30_11.txt");
    // print_data();
    gloutonneV1();
    aleatoire();
    free_tab();
    return 0;
}
