all: ads.out test.out
ads.out: ads.c
	gcc ads.c ads1015.c -o ads.out -lpaho-mqtt3c -Wall
test.out: test.c
	gcc test.c -o test.out -lpaho-mqtt3c -Wall