CFLAGS = -std=c++11 

all: build

build:
	g++ $(CFLAGS) -o main main.cpp LNS.cpp agents_loader.cpp tasks_loader.cpp

run: build
	./main 

clean:
	rm -f main
