#!/bin/bash
#═══════════════════════════════════════════════════════════════════════════
# Script de compilation rapide pour tous les exercices
#═══════════════════════════════════════════════════════════════════════════

echo "╔══════════════════════════════════════════════════════════════════════════╗"
echo "║              COMPILATION DES EXERCICES ASSET ENGINE                      ║"
echo "╚══════════════════════════════════════════════════════════════════════════╝"
echo ""

# Couleurs pour le terminal
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Fonction de compilation
compile() {
    local source=$1
    local output=$2
    local name=$3

    echo -n "[COMPILATION] $name ... "

    if g++ -std=c++11 -Wall -Wextra -O2 "$source" -o "$output" 2>&1 | grep -i "error"; then
        echo -e "${RED}ÉCHEC${NC}"
        return 1
    else
        echo -e "${GREEN}OK${NC}"
        return 0
    fi
}

# Compilation de chaque exercice
compile "ExoJ1_HexView.cpp" "hexview.exe" "Jour 1 - HexView"
compile "ExoJ2_SafeReader.cpp" "safereader.exe" "Jour 2 - SafeReader"
compile "ExoJ3_CryptFile.cpp" "cryptfile.exe" "Jour 3 - CryptFile"

echo ""
echo "╔══════════════════════════════════════════════════════════════════════════╗"
echo "║                        COMPILATION TERMINÉE                              ║"
echo "╚══════════════════════════════════════════════════════════════════════════╝"
echo ""
echo "Exécutables générés:"
echo "  ./hexview.exe      - Visualiseur hexadécimal"
echo "  ./safereader.exe   - Lecteur de fichiers sécurisé"
echo "  ./cryptfile.exe    - Chiffrement/Déchiffrement"
echo ""
echo "Lire README.txt pour les instructions d'utilisation complètes."
