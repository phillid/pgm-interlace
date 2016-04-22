/*
 * pgm-deinterlace - combine separate, h-interlaced PGMs into one
 * Copyright (c) 2015 David Phillips <dbphillipsnz@gmail.com>
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

int main(int argc, char **argv)
{
	unsigned int i, x, y, size, header_size;
	unsigned int clust_total = argc-1;
	char buffer[4096];
	FILE **f = NULL;

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

	/* Fetch sizes from */
	/* FIXME doesn't check magic value */
	/* FIXME extremely hackish, relies on specific whitespace format */
	fgets(buffer, sizeof(buffer), f[0]);
	fgets(buffer, sizeof(buffer), f[0]);
	fgets(buffer, sizeof(buffer), f[0]);
	size = atoi(buffer);
	fprintf(stderr, "Full image size will be %dx%d, using %d images\n", size, size, clust_total);
	fgets(buffer, sizeof(buffer), f[0]);
	header_size = ftell(f[0]);

	for (i = 1; i < argc; i++)
		fseek(f[i-1], header_size, SEEK_SET);

	/* Output PGM Header */
	printf("P5\n%d\n%d\n255\n", size, size);

	/* FIXME use a buffer
	 * FIXME check for EOF */
	for (y = 0; y < size; y++)
		for (x = 0; x < size; x++)
			putchar(fgetc(f[x%clust_total]));


	for (i = 1; i < argc; i++)
		fclose(f[i-1]);

	free(f);
	return 0;
}
