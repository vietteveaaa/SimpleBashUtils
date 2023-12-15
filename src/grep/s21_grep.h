#ifndef SRC_GREP_S21_GREP
#define SRC_GREP_S21_GREP

#define INIT_BUF_SIZE 256

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { false, true } bool;

struct flags {
  bool i;
  bool v;
  bool c;
  bool l;
  bool n;
  bool h;
  bool s;
  bool o;
  bool is_error;
};

void s21_grep(int argc, char *argv[], regex_t *regex, int ind, struct flags fl);

char *read_line(FILE *file);
void resize(char **buffer, int *current_size);
struct flags init();
int parse(int argc, char *argv[], struct flags *fl, regex_t *regex);
void compile_regex(char *expr, regex_t *regex, struct flags *fl);
void extend_regex(char **expr, char *arg);
void handle_f(char **expr, char *arg, struct flags fl);

#endif  // SRC_GREP_S21_GREP