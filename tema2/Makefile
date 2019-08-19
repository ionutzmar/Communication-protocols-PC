all: server.o subscriber.o
	g++ server.o -o server
	g++ subscriber.o -o subscriber

server.o: server.cpp
	g++ server.cpp -Wall -c

subscriber.o: subscriber.cpp
	g++ subscriber.cpp -Wall -c

clean:
	rm server subscriber server.o subscriber.o
