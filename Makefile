Flags = -std=c99 -Wall -Wextra
CC = gcc


main : Makefile   main.o send.o receive.o
	$(CC) $(Flags) -o main   main.o send.o receive.o

main.o : Makefile   main.c srlib.h
	$(CC) -c $(Flags) main.c -o main.o

send.o : Makefile   send.c  srlib.h
	$(CC) -c $(Flags) send.c -o send.o

receive.o : Makefile   receive.c srlib.h
	$(CC) -c $(Flags) receive.c -o  receive.o


clean :     
	rm *.o

distclean :     
	rm *.o main
