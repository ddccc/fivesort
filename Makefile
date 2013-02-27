all: usefivesort useparfivesort

usefivesort: UseFiveSort.c FiveSort.o
	gcc -Wall -Werror -lm -o $@ $^

useparfivesort: UseParFiveSort.c ParFiveSort.o
	gcc -Wall -Werror -lm -lpthread -o $@ $^

clean:
	$(RM) usefivesort useparfivesort *.o
