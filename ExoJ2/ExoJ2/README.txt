=========================================
    ExoJ2 - SafeReader - VERSION C CLASSIQUE
=========================================

DESCRIPTION:
Lecteur de fichiers binaires sécurisé avec validation complète:
- Signature magique ('SAFE')
- Version du format
- Taille déclarée vs réelle
- Checksum (XOR avec rotation)

Outils inclus:
1. SafeReader      : Validateur de fichiers .safe
2. FileGenerator   : Générateur de fichiers valides
3. FileCorruptor   : Outil de corruption pour tests

FONCTIONS C UTILISEES:
- fopen() / fclose()     : Ouverture/fermeture fichiers
- fread() / fwrite()     : Lecture/écriture binaire
- malloc() / free()      : Allocation/libération mémoire
- printf() / fprintf()   : Affichage formaté
- structures C           : typedef struct
- stdint.h               : uint32_t types

FORMAT DE FICHIER .safe:
┌─────────────────────────────────────┐
│ HEADER (16 bytes)                   │
│  - magic    : 4 bytes (0x45464153)  │
│  - version  : 4 bytes (1)           │
│  - dataSize : 4 bytes               │
│  - checksum : 4 bytes               │
├─────────────────────────────────────┤
│ DATA (variable size)                │
└─────────────────────────────────────┘

COMPILATION:
g++ -std=c++11 -Wall -Wextra -O2 SafeReader.cpp -o safereader.exe
g++ -std=c++11 -Wall -Wextra -O2 FileGenerator.cpp -o filegen.exe
g++ -std=c++11 -Wall -Wextra -O2 FileCorruptor.cpp -o corrupt.exe

UTILISATION:

1. Générer un fichier valide:
   ./filegen.exe test.safe "Bonjour, ceci est un test de SafeReader!"

2. Valider le fichier:
   ./safereader.exe test.safe
   => Doit afficher "FICHIER VALIDE"

3. Corrompre le fichier:
   ./corrupt.exe test.safe test_corrupt.safe 5
   => Mode 5 : Corruption d'1 octet de données

4. Valider le fichier corrompu:
   ./safereader.exe test_corrupt.safe
   => Doit afficher "FICHIER INVALIDE" + détails erreur

MODES DE CORRUPTION:
1 - Modifier le magic number
2 - Modifier la version
3 - Modifier la taille déclarée
4 - Modifier le checksum
5 - Corrompre 1 octet de données
6 - Tronquer le fichier

EXEMPLE COMPLET:
# Générer un fichier
./filegen.exe test.safe "Hello World from SafeReader!"

# Vérifier qu'il est valide
./safereader.exe test.safe

# Créer plusieurs corruptions
./corrupt.exe test.safe test_bad_magic.safe 1
./corrupt.exe test.safe test_bad_checksum.safe 4
./corrupt.exe test.safe test_bad_data.safe 5

# Tester que SafeReader détecte toutes les corruptions
./safereader.exe test_bad_magic.safe
./safereader.exe test_bad_checksum.safe
./safereader.exe test_bad_data.safe

OBJECTIFS PÉDAGOGIQUES ATTEINTS:
✓ Implémentation contrôle d'intégrité (checksum XOR + rotation)
✓ Gestion erreurs de lecture (EOF, tailles incohérentes, version)
✓ Simulation corruptions pour robustesse
✓ Logique de "lecture défensive" (ne jamais faire confiance au fichier)
✓ Validation header et champs de contrôle

AVANTAGES VERSION C:
- Fonctions familières et simples
- Contrôle direct de la mémoire
- Portable sur tous les compilateurs C
- Apprentissage des bases système
- Gestion manuelle erreurs
