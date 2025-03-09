#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

int n, m;
int *tab_coef;
int *tab_m;
int **tab_m_poid;
int stdout_copy;
int best_profit = 0; // Variable pour stocker le meilleur profit
volatile sig_atomic_t stop_program = 0; // Variable pour indiquer l'arrêt du programme
char output_file[50];
clock_t start_time = 0;
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

// Fonction pour afficher l'aide
void printHelp()
{
    printf("Utilisation : a.exe [input_file] [exec_time] [output_file]\n");
    printf("Arguments (optionnels) :\n");
    printf("  input_file :      Chemin vers le fichier d'entrée (par défaut : Instances_MKP/100M5_1.txt)\n");
    printf("  exec_time :       Temps en secondes (par défaut : 30)\n");
    printf("  output_file :     Chemin vers le fichier de sortie (par défaut : aucun, sortie standard)\n");
}

// Fonction pour gérer les arguments
void argGestion(int argc, char *argv[], char *input_file, double *exec_time, char *output_file)
{
    char *endptr;

    printf("argc = %d\n", argc);
    printf("argv[1] = %s\n", argv[1]);
    printf("argv[2] = %s\n", argv[2]);
    printf("argv[3] = %s\n", argv[3]);

    switch(argc){
        case 1:
            strcpy(input_file, "Instances_MKP/100M5_1.txt");
            *exec_time = 30;
            strcpy(output_file, ""); // Pas de fichier de sortie
            break;
        case 2:
            strncpy(input_file, argv[1], strlen(argv[1]));
            *exec_time = 30;
            strcpy(output_file, ""); // Pas de fichier de sortie
            break;
        case 3:       
            strncpy(input_file, argv[1], strlen(argv[1]));
            // Vérification et assignation de exec_time
            *exec_time = strtod(argv[2], &endptr);
            if (endptr == argv[2] || *endptr != '\0' || exec_time <= 0) {
                printf("Erreur : L'argument 'exec_time' doit être un nombre positif.\n");
                printHelp();
                return;
            }
            strcpy(output_file, ""); // Pas de fichier de sortie
            break;
        case 4:
            strncpy(input_file, argv[1], strlen(argv[1]));
            // Vérification et assignation de exec_time
            *exec_time = strtod(argv[2], &endptr);
            if (endptr == argv[2] || *endptr != '\0' || exec_time <= 0) {
                printf("Erreur : L'argument 'exec_time' doit être un nombre positif.\n");
                printHelp();
                return;
            }
            strncpy(output_file, argv[3], strlen(argv[3]));
            break;
        default:
            printf("Erreur : Nombre incorrect d'arguments.\n");
            printHelp();
            return;
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

FILE *write_file(char *file)
{
    FILE *fptr = fopen(file, "a");
    if (!fptr)
    {
        printf("Impossible d'ouvrir le fichier \"%s\". Veuillez vérifier que ce dernier existe\n", file);
        return NULL;
    }
    return fptr;
}

void read_file(char *file)
{
    FILE *fptr = fopen(file, "r");
    if (!fptr)
    {
        printf("Impossible d'ouvrir le fichier \"%s\". Veuillez vérifier que ce dernier existe\n", file);
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
        printf("INDEX: %d\n", o[i].index);
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

    // Mettre à jour le meilleur profit
    if (profitTotal > best_profit)
    {
        best_profit = profitTotal;
    }
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

void redirect_stdout_to_file(char *file)
{
    stdout_copy = dup(STDOUT_FILENO); // Sauvegarder stdout
    freopen(file, "a", stdout);
}

void restore_stdout()
{
    fflush(stdout); // Vider le buffer de sortie
    dup2(stdout_copy, STDOUT_FILENO); // Restaurer stdout
    close(stdout_copy); // Fermer le descripteur de fichier sauvegardé
}

void signal_handler(int signum)
{
    stop_program = 1;
    printf("\nArrêt du programme demandé. Nettoyage des ressources...\n");
    free_tab();
    if (stdout_copy != 0)
    {
        restore_stdout();
    }
    if (strlen(output_file) > 0)
    {
        FILE *output = write_file(output_file);
        if (output)
        {
            fprintf(output, "\n******************\n");
            fprintf(output, "Exécution interrompue !\n");
            fprintf(output, "Temps d'exécution : %.2f secondes\n", (double)(clock() - start_time) / CLOCKS_PER_SEC);
            fprintf(output, "Meilleur profit total : %d\n", best_profit);
            fprintf(output, "******************\n");
            fclose(output);
        }
    }
    printf("\n******************\n");
    printf("Exécution interrompue !\n");
    printf("Temps d'exécution : %.2f secondes\n", (double)(clock() - start_time) / CLOCKS_PER_SEC);
    printf("Meilleur profit total : %d\n", best_profit);
    printf("******************\n");
    exit(signum);
}

int main(int argc, char *argv[])
{   
    char input_file[50];
    double exec_time = 0;
    FILE *output;
    double final_exec_time;

    argGestion(argc, argv, input_file, &exec_time, output_file);

    // Installer le gestionnaire de signaux
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Affichage des valeurs assignées
    printf("******************\n");
    printf("Fichier d'entrée : %s\n", input_file);
    printf("Temps : %.2f secondes\n", exec_time);
    if (strlen(output_file) > 0)
    {
        output = write_file(output_file); // Ouvrir le fichier de sortie
        fprintf(output, "******************\n");
        fprintf(output, "Fichier d'entrée : %s\n", input_file);
        fprintf(output, "Temps : %.2f secondes\n", exec_time);
        fprintf(output, "******************\n\n");
        printf("Fichier de sortie : %s\n", output_file);
        printf("******************\n\n");
        printf("Ecriture des résultats dans le fichier de sortie...\n");
        fclose(output); // Fermer le fichier
        redirect_stdout_to_file(output_file); // Rediriger stdout vers le fichier de sortie
    }
    else
    {
        printf("******************\n\n");
    }
    

    srand(time(NULL));
    read_file(input_file);
    //  read_file("Instances_MKP/500M30_11.txt");
    // print_data();

    // Mesurer le temps d'exécution
    start_time = clock();

    // Exécuter les algorithmes
    while (!stop_program && (double)(clock() - start_time) / CLOCKS_PER_SEC < exec_time)
    {
        gloutonneV1();
        aleatoire();
    }

    final_exec_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    free_tab();

    if (strlen(output_file) > 0)
    {
        output = write_file(output_file); // Ouvrir le fichier de sortie
        restore_stdout(); // Restaurer stdout pour écrire dans la console
        printf("\n******************\n");
        printf("Exécution terminée !\n");
        printf("Les résultats ont été enregistrés dans le fichier \"%s\"\n", output_file);
        fprintf(output, "\n******************\n");
        fprintf(output, "Exécution terminée !\n");
        fprintf(output, "Temps d'exécution : %.2f secondes\n", final_exec_time);
        fprintf(output, "Meilleur profit total : %d\n", best_profit);
        fprintf(output, "******************\n");
        fclose(output); // Fermer le fichier
    }
    else
    {
        printf("\n******************\n");
        printf("Exécution terminée !\n");
    }
    printf("Temps d'exécution : %.2f secondes\n", final_exec_time);
    printf("Meilleur profit total : %d\n", best_profit);
    printf("******************\n");

    return 0;
}