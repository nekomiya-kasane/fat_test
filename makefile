box: main.o game.o buffer.o utils.o
	gcc -Wall -ansi -pedantic main.o game.o buffer.o utils.o -o box

utils.o: utils.c utils.h
	gcc -Wall -ansi -D_POSIX_SOURCE -pedantic utils.c -c

buffer.o: buffer.c buffer.h
	gcc -Wall -ansi -pedantic buffer.c -c

game.o: game.c game.h
	gcc -Wall -ansi -pedantic game.c -c

main.o: main.c
	gcc -Wall -ansi -pedantic main.c -c

clean:
	rm demo main.o game.o buffer.o utils.o