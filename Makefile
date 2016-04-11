all: beacon client

beacon: beacon.c
	gcc -g -o beacon beacon.c

client: client.c
	gcc -g -o client client.c

clean:
	rm -f *.o beacon client
