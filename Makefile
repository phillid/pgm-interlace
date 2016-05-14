
.POSIX:
include config.mk

all: pgm-interlace

pgm-interlace: pgm-interlace.o

test:
	./test/test.sh

.PHONY: all clean clean-object test
clean: clean-object
	rm -f pgm-interlace

clean-object:
	rm -fv *.o
