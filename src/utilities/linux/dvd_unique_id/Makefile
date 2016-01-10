sources = main.cpp

all: bin

bin: dvd_unique_id

dvd_unique_id:
	$(CXX) -o dvd_unique_id main.cpp -ldvdread
	cp $@ ../../../bin

clean:
	rm -f *.o *.d dvd_unique_id

