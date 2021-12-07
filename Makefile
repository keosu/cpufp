
all: 
	mkdir -p build
	g++ -O3 src/cpuid_x86.cpp -o ./build/cpuid_x86 
	./build/cpuid_x86 > gen.sh
	chmod 744 gen.sh
	./gen.sh
run:
	./cpufp
	
clean:
	rm -rf *.o cpufp cpuid_x86 gen.sh build