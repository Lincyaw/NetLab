//
// Created by nn on 2020/12/23.
//


#include "client.h"
#include <csignal>

#define MAX_PACKET_SIZE 4096
Client tcp;

void sig_exit(int s)
{
	tcp.exit();
	exit(0);
}

int main(int argc, char *argv[])
{
	if(argc != 4) {
		cerr << "Usage: ./client ip port message" << endl;
		return 0;
	}
	signal(SIGINT, sig_exit);

    if(!tcp.client_init(argv[1], atoi(argv[2]))){
        cerr<<"client init failed"<<endl;
        exit(0);
    }
	while(1)
	{
	    if(!tcp.client_send(argv[3])){
	        cerr<<"Error: send failed."<<endl;
	    }
		string rec = tcp.receive();
		if( rec != "" )
		{
			cout << rec << endl;
		}
		sleep(1);
	}
}