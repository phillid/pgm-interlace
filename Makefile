all: pgm-interlace

pgm-interlace: pgm-interlace.o
	$(CC) -o $@ $^

%.o: %.c
	$(CC) -c -o $@ $< #-Wall -Wextra #-Werror

.PHONY: all clean clean-object
clean: clean-object
	rm -f pgm-interlace

clean-object:
	rm -fv *.o **/*.o
