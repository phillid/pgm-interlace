/*
 * pgm-deinterlace - combine separate, h-interlaced PGMs into one
 * Copyright (c) 2016 David Phillips <dbphillipsnz@gmail.com>
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

const char *pgm_magic = "P5";
const size_t pgm_magic_len = sizeof(pgm_magic) - sizeof('\0');

/* check header values are sane */
int check_sanity(long width, long height, long white, unsigned int clust_total)
{
	if (width <= 0 || height <= 0)
	{
		fprintf(stderr, "image dimensions must be positive\n");
		return 1;
	}

	if (white <= 0)
	{
		fprintf(stderr, "white value must be positive\n");
		return 1;
	}

	/* FIXME only support single-byte pixels, PGM supports 16-bit according
	 * to <http://netpbm.sourceforge.net/doc/pgm.html> */
	if (white > 255)
	{
		fprintf(stderr, "Only a maximum gray value < 256 is supported\n");
		return 1;
	}

	if (width * clust_total != height)
	{
		fprintf(stderr, "%d images of %ldx%ld cannot interlace to a square image %ldx%ld\n",
			clust_total, width, height, height, height);
		return 1;
	}
	return 0;
}

void read_whitespace(FILE *fd)
{
	char c = '\0';
	do
	{
		c = fgetc(fd);
		if (isspace(c))
			continue;

		switch (c)
		{
			case '#':
				/* suck up the current line of the file */
				while (c != '\n' && c != '\r' && c != EOF)
					c = fgetc(fd);
				break;
			case EOF:
				break;
			default:
				ungetc(c, fd);
				break;
		}
	} while (isspace(c) || c == '#');
}

int read_token(FILE *fd, char *token, size_t token_size, const char *allowable)
{
	char c = '\0';
	size_t t = 0;

	while (!feof(fd))
	{
		/* -1 to leave room for null terminator*/
		if (t >= token_size - 1)
		{
			fprintf(stderr, "read_token: token too large\n");
			return 1;
		}

		c = fgetc(fd);

		if (strchr(allowable, c) == NULL)
		{
			ungetc(c, fd);
			break;
		}
		token[t++] = c;
	}

	token[t] = '\0';

	return 0;
}

int read_positive_int(FILE *fd, char *token, size_t token_size)
{
	return read_token(fd, token, token_size, "0123456789");
}

/* parse the file's header and read fields into variables */
int parse_header(FILE *fd, char *magic, long *width, long *height, int *white)
{
	char token[32];
	read_token(fd, magic, sizeof(magic), pgm_magic);

	if (strncmp(magic, pgm_magic, pgm_magic_len) != 0)
	{
		fprintf(stderr, "magic number does not check out, stopping.\n");
		return 1;
	}

	read_whitespace(fd);

	if (read_positive_int(fd, token, sizeof(token)) != 0)
	{
		fprintf(stderr, "Error reading width\n");
		return 1;
	}
	*width = atol(token);
	read_whitespace(fd);

	if (read_positive_int(fd, token, sizeof(token)))
	{
		fprintf(stderr, "Error reading height\n");
		return 1;
	}
	*height = atol(token); /* size == height */
	read_whitespace(fd);

	if (read_positive_int(fd, token, sizeof(token)))
	{
		fprintf(stderr, "Error reading white value\n");
		return 1;
	}
	*white = atol(token);

	/* standard dictates one whitespace character. This character must be
	 * whitespace or EOF because of token parsing logic */
	if (fgetc(fd) == EOF)
	{
		fprintf(stderr, "Premature end of header\n");
		return 1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	int i = 0;
	unsigned int x = 0;
	unsigned int y = 0;
	long width = 0;
	long size = 0;
	long new_width = 0;
	long new_size = 0;
	int white, new_white;
	int clust_total = argc-1;
	FILE **f = NULL;
	char magic[3];
	char new_magic[3];

	if (argc == 1)
	{
		fprintf(stderr, "%s [image 0] [image[1] ...\n", argv[0]);
		return 1;
	}

	if ( (f = malloc(sizeof(FILE*)*(clust_total)) ) == NULL)
	{
		perror("malloc");
		return 1;
	}

	for (i = 1; i < argc; i++)
	{
		if ((f[i-1] = fopen(argv[i], "r")) == NULL)
		{
			perror("fopen");

			/* close those files already opened */
			i--;
			for ( ; i > 1; i--)
				fclose(f[i-1]);
			return 1;
		}
	}

	/* read the first file's header and check that the values are sane */
	if (parse_header(f[0], magic, &width, &size, &white) != 0)
		return 1;

	if (check_sanity(width, size, white, clust_total) != 0)
		return 1;

	/* check that header values all agree */
	for (i = 1; i < clust_total; i++)
	{
		parse_header(f[i], new_magic, &new_width, &new_size, &new_white);
		if (strcmp(magic, new_magic) != 0
		    || width != new_width
		    || size != new_size
		    || white != new_white)
		{
			fprintf(stderr, "'%s' doesn't have identical header to '%s', stop\n", argv[i+1], argv[1]);
			return 1;
		}
	}

	fprintf(stderr, "Full image size will be %ldx%ld, using %d images\n", size, size, clust_total);

	/* Output PGM Header */
	printf("%s\n%ld\n%ld\n%d\n", pgm_magic, size, size, white);

	/* FIXME use a buffer
	 * FIXME check for EOF */
	for (y = 0; y < size; y++)
		for (x = 0; x < size; x++)
			putchar(fgetc(f[x%clust_total]));

	/* close all input files */
	for (i = 1; i < argc; i++)
		fclose(f[i-1]);

	free(f);
	return 0;
}
