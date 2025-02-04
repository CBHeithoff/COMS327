# hw0: dungeon_gen.c dungeon_gen_main.c dungeon_gen.h
# 	gcc dungeon_gen.c dungeon_gen_main.c dungeon_gen.h -Wall -Werror -o dungeon_gen_main -lm

# clean:
# 	rm -f dungeon_gen_main

dungeon_gen_main: dungeon_gen.o dungeon_gen_main.o
	gcc dungeon_gen.o dungeon_gen_main.o -o dungeon_gen_main -lm

dungeon_gen.o: dungeon_gen.c dungeon_gen.h
	gcc -g -Wall -Werror dungeon_gen.c -c -lm

dungeon_gen_main.o: dungeon_gen_main.c dungeon_gen.h
	gcc -g -Wall -Werror dungeon_gen_main.c -c -lm

clean:
	rm -f *.o *~ dungeon_gen_main