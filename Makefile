# space separated libraries each prefixed with -l
LIBS = 

battleship: main.c board.c board.h logic.c logic.h
	gcc main.c board.c logic.c -o battleship $(LIBS)

clean:
	rm -f battleship
