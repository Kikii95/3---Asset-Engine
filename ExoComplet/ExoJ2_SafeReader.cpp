/*===========================================================================
 * JOUR 2 - SafeReader : Lecteur de fichiers binaires sécurisé
 * Version C classique pure avec menu intégré (générer, valider, corrompre)
 *
 * Compilation: g++ -o safereader ExoJ2_SafeReader.cpp
 * Usage:       ./safereader
 *
 * Format fichier .safe:
 *   HEADER (16 bytes): magic(4) + version(4) + dataSize(4) + checksum(4)
 *   DATA (variable)
 *===========================================================================*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define MAGIC_SIGNATURE 0x45464153  /* 'SAFE' en little-endian */
#define CURRENT_VERSION 1
#define HEADER_SIZE 16

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t dataSize;
    uint32_t checksum;
} FileHeader;

typedef struct {
    FileHeader header;
    unsigned char* data;
} SafeFile;

/*===========================================================================
 * Calcul du checksum : XOR avec rotation
 *===========================================================================*/
uint32_t CalculateChecksum(const unsigned char* data, size_t size) {
    uint32_t checksum = 0;
    for (size_t i = 0; i < size; i++) {
        checksum ^= data[i];
        checksum = (checksum << 1) | (checksum >> 31);  /* Rotation gauche */
    }
    return checksum;
}

/*===========================================================================
 * FONCTION 1 : Générer un fichier .safe valide
 *===========================================================================*/
int GenerateSafeFile(const char* outputPath, const char* content) {
    FILE* file = fopen(outputPath, "wb");
    if (file == NULL) {
        printf("[ERREUR] Impossible de créer: %s\n", outputPath);
        return 0;
    }

    size_t contentLen = strlen(content);

    /* Préparer le header */
    FileHeader header;
    header.magic = MAGIC_SIGNATURE;
    header.version = CURRENT_VERSION;
    header.dataSize = (uint32_t)contentLen;
    header.checksum = CalculateChecksum((unsigned char*)content, contentLen);

    /* Écrire header + données */
    fwrite(&header, 1, sizeof(FileHeader), file);
    fwrite(content, 1, contentLen, file);
    fclose(file);

    printf("\n[OK] Fichier généré: %s\n", outputPath);
    printf("     Magic:    0x%08X\n", header.magic);
    printf("     Version:  %u\n", header.version);
    printf("     DataSize: %u bytes\n", header.dataSize);
    printf("     Checksum: 0x%08X\n", header.checksum);

    return 1;
}

/*===========================================================================
 * FONCTION 2 : Valider et lire un fichier .safe
 *===========================================================================*/
int ValidateSafeFile(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("[ERREUR] Impossible d'ouvrir: %s\n", filename);
        return 0;
    }

    /* Obtenir taille du fichier */
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    printf("\n╔════════════════════════════════════════╗\n");
    printf("║   VALIDATION DU FICHIER .safe          ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("[INFO] Fichier: %s (%ld bytes)\n", filename, fileSize);

    /* Lire le header */
    FileHeader header;
    if (fread(&header, 1, HEADER_SIZE, file) != HEADER_SIZE) {
        printf("[ERREUR] Fichier trop court pour contenir un header\n");
        fclose(file);
        return 0;
    }

    printf("\n[HEADER]\n");
    printf("  Magic:    0x%08X ", header.magic);
    if (header.magic != MAGIC_SIGNATURE) {
        printf("❌ (attendu: 0x%08X)\n", MAGIC_SIGNATURE);
        fclose(file);
        return 0;
    }
    printf("✓\n");

    printf("  Version:  %u ", header.version);
    if (header.version != CURRENT_VERSION) {
        printf("❌ (attendu: %u)\n", CURRENT_VERSION);
        fclose(file);
        return 0;
    }
    printf("✓\n");

    printf("  DataSize: %u bytes ", header.dataSize);
    long expectedSize = HEADER_SIZE + header.dataSize;
    if (fileSize != expectedSize) {
        printf("❌ (fichier réel: %ld bytes)\n", fileSize);
        fclose(file);
        return 0;
    }
    printf("✓\n");

    /* Lire et vérifier les données */
    unsigned char* data = (unsigned char*)malloc(header.dataSize);
    if (data == NULL) {
        printf("[ERREUR] Allocation mémoire échouée\n");
        fclose(file);
        return 0;
    }

    if (fread(data, 1, header.dataSize, file) != header.dataSize) {
        printf("[ERREUR] Lecture données incomplète\n");
        free(data);
        fclose(file);
        return 0;
    }

    fclose(file);

    uint32_t calculatedChecksum = CalculateChecksum(data, header.dataSize);
    printf("  Checksum: 0x%08X (calculé: 0x%08X) ", header.checksum, calculatedChecksum);

    if (calculatedChecksum != header.checksum) {
        printf("❌ CORROMPU!\n");
        free(data);
        return 0;
    }
    printf("✓\n");

    /* Afficher le contenu */
    printf("\n[CONTENU]\n");
    printf("╭────────────────────────────────────────╮\n");
    for (uint32_t i = 0; i < header.dataSize; i++) {
        if (data[i] >= 32 && data[i] < 127) {
            printf("%c", data[i]);
        } else if (data[i] == '\n') {
            printf("\n");
        } else {
            printf("[0x%02X]", data[i]);
        }
    }
    printf("\n╰────────────────────────────────────────╯\n");

    free(data);

    printf("\n✓ FICHIER VALIDE - Toutes les vérifications ont réussi!\n");
    return 1;
}

/*===========================================================================
 * FONCTION 3 : Corrompre un fichier .safe
 *===========================================================================*/
int CorruptSafeFile(const char* inputPath, const char* outputPath, int mode) {
    FILE* file = fopen(inputPath, "rb");
    if (file == NULL) {
        printf("[ERREUR] Impossible d'ouvrir: %s\n", inputPath);
        return 0;
    }

    /* Charger le fichier complet */
    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char* data = (unsigned char*)malloc(fileSize);
    if (data == NULL) {
        printf("[ERREUR] Allocation mémoire\n");
        fclose(file);
        return 0;
    }

    fread(data, 1, fileSize, file);
    fclose(file);

    printf("\n[CORRUPTION MODE %d]\n", mode);

    /* Appliquer la corruption selon le mode */
    switch (mode) {
        case 1:  /* Modifier magic */
            if (fileSize >= 4) {
                uint32_t* magic = (uint32_t*)data;
                *magic = 0xDEADBEEF;
                printf("  Magic number -> 0xDEADBEEF\n");
            }
            break;

        case 2:  /* Modifier version */
            if (fileSize >= 8) {
                uint32_t* version = (uint32_t*)(data + 4);
                *version = 999;
                printf("  Version -> 999\n");
            }
            break;

        case 3:  /* Modifier taille */
            if (fileSize >= 12) {
                uint32_t* dataSize = (uint32_t*)(data + 8);
                *dataSize = 0xFFFFFFFF;
                printf("  DataSize -> 0xFFFFFFFF\n");
            }
            break;

        case 4:  /* Modifier checksum */
            if (fileSize >= 16) {
                uint32_t* checksum = (uint32_t*)(data + 12);
                *checksum = 0x00000000;
                printf("  Checksum -> 0x00000000\n");
            }
            break;

        case 5:  /* Corrompre données */
            if (fileSize > HEADER_SIZE) {
                srand((unsigned int)time(NULL));
                size_t offset = HEADER_SIZE + (rand() % (fileSize - HEADER_SIZE));
                data[offset] ^= 0xFF;
                printf("  Octet corrompu à offset 0x%zx\n", offset);
            }
            break;

        default:
            printf("  Mode invalide\n");
            free(data);
            return 0;
    }

    /* Sauvegarder le fichier corrompu */
    file = fopen(outputPath, "wb");
    if (file == NULL) {
        printf("[ERREUR] Impossible de créer: %s\n", outputPath);
        free(data);
        return 0;
    }

    fwrite(data, 1, fileSize, file);
    fclose(file);
    free(data);

    printf("\n[OK] Fichier corrompu sauvegardé: %s\n", outputPath);
    return 1;
}

/*===========================================================================
 * Menu principal
 *===========================================================================*/
void ShowMenu() {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║      SafeReader - Menu Principal       ║\n");
    printf("╠════════════════════════════════════════╣\n");
    printf("║ 1. Générer fichier .safe               ║\n");
    printf("║ 2. Valider fichier .safe               ║\n");
    printf("║ 3. Corrompre fichier .safe             ║\n");
    printf("║ 4. Quitter                             ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("\nChoix: ");
}

/*===========================================================================
 * MAIN
 *===========================================================================*/
int main() {
    printf("╔════════════════════════════════════════╗\n");
    printf("║   JOUR 2 - SafeReader (C Classique)    ║\n");
    printf("║   Lecture défensive de fichiers        ║\n");
    printf("╚════════════════════════════════════════╝\n");

    char choice[10];
    char filename[256];
    char content[1024];
    char output[256];
    int mode;

    while (1) {
        ShowMenu();

        if (fgets(choice, sizeof(choice), stdin) == NULL) break;

        int option = atoi(choice);

        switch (option) {
            case 1:  /* Générer */
                printf("\nNom du fichier de sortie: ");
                if (fgets(filename, sizeof(filename), stdin) == NULL) break;
                filename[strcspn(filename, "\n")] = 0;

                printf("Contenu du fichier: ");
                if (fgets(content, sizeof(content), stdin) == NULL) break;
                content[strcspn(content, "\n")] = 0;

                GenerateSafeFile(filename, content);
                break;

            case 2:  /* Valider */
                printf("\nNom du fichier à valider: ");
                if (fgets(filename, sizeof(filename), stdin) == NULL) break;
                filename[strcspn(filename, "\n")] = 0;

                ValidateSafeFile(filename);
                break;

            case 3:  /* Corrompre */
                printf("\nFichier source: ");
                if (fgets(filename, sizeof(filename), stdin) == NULL) break;
                filename[strcspn(filename, "\n")] = 0;

                printf("Fichier de sortie: ");
                if (fgets(output, sizeof(output), stdin) == NULL) break;
                output[strcspn(output, "\n")] = 0;

                printf("\nModes de corruption:\n");
                printf("  1 - Modifier magic\n");
                printf("  2 - Modifier version\n");
                printf("  3 - Modifier taille\n");
                printf("  4 - Modifier checksum\n");
                printf("  5 - Corrompre données\n");
                printf("Mode: ");

                if (fgets(choice, sizeof(choice), stdin) == NULL) break;
                mode = atoi(choice);

                CorruptSafeFile(filename, output, mode);
                break;

            case 4:  /* Quitter */
                printf("\nAu revoir!\n");
                return 0;

            default:
                printf("\nOption invalide!\n");
        }
    }

    return 0;
}
