// C++ program to illustrate the client application in the
// socket programming
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cerr << "Error: Expected at least 1 argument, recieved " << argc - 1 << " ." << std::endl;
        return -1;
    }

    // creating socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    // specifying address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    // Convert IPv4 and IPv6 addresses from text to binary
    // To send data to a different server, just modify the IP adrees
    // with the one from the server. 
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    // sending connection request
    while(connect(clientSocket, (struct sockaddr*)&serverAddress,
            sizeof(serverAddress)) == -1) {}

    // sending data
    for(int i = 1; i < argc; i++) {
        const char* message = argv[i];
        int sent = send(clientSocket, message, strlen(message), 0);

        if (sent < 0) {
            std::cout << "Error sending" << std::endl;
        } else {
            std::cout << "Sent " << sent << " bytes." << std::endl;
        }
    }

    // closing socket
    close(clientSocket);

    return 0;
}