╔══════════════════════════════════════════════════════════════════════════╗
║                   EXERCICES COMPLETS - ASSET ENGINE                      ║
║                         Version C Classique Pure                         ║
╚══════════════════════════════════════════════════════════════════════════╝

Ce dossier contient 3 exercices autonomes pour apprendre la manipulation
de fichiers binaires en C classique (fopen, fread, malloc, free).

Chaque fichier .cpp est COMPLET et AUTONOME avec menu intégré.
Pas besoin de fichiers séparés, tout est dans un seul programme.

════════════════════════════════════════════════════════════════════════════

📦 JOUR 1 - ExoJ1_HexView.cpp
════════════════════════════════════════════════════════════════════════════

OBJECTIF:
  Visualiseur hexadécimal de fichiers binaires avec mode interactif.
  Permet de voir le contenu brut d'un fichier et de le modifier.

COMPILATION:
  g++ -o hexview ExoJ1_HexView.cpp

UTILISATION:
  ./hexview fichier.bin

EXEMPLE COMPLET:

  1. Créer un fichier de test:
     echo "Bonjour! Ceci est un test binaire." > test.txt

  2. Lancer HexView:
     ./hexview test.txt

  3. Commandes disponibles:
     hexview> d              # Afficher le dump hexadécimal
     hexview> m 0 FF         # Modifier octet à offset 0x00 avec 0xFF
     hexview> s modified.bin # Sauvegarder les modifications
     hexview> q              # Quitter

FONCTIONNALITÉS:
  ✓ Chargement fichier en mémoire
  ✓ Affichage hexadécimal + ASCII
  ✓ Modification interactive d'octets
  ✓ Sauvegarde des modifications

════════════════════════════════════════════════════════════════════════════

🔒 JOUR 2 - ExoJ2_SafeReader.cpp
════════════════════════════════════════════════════════════════════════════

OBJECTIF:
  Lecteur de fichiers sécurisé avec validation d'intégrité.
  Génère, valide et corrompt des fichiers au format .safe

FORMAT FICHIER .safe:
  ┌─────────────────────────────────┐
  │ HEADER (16 bytes)               │
  │  - magic    : 0x45464153        │
  │  - version  : 1                 │
  │  - dataSize : taille données    │
  │  - checksum : XOR avec rotation │
  ├─────────────────────────────────┤
  │ DATA (variable)                 │
  └─────────────────────────────────┘

COMPILATION:
  g++ -o safereader ExoJ2_SafeReader.cpp

UTILISATION:
  ./safereader

SCÉNARIO DE TEST COMPLET:

  1. Lancer le programme:
     ./safereader

  2. Générer un fichier valide:
     Choix: 1
     Nom du fichier de sortie: test.safe
     Contenu du fichier: Bonjour, ceci est un test de SafeReader!

  3. Valider le fichier:
     Choix: 2
     Nom du fichier à valider: test.safe
     => Doit afficher "FICHIER VALIDE" avec le contenu

  4. Corrompre le fichier:
     Choix: 3
     Fichier source: test.safe
     Fichier de sortie: test_bad.safe
     Mode: 4                         # Corrompre le checksum

  5. Valider le fichier corrompu:
     Choix: 2
     Nom du fichier à valider: test_bad.safe
     => Doit détecter "CHECKSUM INVALIDE"

  6. Tester tous les modes de corruption:
     Mode 1 - Magic number invalide
     Mode 2 - Version non supportée
     Mode 3 - Taille incohérente
     Mode 4 - Checksum incorrect
     Mode 5 - Données corrompues

FONCTIONNALITÉS:
  ✓ Génération fichiers .safe valides
  ✓ Validation complète (magic, version, taille, checksum)
  ✓ Détection de corruptions
  ✓ Affichage détaillé des erreurs
  ✓ Lecture défensive (ne fait jamais confiance au fichier)

════════════════════════════════════════════════════════════════════════════

🔐 JOUR 3 - ExoJ3_CryptFile.cpp
════════════════════════════════════════════════════════════════════════════

OBJECTIF:
  Chiffrement/Déchiffrement réversible de fichiers binaires.
  Utilise XOR + rotation de bits avec clé texte.

MÉTHODE:
  - Chiffrement: ((byte << 3) | (byte >> 5)) ^ key
  - Déchiffrement: inverse de la rotation puis XOR
  - La clé texte est répétée sur toute la longueur du fichier

COMPILATION:
  g++ -o cryptfile ExoJ3_CryptFile.cpp

UTILISATION:
  ./cryptfile

SCÉNARIO DE TEST COMPLET:

  1. Créer un fichier de test:
     echo "Ceci est un message secret à chiffrer!" > secret.txt

  2. Lancer CryptFile:
     ./cryptfile

  3. Chiffrer le fichier:
     Choix: 1
     Fichier source: secret.txt
     Fichier de sortie (crypté): secret.enc
     Clé de chiffrement: motdepasse123

  4. Vérifier que le fichier est illisible:
     ./hexview secret.enc
     => Le contenu doit être incompréhensible

  5. Déchiffrer le fichier:
     Choix: 2
     Fichier crypté: secret.enc
     Fichier de sortie (décrypté): secret_decrypted.txt
     Clé de déchiffrement: motdepasse123

  6. Vérifier le résultat:
     cat secret_decrypted.txt
     => Doit afficher le message original

  7. Tester la symétrie automatiquement:
     Choix: 3
     Fichier à tester: secret.txt
     Clé de test: motdepasse123
     => Doit confirmer que encrypt(decrypt(data)) = data

  8. Tester avec une mauvaise clé:
     Choix: 2
     Fichier crypté: secret.enc
     Fichier de sortie (décrypté): wrong.txt
     Clé de déchiffrement: mauvaiseClé
     => Le fichier sera déchiffré mais illisible

FONCTIONNALITÉS:
  ✓ Chiffrement par blocs de 8 Ko
  ✓ Clé texte répétée
  ✓ Opération réversible (symétrique)
  ✓ Test automatique de symétrie
  ✓ Support de fichiers de toute taille
  ✓ Affichage de progression

════════════════════════════════════════════════════════════════════════════

🎯 TESTS AVANCÉS
════════════════════════════════════════════════════════════════════════════

TEST 1: Combiner HexView + SafeReader
--------------------------------------
1. Générer un fichier .safe avec SafeReader
2. L'ouvrir avec HexView pour voir la structure binaire
3. Identifier manuellement: magic, version, taille, checksum
4. Modifier 1 octet avec HexView
5. Valider avec SafeReader => doit détecter la corruption

TEST 2: Combiner SafeReader + CryptFile
-----------------------------------------
1. Générer un fichier .safe
2. Le chiffrer avec CryptFile
3. Essayer de le valider avec SafeReader => doit échouer
4. Déchiffrer le fichier
5. Le valider avec SafeReader => doit réussir

TEST 3: Chiffrer un exécutable
--------------------------------
1. Copier un .exe: cp hexview.exe backup.exe
2. Chiffrer: ./cryptfile
   - Fichier source: backup.exe
   - Clé: secret123
3. Essayer de lancer le .exe chiffré => ne fonctionnera pas
4. Déchiffrer avec la même clé
5. Comparer les hashs pour vérifier l'identité

════════════════════════════════════════════════════════════════════════════

🧠 CONCEPTS CLÉS À COMPRENDRE
════════════════════════════════════════════════════════════════════════════

1. GESTION MÉMOIRE MANUELLE:
   - malloc() / free() : toujours libérer ce qu'on alloue
   - Vérifier les retours NULL
   - Éviter les fuites mémoire

2. LECTURE/ÉCRITURE BINAIRE:
   - "rb" / "wb" : modes binaires obligatoires
   - fread() / fwrite() : retournent le nombre d'octets lus/écrits
   - Toujours vérifier que nbBytes == expected

3. ENDIANNESS:
   - Les uint32_t sont stockés en little-endian sur x86
   - 0x45464153 = "SAFE" en mémoire
   - Important pour la portabilité

4. CHECKSUM/HACHAGE:
   - XOR simple : rapide mais faible
   - Rotation : ajoute de la "diffusion"
   - Pour production: utiliser CRC32 ou SHA256

5. CHIFFREMENT SYMÉTRIQUE:
   - XOR : opération réversible (A ^ B ^ B = A)
   - Rotation : ajoute de la complexité
   - Pour production: utiliser AES-256

════════════════════════════════════════════════════════════════════════════

📚 RESSOURCES POUR ALLER PLUS LOIN
════════════════════════════════════════════════════════════════════════════

1. Améliorer le checksum:
   - Implémenter CRC32
   - Ajouter un HMAC pour l'authentification

2. Améliorer le chiffrement:
   - Utiliser une bibliothèque OpenSSL
   - Implémenter AES ou ChaCha20
   - Ajouter un IV (Initialization Vector)

3. Ajouter de la compression:
   - Intégrer zlib
   - Compresser avant de chiffrer

4. Format de fichier avancé:
   - Ajouter des métadonnées (date, auteur)
   - Support de fichiers multiples (archive)
   - Chiffrement sélectif par sections

════════════════════════════════════════════════════════════════════════════

✅ CHECKLIST D'APPRENTISSAGE
════════════════════════════════════════════════════════════════════════════

JOUR 1 - HexView:
[ ] Je comprends comment charger un fichier en mémoire
[ ] Je sais afficher des octets en hexadécimal
[ ] Je peux modifier la mémoire et sauvegarder
[ ] Je comprends la différence entre représentation hex et ASCII

JOUR 2 - SafeReader:
[ ] Je comprends la structure d'un header binaire
[ ] Je sais calculer et vérifier un checksum
[ ] Je pratique la "lecture défensive" (ne jamais faire confiance)
[ ] Je détecte les corruptions de fichiers

JOUR 3 - CryptFile:
[ ] Je comprends le XOR et ses propriétés
[ ] Je sais implémenter une rotation de bits
[ ] Je comprends la différence encodage/hachage/chiffrement
[ ] Je vérifie la symétrie d'un algorithme

════════════════════════════════════════════════════════════════════════════

🚀 BONNE CHANCE DANS TON APPRENTISSAGE!
════════════════════════════════════════════════════════════════════════════

Ces exercices sont conçus pour être étudiés, modifiés et expérimentés.
N'hésite pas à:
  - Ajouter des printf() pour comprendre le flow
  - Modifier les algorithmes
  - Casser volontairement le code pour voir ce qui se passe
  - Créer tes propres variantes

Le meilleur moyen d'apprendre, c'est de faire!

════════════════════════════════════════════════════════════════════════════
