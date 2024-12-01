#include "client.h"
#include "ip.h"

using std::cerr, std::endl, std::cout, std::cin, std::string;


int main() {
    const int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        cerr << "Socket creation error. ";
        return -1;
    }
    struct sockaddr_in server = {};
    server.sin_family = AF_INET;
    server.sin_port = htons(7432);
    if (inet_pton(AF_INET, IP, &server.sin_addr) <= 0) {
        cerr << "Invalid address/ Address not supported. \n";
        return -1;
    }
    if (connect(sock, reinterpret_cast<sockaddr *>(&server), sizeof(struct sockaddr_in)) < 0) {
        cerr << "Connection Failed. \n";
        return -1;
    }
    cout << "Client connected. \n";
    string buffer;
    char buff[1024];
    bool isDisconnect = false;
    while (!isDisconnect) {
        cout << "Enter query: \n";
        getline(cin, buffer);
        if (buffer == "disconnect") {
            isDisconnect = true;
            send(sock, buffer.c_str(), buffer.size() - 1, 0);
            continue;
        }
        send(sock, buffer.c_str(), buffer.size(), 0);
        auto recvLen = recv(sock, &buff, sizeof buff, 0);
        if (recvLen == -1) {
            cerr << "read failed. \n";
            continue;
        }
        if (recvLen == 0) {
            cerr << "EOF occured. \n";
            continue;
        }
        buff[recvLen] = '\0';
        cout << buff << endl;
    }
    return 0;
}