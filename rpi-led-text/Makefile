FT2DIR = -I/usr/include/freetype2
CXXFLAGS = -Wall -Wextra -O3 -g $(FT2DIR) -L../librgbmatrix -I../librgbmatrix
LIBS = -lrt -lm -lpthread -lrgbmatrix -lfreetype -lpng -lz
RM = rm -f
TARGET = led-matrix

SRCS = $(shell echo *.cc)
OBJS = $(SRCS:.cc=.o)

.PHONY: all
all: ${TARGET}

$(TARGET): $(OBJS)
	$(CXX) ${CXXFLAGS} -o $@ $^ $(LIBS)

$(SRCS:.cc=.d):%.d:%.cc
	$(CXX) $(CXXFLAGS) -MM $< >$@

.PHONY: clean
clean:
	-${RM} ${TARGET} ${OBJS}
