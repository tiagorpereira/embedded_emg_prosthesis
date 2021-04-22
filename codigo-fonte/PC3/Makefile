all: ads.out test.out preditor.out
ads.out: ads.c
	gcc ads.c ads1015.c -o ads.out -lpaho-mqtt3c -Wall
test.out: test.c
	gcc test.c -o test.out -lpaho-mqtt3c -Wall
preditor.out: preditor.cpp
	g++ preditor.cpp -o preditor.out -std=c++11 -O2 -larmadillo -lpaho-mqtt3c -I"lib/sigpack" -Wall
debug: mqtt_test_client.c
	gcc mqtt_test_client.c -o debug.out -lpaho-mqtt3c -Wall 
servo: control.out
control.out: control.o gpio_dev_mem.o
	gcc control.o gpio_dev_mem.o -o control.out
control.o: control.c
	gcc -c control.c
gpio_dev_mem.o: gpio_dev_mem.c
	gcc -c gpio_dev_mem.c
clean:
	rm -f *.out *.o *.bin *.txt *.html