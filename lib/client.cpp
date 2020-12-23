//
// Created by llincyaw on 2020/12/22.
//

#include "client.h"
bool Client::init(const string& addr, int _port) {
    if (sock == -1) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            cout << "Could not create socket" << endl;
        }
    }
    if (inet_addr(addr.c_str()) == -1) {
        struct hostent *he;
        struct in_addr **addr_list;
        if ((he = gethostbyname(addr.c_str())) == nullptr) {
            herror("get host by name");
            cout << "Failed to resolve hostname\n";
            return false;
        }
        addr_list = (struct in_addr **) he->h_addr_list;
        for (int i = 0; addr_list[i] != nullptr; i++) {
            server.sin_addr = *addr_list[i];
            break;
        }
    } else {
        server.sin_addr.s_addr = inet_addr(addr.c_str());
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(_port);
    if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("connect failed. Error");
        return true;
    }
    return true;
}

bool Client::Send(const string& data) const {
    if (send(sock, data.c_str(), strlen(data.c_str()), 0) < 0) {
        cout << "Send failed : " << data << endl;
        return false;
    }
    return true;
}

string Client::receive(int size) const {
    char buffer[size];
    string reply;
    if (recv(sock, buffer, size, 0) < 0)// sizeof(buffer)
    {
        cout << "receive failed!" << endl;
    }
    buffer[size] = '\0';
    reply = buffer;
    return reply;
}

string Client::read() const {
    char buffer[1] = {};
    string reply;
    while (buffer[0] != '\n') {
        if (recv(sock, buffer, sizeof(buffer), 0) < 0) {
            cout << "receive failed!" << endl;
            break;
        }
        reply += buffer[0];
    }
    return reply;
}