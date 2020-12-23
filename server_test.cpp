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

void send_client(struct desc_socket * m) {
    auto *desc = m;

    while (1) {
        if (!tcp.is_online() && Server::get_last_closed_sockets() == desc->id) {
            cerr << "Connessione chiusa: stop send_clients( id:" << desc->id << " ip:" << desc->ip << endl;
            break;
        }
        std::time_t t = std::time(0);
        std::tm *now = std::localtime(&t);
        int hour = now->tm_hour;
        int min = now->tm_min;
        int sec = now->tm_sec;

        std::string date =
                to_string(now->tm_year + 1900) + "-" +
                to_string(now->tm_mon + 1) + "-" +
                to_string(now->tm_mday) + " " +
                to_string(hour) + ":" +
                to_string(min) + ":" +
                to_string(sec) + "\r\n";
        cerr << date << endl;
        tcp.Send(date, desc->id);
        sleep(time_send);
    }
//    pthread_exit(nullptr);
}

void *received(void *m) {
    pthread_detach(pthread_self());
    vector<desc_socket *> desc;
    while (1) {
        desc = tcp.getMessage();
        for (unsigned int i = 0; i < desc.size(); i++) {
            if (!desc[i]->message.empty()) {
                if (!desc[i]->message.empty() && !desc[i]->enable_message_runtime) {
                    desc[i]->enable_message_runtime = true;
                    msg1[num_message] = thread(send_client,desc[i]);
                    msg1[num_message].detach();
//                    if (pthread_create(&msg1[num_message], NULL, send_client, (void *) desc[i]) == 0) {
//                        cerr << "ATTIVA THREAD INVIO MESSAGGI" << endl;
//                    }
                    num_message++;
                    // start message background thread
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

    pthread_t msg;
    vector<int> opts = {SO_REUSEPORT, SO_REUSEADDR};

    if (tcp.setup(atoi(argv[1]), opts) == 0) {
        if (pthread_create(&msg, nullptr, received, (void *) 0) == 0) {
            while (1) {
                tcp.accepted();
            }
        }
    } else
        cerr << "Errore apertura socket" << endl;
    return 0;
}