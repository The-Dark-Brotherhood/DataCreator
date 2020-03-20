#
# FILE : makefile
# PROJECT :  A-03: Hoochamacallit DataCreator
# PROGRAMMER : Michael Gordon & Gabriel Gurgel
# FIRST VERSION : 2020-03-12
# DESCRIPTION : This makefile is from the MyApp Example provided in the SET Program
#
# FINAL BINARY Target
./bin/DataCreator :	./obj/DataCreator.o	./obj/DataCreatorLogging.o
	cc ./obj/DataCreator.o	./obj/DataCreatorLogging.o -o	./bin/DataCreator
#
# =======================================================
#                     Dependencies
# =======================================================
./obj/DataCreator.o :	./src/DataCreator.c ./inc/DataCreator.h
	cc -c ./src/DataCreator.c -o ./obj/DataCreator.o

./obj/DataCreatorLogging.o : ./src/DataCreatorLogging.c ./inc/DataCreator.h
	cc -c ./src/DataCreatorLogging.c -o ./obj/DataCreatorLogging.o

#
# =======================================================
# Other targets
# =======================================================
all : ./bin/DataCreator

clean:
	rm -f ./bin/*
	rm -f ./obj/*.o
	rm -f ./inc/*.h~
	rm -f ./src/*.c~
