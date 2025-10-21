=========================================
       ExoJ1 - VERSION MODERNE C++
=========================================

DESCRIPTION:
Version HexView utilisant les classes et containers C++ modernes

FONCTIONNALITÉS C++ UTILISÉES:
- std::vector<>          : Container dynamique sécurisé
- std::ifstream/ofstream : Flux de fichiers C++
- std::string           : Chaînes de caractères sécurisées
- Classes et méthodes   : Programmation orientée objet
- Gestion automatique   : Pas de malloc/free manuel

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
./hexview.exe test_binary.bin dump_moderne.txt

AVANTAGES VERSION C++:
- Code plus sûr (pas de fuites mémoire)
- Gestion d'erreurs robuste
- Code plus maintenable
- Containers STL performants
- RAII (Resource Acquisition Is Initialization)