#ifndef SRC_s21_cat_H_
#define SRC_s21_cat_H_
#define _POSIX_C_SOURCE 200809L

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct options {
  unsigned char b : 1;  // 0 1
  unsigned char e : 1;
  unsigned char n : 1;
  unsigned char s : 1;
  unsigned char t : 1;
  unsigned char v : 1;
};

int cnt_symbol(char const *line);
void parser_flag(int argc, char **argv, struct options *opt);
void print_b(int *cnt, const char *line);
void print_e(struct options *opt, int idx, char *line);
void print_n(int cnt);
void print_s(struct options *opt, char **line, ssize_t read, size_t size,
             FILE *file, int *cnt_f);
void print_t(struct options *opt, int idx, char *line);
void print_v(struct options *opt, int idx, char *line);
void reader(char **argv, struct options *opt);

#endif  // SRC_s21_cat_H_
