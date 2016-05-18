#!/bin/sh

fail()
{
	echo -e '[\e[1;31mFAIL\e[0m] '$i $@
}


xfail()
{
	echo -e '[\e[1;35mXFAIL\e[0m] '$i $@
}

pass()
{
	echo -e '[\e[0;32mPASS\e[0m] '$i $@
}

#########################
# tests expected to pass

cd $(dirname $0)
export EXECUTABLE="$PWD/../pgm-interlace"

for i in {pass,error,xfail}/*.sh ; do
	test_type=$(dirname $i)
	pushd $test_type > /dev/null

	name=$(basename "${i/.sh/}")
	expected="$name.pgm.expected"
	out="$name.pgm.out"
	log="$name.stderr.log"
	log_expected="$log.expected"
	./$(basename $i) 2> "$log"
	result=$?
	if [ -f "$expected" ] ; then
		if ! diff "$expected" "$out" ; then
			fail "output file doesn't match expectation"
			popd > /dev/null
			continue
		fi
	fi
	if [ -f "$log_expected" ] ; then
		if ! diff "$log_expected" "$log" ; then
			fail "stderr doesn't match expectation"
			popd > /dev/null
			continue
		fi
	fi
	case $test_type in
		pass)
			if [ $result -eq 0 ] ; then
				pass
			else
				fail
			fi
		;;
		error)
			if [ $result -ne 0 ] ; then
				pass
			else
				fail
			fi
		;;
		xfail)
			if [ $result -ne 0 ] ; then
				xfail
			else
				fail
			fi
		;;
		*)
			echo Unexpected test type "$test_type", bailing
			exit 1
		;;
	esac

	rm -f "$out" # "$log"
	popd > /dev/null
done
