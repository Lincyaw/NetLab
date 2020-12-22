//
// Created by llincyaw on 2020/12/22.
//

#ifndef NETLAB_CLIENT_H
#define NETLAB_CLIENT_H

#include <iostream>
#include <netdb.h>

using namespace std;

class Client {
private:
    int sock;
    std::string address;
    int port;
    struct sockaddr_in server;

public:
    Client()=default;

    bool setup(string address, int port);

    bool Send(string data);

    string receive(int size = 512);

    string read();
};

#endif //NETLAB_CLIENT_H
