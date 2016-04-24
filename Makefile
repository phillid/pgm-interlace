
.POSIX:
include config.mk

all: pgm-interlace

pgm-interlace: pgm-interlace.o

.PHONY: all clean clean-object
clean: clean-object
	rm -f pgm-interlace

clean-object:
	rm -fv *.o
