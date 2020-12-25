//
// Created by nn on 2020/12/23.
//


#include "client.h"
#include <csignal>
#include <fstream>
#include <sstream>

#define MAX_PACKET_SIZE 4096
Client tcp;

void sig_exit(int s) {
    tcp.exit();
    cerr << s << endl;
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        cerr << "Usage: ./client ip port opt sourceFileDir" << endl;
        cerr << "opt: \n\t-d download" << "\n\t-u upload" << endl;
        return 0;
    }
    signal(SIGINT, sig_exit);

    if (!tcp.clientInit(argv[1], atoi(argv[2]))) {
        cerr << "client init failed" << endl;
        exit(0);
    }
    if (strcmp(argv[3], "-u") == 0) {
        ifstream t(argv[4]);
        if (!t) {
            cerr << "no such file." << endl;
            t.close();
            return -1;
        }
        stringstream buffer;
        buffer << t.rdbuf();
        string contents(buffer.str());
        t.close();
        if (!tcp.clientSend(contents)) {
            cerr << "Error: send failed." << endl;
        }
    } else if (strcmp(argv[3], "-d") == 0) {
//        while(true){
        if (!tcp.clientSend(argv[4])) {
            cerr << "Error: send failed." << endl;
        }
        string rec = tcp.receive();
        if (rec == "error") {
            cout << "no such file" << endl;
            return 0;
        } else if (rec == "reconnect") {
            cout << "reconnect" << endl;
        } else if (!rec.empty()) {
            cout << rec << endl;
        }

//        }
    } else {
        cerr << "opt input error." << endl;
        exit(0);
    }
}