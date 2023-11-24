CC=gcc
CFLAGS=-Wall -Wextra -Werror -O2 -std=c11
TEST=-fsanitize=address -g -pedantic

all: build

build: s21_cat

s21_cat: s21_cat.o
	${CC} ${CFLAGS} s21_cat.o -o s21_cat

s21_cat_test: s21_cat.o
	${CC} ${CLFAGS} ${TEST} s21_cat.o -o s21_cat

s21_cat.o: s21_cat.c
	${CC} -c s21_cat.c

clean:
	rm -rf *.o s21_cat

rebuild: clean s21_cat

.PHONY: all clean rebuild
