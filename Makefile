all: usefivesort useparfivesort

usefivesort: UseFiveSort.c FiveSort.o
	gcc -O2 -Wall -Werror -o $@ $^ -lm

useparfivesort: UseParFiveSort.c ParFiveSort.o
	gcc -O2 -Wall -Werror -o $@ $^ -lm -lpthread

clean:
	$(RM) usefivesort useparfivesort *.o
