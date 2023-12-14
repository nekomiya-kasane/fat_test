nyufile: disk.o input.o recover.o utils.o file.o
	cc -o nyufile disk.o input.o recover.o utils.o file.o
disk.o: nyudisk.c nyudisk.h
	cc -c nyudisk.c
input.o: nyuinput.c nyuinput.h
	cc -c nyuinput.c
recover.o: nyurecover.c nyurecover.h
	cc -c nyurecover.c
utils.o: nyuutils.c nyuutils.h
	cc -c nyuutils.c
file.o: nyufile.c nyufile.h
	cc -c nyufile.c
clean :
    rm nyufile nyu*.o