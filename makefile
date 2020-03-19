#
# FILE : makefile
# PROJECT :  A-03: Hoochamacallit DataCreator
# PROGRAMMER : Michael Gordon & Gabriel Gurgel
# FIRST VERSION : 2020-03-12
# DESCRIPTION : This makefile is from the MyApp Example provided in the SET Program
# 
# FINAL BINARY Target
./bin/cryptoMagic : ./obj/DataCreator.o
	cc ./obj/DataCreator.o -o ./bin/DataCreator
#
# =======================================================
#                     Dependencies
# =======================================================                     
./obj/DataCreator.o : ./src/DataCreator.c ./inc/DataCreator.h
	cc -c ./src/DataCreator.c -o ./obj/DataCreator.o

#./obj/parsing.o : ./src/parsing.c ./inc/cryptoMagic.h
#	cc -c ./src/parsing.c -o ./obj/parsing.o

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
