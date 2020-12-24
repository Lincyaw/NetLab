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

using namespace std;
#define MAX_PACKET_SIZE 4096
#define MAX_CLIENT 1000

struct desc_socket {
    int socket = -1;
    string ip;
    int id = -1;
    std::string message;
    bool enable_message_runtime = false;
};

class Server {
public:
    Server() = default;

    int setup(int port, const vector<int>& opts = vector<int>());

    vector<desc_socket *> getMessage();

    void accepted();

    void Send(string msg, int id);

    static void detach(int id);

    static void clean(int id);

    bool is_online();

    static string get_ip_addr(int id);

    static int get_last_closed_sockets();

    void closed();

private:
    int sock_fd, n, pid;
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    // Thread Pool
    pthread_t serverThread[MAX_CLIENT];
    static vector<desc_socket *> new_sock_fd;
    static char msg[MAX_PACKET_SIZE];
    static vector<desc_socket *> Message;
    static bool is_Online;
    static int last_closed;
    static int num_client;
    static std::mutex mt;

    static void *task(void *argv);
};

#endif //NETLAB_SERVER_H