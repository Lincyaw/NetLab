//
// Created by nn on 2020/12/23.
//

#include "lib/server.h"
#include <csignal>
#include <ctime>

Server tcp;
thread msg1[MAX_CLIENT];
int num_message = 0;
int time_send = 2700;

void close_app(int s) {
    tcp.closed();
    exit(0);
}

void send_client(struct desc_socket *m) {

    while (true) {
        // server关闭了，并且最后结束的线程是该线程
        if (!Server::is_online() && Server::get_last_closed_sockets() == m->id) {
            cerr << "stop send_clients id:" << m->id << " ip:" << m->ip << " socket: " << m->socket << endl;
            break;
        }
        std::time_t t = std::time(nullptr);
        std::tm *now = std::localtime(&t);

        std::string date =
                to_string(now->tm_year + 1900) + "-" +
                to_string(now->tm_mon + 1) + "-" +
                to_string(now->tm_mday) + " " +
                to_string(now->tm_hour) + ":" +
                to_string(now->tm_min) + ":" +
                to_string(now->tm_sec) + "\r\n";
        cerr << date << endl;
        Server::server_send(date, m->id);
        sleep(time_send);
    }
}

void received() {
    vector<desc_socket *> desc;
    while (1) {
        desc = tcp.getMessage();
        for (unsigned int i = 0; i < desc.size(); i++) {
            if (!desc[i]->message.empty()) {
                if (!desc[i]->enable_message_runtime) {
                    desc[i]->enable_message_runtime = true;
                    msg1[num_message] = thread(send_client, desc[i]);
                    msg1[num_message].detach();
                    num_message++;
                }
                cout << "id:      " << desc[i]->id << endl
                     << "ip:      " << desc[i]->ip << endl
                     << "message: " << desc[i]->message << endl
                     << "socket:  " << desc[i]->socket << endl
                     << "enable:  " << desc[i]->enable_message_runtime << endl;
                tcp.clean(i);
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
        time_send = atoi(argv[2]);
    signal(SIGINT, close_app);
//SO_REUSEPORT支持多个进程或者线程绑定到同一端口，提高服务器程序的性能
//SO_REUSEADDR同一地址
    vector<int> opts = {SO_REUSEPORT, SO_REUSEADDR};
    if (tcp.server_init(atoi(argv[1]), opts) == 0) {
        // server 创建成功之后，需要监听客户端信息
        thread msg(received);
        msg.detach();
        while (true) {
            // 循环等待接受客户端
            if(tcp.accepted()==-1){
                cerr<<"accept error"<<endl;
                break;
            }
        }
    } else {
        cerr << "Error in init socket" << endl;
    }
    return 0;
}