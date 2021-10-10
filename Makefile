GPP = g++
GPPFLAGS = -std=c++11
SERVNANE = server
CLIENTNAME = client

all: server.o client.o server_func.o tree.o
	$(GPP) $(GPPFLAGS) server.o server_func.o tree.o -o $(SERVNANE) -lzmq
	$(GPP) $(GPPFLAGS) client.o server_func.o -o $(CLIENTNAME) -lzmq

server.o: server_func.h tree.h
	$(GPP) $(GPPFLAGS) -c server.cpp -o server.o

client.o: server_func.h
	$(GPP) $(GPPFLAGS) -c client.cpp -o client.o

server_func.o: server_func.h
	$(GPP) $(GPPFLAGS) -c server_func.cpp -o server_func.o

tree.o: tree.h
	$(GPP) $(GPPFLAGS) -c tree.cpp -o tree.o

clean:
	rm *.o $(CLIENTNAME) $(SERVNANE)
