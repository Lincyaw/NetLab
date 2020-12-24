//
// Created by nn on 2020/12/23.
//


#include "client.h"
#include <signal.h>

#define MAX_PACKET_SIZE 4096
Client tcp;

void sig_exit(int s) {
    tcp.exit();
    exit(0);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, sig_exit);
    tcp.init("127.0.0.1", 11999);
    while(true){
        if(!(tcp.client_send(argv[1]))){
            cout<<"send error"<<endl;
            exit(0);
        }
        string rec = tcp.receive();
        if (!rec.empty()) {
            cout << "Server Response:" << rec << endl;
        }else{
            cout<<"receive error"<<endl;
        }
    }
}

