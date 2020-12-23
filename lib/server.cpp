//
// Created by llincyaw on 2020/12/22.
//

#include "server.h"

string Server::Message;

Server::Server(int port) {
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serverAddress, 0, sizeof(serverAddress));
    // sin_family指代协议族，在socket编程中只能是AF_INET
    serverAddress.sin_family = AF_INET;
    // sin_addr存储IP地址，使用in_addr这个数据结构. htonl()函数将无符号整数hostlong从主机字节顺序转换为网络字节顺序。
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    // sin_port存储端口号（使用网络字节顺序），在linux下，端口号的范围0~65535,同时0~1024范围的端口号已经被系统使用或保留。
    serverAddress.sin_port = htons(port);

    if (bind(sock_fd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1) {
        fprintf(stderr, "error in bind socket.  %s. \n", strerror(errno));
        exit(0);
    }

    listen(sock_fd, 5);
}

void Server::task(int argv) {
    int new_sock_fd = argv;
    // 容许线程从线程句柄独立开来执行
    char msg[MAX_PACKET_SIZE];
    while (true) {
        int n = recv(new_sock_fd, msg, MAX_PACKET_SIZE, 0);
        if (n == 0) {
            close(new_sock_fd);
            exit(0);
        }
        msg[n] = 0;
//        strcpy(msg, "you are my son.");
        send(new_sock_fd, msg, n, 0);
        Message = string(msg);
    }
}

string Server::receive() {
    while (true) {
        socklen_t sock_size = sizeof(clientAddress);
        new_sock_fd = accept(sock_fd, (struct sockaddr *) &clientAddress, &sock_size);
        if (new_sock_fd == -1) {
            fprintf(stderr, "error in accept.  %s. \n", strerror(errno));
            exit(0);
        }
        serverThread = thread(task, new_sock_fd);
        serverThread.detach();
    }
}

string Server::getMessage() {
    return Message;
}

void Server::clean() {
    Message = "";
}

void Server::detach() const {
    close(sock_fd);
    close(new_sock_fd);
}
