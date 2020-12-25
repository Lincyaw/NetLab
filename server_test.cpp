//
// Created by nn on 2020/12/23.
//

#include "lib/server.h"
#include <csignal>
#include <ctime>
#include <sstream>
#include <fstream>

Server tcp;
thread msg1[MAX_CLIENT];
int numMessage = 0;
int seconds = 2700;

void handler(int s) {
    tcp.closed();
    cerr << s << endl;
    exit(0);
}

void sendClient(struct socketDescriptor *m) {

    // server关闭了，并且最后结束的线程是该线程
    if (!Server::isOnline() && Server::getLastClosedSockets() == m->key) {
        cerr << "stop send_clients id:" << m->id << " ip:" << m->ip << " socket: " << m->socket << endl;
        Server::serverSend("reconnect", m->id);
        return;
    }
#if DEBUG == 1
//    std::time_t t = std::time(nullptr);
//    std::tm *now = std::localtime(&t);
//
//    std::string date =
//            to_string(now->tm_year + 1900) + "-" +
//            to_string(now->tm_mon + 1) + "-" +
//            to_string(now->tm_mday) + " " +
//            to_string(now->tm_hour) + ":" +
//            to_string(now->tm_min) + ":" +
//            to_string(now->tm_sec) + "\r\n";
//    cerr << endl << date << endl;
#endif
    mutex mt;
    lock_guard<mutex> guard(mt);
    ifstream t(m->message);
    if (!t) {
        Server::serverSend("error", m->id);
        t.close();
        return;
    }
    stringstream buffer;
    buffer << t.rdbuf();
    string contents(buffer.str());
    t.close();
    Server::serverSend(contents, m->id);
//        sleep(1000);
}

char *simple_tok(char *p, char d) {
    if (p == nullptr) {
        return nullptr;
    }
    char *t = p;
    while (*t != '\0' && *t != d)
        t++;
    if (*t == '\0')
        return p;
    *t = '\0';
    return t + 1;
}

void received() {
    vector<socketDescriptor *> desc;
    while (true) {
        desc = Server::getMessage();
        for (int i = 0; i < desc.size(); i++) {
            if (!desc[i]->message.empty() && !desc[i]->messageRuntime) {
                desc[i]->messageRuntime = true;
                msg1[numMessage] = thread(sendClient, desc[i]);
                msg1[numMessage].detach();
                numMessage++;

                char *filename = (char *) (desc[i]->message.c_str());
                if (filename[0] == '^' && filename[1] == '@') {
                    filename += 2;
#if DEBUG == 1
                    cout << filename << endl;
#endif
                    char *file = simple_tok(filename, '\n');
#if DEBUG == 1

                    cout << filename << endl << endl;
                    cout << file << endl;
#endif
                    mutex mt;
                    lock_guard<mutex> guard(mt);
                    ofstream t(filename);
                    t << file;
                    t.close();
                }

#if DEBUG == 1
                cout << "\nid:      " << desc[i]->id << endl
                     << "ip:      " << desc[i]->ip << endl
                     << "message: \n" << desc[i]->message << endl
                     << "socket:  " << desc[i]->socket << endl
                     << "enable:  " << desc[i]->messageRuntime << endl << endl;
#endif
                Server::clean(i);
            }
        }
        usleep(1000);
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        cerr << "Usage: ./server port (opt)time-send" << endl;
        return 0;
    }
    if (argc == 3)
        seconds = atoi(argv[2]);
    signal(SIGINT, handler);
//SO_REUSEPORT支持多个进程或者线程绑定到同一端口，提高服务器程序的性能
//SO_REUSEADDR同一地址
    vector<int> opts = {SO_REUSEPORT, SO_REUSEADDR};
    if (tcp.serverInit(atoi(argv[1]), opts) == 0) {
        // server 创建成功之后，需要监听客户端信息
        thread msg(received);
        msg.detach();
        while (true) {
            // 循环等待接受客户端
            if (tcp.accepted() == -1) {
                cerr << "accept error" << endl;
                break;
            }
        }
    } else {
        cerr << "Error in init socket" << endl;
    }
    return 0;
}