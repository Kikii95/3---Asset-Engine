// Exo J1.cpp :
#include <stdlib.h>
#include <iostream>


FILE* OpenRead(const char* path)
{
    std::cout << "Ouverture d'un fichier...";
    FILE* file;
    errno_t fp = fopen_s(&file, path, "r");

    if (fp == NULL) 
    {
        std::cout << "Fichier inexistant.";
        return NULL;
    }

    return file;
}

FILE* OpenWrite(const char* path)
{
    FILE* file;

    if (OpenRead(path) == NULL)
    {
        std::cout << "Creation d'un nouveau fichier...";

        errno_t fp = fopen_s(&file, path, "w+");

        return file;
    }
    
    errno_t fp = fopen_s(&file, path, "r+");
    
    return file;
}

void Close(FILE* file) {
    fclose(file);
}

void Write(const char* text, FILE* file)
{
    size_t size = sizeof(char);
    size_t length = strlen(text);

    fwrite(text, size, length, file);
}

void Read(char* text, FILE* file)
{
    size_t size = sizeof(char);
    size_t length = strlen(text);

    fread(text, size, length, file);

}






int main()
{
    const char* path = "C:/Users/kabboud/Documents/GitHub/3---Asset-Engine/Exo J1/Test1";

    FILE* file = OpenWrite(path);

    const char* text1 = "sssssssssllllllllllllllangvk begnz,dl,,jnzdnnl ggg";
    const char* text2 = "445161 11464868461 56168484316818";

    Write(text1, file);
    Write(text2, file);

    Close(file);



}
