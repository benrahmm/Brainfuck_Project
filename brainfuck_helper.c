#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


#define DATA_ARRAY_SIZE 32000


/**
 * @brief Structure de données pour représenter une boucle Brainfuck
 */
typedef struct Loop {
    char* start; // Pointeur vers le début de la boucle
    char* end;   // Pointeur vers la fin de la boucle
} Loop;

/**
 * @brief Structure de données pour représenter les boucles d'un programme Brainfuck
 */
typedef struct Loops {
    size_t count; // Nombre total de boucles
    Loop* array;  // Tableau de boucles
} Loops;


/**
 * @brief Récupère le programme Brainfuck à interpréter depuis un fichier
 *
 * @param input_filename le nom du fichier Brainfuck à récupérer
 * @return un tableau de caractère terminé par le caractère '\0' ou NULL si input_filename
 *         n'existe pas dans le répertoire courant
 */
char* get_input_prog(char* input_filename){
    FILE* file = fopen(input_filename, "r");
    if (file == NULL) {
        return NULL; // Le fichier n'existe pas
    }

    // Calculer la taille du fichier
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allouer de la mémoire pour stocker le contenu du fichier
    char* input_prog = (char*)malloc(size + 1); // +1 pour le caractère de fin de chaîne '\0'

    // Lire le contenu du fichier dans le tableau input_prog
    fread(input_prog, 1, size, file);
    input_prog[size] = '\0'; // Ajouter le caractère de fin de chaîne

    fclose(file);
    return input_prog;
}

/**
 * @brief Libère ce qui a été alloué par get_input_prog.
 *
 * @param loops
 */
void free_input_prog(char* input_prog){
    free(input_prog);
}

/**
 * @brief Analyse le programme Brainfuck passé en paramètre pour construire "quelque chose"
 *        qui représente les boucles du programme. Ce "quelque chose" sera ensuite utilisé
 *        lors de l'exécution des instructions, cf 'execute_instruction'.
 *        C'est à vous de décider ce que sera "quelque chose".
 *
 * @param input_prog le programme BrainFuck à analyser
 * @return "quelque chose" qui représente les boucles du programme
 */
Loops* build_loops(char* input_prog) {
    Loops* loops = (Loops*)malloc(sizeof(Loops));
    if (loops == NULL) {
        return NULL; // Échec de l'allocation de mémoire
    }

    loops->count = 0;
    loops->array = NULL;

    size_t input_prog_length = strlen(input_prog);
    char* stack = (char*)malloc(input_prog_length * sizeof(char)); // Utilisé comme pile pour gérer les boucles
    size_t stack_index = 0;

    // Parcours du programme Brainfuck pour détecter les boucles
    for (char* ip = input_prog; *ip != '\0'; ++ip) {
        if (*ip == '[') {
            // Empiler l'adresse de la boucle actuelle
            stack[stack_index++] = *ip;
        } else if (*ip == ']') {
            if (stack_index == 0) {
                // Erreur : boucle fermante sans boucle ouverte correspondante
                free(stack);
                free(loops);
                return NULL;
            }

            // Récupérer l'adresse de la boucle ouverte correspondante
            char* start = &stack[--stack_index];
            // Ajouter la boucle à la liste des boucles
            loops->array = (Loop*)realloc(loops->array, (loops->count + 1) * sizeof(Loop));
            loops->array[loops->count].start = start;
            loops->array[loops->count].end = ip;
            ++(loops->count);
        }
    }

    if (stack_index != 0) {
        // Erreur : des boucles ouvertes n'ont pas de boucles fermantes correspondantes
        free(stack);
        free(loops);
        return NULL;
    }

    free(stack);
    return loops;
}


/**
 * @brief Libère ce qui a été alloué par build_loops.
 *
 * @param loops
 */
void free_loops(void* loops){
    Loops* loops_ptr = (Loops*)loops;
    free(loops_ptr->array);
    free(loops);
}

/**
 * @brief Exécute l'instruction pointée par le pointeur pointé par ipp,
 *........et incrémente ce pointeur.
 *
 * @param ipp un pointeur vers le pointeur d'instructions
 * @param dpp un pointeur vers le pointeur de données
 * @param loops le "quelque chose" représentant les boucles, cf 'build_loops'.
 */
void execute_instruction(char** ipp, uint8_t** dpp, void* loops, bool* is_program_finished){
    (void)loops; 
    char* ip = *ipp;
    uint8_t* dp = *dpp;

    switch (*ip) {
        case '>':
            ++dp; // Incrémente le pointeur de données
            break;
        case '<':
            --dp; // Décrémente le pointeur de données
            break;
        case '+':
            ++(*dp); // Incrémente la valeur de la cellule de données pointée
            break;
        case '-':
            --(*dp); // Décrémente la valeur de la cellule de données pointée
            break;
        case '.':
            putchar(*dp); // Affiche le caractère ASCII correspondant à la valeur de la cellule de données pointée
            break;
        case ',':
            *dp = getchar(); // Enregistre la valeur ASCII du caractère saisi dans la cellule de données pointée
            break;
        case '[':
            // Si la valeur de la cellule de données pointée est 0, avance jusqu'à la prochaine instruction après le ']'
            if (*dp == 0) {
                int count = 1;
                while (count != 0) {
                    ++ip;
                    if (*ip == '[') {
                        ++count;
                    } else if (*ip == ']') {
                        --count;
                    }
                }
            }
            break;
        case ']':
            // Si la valeur de la cellule de données pointée est différente de 0, retourne à la précédente instruction après le '['
            if (*dp != 0) {
                int count = 1;
                while (count != 0) {
                    --ip;
                    if (*ip == ']') {
                        ++count;
                    } else if (*ip == '[') {
                        --count;
                    }
                }
            }
            break;
        default:
            // Les autres caractères sont considérés comme des commentaires et sont ignorés
            break;
    }

    // Incrémente le pointeur d'instruction pour passer à l'instruction suivante
    ++ip;
    *ipp = ip;
    *dpp = dp;
    
    // Vérifie si l'exécution du programme est terminée
    if (*ip == '\0') {
        *is_program_finished = true;
    }
}

