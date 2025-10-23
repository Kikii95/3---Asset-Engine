/*===========================================================================
 * FileGenerator - Génère des fichiers .safe valides pour tester SafeReader
 * Version C classique pure
 *===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAGIC_SIGNATURE 0x45464153  /* 'SAFE' en little-endian */
#define CURRENT_VERSION 1

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t dataSize;
    uint32_t checksum;
} FileHeader;

/*===========================================================================
 * Calcul du checksum (identique à SafeReader)
 *===========================================================================*/
uint32_t CalculateChecksum(const unsigned char* data, size_t size) {
    uint32_t checksum = 0;
    for (size_t i = 0; i < size; i++) {
        checksum ^= data[i];
        checksum = (checksum << 1) | (checksum >> 31);
    }
    return checksum;
}

/*===========================================================================
 * Génération d'un fichier .safe
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

    /* Écrire le header */
    size_t written = fwrite(&header, 1, sizeof(FileHeader), file);
    if (written != sizeof(FileHeader)) {
        printf("[ERREUR] Échec écriture header\n");
        fclose(file);
        return 0;
    }

    /* Écrire les données */
    written = fwrite(content, 1, contentLen, file);
    if (written != contentLen) {
        printf("[ERREUR] Échec écriture données\n");
        fclose(file);
        return 0;
    }

    fclose(file);

    printf("[OK] Fichier généré: %s\n", outputPath);
    printf("     Magic:    0x%08X\n", header.magic);
    printf("     Version:  %u\n", header.version);
    printf("     DataSize: %u bytes\n", header.dataSize);
    printf("     Checksum: 0x%08X\n", header.checksum);

    return 1;
}

/*===========================================================================
 * Usage
 *===========================================================================*/
void ShowUsage(const char* programName) {
    printf("========================================\n");
    printf("  FileGenerator - Création fichiers .safe\n");
    printf("========================================\n");
    printf("Usage: %s <output.safe> \"<contenu>\"\n\n", programName);
    printf("Génère un fichier .safe valide avec le contenu spécifié.\n");
}

/*===========================================================================
 * MAIN
 *===========================================================================*/
int main(int argc, char* argv[]) {
    if (argc != 3) {
        ShowUsage(argv[0]);
        return 1;
    }

    const char* outputPath = argv[1];
    const char* content = argv[2];

    printf("╔══════════════════════════════════════╗\n");
    printf("║      FileGenerator - SAFE v1.0       ║\n");
    printf("╚══════════════════════════════════════╝\n\n");

    if (GenerateSafeFile(outputPath, content)) {
        printf("\n✓ Génération réussie\n");
        return 0;
    } else {
        printf("\n✗ Génération échouée\n");
        return 1;
    }
}
