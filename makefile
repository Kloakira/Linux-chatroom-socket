CFLAGS = -fPIC -no-pie

all : server client

server : news.c serverLogin.o dbtest.o
	gcc $(CFLAGS) news.c serverLogin.o dbtest.o -o server -lpthread -lmysqlclient  

client : newc.c clientLogin.o
	gcc $(CFLAGS) newc.c clientLogin.o -o client -lpthread -lmenu -lncursesw

serverLogin.o : serverLogin.c
	gcc $(CFLAGS) -c serverLogin.c 

clientLogin.o : clientLogin.c
	gcc $(CFLAGS) -c clientLogin.c

dbtest.o: dbtest.c
	gcc $(CFLAGS) -c dbtest.c 

clean:
	rm *.o main
