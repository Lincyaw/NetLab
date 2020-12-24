//
// Created by llincyaw on 2020/12/22.
//

#include "client.h"
/**
 * 初始化客户端 socket
 * @param addr ip地址
 * @param po 端口号
 * @return 成功 true， 失败 false
 */
bool Client::client_init(const string &addr, int po) {
    // 初始化的时候sock被初始化为-1
    if (sock == -1) {
        // 如果没有sock，就获取一个
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            cerr << "Error: create sock failed." << endl;
        }
    }
    // 把输入的类似于127.0.0.1的地址转换为二进制
    // 如果是255.255.255.255，那么要找本网络的
    if ((signed) inet_addr(addr.c_str()) == -1) {
        struct hostent *hostEntry;
        struct in_addr **addr_list;
        if ((hostEntry = gethostbyname(addr.c_str())) == nullptr) {
            cerr << "Failed to resolve hostname\n" << endl;
            return false;
        }
        addr_list = (struct in_addr **) hostEntry->h_addr_list;
        for (int i = 0; addr_list[i] != nullptr; i++) {
            server.sin_addr = *addr_list[i];
            break;
        }
    } else {
        // 否则直接赋值即可
        server.sin_addr.s_addr = inet_addr(addr.c_str());
    }
    // ipv4
    server.sin_family = AF_INET;
    // 设置对应的端口号
    server.sin_port = htons(po); // host to net (short)
    // connect
    if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
        cerr << "connect failed. Error" << endl;
        return false;
    }
    return true;
}

/**
 * 封装后的send
 * @param data 要发送的数据
 * @return 成功返回true，失败false
 */
bool Client::client_send(const string &data) const {
    // socket没初始化成功，或者发送失败则失败
    if (sock == -1 || send(sock, data.c_str(), strlen(data.c_str()), 0) < 0) {
        cerr << "Error: server_send failed : " << data << endl;
        return false;
    }
    return true;
}

/**
 * 接受数据
 * @param size 窗口大小
 * @return 收到的数据（字符串格式）
 */
string Client::receive(int size) const {
    char buffer[size];
    memset(buffer, 0, sizeof(buffer));
    if (recv(sock, buffer, size, 0) < 0)// sizeof(buffer)
    {
        cout << "receive failed!" << endl;
        return "";
    }
    buffer[size - 1] = '\0';
    return buffer;
}


void Client::exit() const {
    close(sock);
}