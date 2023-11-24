#define _GNU_SOURCE
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct flags {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
};
void fflag_write(int *error, char ***regs, int *regs_len);
void duo_parser(char *argv[], int argc, int *error, struct flags *flags,
                char ***regs, int *regs_len);
void compile(regex_t **patterns, char **regs, int *error, struct flags *flags,
             int *regs_len);
int my_parser(int argc, char *argv[], struct flags *flags, int *error,
              regex_t **patterns);
void cmprr(regex_t *patterns, struct flags *flags, FILE *trg, int j,
           char *argv[], int argc, int *error, int patt_len);
void regs_write(char ***regs, char *str, int *regs_len);
void output(struct flags *flags, char *argv[], int argc, regex_t *patterns,
            int *error, int patt_len);
void print_result(struct flags *flags, char **result, int cntr, int *str_nums,
                  char *argv[], int argc, int j, int *error);
void flag_o(regex_t *patterns, char *buffer, char ***result, int patt_len,
            int *cntr, int **str_nums, int strnum);

int main(int argc, char **argv) {
  regex_t *patterns = (regex_t *)calloc(1, sizeof(regex_t));
  struct flags flags = {0};
  flags.i = REG_NEWLINE;
  int error = 0;
  int patt_len = my_parser(argc, argv, &flags, &error, &patterns);
  output(&flags, argv, argc, patterns, &error, patt_len);
  for (int i = 0; i < patt_len; i++) {
    regfree(&patterns[i]);
  }
  free(patterns);
  return error;
}

int my_parser(int argc, char *argv[], struct flags *flags, int *error,
              regex_t **patterns) {
  static struct option long_flags[] = {{0, 0, 0, 0}};
  int opt;
  char **regs = (char **)calloc(1, sizeof(char *));
  int regs_len = 0;
  int option_index;
  opterr = 0;
  while ((opt = getopt_long(argc, argv, "e:ivclnhsf:o", long_flags,
                            &option_index)) != -1) {
    switch (opt) {
      case 'e':
        flags->e = 1;
        regs_write(&regs, optarg, &regs_len);
        break;
      case 'i':
        flags->i |= REG_ICASE;
        break;
      case 'v':
        flags->v = 1;
        break;
      case 'c':
        flags->c = 1;
        break;
      case 'l':
        flags->l = 1;
        break;
      case 'n':
        flags->n = 1;
        break;
      case 'h':
        flags->h = 1;
        break;
      case 's':
        flags->s = 1;
        break;
      case 'f':
        flags->f = 1;
        fflag_write(error, &regs, &regs_len);
        break;
      case 'o':
        flags->o = 1;
        break;
      case '?':
        fprintf(stderr, "grep: invalid option -- %c\n", optopt);
        *error = 1;
        break;
    }
  }
  duo_parser(argv, argc, error, flags, &regs, &regs_len);
  compile(patterns, regs, error, flags, &regs_len);
  for (int i = 0; i < regs_len; i++) {
    free(regs[i]);
  }
  free(regs);
  if (flags->c || flags->v || flags->l) {
    flags->o = 0;
  }
  return regs_len;
}

void regs_write(char ***regs, char *str, int *regs_len) {
  (*regs)[*regs_len] = (char *)calloc(strlen(str) + 1, sizeof(char));
  for (size_t j = 0; j < strlen(str); j++) {
    (*regs)[*regs_len][j] = str[j];
  }
  (*regs_len)++;
  *regs = (char **)realloc(*regs, (*regs_len + 1) * sizeof(char **));
  (*regs)[*regs_len] = NULL;
}

void compile(regex_t **patterns, char **regs, int *error, struct flags *flags,
             int *regs_len) {
  for (int i = 0; i < *regs_len; i++) {
    if (regcomp((*patterns) + i, regs[i], flags->i) != 0) (*error)++;
    *patterns = (regex_t *)realloc(*patterns, (i + 2) * (sizeof(regex_t)));
  }
}

void duo_parser(char *argv[], int argc, int *error, struct flags *flags,
                char ***regs, int *regs_len) {
  if (argc <= 2) {
    (*error)++;
  } else if (argc - optind >= 2) {
    if (flags->e == 0 && flags->f == 0) {
      regs_write(regs, argv[optind], regs_len);
      optind++;
    }
  }
}

void fflag_write(int *error, char ***regs, int *regs_len) {
  FILE *fp;
  if ((fp = fopen(optarg, "r")) == NULL) {
    fprintf(stderr, "grep: %s: No such file or directory", optarg);
    (*error)++;
  } else {
    ssize_t line = 0;
    char *buffer = NULL;
    size_t len = 0;
    while ((line = getline(&buffer, &len, fp)) != -1) {
      if (buffer[strlen(buffer) - 1] == '\n') buffer[strlen(buffer) - 1] = '\0';
      regs_write(regs, buffer, regs_len);
    }
    free(buffer);
    fclose(fp);
  }
}

void cmprr(regex_t *patterns, struct flags *flags, FILE *trg, int j,
           char *argv[], int argc, int *error, int patt_len) {
  int cntr = 0;
  char **result = (char **)calloc(1, sizeof(char *));
  int *str_nums = (int *)calloc(1, sizeof(int));
  char *buffer = NULL;
  size_t len = 0;
  int strnum = 1;
  ssize_t line = 0;
  while ((line = getline(&buffer, &len, trg)) != -1) {
    if (buffer[strlen(buffer) - 1] == '\n') {
      buffer[strlen(buffer) - 1] = '\0';
    }
    if (flags->o) {
      flag_o(patterns, buffer, &result, patt_len, &cntr, &str_nums, strnum);
    } else
      for (int i = 0; i < patt_len; i++) {
        if (regexec(patterns + i, buffer, 0, NULL, 0) == flags->v) {
          str_nums[cntr] = strnum;
          regs_write(&result, buffer, &cntr);
          str_nums = (int *)realloc(str_nums, (cntr + 1) * sizeof(int));
          break;
        }
      }
    strnum++;
  }
  print_result(flags, result, cntr, str_nums, argv, argc, j, error);
  free(str_nums);
  free(buffer);
  for (int i = 0; i < cntr; i++) {
    free(result[i]);
  }
  free(result);
}

void print_result(struct flags *flags, char **result, int cntr, int *str_nums,
                  char *argv[], int argc, int j, int *error) {
  if ((flags->e && !flags->n && (!flags->l || !flags->c)) ||
      (flags->o && !flags->n) || (flags->f && !flags->n) ||
      (!flags->e && !flags->o && !flags->f && !flags->n && !flags->l &&
       !flags->c)) {
    for (int i = 0; i < cntr; i++) {
      if (argc - optind != 1 && !flags->h) {
        if (i != 0 && str_nums[i] == str_nums[i - 1]) {
          printf("%s\n", result[i]);
        } else {
          printf("%s:%s\n", argv[j], result[i]);
        }
      } else {
        printf("%s\n", result[i]);
      }
    }
  }
  if (flags->c == 1 && flags->l == 0) {
    if (flags->h != 1) {
      if (argc - optind != 1) {
        printf("%s:%d\n", argv[j], cntr);
      } else {
        printf("%d\n", cntr);
      }
    } else {
      printf("0\n");
      (*error)++;
    }
  }
  if (flags->l == 1 && flags->c == 0 && cntr != 0) {
    printf("%s\n", argv[j]);
  }
  if (flags->n == 1 && flags->l == 0 && flags->c == 0 && flags->o == 0) {
    for (int i = 0; i < cntr; i++) {
      if (flags->h != 1 && argc - optind != 1 && cntr != 0) {
        printf("%s:", argv[j]);
      }
      printf("%d:%s\n", str_nums[i], result[i]);
    }
  }
  if (flags->c == 1 && flags->l == 1) {
    if (cntr == 0) {
      if (!flags->h) {
        printf("%s:", argv[j]);
      }
      printf("0\n");
    } else {
      if (!flags->h) {
        printf("%s:", argv[j]);
      }
      printf("1\n");
      printf("%s\n", argv[j]);
    }
  }
  if (flags->o == 1 && flags->n == 1) {
    for (int i = 0; i < cntr; i++) {
      if (argc - optind != 1 && !flags->h) {
        printf("%s:%d:%s\n", argv[j], str_nums[i], result[i]);
      } else {
        printf("%d:%s\n", str_nums[i], result[i]);
      }
    }
  }
}

void output(struct flags *flags, char *argv[], int argc, regex_t *patterns,
            int *error, int patt_len) {
  FILE *trg = NULL;
  for (int j = optind; j < argc; j++) {
    if ((trg = fopen(argv[j], "r")) == NULL) {
      if (!flags->s) {
        fprintf(stderr, "./s21_grep: %s: No such file or directory\n", argv[j]);
        (*error)++;
      }
    } else {
      cmprr(patterns, flags, trg, j, argv, argc, error, patt_len);
      fclose(trg);
    }
  }
}

void flag_o(regex_t *patterns, char *buffer, char ***result, int patt_len,
            int *cntr, int **str_nums, int strnum) {
  regmatch_t pmatch[1];
  int step = 0;
  char *line = NULL;
  int check = 0;
  for (int i = 0; i < patt_len; i++) {
    while ((check = regexec(patterns + i, buffer + step, 1, pmatch, 0)) != 1) {
      line = strndup(buffer + step + pmatch[0].rm_so,
                     pmatch[0].rm_eo - pmatch[0].rm_so);
      (*str_nums)[*cntr] = strnum;
      regs_write(result, line, cntr);
      free(line);
      step += pmatch[0].rm_eo;
      *str_nums = (int *)realloc(*str_nums, (*cntr + 1) * sizeof(int));
    }
  }
}
