#target: list of dependencies
#	command to build target from dependencies

all: hello random times hello.class word_search.c

hello: hello.c
	gcc hello.c -o hello

times: times_tables.c
	gcc times_tables.c -o times

hello.class: hello.java
	javac hello.java

word_search: word_search.c
	gcc word_search.c -o word_search -Wall -Werror

clean:
	rm -f hello times_tables random hello.class *~