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
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>
#include <mutex>
#include <vector>
#include <random>
using namespace std;
#define MAX_PACKET_SIZE 4096
#define MAX_CLIENT 1000
#define DEBUG  (1)
struct socketDescriptor {
    int socket = -1;
    string ip;
    int id = -1;
    std::string message;
    bool messageRuntime = false;
    int key;
};

class Server {
public:
    Server() = default;

    int serverInit(int port, const vector<int>& opts = vector<int>());

    static vector<socketDescriptor *> getMessage();

    int accepted();

    static void serverSend(const string& msg, int id);

    static void detach(int id);

    static void clean(int id);

    static bool isOnline();

    static string getIpAddr(int id);

    static int getLastClosedSockets();

    void closed() const;

private:
    int sock_fd{};
    struct sockaddr_in serverAddress{};
    struct sockaddr_in clientAddress{};
    // Thread Pool
    thread server_thread[MAX_CLIENT];
    static vector<socketDescriptor *> newSockFd;
    static char msg[MAX_PACKET_SIZE];
    static vector<socketDescriptor *> message;
    static bool online;
    static int lastClosed;
    static int clientNum;
    static std::mutex mt;
    static int _counter;
    static void task(socketDescriptor *argv);
};

#endif //NETLAB_SERVER_H