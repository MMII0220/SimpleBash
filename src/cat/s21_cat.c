#include "s21_cat.h"

int main(int argc, char **argv) {
  struct options opt = {0};
  parser_flag(argc, argv, &opt);
  while (optind < argc) {
    reader(argv, &opt);
    optind++;
  }
  return 0;
}

void parser_flag(int argc, char **argv, struct options *opt) {
  int rez = 0, long_opt = 0;
  const char *short_options = "+benstvTE";
  static struct option long_options[] = {{"number-nonblank", 0, NULL, 'b'},
                                         {"number", 0, NULL, 'n'},
                                         {"squeeze-blank", 0, NULL, 's'},
                                         {NULL, 0, NULL, 0}};
  while (-1 != (rez = getopt_long(argc, argv, short_options, long_options,
                                  &long_opt))) {
    switch (rez) {
      case 'b':
        opt->b = 1;
        break;
      case 'e':
        opt->e = 1;
        opt->v = 1;
        break;
      case 'E':
        opt->e = 1;
        break;
      case 'n':
        opt->n = 1;
        break;
      case 's':
        opt->s = 1;
        break;
      case 't':
        opt->t = 1;
        opt->v = 1;
        break;
      case 'T':
        opt->t = 1;
        break;
      case 'v':
        opt->v = 1;
        break;
      default:
        fprintf(stderr, "usage: [-benstv] [file ...]");
        exit(1);
    }
  }
}

void reader(char **argv, struct options *opt) {
  FILE *fl = fopen(argv[optind], "r");
  if (fl) {
    int cnt = 1, cnt_f = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, fl)) != -1) {
      int str_len = (int)read, idx = 0;
      if (opt->b) {
        print_b(&cnt, line);
      } else if (opt->n) {
        print_n(cnt);
        cnt++;
      }
      for (; idx < str_len; idx++) {
        if ('\t' == line[idx])
          print_t(opt, idx, line);
        else if ('\n' == line[idx])
          print_e(opt, idx, line);
        else if ((-97 >= (int)line[idx] && -128 <= (int)line[idx]) ||
                 (0 <= (int)line[idx] && 31 >= (int)line[idx]) ||
                 127 == (int)line[idx])
          print_v(opt, idx, line);
        else
          printf("%c", line[idx]);
      }
      print_s(opt, &line, read, len, fl, &cnt_f);
    }
    if (line) {
      free(line);
    }
    fclose(fl);
  } else {
    fprintf(stderr, "cat: %s: No such file or directory", argv[optind]);
  }
}

void print_b(int *cnt, char const *line) {
  if ('\n' != line[0]) {
    printf("%6d\t", *cnt);
    *cnt += 1;
  }
}

void print_e(struct options *opt, int idx, char *line) {
  ('\n' == line[idx] && opt->e) ? printf("$\n") : printf("%c", line[idx]);
}

void print_n(int cnt) { printf("%6d\t", cnt); }

void print_t(struct options *opt, int idx, char *line) {
  ('\t' == line[idx] && opt->t) ? printf("^I") : printf("%c", line[idx]);
}

void print_v(struct options *opt, int idx, char *line) {
  if (0 <= (int)line[idx] && 31 >= (int)line[idx] && opt->v) {
    printf("^%c", (char)((int)line[idx] + 64));
  } else if ((127 == (int)line[idx]) && opt->v) {
    printf("^%c", (char)((int)line[idx] - 64));
  } else if (-97 >= (int)line[idx] && -128 <= (int)line[idx] && opt->v) {
    printf("M-^%c", (char)((int)line[idx] + 192));
  } else {
    printf("%c", line[idx]);
  }
}

void print_s(struct options *opt, char **line, ssize_t read, size_t size,
             FILE *file, int *cnt_f) {
  if (opt->s && '\n' == *line[0]) {
    while ('\n' == *line[0]) {
      if (-1 == (read = getline(line, &size, file))) {
        *cnt_f = *cnt_f + 1;
        break;
      }
    }
    if (*cnt_f == 0) fseek(file, -strlen(*line), SEEK_CUR);
  }
}
