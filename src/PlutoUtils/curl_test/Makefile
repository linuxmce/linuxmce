
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -DDEBUG
LDFLAGS = 
BASELDLIBS = -lcurl

MYNAME = HttpUtilsTest
VERSION = 2.0.0
MYDEVTEMPLATEID = 8mplateID

sources = curl_test.cpp ../HttpUtils.cpp

CXXFLAGS += -g -O0
CPPFLAGS += 
LDFLAGS += 
XTRALDLIBS = 

LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make all'"

all: bin

bin: HttpUtilsTest

HttpUtilsTest: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $+ $(LDFLAGS) $(LDLIBS)

clean: 
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) HttpUtilsTest.bin > /dev/null

 