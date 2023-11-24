#!/bin/bash

SUCCESS=0
FAIL=0
COUNTER=0
DIFF_RES=""


declare -a extra=(
"-e for s21_grep.c Makefile"
"-i for s21_grep.c Makefile"
"-v for s21_grep.c Makefile"
"-c for s21_grep.c Makefile"
"-l for s21_grep.c Makefile"
"-n for s21_grep.c Makefile"
"-h for s21_grep.c Makefile"
"-s for s21_grep.c Makefile"
"-o for s21_grep.c Makefile"
"-f patternForTest.txt s21_grep.c Makefile"
"-o -f patternForTest.txt s21_grep.c Makefile"
"-i -e for s21_grep.c Makefile"
"-i -v for s21_grep.c Makefile"
"-i -c for s21_grep.c Makefile"
"-i -l for s21_grep.c Makefile"
"-i -n for s21_grep.c Makefile"
"-i -f patternForTest.txt s21_grep.c Makefile"
"-i -e for s21_grep.c Makefile"
"-c -i -v for s21_grep.c Makefile"
"-i -l -c for s21_grep.c Makefile"
"-i -n -l for s21_grep.c Makefile"
"-c -i -v for s21_grep.c Makefile"
"-i -l -c -e for s21_grep.c Makefile"
"-i -n -v -l for s21_grep.c Makefile"
"-e for -e grep s21_grep.c Makefile"
"-o -e for -e grep s21_grep.c Makefile"
"-invl for s21_grep.c Makefile"
"-ino for s21_grep.c Makefile"
"-inh for s21_grep.c Makefile"
"-inoh -e for s21_grep.c Makefile"
"-ioh for s21_grep.c Makefile"
"-inoh for s21_grep.c Makefile"
"-inh -f patternForTest.txt s21_grep.c Makefile"
)

testing()
{
    ./s21_grep $@ > test_s21_grep.log
#    valgrind --leak-check=full --show-leak-kinds=all ./s21_grep $@
    grep $@ > test_sys_grep.log
    DIFF_RES="$(diff -s test_s21_grep.log test_sys_grep.log)"
    (( COUNTER++ ))
    if [ "$DIFF_RES" == "Files test_s21_grep.log and test_sys_grep.log are identical" ]
    then
      (( SUCCESS++ ))
      echo "|$FAIL|$SUCCESS|$COUNTER | success | grep $@"
    else
      (( FAIL++ ))
      echo "$FAIL|$SUCCESS|$COUNTER | fail | grep $@"
    fi
    rm -rf test_s21_grep.log test_sys_grep.log
}

# специфические тесты
for i in "${extra[@]}"
do
    var="-"
    testing $i
done


echo "\033[31mFAIL: $FAIL\033[0m"
echo "\033[32mSUCCESS: $SUCCESS\033[0m"
echo "ALL: $COUNTER"
