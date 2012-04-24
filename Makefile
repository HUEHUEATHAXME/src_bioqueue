all: compile

compile:
	gcc -Wall -o bioqueue testpriqueue.c bioqueue.c time.c

clean:
	rm -f *.o
	rm -f *~
	rm -f ./bioqueue
