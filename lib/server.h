//
// Created by llincyaw on 2020/12/22.
//

#ifndef NETLAB_SERVER_H
#define NETLAB_SERVER_H

#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <pthread.h>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>

using namespace std;
#define MAX_PACKET_SIZE 4096

class Server {
public:
    Server():sock_fd(-1),new_sock_fd(-1){};

    explicit Server(int port);

    int sock_fd, new_sock_fd{};

    struct sockaddr_in serverAddress{};

    struct sockaddr_in clientAddress{};
    thread serverThread;

    static string Message;

    string receive();

    static string getMessage();

    void detach() const;

    static void clean();

private:
    static void task(int argv);
};

#endif //NETLAB_SERVER_H