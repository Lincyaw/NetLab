//
// Created by llincyaw on 2020/12/22.
//

#include <algorithm>
#include "server.h"

char Server::msg[MAX_PACKET_SIZE];
int Server::client_num;
int Server::last_closed;
vector<desc_socket *> Server::message;
vector<desc_socket *> Server::new_sock_fd;
std::mutex Server::mt;
bool Server::is_Online;
/**
 * 初始化server端的socket
 * @param port 端口号
 * @param opts 可选项
 * @return
 */
int Server::server_init(int port, const vector<int> &opts) {
#if DEBUG == 1
    cerr << "Server init begin" << endl;
#endif
    int opt = 1;
    is_Online = false;
    last_closed = -1;
    // ipv4, stream, 接受ip
    sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    memset(&serverAddress, 0, sizeof(serverAddress));
    for (uint32_t i = 0; i < opts.size(); i++) {
        if ((setsockopt(sock_fd, SOL_SOCKET, opts[i], (char *) &opt, sizeof(opt))) < 0) {
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
    client_num = 0;
    is_Online = true;

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
    auto sockDes = new desc_socket;
    sockDes->socket = accept(sock_fd, (struct sockaddr *) &clientAddress, &socklen);
    if(sockDes->socket == -1){
        return -1;
    }
    sockDes->id = client_num;
    sockDes->ip = inet_ntoa(clientAddress.sin_addr);
    new_sock_fd.push_back(sockDes);
    cerr << "accept client[ id:" << new_sock_fd[client_num]->id <<
         " ip:" << new_sock_fd[client_num]->ip <<
         " handle:" << new_sock_fd[client_num]->socket << " ]" << endl;
    server_thread[client_num] = thread(task, new_sock_fd[client_num]);
    server_thread[client_num].detach();
    client_num++;
#if DEBUG == 1
    cerr << "server accept end" << endl;
#endif
    return 1;
}

void Server::task(desc_socket *argv) {
#if DEBUG == 1
    cerr << "Server Task started" << endl;
#endif
    cerr << "open client[ id:" << argv->id << " ip:" << argv->ip << " socket:" << argv->socket << " send:"
         << argv->enable_message_runtime << " ]" << endl;
    cerr << "Current thread id: " << this_thread::get_id() << endl;
    while (true) {
        // flag取0,表示与read相同，常规操作
        int n = recv(argv->socket, msg, MAX_PACKET_SIZE, 0);
        if (n != -1) {
            if (n == 0) {
                is_Online = false;
                cerr << "close client[ id:" << argv->id << " ip:" << argv->ip << " socket:" << argv->socket << " ]"
                     << endl;
                last_closed = argv->id;
                close(argv->socket);
                int id = argv->id;
                auto new_end = std::remove_if(new_sock_fd.begin(), new_sock_fd.end(),
                                              [id](desc_socket *device) { return device->id == id; });
                new_sock_fd.erase(new_end, new_sock_fd.end());

                if (client_num > 0)
                    client_num--;
                break;
            }
            msg[n] = 0;
            argv->message = string(msg);
            lock_guard<mutex> guard(mt);
            message.push_back(argv);
        }
        usleep(600);
    }
    cerr << "exit thread: " << this_thread::get_id() << endl;
}


vector<desc_socket *> Server::getMessage() {
    lock_guard<mutex> guard(mt);
    return message;
}

void Server::server_send(const string &mess, int id) {
    send(new_sock_fd[id]->socket, mess.c_str(), mess.length(), 0);
}

int Server::get_last_closed_sockets() {
    return last_closed;
}

void Server::clean(int id) {
    message[id]->message = "";
    memset(msg, 0, MAX_PACKET_SIZE);
}

string Server::get_ip_addr(int id) {
    return new_sock_fd[id]->ip;
}

bool Server::is_online() {
    return is_Online;
}

void Server::detach(int id) {
#if DEBUG == 1
    cerr << "server detach" << endl;
#endif
    close(new_sock_fd[id]->socket);
    new_sock_fd[id]->ip = "";
    new_sock_fd[id]->id = -1;
    new_sock_fd[id]->message = "";
}

void Server::closed() const {
    close(sock_fd);
}
