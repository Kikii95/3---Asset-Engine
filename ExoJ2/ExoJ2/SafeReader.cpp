/*===========================================================================
 * SafeReader - Lecteur de fichiers binaires sécurisé (Jour 2)
 * Version C classique pure : fopen, fread, malloc, free
 *
 * Format de fichier :
 *   [HEADER: 16 bytes]
 *     - magic     : 4 bytes ('S','A','F','E')
 *     - version   : 4 bytes (uint32_t)
 *     - dataSize  : 4 bytes (uint32_t)
 *     - checksum  : 4 bytes (uint32_t)
 *   [DATA: variable size]
 *
 * Checksum : Somme XOR de tous les octets de données
 *===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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
 * Calcul du checksum : XOR de tous les octets
 *===========================================================================*/
uint32_t CalculateChecksum(const unsigned char* data, size_t size) {
    uint32_t checksum = 0;
    for (size_t i = 0; i < size; i++) {
        checksum ^= data[i];
        checksum = (checksum << 1) | (checksum >> 31); /* Rotation */
    }
    return checksum;
}

/*===========================================================================
 * Lecture du header avec validation basique
 *===========================================================================*/
int ReadHeader(FILE* file, FileHeader* header) {
    size_t bytesRead = fread(header, 1, HEADER_SIZE, file);

    if (bytesRead != HEADER_SIZE) {
        printf("[ERREUR] Fichier trop court pour contenir un header valide\n");
        printf("         Attendu: %d bytes, Lu: %zu bytes\n", HEADER_SIZE, bytesRead);
        return 0;
    }

    return 1;
}

/*===========================================================================
 * Validation du header
 *===========================================================================*/
int ValidateHeader(const FileHeader* header, long fileSize) {
    /* Vérification du magic number */
    if (header->magic != MAGIC_SIGNATURE) {
        printf("[ERREUR] Signature invalide: 0x%08X (attendu: 0x%08X)\n",
               header->magic, MAGIC_SIGNATURE);
        return 0;
    }

    /* Vérification de la version */
    if (header->version != CURRENT_VERSION) {
        printf("[ERREUR] Version non supportée: %u (attendu: %u)\n",
               header->version, CURRENT_VERSION);
        return 0;
    }

    /* Vérification de la cohérence taille */
    long expectedFileSize = HEADER_SIZE + header->dataSize;
    if (fileSize != expectedFileSize) {
        printf("[ERREUR] Taille de fichier incohérente\n");
        printf("         Taille déclarée (header): %u bytes\n", header->dataSize);
        printf("         Taille réelle (fichier):  %ld bytes\n", fileSize - HEADER_SIZE);
        return 0;
    }

    return 1;
}

/*===========================================================================
 * Lecture et validation complète du fichier
 *===========================================================================*/
int LoadSafeFile(const char* filename, SafeFile* safeFile) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("[ERREUR] Impossible d'ouvrir le fichier: %s\n", filename);
        return 0;
    }

    /* Déterminer la taille du fichier */
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    printf("\n[INFO] Fichier: %s (%ld bytes)\n", filename, fileSize);

    /* Lire le header */
    if (!ReadHeader(file, &safeFile->header)) {
        fclose(file);
        return 0;
    }

    printf("[INFO] Header lu avec succès\n");
    printf("       Magic:    0x%08X\n", safeFile->header.magic);
    printf("       Version:  %u\n", safeFile->header.version);
    printf("       DataSize: %u bytes\n", safeFile->header.dataSize);
    printf("       Checksum: 0x%08X\n", safeFile->header.checksum);

    /* Valider le header */
    if (!ValidateHeader(&safeFile->header, fileSize)) {
        fclose(file);
        return 0;
    }

    /* Allouer mémoire pour les données */
    safeFile->data = (unsigned char*)malloc(safeFile->header.dataSize);
    if (safeFile->data == NULL) {
        printf("[ERREUR] Allocation mémoire échouée (%u bytes)\n",
               safeFile->header.dataSize);
        fclose(file);
        return 0;
    }

    /* Lire les données */
    size_t bytesRead = fread(safeFile->data, 1, safeFile->header.dataSize, file);
    fclose(file);

    if (bytesRead != safeFile->header.dataSize) {
        printf("[ERREUR] Lecture incomplète des données\n");
        printf("         Attendu: %u bytes, Lu: %zu bytes\n",
               safeFile->header.dataSize, bytesRead);
        free(safeFile->data);
        return 0;
    }

    /* Calculer et vérifier le checksum */
    uint32_t calculatedChecksum = CalculateChecksum(safeFile->data,
                                                     safeFile->header.dataSize);

    printf("[INFO] Vérification du checksum...\n");
    printf("       Checksum déclaré: 0x%08X\n", safeFile->header.checksum);
    printf("       Checksum calculé: 0x%08X\n", calculatedChecksum);

    if (calculatedChecksum != safeFile->header.checksum) {
        printf("[ERREUR] CHECKSUM INVALIDE - Fichier corrompu!\n");
        free(safeFile->data);
        return 0;
    }

    printf("[OK] Checksum valide\n");
    return 1;
}

/*===========================================================================
 * Affichage du contenu (si texte ASCII)
 *===========================================================================*/
void DisplayContent(const SafeFile* safeFile) {
    printf("\n========== CONTENU DU FICHIER ==========\n");

    for (uint32_t i = 0; i < safeFile->header.dataSize; i++) {
        unsigned char c = safeFile->data[i];
        if (c >= 32 && c < 127) {
            printf("%c", c);
        } else if (c == '\n') {
            printf("\n");
        } else {
            printf("[0x%02X]", c);
        }
    }

    printf("\n========================================\n");
}

/*===========================================================================
 * Libération mémoire
 *===========================================================================*/
void FreeSafeFile(SafeFile* safeFile) {
    if (safeFile->data != NULL) {
        free(safeFile->data);
        safeFile->data = NULL;
    }
}

/*===========================================================================
 * Usage
 *===========================================================================*/
void ShowUsage(const char* programName) {
    printf("========================================\n");
    printf("  SafeReader - Lecteur défensif v1.0\n");
    printf("========================================\n");
    printf("Usage: %s <fichier.safe>\n\n", programName);
    printf("Valide l'intégrité d'un fichier binaire sécurisé.\n");
    printf("Vérifie : signature, version, taille, checksum\n");
}

/*===========================================================================
 * MAIN
 *===========================================================================*/
int main(int argc, char* argv[]) {
    if (argc != 2) {
        ShowUsage(argv[0]);
        return 1;
    }

    SafeFile safeFile = {0};

    printf("╔══════════════════════════════════════╗\n");
    printf("║   SafeReader - Mode Défensif Actif   ║\n");
    printf("╚══════════════════════════════════════╝\n");

    if (LoadSafeFile(argv[1], &safeFile)) {
        printf("\n✓ FICHIER VALIDE\n");
        printf("  Toutes les vérifications ont réussi.\n");

        DisplayContent(&safeFile);

        FreeSafeFile(&safeFile);
        return 0;
    } else {
        printf("\n✗ FICHIER INVALIDE\n");
        printf("  Le fichier est corrompu ou invalide.\n");
        return 1;
    }
}
