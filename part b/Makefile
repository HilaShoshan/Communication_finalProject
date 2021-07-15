g++ -std=c++11 -g Udp-Server.cpp Udp-Client.cpp

all: 
	g++ Udp-Server.cpp Udp-Server.o
	g++ Udp-Client.cpp Udp-Client.o

server:
	g++ Udp-Server.cpp Udp-Server.o
	sudo ./Udp-Server.o

client:
	g++ Udp-Client.cpp Udp-Client.o
	sudo ./Udp-Client.o

clean:
	rm -f *.o 
