//
// Created by llincyaw on 2020/12/22.
//

#ifndef NETLAB_CLIENT_H
#define NETLAB_CLIENT_H

#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <cstring>
#include <arpa/inet.h>

using namespace std;

class Client {
    int sock;
    std::string address;
    int port;
    struct sockaddr_in server{};

public:
    Client():sock(-1),port(0){};

    bool client_init(const string &addr, int po);

    bool client_send(const string &data) const;

    string receive(int size = 4096) const;

    void exit() const;
};

#endif //NETLAB_CLIENT_H