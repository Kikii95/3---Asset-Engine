/*===========================================================================
 * FileCorruptor - Outil de corruption de fichiers pour tester SafeReader
 * Version C classique pure
 *
 * Modes de corruption:
 *   1 - Modifier le magic number
 *   2 - Modifier la version
 *   3 - Modifier la taille déclarée
 *   4 - Modifier le checksum
 *   5 - Corrompre 1 octet de données
 *   6 - Tronquer le fichier
 *===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define HEADER_SIZE 16

typedef struct {
    unsigned char* data;
    size_t size;
} FileData;

/*===========================================================================
 * Charger fichier en mémoire
 *===========================================================================*/
int LoadFile(const char* filename, FileData* fileData) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("[ERREUR] Impossible d'ouvrir: %s\n", filename);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    fileData->size = ftell(file);
    fseek(file, 0, SEEK_SET);

    fileData->data = (unsigned char*)malloc(fileData->size);
    if (fileData->data == NULL) {
        printf("[ERREUR] Allocation mémoire échouée\n");
        fclose(file);
        return 0;
    }

    size_t bytesRead = fread(fileData->data, 1, fileData->size, file);
    fclose(file);

    if (bytesRead != fileData->size) {
        printf("[ERREUR] Lecture incomplète\n");
        free(fileData->data);
        return 0;
    }

    return 1;
}

/*===========================================================================
 * Sauvegarder fichier
 *===========================================================================*/
int SaveFile(const char* filename, const FileData* fileData) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        printf("[ERREUR] Impossible de créer: %s\n", filename);
        return 0;
    }

    size_t written = fwrite(fileData->data, 1, fileData->size, file);
    fclose(file);

    if (written != fileData->size) {
        printf("[ERREUR] Écriture incomplète\n");
        return 0;
    }

    return 1;
}

/*===========================================================================
 * Corruption : modifier magic number
 *===========================================================================*/
void CorruptMagic(FileData* fileData) {
    if (fileData->size < 4) return;

    uint32_t* magic = (uint32_t*)fileData->data;
    *magic = 0xDEADBEEF;

    printf("[CORRUPTION] Magic number -> 0xDEADBEEF\n");
}

/*===========================================================================
 * Corruption : modifier version
 *===========================================================================*/
void CorruptVersion(FileData* fileData) {
    if (fileData->size < 8) return;

    uint32_t* version = (uint32_t*)(fileData->data + 4);
    *version = 999;

    printf("[CORRUPTION] Version -> 999\n");
}

/*===========================================================================
 * Corruption : modifier taille déclarée
 *===========================================================================*/
void CorruptSize(FileData* fileData) {
    if (fileData->size < 12) return;

    uint32_t* dataSize = (uint32_t*)(fileData->data + 8);
    *dataSize = 0xFFFFFFFF;

    printf("[CORRUPTION] DataSize -> 0xFFFFFFFF\n");
}

/*===========================================================================
 * Corruption : modifier checksum
 *===========================================================================*/
void CorruptChecksum(FileData* fileData) {
    if (fileData->size < 16) return;

    uint32_t* checksum = (uint32_t*)(fileData->data + 12);
    *checksum = 0x00000000;

    printf("[CORRUPTION] Checksum -> 0x00000000\n");
}

/*===========================================================================
 * Corruption : modifier 1 octet de données
 *===========================================================================*/
void CorruptData(FileData* fileData) {
    if (fileData->size <= HEADER_SIZE) return;

    srand((unsigned int)time(NULL));
    size_t offset = HEADER_SIZE + (rand() % (fileData->size - HEADER_SIZE));
    unsigned char oldValue = fileData->data[offset];
    unsigned char newValue = oldValue ^ 0xFF;
    fileData->data[offset] = newValue;

    printf("[CORRUPTION] Offset 0x%zx: 0x%02X -> 0x%02X\n",
           offset, oldValue, newValue);
}

/*===========================================================================
 * Corruption : tronquer le fichier
 *===========================================================================*/
void TruncateFile(FileData* fileData) {
    if (fileData->size <= HEADER_SIZE) return;

    size_t newSize = fileData->size / 2;
    fileData->size = newSize;

    printf("[CORRUPTION] Fichier tronqué à %zu bytes\n", newSize);
}

/*===========================================================================
 * Usage
 *===========================================================================*/
void ShowUsage(const char* programName) {
    printf("========================================\n");
    printf("  FileCorruptor - Test de robustesse\n");
    printf("========================================\n");
    printf("Usage: %s <input.safe> <output.safe> <mode>\n\n", programName);
    printf("Modes de corruption:\n");
    printf("  1 - Modifier le magic number\n");
    printf("  2 - Modifier la version\n");
    printf("  3 - Modifier la taille déclarée\n");
    printf("  4 - Modifier le checksum\n");
    printf("  5 - Corrompre 1 octet de données\n");
    printf("  6 - Tronquer le fichier\n");
}

/*===========================================================================
 * MAIN
 *===========================================================================*/
int main(int argc, char* argv[]) {
    if (argc != 4) {
        ShowUsage(argv[0]);
        return 1;
    }

    const char* inputPath = argv[1];
    const char* outputPath = argv[2];
    int mode = atoi(argv[3]);

    printf("╔══════════════════════════════════════╗\n");
    printf("║     FileCorruptor - Evil Mode        ║\n");
    printf("╚══════════════════════════════════════╝\n\n");

    FileData fileData = {0};

    if (!LoadFile(inputPath, &fileData)) {
        return 1;
    }

    printf("[INFO] Fichier chargé: %zu bytes\n", fileData.size);

    /* Appliquer la corruption */
    switch (mode) {
        case 1: CorruptMagic(&fileData); break;
        case 2: CorruptVersion(&fileData); break;
        case 3: CorruptSize(&fileData); break;
        case 4: CorruptChecksum(&fileData); break;
        case 5: CorruptData(&fileData); break;
        case 6: TruncateFile(&fileData); break;
        default:
            printf("[ERREUR] Mode invalide: %d\n", mode);
            free(fileData.data);
            return 1;
    }

    if (SaveFile(outputPath, &fileData)) {
        printf("\n✓ Fichier corrompu sauvegardé: %s\n", outputPath);
        free(fileData.data);
        return 0;
    } else {
        printf("\n✗ Échec de la sauvegarde\n");
        free(fileData.data);
        return 1;
    }
}
