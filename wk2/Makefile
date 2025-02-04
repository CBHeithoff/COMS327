# queue_main: queue.c queue_main.c queue.h
# 	gcc queue.c queue_main.c -Wall -Werror -o queue_main

queue_main: queue.o queue_main.o
	gcc queue.o queue_main.o -o queue_main

queue.o: queue.c queue.h
	gcc -g -Wall -Werror queue.c -c 

queue_main.o: queue_main.c queue.h
	gcc -g -Wall -Werror queue_main.c -c

clean:
	rm -f *.o *~ queue_main
