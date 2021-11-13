#!/bin/bash
TEST="tests_main/tests/test"
ANS="tests_main/keys/test"
TESTER=tests_main/tester
MAIN=main
TESTS_AMOUNT=11

for i in $(seq 0 $TESTS_AMOUNT)
do
    case $i in
        10|11) code=2;;
        1|2) code=0;;
        3) code=3;;
        *) code=1;;
    esac
    echo -n $i": "
    $TESTER $MAIN $TEST$i $ANS$i"_keys" $code
done
