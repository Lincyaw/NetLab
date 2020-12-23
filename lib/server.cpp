//
// Created by llincyaw on 2020/12/22.
//

#include "server.h"

char Server::msg[MAX_PACKET_SIZE];
int Server::num_client;
int Server::last_closed;
vector<desc_socket *> Server::Message;
vector<desc_socket *> Server::new_sock_fd;
std::mutex Server::mt;
bool Server::is_Online;

int Server::setup(int port, const vector<int>& opts)
{
	int opt = 1;
	is_Online = false;
	last_closed = -1;
	sock_fd = socket(AF_INET,SOCK_STREAM,0);
 	memset(&serverAddress,0,sizeof(serverAddress));

	for(unsigned int i = 0; i < opts.size(); i++) {
		if( (setsockopt(sock_fd, SOL_SOCKET, opts.size(), (char *)&opt, sizeof(opt))) < 0 ) {
			cerr << "Errore setsockopt" << endl;
      			return -1;
	      	}
	}

	serverAddress.sin_family      = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port        = htons(port);

	if((bind(sock_fd,(struct sockaddr *)&serverAddress, sizeof(serverAddress))) < 0){
		cerr << "Error bind" << endl;
		return -1;
	}

 	if(listen(sock_fd,5) < 0){
		cerr << "Error listen" << endl;
		return -1;
	}
	num_client = 0;
	is_Online = true;
	return 0;
}

void* Server::task(void *argv) {
    int n;
    auto *desc = (struct desc_socket *) argv;
    pthread_detach(pthread_self());

    cerr << "open client[ id:" << desc->id << " ip:" << desc->ip << " socket:" << desc->socket << " ]" << endl;
    while (true) {
        n = recv(desc->socket, msg, MAX_PACKET_SIZE, 0);
        if (n != -1) {
            mt.lock();
            if (n == 0) {
                is_Online = false;
                cerr << "close client[ id:" << desc->id << " ip:" << desc->ip << " socket:" << desc->socket << " ]"
                     << endl;
                last_closed = desc->id;
                close(desc->socket);
                new_sock_fd.erase(new_sock_fd.begin() + desc->id);
                if (num_client > 0) num_client--;
                break;
            }
            msg[n] = 0;
            desc->message = string(msg);
            Message.push_back(desc);
            mt.unlock();
        }
        usleep(600);
    }
    free(desc);
    pthread_exit(nullptr);
}

void Server::accepted()
{
	socklen_t sosize    = sizeof(clientAddress);
	auto *so = new desc_socket;
	so->socket          = accept(sock_fd,(struct sockaddr*)&clientAddress,&sosize);
	so->id              = num_client;
	so->ip              = inet_ntoa(clientAddress.sin_addr);
	new_sock_fd.push_back( so );
	cerr << "accept client[ id:" << new_sock_fd[num_client]->id <<
	                      " ip:" << new_sock_fd[num_client]->ip <<
		              " handle:" << new_sock_fd[num_client]->socket << " ]" << endl;
	pthread_create(&serverThread[num_client], nullptr, &task, (void *)new_sock_fd[num_client]);
	num_client++;
}

vector<desc_socket*> Server::getMessage()
{
	return Message;
}

void Server::Send(string msg, int id)
{
	send(new_sock_fd[id]->socket,msg.c_str(),msg.length(),0);
}

int Server::get_last_closed_sockets()
{
	return last_closed;
}

void Server::clean(int id)
{
	Message[id]->message = "";
	memset(msg, 0, MAX_PACKET_SIZE);
}

string Server::get_ip_addr(int id)
{
	return new_sock_fd[id]->ip;
}

bool Server::is_online()
{
	return is_Online;
}

void Server::detach(int id)
{
	close(new_sock_fd[id]->socket);
	new_sock_fd[id]->ip = "";
	new_sock_fd[id]->id = -1;
	new_sock_fd[id]->message = "";
}

void Server::closed()
{
	close(sock_fd);
}
