#include "s21_cat.h"

int main(int argc, char *argv[]) {
  switch (argc) {
    case 1:;
      printf("Arguments required");
      break;
    default:;
      parse_args(argc, argv);
      break;
  }
  return 0;
}

struct flags init() {
  struct flags fl;
  fl.b = false;
  fl.e = false;
  fl.n = false;
  fl.s = false;
  fl.t = false;
  fl.v = false;
  fl.is_error = false;
  return fl;
}

void parse_args(int argc, char *argv[]) {
  struct flags fl = init();
  int opt;
  static struct option long_options[] = {
      {"number-nonblank", no_argument, 0, 'b'},
      {"number", no_argument, 0, 'n'},
      {"squeeze-blank", no_argument, 0, 's'},
      {0, 0, 0, 0}};

  while ((opt = getopt_long(argc, argv, "benstE:T:v", long_options, NULL)) !=
         -1) {
    switch (opt) {
      case 'b':
        fl.b = true;
        break;
      case 'e':
        fl.e = true;
        fl.v = true;
        break;
      case 'n':
        fl.n = true;
        break;
      case 's':
        fl.s = true;
        break;
      case 't':
        fl.t = true;
        fl.v = true;
        break;
      case 'E':
        fl.e = true;
        break;
      case 'T':
        fl.t = true;
        break;
      case 'v':
        fl.v = true;
        break;
      default:
        printf("Usage: %s [-benstE:T:v] [file...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }
  int ind = optind;
  for (int i = ind; i < argc; i++) {
    FILE *fp = fopen(argv[i], "rb");
    if (fp == NULL) {
      printf("cat: %s: No such file or directory", argv[i]);
    }
    output(fp, fl);
  }
}

void output(FILE *fp, struct flags fl) {
  fseek(fp, 0, SEEK_END);
  long fsize = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  char *contents = malloc(fsize * 2 + 1);
  if (contents == NULL) {
    printf("Unable to allocate memory");
  } else {
    fread(contents, 1, fsize, fp);
    contents[fsize] = '\0';
    if (fl.s) {
      modify_s(contents);
    }
    if (fl.v) {
      modify_v(contents);
    }
    if (fl.t) {
      modify_t(contents);
    }
    if (!(fl.b || fl.n)) {
      if (fl.e) {
        modify_e(contents);
      }
      printf("%s", contents);
    } else {
      enumerated_output(contents, fl);
    }
    free(contents);
  }
}

void enumerated_output(char *contents, const struct flags fl) {
  int num_lines = 0, current_line = 0, len = strlen(contents), align;
  if (fl.b) {
    num_lines = count(contents, b_cond);
    align = num_lines > 999999 ? count_digits(num_lines) : INIT_ALIGN_NUM;
    if (!is_empty_line(contents)) {
      printf("%*d\t", align, ++current_line);
    }
    for (int i = 0; i < len; ++i) {
      if (contents[i] == '\n') {
        if (fl.e && is_empty_line_backtrack(contents, contents + i - 1)) {
          printf("%*c\t%c", align, ' ', '$');
        } else if (fl.e) {
          printf("%c", '$');
        }
        if (!is_empty_line(contents + i + 1)) {
          printf("%c%*d\t", contents[i], align, ++current_line);
        } else {
          printf("%c", contents[i]);
        }
      } else {
        printf("%c", contents[i]);
      }
    }
  } else if (fl.n) {
    num_lines = count(contents, n_cond);
    align = num_lines > 999999 ? count_digits(num_lines) : INIT_ALIGN_NUM;
    if (len != 0) {
      printf("%*d\t", align, ++current_line);
    }
    for (int i = 0; i < len; i++) {
      if (contents[i] == '\n') {
        if (fl.e) {
          printf("%c", '$');
        }
        if (i != len - 1) {
          printf("%c%*d\t", contents[i], align, ++current_line);
        } else {
          printf("%c", contents[i]);
        }
      } else {
        printf("%c", contents[i]);
      }
    }
  }
}

int count(char *contents, bool (*condition)(char *)) {
  int result = 0;
  for (int i = 0; contents[i] != '\0'; i++) {
    if ((*condition)(contents + i)) {
      result++;
    }
  }
  return result;
}

bool b_cond(char *contents) {
  if (*contents == '\n' && !is_empty_line(contents + 1)) {
    return true;
  } else {
    return false;
  }
}

bool n_cond(char *contents) {
  if (*contents == '\n') {
    return true;
  } else {
    return false;
  }
}

bool is_empty_line(char *start) {
  if (*start == '\n' || *start == '\0') {
    return true;
  } else {
    return false;
  }
}

// bool is_empty_line_backtrack(char *string_start, char *end) {
//   for (char *runner = end; end >= string_start; --runner) {
//     if (*runner == '\n') {
//       return true;
//     } else if (!(*runner == ' ' || *runner == '\t')) {
//       return false;
//     }
//   }
//   return true;
// }

bool is_empty_line_backtrack(char *string_start, char *end) {
  if (end <= string_start) {
    return true;
  } else if (*end == '\n') {
    return true;
  } else {
    return false;
  }
}

int count_digits(int num) {
  int i = 0;
  if (num == 0) {
    i = 1;
  } else {
    while (num > 0) {
      i++;
      num /= 10;
    }
  }
  return i;
}

void shift(char *contents, int begin, int dir) {
  int len = strlen(contents);
  if (dir == 1) {
    for (int i = len; i > begin; --i) {
      contents[i] = contents[i - 1];
    }
    contents[len + 1] = '\0';
  } else if (dir == -1) {
    for (int i = begin; i < len - 1; ++i) {
      contents[i] = contents[i + 1];
    }
    contents[len - 1] = '\0';
  } else {
    printf("wrong direction");
    exit(1);
  }
}

void modify_e(char *contents) {
  char *e_pattern = "\n";
  regex_t regex;
  regmatch_t match;
  regcomp(&regex, e_pattern, 0);
  size_t offset = 0;
  int reti;
  while ((reti = regexec(&regex, contents + offset, 1, &match, 0)) == 0) {
    size_t position = match.rm_so + offset;
    shift(contents, position, 1);
    contents[position] = '$';
    contents[position + 1] = '\n';
    offset = position + 2;
  }
  regfree(&regex);
}

void modify_s(char *contents) {
  // char *s_pattern = "(\n[[:space:]]*){3,}";
  if (contents[0] == '\n') {
    while (contents[1] == '\n') {
      shift(contents, 1, -1);
    }
  }
  char *s_pattern = "\n\n\n+";
  regex_t regex;
  regmatch_t match;
  regcomp(&regex, s_pattern, REG_EXTENDED | REG_NEWLINE);
  while (regexec(&regex, contents, 1, &match, 0) == 0) {
    for (regoff_t i = match.rm_so + 2; i < match.rm_eo; i++) {
      contents[i] = '\0';
    }
    char *tmp = malloc((strlen(contents + match.rm_eo) + 1) * sizeof(char));
    if (tmp == NULL) {
      printf("Memory allocation error");
      exit(1);
    } else {
      strcpy(tmp, contents + match.rm_eo);
      strcpy(contents + match.rm_so + 2, tmp);
      free(tmp);
    }
  }
  regfree(&regex);
}

void modify_t(char *contents) {
  regex_t regex;
  regmatch_t match;
  regcomp(&regex, "\t", 0);
  while (regexec(&regex, contents, 1, &match, 0) == 0) {
    shift(contents, match.rm_so, 1);
    contents[match.rm_so] = '^';
    contents[match.rm_so + 1] = 'I';
  }
  regfree(&regex);
}

void modify_v(char *contents) {
  size_t len = strlen(contents);
  for (size_t i = 0; i < len; i++) {
    if (contents[i] < 32 && contents[i] != 9 && contents[i] != 10) {
      shift(contents, i, 1);
      contents[i + 1] = contents[i] + 64;
      contents[i] = '^';
      ++len;
      ++i;
    } else if (contents[i] == 127) {
      shift(contents, i, 1);
      contents[i + 1] = 63;
      contents[i] = '^';
      ++len;
      ++i;
    }
  }
}
