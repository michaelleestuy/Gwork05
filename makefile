run:
	gcc matrix.c -lm -o test.out
	rm -f *.ppm *.png
	./test.out
