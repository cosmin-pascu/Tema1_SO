all: so-cpp

so-cpp: so-cpp.o func_define.o parseArguments.o myHashMap.o
	gcc -g -o so-cpp so-cpp.o myHashMap.o parseArguments.o func_define.o

so-cpp.o: so-cpp.c
	gcc -g -Wall -c so-cpp.c

func_define.o: func_define.c
	gcc -g -Wall -c func_define.c

parseArguments.o: parseArguments.c
	gcc -g -Wall -c parseArguments.c

myHashMap.o: myHashMap.c
	gcc -g -Wall -c myHashMap.c

clean:
	rm -f *.o *~so-cpp
