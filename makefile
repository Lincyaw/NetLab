all:
	g++ -Wall -o server server_test.cpp -I./ lib/server.cpp lib/client.cpp -std=c++11 -lpthread
	g++ -Wall -o client client_test.cpp -I./ lib/server.cpp lib/client.cpp -std=c++11 -lpthread

clean:
	rm server client