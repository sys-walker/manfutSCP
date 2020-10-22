FLAGS = -lm -lpthread

.SILENT:

all:
	gcc ConcManfut.c -o ConcManfut  $(FLAGS)
	gcc manfut.c -o manfut  $(FLAGS)

ex1-sec:
	
	./manfut 300 mercat15j.csv
ex1-conc:
	
	./ConcManfut 300 mercat50j.csv 8


test-sec:
	
	time ./manfut 300 mercat50j.csv
test-conc:
	
	time ./ConcManfut 300 mercat50j.csv 8
clean:
	rm -fv ConcManfut manfut 
pack:

	tar cvfz RollonBaigesPRA1.tgz Informe_prac1.pdf *.c *.h Makefile *.csv
	ls
