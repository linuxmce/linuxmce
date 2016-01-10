CPPFLAGS=-I/usr/include/SDL -I../ControllerGen -I../DCE -I/usr/include/mysql -I..
CXXFLAGS=-pipe -ggdb3
LDLIBS=-lSDL -lSDL_ttf -lSDL_image -lSGE
sources=TextSplitter.cpp TextWrapper.cpp ../ControllerGen/Renderer.cpp ../DCE/StringUtils.cpp

test: $(sources:.cpp=.o) main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LDFLAGS) $(LDLIBS)
