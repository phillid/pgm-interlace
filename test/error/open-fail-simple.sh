#!/bin/sh

t="$0.tmp"

cp ../input/white.pgm $t
chmod -r $t

${EXECUTABLE} $t > /dev/null
ret=$?

rm $t

exit $ret
