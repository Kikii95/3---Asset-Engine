/*===========================================================================
 * JOUR 1 - HexView : Visualiseur hexadécimal de fichiers binaires
 * Version C classique pure (fopen, fread, malloc, free)
 *
 * Compilation: g++ -o hexview ExoJ1_HexView.cpp
 * Usage:       ./hexview fichier.bin
 *===========================================================================*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    unsigned char* data;
    size_t size;
    char filename[256];
} FileData;

/*===========================================================================
 * Charger un fichier binaire en mémoire
 *===========================================================================*/
int LoadFileData(const char* filename, FileData* fileData) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Erreur: Impossible d'ouvrir le fichier %s\n", filename);
        return 0;
    }

    /* Obtenir la taille du fichier */
    fseek(file, 0, SEEK_END);
    fileData->size = ftell(file);
    fseek(file, 0, SEEK_SET);

    /* Allouer la mémoire */
    fileData->data = (unsigned char*)malloc(fileData->size);
    if (fileData->data == NULL) {
        printf("Erreur: Pas assez de mémoire\n");
        fclose(file);
        return 0;
    }

    /* Lire le contenu */
    size_t bytesRead = fread(fileData->data, 1, fileData->size, file);
    if (bytesRead != fileData->size) {
        printf("Erreur de lecture du fichier\n");
        free(fileData->data);
        fclose(file);
        return 0;
    }

    strncpy(fileData->filename, filename, sizeof(fileData->filename) - 1);
    fileData->filename[sizeof(fileData->filename) - 1] = '\0';

    fclose(file);
    printf("Fichier chargé: %s (%zu octets)\n\n", filename, fileData->size);
    return 1;
}

/*===========================================================================
 * Afficher le dump hexadécimal (format classique)
 *===========================================================================*/
void DisplayHexDump(const FileData* fileData) {
    const size_t BYTES_PER_LINE = 16;

    printf("╔════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                            DUMP HEXADECIMAL                                ║\n");
    printf("╠════════════════════════════════════════════════════════════════════════════╣\n");

    for (size_t offset = 0; offset < fileData->size; offset += BYTES_PER_LINE) {
        /* Afficher l'offset */
        printf("%08zx  ", offset);

        /* Afficher les octets en hexadécimal */
        for (size_t i = 0; i < BYTES_PER_LINE; i++) {
            if (offset + i < fileData->size) {
                printf("%02x ", fileData->data[offset + i]);
            } else {
                printf("   ");
            }

            if (i == 7) printf(" ");  /* Séparateur au milieu */
        }

        printf(" |");

        /* Afficher la représentation ASCII */
        for (size_t i = 0; i < BYTES_PER_LINE && offset + i < fileData->size; i++) {
            unsigned char c = fileData->data[offset + i];
            printf("%c", isprint(c) ? c : '.');
        }

        printf("|\n");
    }

    printf("╚════════════════════════════════════════════════════════════════════════════╝\n");
}

/*===========================================================================
 * Modifier un octet à un offset spécifique
 *===========================================================================*/
int ModifyByteAtOffset(FileData* fileData, size_t offset, unsigned char newValue) {
    if (offset >= fileData->size) {
        printf("Erreur: Offset %zu hors limites (taille: %zu)\n", offset, fileData->size);
        return 0;
    }

    unsigned char oldValue = fileData->data[offset];
    fileData->data[offset] = newValue;

    printf("Modification: Offset 0x%zx: 0x%02x -> 0x%02x\n", offset, oldValue, newValue);
    return 1;
}

/*===========================================================================
 * Sauvegarder les modifications dans un nouveau fichier
 *===========================================================================*/
int SaveModifiedFile(const FileData* fileData, const char* outputPath) {
    FILE* file = fopen(outputPath, "wb");

    if (file == NULL) {
        printf("Erreur: Impossible de sauvegarder %s\n", outputPath);
        return 0;
    }

    size_t bytesWritten = fwrite(fileData->data, 1, fileData->size, file);
    fclose(file);

    if (bytesWritten != fileData->size) {
        printf("Erreur lors de l'écriture\n");
        return 0;
    }

    printf("Fichier sauvegardé: %s\n", outputPath);
    return 1;
}

/*===========================================================================
 * Libérer la mémoire
 *===========================================================================*/
void FreeFileData(FileData* fileData) {
    if (fileData->data != NULL) {
        free(fileData->data);
        fileData->data = NULL;
    }
    fileData->size = 0;
}

/*===========================================================================
 * Mode interactif
 *===========================================================================*/
void InteractiveMode(FileData* fileData) {
    char command[256];

    printf("\n╔════════════════════════════════════════╗\n");
    printf("║       MODE INTERACTIF ACTIVÉ           ║\n");
    printf("╠════════════════════════════════════════╣\n");
    printf("║ d           - Afficher le dump         ║\n");
    printf("║ m <off> <val> - Modifier octet (hex)   ║\n");
    printf("║ s <fichier> - Sauvegarder              ║\n");
    printf("║ q           - Quitter                  ║\n");
    printf("╚════════════════════════════════════════╝\n\n");

    while (1) {
        printf("hexview> ");

        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }

        command[strcspn(command, "\n")] = 0;

        if (strlen(command) == 0) continue;

        char cmd[10];
        sscanf(command, "%9s", cmd);

        if (strcmp(cmd, "q") == 0 || strcmp(cmd, "quit") == 0) {
            break;
        }
        else if (strcmp(cmd, "d") == 0 || strcmp(cmd, "dump") == 0) {
            DisplayHexDump(fileData);
        }
        else if (strcmp(cmd, "m") == 0 || strcmp(cmd, "modify") == 0) {
            size_t offset;
            unsigned int value;
            if (sscanf(command, "%*s %zx %x", &offset, &value) == 2) {
                ModifyByteAtOffset(fileData, offset, (unsigned char)value);
            } else {
                printf("Usage: m <offset_hex> <valeur_hex>\n");
                printf("Exemple: m A0 FF\n");
            }
        }
        else if (strcmp(cmd, "s") == 0 || strcmp(cmd, "save") == 0) {
            char filename[256];
            if (sscanf(command, "%*s %255s", filename) == 1) {
                SaveModifiedFile(fileData, filename);
            } else {
                printf("Usage: s <nom_fichier>\n");
            }
        }
        else {
            printf("Commande inconnue. Tapez 'q' pour quitter.\n");
        }
    }
}

/*===========================================================================
 * MAIN
 *===========================================================================*/
int main(int argc, char* argv[]) {
    printf("╔════════════════════════════════════════╗\n");
    printf("║     JOUR 1 - HexView Visualiseur      ║\n");
    printf("║           Version C Classique          ║\n");
    printf("╚════════════════════════════════════════╝\n\n");

    if (argc < 2) {
        printf("Usage: %s <fichier_binaire>\n", argv[0]);
        return 1;
    }

    FileData fileData = {0};

    if (!LoadFileData(argv[1], &fileData)) {
        return 1;
    }

    /* Affichage initial */
    DisplayHexDump(&fileData);

    /* Mode interactif */
    InteractiveMode(&fileData);

    /* Nettoyage */
    FreeFileData(&fileData);

    printf("\nAu revoir!\n");
    return 0;
}
