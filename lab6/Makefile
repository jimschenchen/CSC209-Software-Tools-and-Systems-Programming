
test: copy.c
	gcc -g -Wall -std=gnu99 -o copy copy.c

overflow: overflow.c
	gcc -Wall -std=gnu99 -g -o overflow overflow.c

.PHONY: run cealn

run:
	./copy

clean:
	rm -r copy overflow *.dSYM
