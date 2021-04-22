all: ads.out test.out preditor.out
ads.out: ads.c
	gcc ads.c ads1015.c -o ads.out -lpaho-mqtt3c -Wall
test.out: test.c
	gcc test.c -o test.out -lpaho-mqtt3c -Wall
preditor.out: preditor.cpp
	g++ preditor.cpp -o preditor.out -std=c++11 -O2 -larmadillo -lpaho-mqtt3c -Wall
debug: mqtt_test_client.c
	gcc mqtt_test_client.c -o debug.out -lpaho-mqtt3c -Wall