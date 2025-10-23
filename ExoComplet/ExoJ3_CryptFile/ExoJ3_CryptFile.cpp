/*===========================================================================
 * JOUR 3 - CryptFile : Chiffrement/Déchiffrement de fichiers binaires
 * Version C classique pure avec menu intégré (encrypt, decrypt, test)
 *
 * Compilation: g++ -o cryptfile ExoJ3_CryptFile.cpp
 * Usage:       ./cryptfile
 *
 * Méthode: XOR avec rotation de bits + clé répétée
 * Opération réversible: encrypt(encrypt(data, key), key) = data
 *===========================================================================*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 8192  /* 8 Ko par bloc */

/*===========================================================================
 * Chiffrement d'un octet avec rotation de bits
 * Formule: ((byte << 3) | (byte >> 5)) ^ key
 * Cette opération est REVERSIBLE en appliquant la même fonction 2 fois
 *===========================================================================*/
unsigned char EncryptByte(unsigned char byte, unsigned char key) {
    /* Rotation de 3 bits à gauche */
    unsigned char rotated = (byte << 3) | (byte >> 5);
    /* XOR avec la clé */
    return rotated ^ key;
}

/*===========================================================================
 * Déchiffrement (identique au chiffrement car opération réversible)
 *===========================================================================*/
unsigned char DecryptByte(unsigned char byte, unsigned char key) {
    /* Le XOR est son propre inverse */
    unsigned char unxored = byte ^ key;
    /* Rotation inverse: 5 bits à gauche = 3 bits à droite */
    return (unxored << 5) | (unxored >> 3);
}

/*===========================================================================
 * Chiffrer un fichier avec une clé texte
 *===========================================================================*/
int EncryptFile(const char* inputPath, const char* outputPath, const char* key) {
    FILE* input = fopen(inputPath, "rb");
    if (input == NULL) {
        printf("[ERREUR] Impossible d'ouvrir: %s\n", inputPath);
        return 0;
    }

    FILE* output = fopen(outputPath, "wb");
    if (output == NULL) {
        printf("[ERREUR] Impossible de créer: %s\n", outputPath);
        fclose(input);
        return 0;
    }

    size_t keyLen = strlen(key);
    if (keyLen == 0) {
        printf("[ERREUR] Clé vide!\n");
        fclose(input);
        fclose(output);
        return 0;
    }

    printf("\n╔════════════════════════════════════════╗\n");
    printf("║         CHIFFREMENT EN COURS           ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("[INFO] Fichier source: %s\n", inputPath);
    printf("[INFO] Fichier crypté: %s\n", outputPath);
    printf("[INFO] Clé: %s (longueur: %zu)\n", key, keyLen);

    unsigned char buffer[BUFFER_SIZE];
    size_t totalBytes = 0;
    size_t keyIndex = 0;

    /* Lire et chiffrer par blocs */
    while (1) {
        size_t bytesRead = fread(buffer, 1, BUFFER_SIZE, input);
        if (bytesRead == 0) break;

        /* Chiffrer chaque octet */
        for (size_t i = 0; i < bytesRead; i++) {
            unsigned char keyByte = key[keyIndex % keyLen];
            buffer[i] = EncryptByte(buffer[i], keyByte);
            keyIndex++;
        }

        /* Écrire le bloc chiffré */
        fwrite(buffer, 1, bytesRead, output);
        totalBytes += bytesRead;

        /* Afficher progression */
        if (totalBytes % (BUFFER_SIZE * 10) == 0) {
            printf("  Chiffré: %zu Ko\n", totalBytes / 1024);
        }
    }

    fclose(input);
    fclose(output);

    printf("\n[OK] Chiffrement terminé: %zu bytes traités\n", totalBytes);
    return 1;
}

/*===========================================================================
 * Déchiffrer un fichier
 *===========================================================================*/
int DecryptFile(const char* inputPath, const char* outputPath, const char* key) {
    FILE* input = fopen(inputPath, "rb");
    if (input == NULL) {
        printf("[ERREUR] Impossible d'ouvrir: %s\n", inputPath);
        return 0;
    }

    FILE* output = fopen(outputPath, "wb");
    if (output == NULL) {
        printf("[ERREUR] Impossible de créer: %s\n", outputPath);
        fclose(input);
        return 0;
    }

    size_t keyLen = strlen(key);
    if (keyLen == 0) {
        printf("[ERREUR] Clé vide!\n");
        fclose(input);
        fclose(output);
        return 0;
    }

    printf("\n╔════════════════════════════════════════╗\n");
    printf("║        DÉCHIFFREMENT EN COURS          ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("[INFO] Fichier crypté: %s\n", inputPath);
    printf("[INFO] Fichier décrypté: %s\n", outputPath);
    printf("[INFO] Clé: %s (longueur: %zu)\n", key, keyLen);

    unsigned char buffer[BUFFER_SIZE];
    size_t totalBytes = 0;
    size_t keyIndex = 0;

    /* Lire et déchiffrer par blocs */
    while (1) {
        size_t bytesRead = fread(buffer, 1, BUFFER_SIZE, input);
        if (bytesRead == 0) break;

        /* Déchiffrer chaque octet */
        for (size_t i = 0; i < bytesRead; i++) {
            unsigned char keyByte = key[keyIndex % keyLen];
            buffer[i] = DecryptByte(buffer[i], keyByte);
            keyIndex++;
        }

        /* Écrire le bloc déchiffré */
        fwrite(buffer, 1, bytesRead, output);
        totalBytes += bytesRead;

        /* Afficher progression */
        if (totalBytes % (BUFFER_SIZE * 10) == 0) {
            printf("  Déchiffré: %zu Ko\n", totalBytes / 1024);
        }
    }

    fclose(input);
    fclose(output);

    printf("\n[OK] Déchiffrement terminé: %zu bytes traités\n", totalBytes);
    return 1;
}

/*===========================================================================
 * Tester la symétrie du chiffrement (encrypt + decrypt = original)
 *===========================================================================*/
int TestSymmetry(const char* originalPath, const char* key) {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║      TEST DE SYMÉTRIE EN COURS         ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("[INFO] Fichier original: %s\n", originalPath);
    printf("[INFO] Clé: %s\n\n", key);

    /* Étape 1: Chiffrer */
    printf("[ÉTAPE 1] Chiffrement...\n");
    if (!EncryptFile(originalPath, "test_encrypted.tmp", key)) {
        return 0;
    }

    /* Étape 2: Déchiffrer */
    printf("\n[ÉTAPE 2] Déchiffrement...\n");
    if (!DecryptFile("test_encrypted.tmp", "test_decrypted.tmp", key)) {
        return 0;
    }

    /* Étape 3: Comparer les fichiers */
    printf("\n[ÉTAPE 3] Vérification de l'identité...\n");

    FILE* original = fopen(originalPath, "rb");
    FILE* decrypted = fopen("test_decrypted.tmp", "rb");

    if (original == NULL || decrypted == NULL) {
        printf("[ERREUR] Impossible d'ouvrir les fichiers de comparaison\n");
        if (original) fclose(original);
        if (decrypted) fclose(decrypted);
        return 0;
    }

    /* Obtenir tailles */
    fseek(original, 0, SEEK_END);
    size_t origSize = ftell(original);
    fseek(original, 0, SEEK_SET);

    fseek(decrypted, 0, SEEK_END);
    size_t decrSize = ftell(decrypted);
    fseek(decrypted, 0, SEEK_SET);

    printf("  Taille originale: %zu bytes\n", origSize);
    printf("  Taille déchiffrée: %zu bytes\n", decrSize);

    if (origSize != decrSize) {
        printf("\n❌ ÉCHEC: Tailles différentes!\n");
        fclose(original);
        fclose(decrypted);
        return 0;
    }

    /* Comparer octet par octet */
    unsigned char bufferOrig[BUFFER_SIZE];
    unsigned char bufferDecr[BUFFER_SIZE];
    size_t totalChecked = 0;
    int identical = 1;

    while (totalChecked < origSize) {
        size_t toRead = (origSize - totalChecked > BUFFER_SIZE) ? BUFFER_SIZE : (origSize - totalChecked);

        fread(bufferOrig, 1, toRead, original);
        fread(bufferDecr, 1, toRead, decrypted);

        for (size_t i = 0; i < toRead; i++) {
            if (bufferOrig[i] != bufferDecr[i]) {
                printf("\n❌ DIFFÉRENCE à l'offset %zu: 0x%02X != 0x%02X\n",
                       totalChecked + i, bufferOrig[i], bufferDecr[i]);
                identical = 0;
                break;
            }
        }

        if (!identical) break;

        totalChecked += toRead;
    }

    fclose(original);
    fclose(decrypted);

    /* Nettoyer les fichiers temporaires */
    remove("test_encrypted.tmp");
    remove("test_decrypted.tmp");

    if (identical) {
        printf("\n✓ SUCCÈS: Fichiers identiques!\n");
        printf("  La symétrie du chiffrement est vérifiée.\n");
        printf("  Encrypt(Decrypt(data)) = data ✓\n");
        return 1;
    } else {
        printf("\n❌ ÉCHEC: Les fichiers diffèrent!\n");
        return 0;
    }
}

/*===========================================================================
 * Menu principal
 *===========================================================================*/
void ShowMenu() {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║      CryptFile - Menu Principal        ║\n");
    printf("╠════════════════════════════════════════╣\n");
    printf("║ 1. Chiffrer un fichier                 ║\n");
    printf("║ 2. Déchiffrer un fichier               ║\n");
    printf("║ 3. Tester la symétrie                  ║\n");
    printf("║ 4. Quitter                             ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("\nChoix: ");
}

/*===========================================================================
 * MAIN
 *===========================================================================*/
int main() {
    printf("╔════════════════════════════════════════╗\n");
    printf("║   JOUR 3 - CryptFile (C Classique)     ║\n");
    printf("║   Chiffrement XOR réversible           ║\n");
    printf("╚════════════════════════════════════════╝\n");

    char choice[10];
    char input[256];
    char output[256];
    char key[256];

    while (1) {
        ShowMenu();

        if (fgets(choice, sizeof(choice), stdin) == NULL) break;

        int option = atoi(choice);

        switch (option) {
            case 1:  /* Chiffrer */
                printf("\nFichier source: ");
                if (fgets(input, sizeof(input), stdin) == NULL) break;
                input[strcspn(input, "\n")] = 0;

                printf("Fichier de sortie (crypté): ");
                if (fgets(output, sizeof(output), stdin) == NULL) break;
                output[strcspn(output, "\n")] = 0;

                printf("Clé de chiffrement: ");
                if (fgets(key, sizeof(key), stdin) == NULL) break;
                key[strcspn(key, "\n")] = 0;

                EncryptFile(input, output, key);
                break;

            case 2:  /* Déchiffrer */
                printf("\nFichier crypté: ");
                if (fgets(input, sizeof(input), stdin) == NULL) break;
                input[strcspn(input, "\n")] = 0;

                printf("Fichier de sortie (décrypté): ");
                if (fgets(output, sizeof(output), stdin) == NULL) break;
                output[strcspn(output, "\n")] = 0;

                printf("Clé de déchiffrement: ");
                if (fgets(key, sizeof(key), stdin) == NULL) break;
                key[strcspn(key, "\n")] = 0;

                DecryptFile(input, output, key);
                break;

            case 3:  /* Tester symétrie */
                printf("\nFichier à tester: ");
                if (fgets(input, sizeof(input), stdin) == NULL) break;
                input[strcspn(input, "\n")] = 0;

                printf("Clé de test: ");
                if (fgets(key, sizeof(key), stdin) == NULL) break;
                key[strcspn(key, "\n")] = 0;

                TestSymmetry(input, key);
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
