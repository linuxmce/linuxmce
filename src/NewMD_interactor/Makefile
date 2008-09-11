PROGRAMS=interactor interactor-server
PROGRAMSDST=../bin/interactor ../bin/interactor-server
ROOT = ..
CPPFLAGS=-g -ggdb3 -W -Wall

all: ../bin/interactor ../bin/interactor-server

interactor: interactor.o
	$(CC) $(CPPFLAGS) -o interactor $+

interactor-server: interactor-server.o GetCommandOutput.o
	$(CXX) $(CPPFLAGS) -o interactor-server $+

../bin/interactor: interactor
	cp $< $@

../bin/interactor-server: interactor-server
	cp interactor-server $@

clean:
	rm -f $(PROGRAMS) $(PROGRAMSDST) *.o

include $(ROOT)/PlutoUtils/make.rules
