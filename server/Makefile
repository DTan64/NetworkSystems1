server: udp_server.c
	mkdir files; gcc -W -o server udp_server.c
clean:
	rm server; rm -dfr files
run:
	./server 5001
compile:
	make clean; make
