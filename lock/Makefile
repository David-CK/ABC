all: lock server client

lock:
	g++ -pthread -o lock lock.c SerialPort.cpp Common.cpp

server:
	gcc -pthread -o server server.c

client:
	gcc -pthread -o client client.c

