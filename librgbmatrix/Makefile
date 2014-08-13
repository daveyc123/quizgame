# Ugh, need to have the freetype2 dir on the include path... add /usr/local/include/freetype2 
FT2DIR = -I/Volumes/xtools/arm-none-linux-gnueabi/arm-none-linux-gnueabi/sysroot/usr/include/freetype2 -I/usr/local/include/freetype2
CXXFLAGS = -fPIC -Wall -Wextra -O3 -g $(FT2DIR)
RM = rm -f
TARGET_LIB = librgbmatrix.a

SRCS = $(shell echo *.cc)
OBJS = $(SRCS:.cc=.o)

.PHONY: all
all: ${TARGET_LIB}

$(TARGET_LIB): $(OBJS)
	$(AR) -rs $@ $^

$(SRCS:.cc=.d):%.d:%.cc
	$(CXX) $(CXXFLAGS) -MM $< >$@

.PHONY: clean
clean:
	-${RM} ${TARGET_LIB} ${OBJS}
