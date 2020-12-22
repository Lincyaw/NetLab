//
// Created by llincyaw on 2020/12/22.
//

#ifndef NETLAB_SERVER_H
#define NETLAB_SERVER_H

#include <iostream>
#include <netdb.h>

using namespace std;
#define MAX_PACKET_SIZE 4096

class Server {
public:
    int sock_fd, new_sock_fd, n, pid;
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    pthread_t serverThread;
    char msg[MAX_PACKET_SIZE];
    static string Message;

    void setup(int port);

    Server() = default;

    string receive();

    string getMessage();

    void detach();

    void clean();

private:
    static void *Task(void *argv);
};

#endif //NETLAB_SERVER_H
