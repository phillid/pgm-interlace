#!/bin/sh

t="$0.tmp"

for i in "$t."{1,2,3} ; do
	cp ../input/white.pgm "$i"
done
chmod -r "$t.3"

${EXECUTABLE} $t.{1,2,3} > /dev/null
ret=$?

rm "$t."{1,2,3}

exit $ret
