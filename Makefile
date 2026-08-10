obj = build/main.o build/piece.o

build/chess : $(obj)
	g++ $(obj) -o build/chess

build/main.o : main.cpp
	mkdir -p build/
	g++ -c main.cpp -o build/main.o

build/piece.o : src/piece.cpp
	g++ -c src/piece.cpp -o build/piece.o

clean:
	rm -rf build/