#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    unsigned char* data;
    size_t size;
    char filename[256];
} FileData;

FILE* OpenBinaryRead(const char* path) {
    FILE* file = fopen(path, "rb"); 
    
    if (file == NULL) {
        printf("Erreur: Impossible d'ouvrir le fichier %s\n", path);
        return NULL;
    }
    
    return file;
}

FILE* OpenWrite(const char* path) {
    FILE* file = fopen(path, "w");
    
    if (file == NULL) {
        printf("Erreur: Impossible de créer le fichier %s\n", path);
        return NULL;
    }
    
    return file;
}

void CloseFile(FILE* file) {
    if (file != NULL) {
        fclose(file);
    }
}

int LoadFileData(const char* filename, FileData* fileData) {
    FILE* file = OpenBinaryRead(filename);
    if (file == NULL) return 0;
    
    fseek(file, 0, SEEK_END);
    fileData->size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    fileData->data = (unsigned char*)malloc(fileData->size);
    if (fileData->data == NULL) {
        printf("Erreur: Pas assez de mémoire\n");
        CloseFile(file);
        return 0;
    }
    
    size_t bytesRead = fread(fileData->data, 1, fileData->size, file);
    if (bytesRead != fileData->size) {
        printf("Erreur de lecture du fichier\n");
        free(fileData->data);
        CloseFile(file);
        return 0;
    }
    
    strncpy(fileData->filename, filename, sizeof(fileData->filename) - 1);
    fileData->filename[sizeof(fileData->filename) - 1] = '\0';
    
    CloseFile(file);
    printf("Fichier chargé: %s (%zu octets)\n", filename, fileData->size);
    return 1;
}

void DisplayHexDump(const FileData* fileData) {
    const size_t BYTES_PER_LINE = 16;
    
    for (size_t offset = 0; offset < fileData->size; offset += BYTES_PER_LINE) {
        printf("%08zx  ", offset);
        
        for (size_t i = 0; i < BYTES_PER_LINE; i++) {
            if (offset + i < fileData->size) {
                printf("%02x ", fileData->data[offset + i]);
            } else {
                printf("   "); 
            }
            
            if (i == 7) printf(" ");
        }
        
        printf(" |");
        
        for (size_t i = 0; i < BYTES_PER_LINE && offset + i < fileData->size; i++) {
            unsigned char c = fileData->data[offset + i];
            printf("%c", isprint(c) ? c : '.');
        }
        
        printf("|\n");
    }
}

int SaveDumpToFile(const FileData* fileData, const char* outputPath) {
    FILE* outFile = OpenWrite(outputPath);
    if (outFile == NULL) return 0;
    
    const size_t BYTES_PER_LINE = 16;
    
    for (size_t offset = 0; offset < fileData->size; offset += BYTES_PER_LINE) {
        fprintf(outFile, "%08zx  ", offset);
        
        for (size_t i = 0; i < BYTES_PER_LINE; i++) {
            if (offset + i < fileData->size) {
                fprintf(outFile, "%02x ", fileData->data[offset + i]);
            } else {
                fprintf(outFile, "   ");
            }
            
            if (i == 7) fprintf(outFile, " ");
        }
        
        fprintf(outFile, " |");
        
        for (size_t i = 0; i < BYTES_PER_LINE && offset + i < fileData->size; i++) {
            unsigned char c = fileData->data[offset + i];
            fprintf(outFile, "%c", isprint(c) ? c : '.');
        }
        
        fprintf(outFile, "|\n");
    }
    
    CloseFile(outFile);
    printf("Dump sauvegardé dans: %s\n", outputPath);
    return 1;
}

int ModifyByteAtOffset(FileData* fileData, size_t offset, unsigned char newValue) {
    if (offset >= fileData->size) {
        printf("Erreur: Offset %zu hors limites (taille: %zu)\n", offset, fileData->size);
        return 0;
    }
    
    unsigned char oldValue = fileData->data[offset];
    fileData->data[offset] = newValue;
    
    printf("Offset 0x%zx: 0x%02x -> 0x%02x\n", offset, oldValue, newValue);
    return 1;
}

int SaveModifiedFile(const FileData* fileData, const char* outputPath) {
    FILE* file = fopen(outputPath, "wb");  
    
    if (file == NULL) {
        printf("Erreur: Impossible de sauvegarder %s\n", outputPath);
        return 0;
    }
    
    size_t bytesWritten = fwrite(fileData->data, 1, fileData->size, file);
    CloseFile(file);
    
    if (bytesWritten != fileData->size) {
        printf("Erreur lors de l'écriture\n");
        return 0;
    }
    
    printf("Fichier modifié sauvegardé: %s\n", outputPath);
    return 1;
}

void FreeFileData(FileData* fileData) {
    if (fileData->data != NULL) {
        free(fileData->data);
        fileData->data = NULL;
    }
    fileData->size = 0;
}

void ShowUsage(const char* programName) {
    printf("Usage: %s <fichier_binaire> [fichier_dump.txt]\n", programName);
    printf("  <fichier_binaire>  : Fichier à analyser\n");
    printf("  [fichier_dump.txt] : Fichier de sortie pour le dump (optionnel)\n");
    printf("\nCommandes interactives:\n");
    printf("  d           : Afficher le dump hexadécimal\n");
    printf("  m <offset> <value> : Modifier un octet (en hexadécimal)\n");
    printf("  s [fichier] : Sauvegarder les modifications\n");
    printf("  q           : Quitter\n");
}

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        ShowUsage(argv[0]);
        return 1;
    }

    FileData fileData = {0}; 
    
    if (!LoadFileData(argv[1], &fileData)) {
        return 1;
    }

    DisplayHexDump(&fileData);

    if (argc == 3) {
        SaveDumpToFile(&fileData, argv[2]);
    }

    printf("\nMode interactif activé. Tapez 'q' pour quitter.\n");
    char command[256];
    
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
            DisplayHexDump(&fileData);
        }
        else if (strcmp(cmd, "m") == 0 || strcmp(cmd, "modify") == 0) {
            size_t offset;
            unsigned int value;
            if (sscanf(command, "%*s %zx %x", &offset, &value) == 2) {
                ModifyByteAtOffset(&fileData, offset, (unsigned char)value);
            } else {
                printf("Usage: m <offset_hex> <valeur_hex>\n");
                printf("Exemple: m A0 FF\n");
            }
        }
        else if (strcmp(cmd, "s") == 0 || strcmp(cmd, "save") == 0) {
            char filename[256];
            if (sscanf(command, "%*s %255s", filename) == 1) {
                SaveModifiedFile(&fileData, filename);
            } else {
                SaveModifiedFile(&fileData, fileData.filename);
            }
        }
        else {
            printf("Commande inconnue. Tapez 'q' pour quitter.\n");
        }
    }

    FreeFileData(&fileData);

    return 0;
}