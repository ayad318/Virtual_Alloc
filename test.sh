#!/bin/bash

# Trigger all your test cases with this script

echo "##############################"
echo "### Testing virtual_alloc.c ##"
echo "##############################"

count=0 # number of test cases run so far

# Assume all `.in` and `.out` files are located in a separate `tests` directory
for test in tests_file/*.exp; do
	name=$(basename $test .exp)
	expected=tests_file/$name.exp
	output="tests_file/$name.out" 
	eval ./tests | diff $output  $expected || echo "Test $name: failed!\n"
	count=$((count+1))
done

echo "Finished running $count tests!"
# Trigger all your test cases with this script