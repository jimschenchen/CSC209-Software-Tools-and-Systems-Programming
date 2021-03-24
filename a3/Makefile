FLAGS = -Wall -g -std=gnu99 

all: classifier 

classifier : classifier.o knn.o
	gcc ${FLAGS} -o $@ $^ -lm

test_distance : test_distance.o knn.o
	gcc ${FLAGS} -o $@ $^ -lm


%.o : %.c knn.h
	gcc ${FLAGS} -c $<


.PHONY: clean all test

clean:	
	rm classifier test_distance *.o

testdata:
	./classifier -K 3 -p 8 -d eucl -v ./datasets/training_data.bin ./datasets/testing_data.bin 
test:
	./classifier -K 3 -p 8 -d eucl -v ./datasets/training_1000.bin ./datasets/testing_1000.bin 
test1:
	./classifier -K 2 -p 8 -d eucl ./datasets/training_1.bin ./datasets/testing_1.bin 
testcos:
	./classifier -K 3 -p 8 -d cos ./datasets/training_10000.bin ./datasets/testing_100.bin 
testfull:
	./classifier -K 3 -p 8 -d eucl ./datasets/training_10000.bin ./datasets/testing_data.bin 