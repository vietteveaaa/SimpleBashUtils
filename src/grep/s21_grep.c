#include "s21_grep.h"

int main(int argc, char *argv[]) {
  struct flags fl = init();
  regex_t regex;
  int ind = parse(argc, argv, &fl, &regex);
  s21_grep(argc, argv, &regex, ind, fl);
  regfree(&regex);
  return 0;
}

void s21_grep(int argc, char *argv[], regex_t *regex, int ind,
              struct flags fl) {
  bool print_file_names = (ind < argc - 1);
  char *buffer;
  int match_count = 0;
  for (; ind < argc; ind++) {
    FILE *fp = fopen(argv[ind], "r");
    if (fp != NULL) {
      int line_num = 0;
      while ((buffer = read_line(fp)) != NULL) {
        if (fl.n || !fl.c) line_num++;
        int start = 0;
        bool found_match = false;
        while (true) {
          regmatch_t match;
          int status = regexec(regex, buffer + start, 1, &match, 0);
          if (fl.v) status = !status;
          if (status != 0) break;
          found_match = true;
          if (!fl.l && !fl.c) {
            if (print_file_names && !fl.h) printf("%s:", argv[ind]);
            if (fl.n) printf("%d:", line_num);
            if (fl.o) {
              for (int i = match.rm_so; i < match.rm_eo; i++) {
                printf("%c", buffer[start + i]);
              }
              printf("\n");
            } else {
              printf("%s\n", buffer);
              break;
            }
          }
          start += match.rm_eo;
        }
        free(buffer);
        if (found_match) match_count++;
        if (fl.l && found_match) {
          break;
        }
      }
      fclose(fp);

      if (fl.c && print_file_names) {
        printf("%s:%d\n", argv[ind], match_count);
      } else if (fl.c) {
        printf("%d\n", match_count);
      }
      if (fl.l && match_count > 0) {
        printf("%s\n", argv[ind]);
      }
      match_count = 0;
    } else {
      if (!fl.s) {
        printf("grep: %s: No such file or directory\n", argv[ind]);
      }
    }
  }
}

char *read_line(FILE *file) {
  char *buffer = malloc(INIT_BUF_SIZE * sizeof(char));
  if (buffer == NULL) return NULL;
  int position = 0;
  int current_size = INIT_BUF_SIZE;
  int ch;
  while ((ch = fgetc(file)) != '\n' && ch != EOF) {
    if (position == current_size - 1) {
      resize(&buffer, &current_size);
    }
    buffer[position++] = ch;
  }
  if (position == 0 && ch == EOF) {
    free(buffer);
    return NULL;
  }
  buffer[position] = '\0';
  return buffer;
}

void resize(char **buffer, int *current_size) {
  *current_size *= 2;
  char *tmp = realloc(*buffer, *current_size * sizeof(char));
  if (tmp != NULL) {
    *buffer = tmp;
  } else {
    printf("memory reallocation error");
    exit(1);
  }
}

struct flags init() {
  struct flags fl;
  fl.i = false;
  fl.v = false;
  fl.c = false;
  fl.l = false;
  fl.n = false;
  fl.h = false;
  fl.s = false;
  fl.o = false;
  fl.is_error = false;
  return fl;
}

int parse(int argc, char *argv[], struct flags *fl, regex_t *regex) {
  int opt;
  char *expr = NULL;
  while ((opt = getopt(argc, argv, "-:e:ivclnhsf:o")) != -1) {
    switch (opt) {
      case 'e':
        extend_regex(&expr, optarg);
        break;

      case 'i':
        fl->i = true;
        break;

      case 'v':
        fl->v = true;
        break;

      case 'c':
        fl->c = true;
        break;

      case 'l':
        fl->l = true;
        break;

      case 'n':
        fl->n = true;
        break;

      case 'h':
        fl->h = true;
        break;

      case 's':
        fl->s = true;
        break;

      case 'f':
        handle_f(&expr, optarg, *fl);
        break;

      case 'o':
        fl->o = true;
        break;

      case ':':
        printf("grep: option requires an argument -- %c", optopt);
        break;

      default: /* '?' */
        printf("grep: unknown option -- %c", optopt);
    }
  }
  int result;
  if (expr == NULL) {
    extend_regex(&expr, argv[optind]);
    result = optind + 1;
  } else {
    result = optind;
  }
  compile_regex(expr, regex, fl);
  free(expr);
  return result;
}

void compile_regex(char *expr, regex_t *regex, struct flags *fl) {
  int ret;
  if (fl->i) {
    ret = regcomp(regex, expr, REG_ICASE | REG_EXTENDED);
  } else {
    ret = regcomp(regex, expr, REG_EXTENDED);
  }
  if (ret != 0) {
    printf("regex comilation error");
    exit(1);
  }
}

void extend_regex(char **expr, char *arg) {
  if (*expr != NULL) {
    char *tmp =
        realloc(*expr, (strlen(arg) + strlen(*expr) + 2) * sizeof(char));
    if (tmp != NULL) {
      *expr = tmp;
      strcat(*expr, "|");
      strcat(*expr, arg);
    } else {
      printf("memory reallocation error");
      exit(1);
    }
  } else {
    *expr = malloc((strlen(arg) + 1) * sizeof(char));
    if (*expr != NULL) {
      strcpy(*expr, arg);
    } else {
      printf("memory allocation error");
      exit(1);
    }
  }
}

void handle_f(char **expr, char *arg, struct flags fl) {
  FILE *fp = fopen(arg, "r");
  if (fp == NULL) {
    if (!fl.s) printf("grep: %s: No such file or directory\n", arg);
    exit(1);
  } else {
    char *buffer;
    while ((buffer = read_line(fp)) != NULL) {
      extend_regex(expr, buffer);
      free(buffer);
    }
    fclose(fp);
  }
}
