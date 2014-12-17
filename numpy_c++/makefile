CC = g++
CPPFLAGS = -std=c++11 -D_GLIBCXX_DEBUG
//CPPFLAGS = -std=c++11 -O2 -march=native -mtune=native

main.exe : main.o load_npy.o
	g++ -o main.exe $(CPPFLAGS) main.o load_npy.o


main.o : main.cpp load_npy.h
	g++ -c -o main.o $(CPPFLAGS) main.cpp

load_npy.o : load_npy.cpp load_npy.h
	g++ -c -o load_npy.o $(CPPFLAGS) load_npy.cpp

clean :
	-rm main.exe main.o load_npy.o
