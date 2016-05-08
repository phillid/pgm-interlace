# pgm-interlace
Vertically interlace *n* PGM frames, each one *height/n* pixels in width into
one square image with a width the same as the height.

The order in which the frames are specified on the command line is critical for
the reconstruction of the image. pgm-interlace has no way of telling in which
order a handful of pgm frames should be interlaced unless you tell it how to
stitch them together.
