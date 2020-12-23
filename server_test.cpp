//
// Created by nn on 2020/12/23.
//

#include "lib/server.h"

Server tcp(11999);


void loop() {
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

    thread msg(loop);
    msg.detach();
    tcp.receive();

    return 0;
}