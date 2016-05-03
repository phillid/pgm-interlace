# pgm-interlace
Vertically interlace *n* PGM frames, each one *height/n* pixels in width into
one square image with a width the same as the height.

By example, it would take two 2x4 images:

	Image A:
	  0 1
	1 A B
	2 C D
	3 E F
	4 G H

	Image B:
	  0 1
	1 I J
	2 K L
	3 M N
	4 O P

combining them into one single 4x4 image from the command `pgm-interlace imageA imageB`

	  0 1 2 3
	0 A I B J
	1 C K D L
	2 E M F N
	3 G O H P

The order in which the frames are specified on the command line is critical for
the reconstruction of the image. pgm-interlace has no way of telling in which
order a handful of pgm frames should be interlaced unless you tell it how to
stitch them together.
