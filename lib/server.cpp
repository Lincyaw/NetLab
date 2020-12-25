//
// Created by llincyaw on 2020/12/22.
//

#include <algorithm>
#include "server.h"

char Server::msg[MAX_PACKET_SIZE];
int Server::clientNum;
int Server::lastClosed;
vector<socketDescriptor *> Server::message;
vector<socketDescriptor *> Server::newSockFd;
std::mutex Server::mt;
bool Server::online;
int Server::_counter = 0;
/**
 * 初始化server端的socket
 * @param port 端口号
 * @param opts 可选项
 * @return
 */
int Server::serverInit(int port, const vector<int> &opts) {
#if DEBUG == 1
    cerr << "Server init begin" << endl;
#endif
    int opt = 1;
    online = false;
    lastClosed = -1;
    // ipv4, stream, 接受ip
    sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    memset(&serverAddress, 0, sizeof(serverAddress));
    for (int i : opts) {
        if ((setsockopt(sock_fd, SOL_SOCKET, i, (char *) &opt, sizeof(opt))) < 0) {
            cerr << "Error: set sock option failed." << endl;
            return -1;
        }
    }
    /*ipv4*/
    serverAddress.sin_family = AF_INET;
    /* Internet address. 绑定INADDR_ANY, 不管数据是从哪个网卡过来的，只要是绑定的端口号过来的数据，都可以接收到。 */
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); // host to net (long)
    /* Port number.  */
    serverAddress.sin_port = htons(port);

    if ((bind(sock_fd, (struct sockaddr *) &serverAddress, sizeof(serverAddress))) < 0) {
        cerr << "Error bind" << endl;
        return -1;
    }
    // N=5 connection requests will be queued before further requests are refused.
    if (listen(sock_fd, 5) < 0) {
        cerr << "Error listen" << endl;
        return -1;
    }
    clientNum = 0;
    online = true;

#if DEBUG == 1
    cerr << "Server init end" << endl;
#endif
    return 0;
}

/**
 *  该函数将被不断调用。函数内部创建了task函数的线程，用于处理多线程客户端。一个task线程对应一个客户端
 */
int Server::accepted() {
#if DEBUG == 1
    cerr << "Server accept, 阻塞等待连接，获取套接字中" << endl;
#endif
    socklen_t socklen = sizeof(clientAddress);
    auto sockDes = new socketDescriptor;
    sockDes->socket = accept(sock_fd, (struct sockaddr *) &clientAddress, &socklen);
    if (sockDes->socket == -1) {
        return -1;
    }
    // TODO: id is set as clientNum , so as a client reconnect a server, it doesn't works.
    sockDes->id = clientNum;
    sockDes->ip = inet_ntoa(clientAddress.sin_addr);
    newSockFd.push_back(sockDes);
    cerr<< "--------------------------------------------------------" << endl;
    cerr << endl << "accept client[ id:" << newSockFd[clientNum]->id <<
         " ip:" << newSockFd[clientNum]->ip <<
         " socket:" << newSockFd[clientNum]->socket << " ]" << endl << endl;
    server_thread[clientNum] = thread(task, newSockFd[clientNum]);
    server_thread[clientNum].detach();
//    cerr<<endl<<"client ++"<<endl;
    clientNum++;
#if DEBUG == 1
    cerr << "server accept end" << endl;
#endif
    return 1;
}

void Server::task(socketDescriptor *argv) {
#if DEBUG == 1
    cerr << "Server Task started" << endl;
#endif
    cerr << "open client[ id:" << argv->id << " ip:" << argv->ip << " socket:" << argv->socket << " send:"
         << argv->messageRuntime << " ]" << endl;
    cerr << "Current thread id: " << this_thread::get_id() << endl;
    while (true) {
        // flag取0,表示与read相同，常规操作
        int n = recv(argv->socket, msg, MAX_PACKET_SIZE, 0);
        if (n != -1) {
            if (n == 0) {
                online = false;
                cerr << "close client[ id:" << argv->id << " ip:" << argv->ip << " socket:" << argv->socket << " ]"
                     << endl;
                // TODO: i changed here
                lastClosed = argv->key;
                close(argv->socket);
                int id = argv->id;
                auto new_end = std::remove_if(newSockFd.begin(), newSockFd.end(),
                                              [id](socketDescriptor *device) { return device->id == id; });
                newSockFd.erase(new_end, newSockFd.end());

                if (clientNum > 0) {
//                    cerr<<endl<<"client --"<<endl;
                    clientNum--;
                }
                break;
            }
            msg[n] = 0;
            argv->message = string(msg);
            argv->key = (int)random()%4096;
            lock_guard<mutex> guard(mt);
            message.push_back(argv);
        }
        usleep(600);
    }
    cerr << endl << "exit thread: " << this_thread::get_id() << endl;
}


vector<socketDescriptor *> Server::getMessage() {
    lock_guard<mutex> guard(mt);
    return message;
}

void Server::serverSend(const string &messages, int id) {
    send(newSockFd[id]->socket, messages.c_str(), messages.length(), 0);
}

int Server::getLastClosedSockets() {
    return lastClosed;
}

void Server::clean(int id) {
    message[id]->message = "";
    memset(msg, 0, MAX_PACKET_SIZE);
}

string Server::getIpAddr(int id) {
    return newSockFd[id]->ip;
}

bool Server::isOnline() {
    return online;
}

void Server::detach(int id) {
#if DEBUG == 1
    cerr << "server detach" << endl;
#endif
    close(newSockFd[id]->socket);
    newSockFd[id]->ip = "";
    newSockFd[id]->id = -1;
    newSockFd[id]->message = "";
}

void Server::closed() const {
    close(sock_fd);
}
