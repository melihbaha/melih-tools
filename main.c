#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  TOKEN_UNKNOWN = 0,
  TOKEN_FILE,
  TOKEN_COPY_FROM,
  TOKEN_INPUT_MODE,
  TOKEN_TYPE_COUNT
} token_type_e;

typedef struct {
  token_type_e type;
  union {
    char *string_val;
  } data;
} token_s;

typedef struct {
  const char *name;
  token_type_e type;
} command_type_entry_s;

static const command_type_entry_s COMMANDS[] = {
    {"--file", TOKEN_FILE},
    {"--copy-from", TOKEN_COPY_FROM},
    {"--input", TOKEN_INPUT_MODE},
};

static const char *const token_type_e_str[] = {
    [TOKEN_UNKNOWN] = "TOKEN_UNKNOWN",
    [TOKEN_FILE] = "TOKEN_FILE",
    [TOKEN_COPY_FROM] = "TOKEN_COPY_FROM",
    [TOKEN_INPUT_MODE] = "TOKEN_INPUT_MODE"};
static constexpr size_t COMMANDS_COUNT = sizeof(COMMANDS) / sizeof(COMMANDS[0]);
static constexpr size_t TOKEN_TYPE_E_STR_COUNT = sizeof(token_type_e_str) / sizeof(token_type_e_str[0]);

/* input functions */
char *input_get_string(char *target, int size);

/* token functions */
void clear_token(token_s *token, size_t size);

/* print functions */
[[maybe_unused]] void print_token(const token_s *token, size_t size);

int main(int argc, char *argv[argc + 1]) {
  if (argc < 2) {
    printf("Not Enough Parameter\n");
    return EXIT_FAILURE;
  }
  token_s token[argc - 1];
  clear_token(token, sizeof(token) / sizeof(token[0]));
  for (int i = 1; i < argc; i++) {
      token[i - 1].data.string_val = argv[i];
      token[i - 1].type = TOKEN_UNKNOWN;

      for (size_t j = 0; j < COMMANDS_COUNT; j++) {
          if (strcmp(argv[i], COMMANDS[j].name) == 0) {
              token[i - 1].type = COMMANDS[j].type;
              break;
          }
      }
  }

  print_token(token, sizeof(token) / sizeof(token[0]));
  return EXIT_SUCCESS;
}

char *input_get_string(char *target, int size) {
  if (fgets(target, size, stdin) != nullptr) {
    char *p = strchr(target, '\n');
    if (p == nullptr) {
      int c = 0;
      while ((c = getchar()) != EOF && c != '\n')
        ;
    } else {
      *p = '\0';
    }
  }
  return target;
}

void print_token(const token_s *token, size_t size) {
  if (token == nullptr) {
    return;
  }
  if (TOKEN_TYPE_E_STR_COUNT != TOKEN_TYPE_COUNT) {
    printf("The string representations for all variables within the enum "
           "structure have not been defined\n");
    return;
  }

  constexpr int W_NUM = 14;
  constexpr int W_TYPE = 18;

  printf("%-*s | %-*s | %s\n", W_NUM, "TOKEN_NUMBER", W_TYPE, "TOKEN_TYPE",
         "TOKEN_VALUE");

  for (size_t i = 0; i < size; i++) {
    printf("%-*zu | %-*s | %s\n", W_NUM, i, W_TYPE,
           token_type_e_str[token[i].type],
           (token[i].data.string_val != nullptr) ? token[i].data.string_val
                                                 : "NULL");
  }
}

void clear_token(token_s *token, size_t size) {
  memset(token, 0, sizeof(token_s) * size);
  for (size_t i = 0; i < size; i++) {
    token[i].type = TOKEN_UNKNOWN;
  }
}
