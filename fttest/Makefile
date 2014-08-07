INCLUDES+=-I /opt/X11/include/freetype2
LIBS+=-L /opt/X11/lib
CFLAGS=-Wall -O3 -g $(INCLUDES) $(LIBS)
CXXFLAGS=-Wall -O3 -g $(INCLUDES) $(LIBS)
OBJECTS=fttest.o
BINARIES=fttest
LDFLAGS=-lfreetype

all : $(BINARIES)

fttest.o: fttest.c

fttest : $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJECTS) $(BINARIES)

