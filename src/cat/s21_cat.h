#ifndef SRC_CAT_S21_CAT
#define SRC_CAT_S21_CAT

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_ALIGN_NUM 6

typedef enum { false, true } bool;

struct flags {
  bool b;
  bool e;
  bool n;
  bool s;
  bool t;
  bool v;
  bool is_error;
};

void s21_cat(int argc, char *argv[]);

struct flags init();
void parse_args(int argc, char *argv[]);
void output(FILE *fp, const struct flags fl);
void enumerated_output(char *contents, const struct flags fl);
int count(char *contents, bool (*condition)(char *));
bool b_cond(char *contents);
bool n_cond(char *contents);
bool is_empty_line(char *start);
bool is_empty_line_backtrack(char *string_start, char *end);
int count_digits(int num);
void shift(char *contents, int begin, int dir);
void modify_e(char *contents);
void modify_s(char *contents);
void modify_t(char *contents);
void modify_v(char *contents);

#endif  // SRC_CAT_S21_CAT