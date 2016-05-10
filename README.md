# pgm-interlace
Vertically interlace *n* PGM frames, each one *width/n* pixels in height into
one square image.

## Syntax

	pgm-interlace [frame 0] [frame 1] [frame 2] ...

The order in which the frames are specified on the command line is critical for
the reconstruction of the image. pgm-interlace has no way of telling in which
order a handful of pgm frames should be interlaced unless you tell it how to
stitch them together.
