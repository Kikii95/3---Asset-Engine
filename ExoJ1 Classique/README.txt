=========================================
       ExoJ1 - VERSION CLASSIQUE C
=========================================

DESCRIPTION:
Version HexView utilisant les fonctions C classiques (fopen, fread, fwrite, malloc, free)

FONCTIONS C UTILISEES:
- fopen() / fclose()     : Ouverture/fermeture fichiers
- fread() / fwrite()     : Lecture/écriture binaire
- malloc() / free()      : Allocation/libération mémoire
- printf() / fprintf()   : Affichage formaté
- structures C           : typedef struct

COMPILATION:
g++ -std=c++17 -Wall -Wextra -O2 HexView.cpp -o hexview.exe

UTILISATION:
./hexview.exe fichier.bin [dump.txt]

COMMANDES INTERACTIVES:
d           - Afficher le dump hexadécimal
m A0 FF     - Modifier octet à offset 0xA0 avec valeur 0xFF
s [fichier] - Sauvegarder modifications
q           - Quitter

EXEMPLE:
./hexview.exe test_binary.bin dump_classique.txt

AVANTAGES VERSION C:
- Fonctions familières et simples
- Contrôle direct de la mémoire
- Portable sur tous les compilateurs C
- Apprentissage des bases système