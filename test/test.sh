#!/bin/sh

fail()
{
	echo -e '[\e[1;31mFAIL\e[0m] '$i
}

pass()
{
	echo -e '[\e[0;32mPASS\e[0m] '$i
}


#########################
# tests expected to pass

cd $(dirname $0)
export EXECUTABLE="$PWD/../pgm-interlace"



pushd pass >/dev/null
for i in *.sh ; do
	name=${i/.sh/}
	expected="$name.pgm.expected"
	out="$name.pgm.out"
	./$i
	diff $expected $out
	if [ $? -ne 0 ] ; then
		fail
		exit 1
	fi
	pass
	rm $out
done
popd > /dev/null
