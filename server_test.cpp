//
// Created by nn on 2020/12/23.
//

#include "lib/server.h"


[[noreturn]] void *loop(void *m) {
    pthread_detach(pthread_self());
    while (true) {
        string str = Server::getMessage();
        if (!str.empty()) {
            cout << "Message:" << str << endl;
            Server::clean();
        }
        usleep(1000);
    }
}

int main() {
    Server tcp(11999);

    pthread_t msg;
    if (pthread_create(&msg, nullptr, loop, (void *) 0) == 0) {
        tcp.receive();
    }
    return 0;
}