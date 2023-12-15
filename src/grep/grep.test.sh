#!/bin/bash

TEST_TEMPLATE="../common/__test__/test_template.txt"
TEST_TMP1="tmp1"
TEST_TMP2="tmp2"
Red=$'\033[0;31m'
Green=$'\033[0;32m'
Purple=$'\033[0;35m'
NC=$'\033[0m'

make all

SUCCESS_COUNT=0
FAIL_COUNT=0

function fn {
	grep -e ret -f $TEST_TEMPLATE $1 $2 > $TEST_TMP1
	./s21_grep -e ret -f $TEST_TEMPLATE $1 $2 > $TEST_TMP2
	cmp $TEST_TMP1 $TEST_TMP2
	if  [ $? -eq 0 ];
	then
		echo "${Purple}Command: grep -e ret -f $TEST_TEMPLATE $1 $2${NC}"
		echo "${Green}SUCCESS${NC}";
		SUCCESS_COUNT=$((SUCCESS_COUNT + 1))
	else
		echo "${Purple}Command: grep -e ret -f $TEST_TEMPLATE $1 $2${NC}"
		echo "${Red}FAIL${NC}";
		FAIL_COUNT=$((FAIL_COUNT + 1))
	fi
	rm $TEST_TMP1
	rm $TEST_TMP2
}

# option testing array
options=("-i" "-v" "-c" "-l" "-n" "-h" "-s" "-o" "-iv" "-in" "-il" "-ic" "-ih" "-is" "-io" "-ln" "-lh" "-ls" "-lo" "-nh" "-ns" "-no" "-hs" "-ho" "-so"  "-cn" "-ch" "-cs" "-co" "-cl")
files=("Makefile" "../common/__test__/test_0_grep.txt" "../common/__test__/test_1_grep.txt" "../common/__test__/test_2_grep.txt" "../common/__test__/test_3_grep.txt" "../common/__test__/test_4_grep.txt" "../common/__test__/test_5_grep.txt" "../common/__test__/test_6_grep.txt")
# "-vc" "-vl" "-vn" "-vh" "-vs" "-vo"
for ix in ${!options[@]}
do
	for x in ${!files[@]}
	do
  		fn ${options[$ix]} ${files[$x]}
	done
done

make clean

echo "${Green}Success: $SUCCESS_COUNT${NC}"
echo "${Red}Fail: $FAIL_COUNT${NC}"