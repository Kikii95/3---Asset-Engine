// HexView - Outil de visualisation et édition hexadécimale
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>

class HexView {
private:
    std::vector<unsigned char> data;
    std::string filename;

public:
    bool loadFile(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            std::cerr << "Erreur: Impossible d'ouvrir le fichier " << path << std::endl;
            return false;
        }

        filename = path;
        data.clear();
        
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        data.resize(fileSize);
        file.read(reinterpret_cast<char*>(data.data()), fileSize);
        file.close();
        
        std::cout << "Fichier chargé: " << path << " (" << fileSize << " octets)" << std::endl;
        return true;
    }

    void displayHexDump() {
        const size_t bytesPerLine = 16;
        
        for (size_t offset = 0; offset < data.size(); offset += bytesPerLine) {
            // Afficher l'offset
            std::cout << std::hex << std::setfill('0') << std::setw(8) << offset << "  ";
            
            // Afficher les octets en hexadécimal
            for (size_t i = 0; i < bytesPerLine; ++i) {
                if (offset + i < data.size()) {
                    std::cout << std::hex << std::setfill('0') << std::setw(2) 
                             << static_cast<unsigned int>(data[offset + i]) << " ";
                } else {
                    std::cout << "   ";
                }
                
                if (i == 7) std::cout << " ";  // Séparateur au milieu
            }
            
            std::cout << " |";
            
            // Afficher les caractères ASCII imprimables
            for (size_t i = 0; i < bytesPerLine && offset + i < data.size(); ++i) {
                char c = static_cast<char>(data[offset + i]);
                std::cout << (std::isprint(c) ? c : '.');
            }
            
            std::cout << "|" << std::endl;
        }
        std::cout << std::dec; // Retour en décimal
    }

    bool saveDumpToFile(const std::string& outputPath) {
        std::ofstream outFile(outputPath);
        if (!outFile) {
            std::cerr << "Erreur: Impossible de créer le fichier " << outputPath << std::endl;
            return false;
        }

        const size_t bytesPerLine = 16;
        
        for (size_t offset = 0; offset < data.size(); offset += bytesPerLine) {
            // Écrire l'offset
            outFile << std::hex << std::setfill('0') << std::setw(8) << offset << "  ";
            
            // Écrire les octets en hexadécimal
            for (size_t i = 0; i < bytesPerLine; ++i) {
                if (offset + i < data.size()) {
                    outFile << std::hex << std::setfill('0') << std::setw(2) 
                           << static_cast<unsigned int>(data[offset + i]) << " ";
                } else {
                    outFile << "   ";
                }
                
                if (i == 7) outFile << " ";
            }
            
            outFile << " |";
            
            // Écrire les caractères ASCII
            for (size_t i = 0; i < bytesPerLine && offset + i < data.size(); ++i) {
                char c = static_cast<char>(data[offset + i]);
                outFile << (std::isprint(c) ? c : '.');
            }
            
            outFile << "|" << std::endl;
        }
        
        outFile.close();
        std::cout << "Dump sauvegardé dans: " << outputPath << std::endl;
        return true;
    }

    bool modifyByteAtOffset(size_t offset, unsigned char newValue) {
        if (offset >= data.size()) {
            std::cerr << "Erreur: Offset " << offset << " hors limites (taille: " << data.size() << ")" << std::endl;
            return false;
        }
        
        unsigned char oldValue = data[offset];
        data[offset] = newValue;
        
        std::cout << "Offset 0x" << std::hex << offset << ": 0x" 
                  << std::setfill('0') << std::setw(2) << static_cast<unsigned int>(oldValue)
                  << " -> 0x" << std::setw(2) << static_cast<unsigned int>(newValue) << std::dec << std::endl;
        return true;
    }

    bool replaceBlock(size_t startOffset, const std::vector<unsigned char>& newData) {
        if (startOffset + newData.size() > data.size()) {
            std::cerr << "Erreur: Bloc dépasse la taille du fichier" << std::endl;
            return false;
        }
        
        for (size_t i = 0; i < newData.size(); ++i) {
            data[startOffset + i] = newData[i];
        }
        
        std::cout << "Bloc remplacé à l'offset 0x" << std::hex << startOffset 
                  << " (" << std::dec << newData.size() << " octets)" << std::endl;
        return true;
    }

    bool saveModifiedFile(const std::string& outputPath = "") {
        std::string savePath = outputPath.empty() ? filename : outputPath;
        
        std::ofstream file(savePath, std::ios::binary);
        if (!file) {
            std::cerr << "Erreur: Impossible de sauvegarder " << savePath << std::endl;
            return false;
        }
        
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        file.close();
        
        std::cout << "Fichier modifié sauvegardé: " << savePath << std::endl;
        return true;
    }
};

void showUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " <fichier_binaire> [fichier_dump.txt]" << std::endl;
    std::cout << "  <fichier_binaire>  : Fichier à analyser" << std::endl;
    std::cout << "  [fichier_dump.txt] : Fichier de sortie pour le dump (optionnel)" << std::endl;
    std::cout << std::endl;
    std::cout << "Commandes interactives:" << std::endl;
    std::cout << "  d           : Afficher le dump hexadécimal" << std::endl;
    std::cout << "  m <offset> <value> : Modifier un octet à l'offset donné" << std::endl;
    std::cout << "  s [fichier] : Sauvegarder les modifications" << std::endl;
    std::cout << "  q           : Quitter" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        showUsage(argv[0]);
        return 1;
    }

    HexView hexView;
    
    if (!hexView.loadFile(argv[1])) {
        return 1;
    }

    // Affichage automatique du dump
    hexView.displayHexDump();

    // Sauvegarde automatique si fichier de sortie spécifié
    if (argc == 3) {
        hexView.saveDumpToFile(argv[2]);
    }

    // Mode interactif
    std::cout << std::endl << "Mode interactif activé. Tapez 'q' pour quitter." << std::endl;
    std::string command;
    
    while (true) {
        std::cout << "hexview> ";
        std::getline(std::cin, command);
        
        if (command.empty()) continue;
        
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;
        
        if (cmd == "q" || cmd == "quit") {
            break;
        }
        else if (cmd == "d" || cmd == "dump") {
            hexView.displayHexDump();
        }
        else if (cmd == "m" || cmd == "modify") {
            size_t offset;
            unsigned int value;
            if (iss >> std::hex >> offset >> value) {
                hexView.modifyByteAtOffset(offset, static_cast<unsigned char>(value));
            } else {
                std::cout << "Usage: m <offset_hex> <valeur_hex>" << std::endl;
            }
        }
        else if (cmd == "s" || cmd == "save") {
            std::string filename;
            iss >> filename;
            hexView.saveModifiedFile(filename);
        }
        else {
            std::cout << "Commande inconnue. Tapez 'q' pour quitter." << std::endl;
        }
    }

    return 0;
}