//
// Created by nn on 2020/12/23.
//


#include "client.h"

#define MAX_PACKET_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc >= 2) {
        Client tcp;
        tcp.init("127.0.0.1", 11999);
        int num = atoi(argv[2]);
        cout << "Num Request:" << num << endl;
        for (int i = 0; i < num; i++) {
            string msg = argv[1];
            tcp.Send(msg);
            cout << tcp.receive(MAX_PACKET_SIZE) << endl;
            sleep(1);
        }
        exit(0);
    } else {
        cout << "Error: message num-request" << endl;
    }
}