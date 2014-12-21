CC = g++
CPPFLAGS = -std=c++11 -D_GLIBCXX_DEBUG
//CPPFLAGS = -std=c++11 -O2 -march=native -mtune=native

main.exe : main.o load_npy.o save_npy.o
	g++ $(CPPFLAGS) $^ -o $@


main.o : main.cpp load_npy.h save_npy.h
	g++ -c $(CPPFLAGS) $< -o $@

load_npy.o : load_npy.cpp load_npy.h
	g++ -c $(CPPFLAGS) $< -o $@

save_npy.o : save_npy.cpp save_npy.h
	g++ -c $(CPPFLAGS) $< -o $@

clean :
	-rm main.exe main.o load_npy.o save_npy.o
