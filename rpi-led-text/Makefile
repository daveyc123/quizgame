INCLUDES=-I /Volumes/xtools/arm-none-linux-gnueabi/include -I /Volumes/xtools/arm-none-linux-gnueabi/include/freetype2
LIBS=-L /Volumes/xtools/arm-none-linux-gnueabi/lib
CFLAGS=-Wall -O3 -g $(INCLUDES) $(LIBS)
CXXFLAGS=-Wall -O3 -g $(INCLUDES) $(LIBS)
OBJECTS=main.o gpio.o led-matrix.o thread.o
BINARIES=led-matrix
LDFLAGS=-lrt -lm -lpthread -lfreetype -lpng -lz

all : $(BINARIES)

led-matrix.o: led-matrix.cc led-matrix.h
main.o: led-matrix.h

led-matrix : $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJECTS) $(BINARIES)
