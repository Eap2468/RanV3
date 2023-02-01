#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>

void decryptFile(std::string path, int key)
{
    std::fstream file(path, std::ios::in | std::ios::binary);
    std::fstream newFile(path.substr(0, path.length() - 4), std::ios::out | std::ios::binary);

    file.seekg(0, file.end);
    int size = file.tellg();
    file.seekg(0, file.beg);

    char buffer[size];
    file.read(buffer, size);

    for(int i = 0; i < size; i++)
    {
        buffer[i] -= key / 10;
    }

    newFile.write(buffer, size);

    file.close();
    newFile.close();

    remove(path.c_str());
}

void decryptDir(std::string path, int key)
{
    for(auto file : std::filesystem::directory_iterator(path))
    {
        if(file.is_directory())
        {
            decryptDir(file.path(), key);
        }
        else
        {
            decryptFile(file.path(), key);
        }
    }
}

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        std::cout << "First arg: Directory Path, Second arg: Key" << std::endl;
        return 0;
    }
    std::string path = argv[1];
    std::string number = argv[2];
    int key = std::stoi(number);

    decryptDir(path, key);
    return 0;
}