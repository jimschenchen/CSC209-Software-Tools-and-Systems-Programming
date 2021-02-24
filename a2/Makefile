
all: classifier 

classifier: dectree.c classifier.c
	gcc -g -Wall -std=gnu99 -o classifier dectree.c classifier.c -lm
	
run:
	./classifier datasets/training_data.bin datasets/testing_data.bin

.PHONY: clean all

test_load_dataset: dectree.c test_load_dataset.c
	gcc -g -Wall -std=gnu99 -o $@ dectree.c test_load_dataset.c -lm
	./test_load_dataset ./datasets/training_data.bin

clean:	
	rm classifier test_load_dataset
