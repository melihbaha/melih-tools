#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG
// #define TEST

typedef enum {
  TOKEN_UNKNOWN = 0,
  TOKEN_MAIN_FILE,
  TOKEN_MAIN_FILE_CREATE,
  TOKEN_FILE,
  TOKEN_FILE_NAME,
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

typedef enum : uint8_t {
  SEEN_NONE = 0,
  SEEN_UNKNOWN = 1u << 0,
  SEEN_MAIN_FILE = 1u << 1,
  SEEN_MAIN_FILE_CREATE = 1u << 2,
  SEEN_FILE = 1u << 3,
  SEEN_FILE_NAME = 1u << 4,
  SEEN_INPUT_MODE = 1u << 5
} token_seen_e;

static const command_type_entry_s COMMANDS[] = {
    {"create", TOKEN_MAIN_FILE_CREATE},
    {"--file", TOKEN_FILE},
    {"--input", TOKEN_INPUT_MODE},
};
static constexpr size_t COMMANDS_COUNT = sizeof(COMMANDS) / sizeof(COMMANDS[0]);

#ifdef DEBUG
static const char *const token_type_e_str[] = {
    [TOKEN_UNKNOWN] = "TOKEN_UNKNOWN",
    [TOKEN_MAIN_FILE] = "TOKEN_MAIN_FILE",
    [TOKEN_MAIN_FILE_CREATE] = "TOKEN_MAIN_FILE_CREATE",
    [TOKEN_FILE] = "TOKEN_FILE",
    [TOKEN_FILE_NAME] = "TOKEN_FILE_NAME",
    [TOKEN_INPUT_MODE] = "TOKEN_INPUT_MODE"};
static constexpr size_t TOKEN_TYPE_E_STR_COUNT = sizeof(token_type_e_str) / sizeof(token_type_e_str[0]);
#endif

static int token_main_file_index = 0;

/* input functions */
char *input_get_string(char *target, int size);

/* token functions */
void clear_token(token_s *token, size_t size);

/* print functions */
#ifdef DEBUG
void print_token(const token_s *token, size_t size);
#endif

int main(int argc, char *argv[argc + 1]) {
  if (argc < 2) {
    /* check parameter number*/
    printf("Not Enough Parameter\n");
    return EXIT_FAILURE;
  }

  /* create tokens according to argc*/
  token_s token[argc - 1];
  size_t token_size = argc - 1;

  /* fill the tokens with zeros */
  clear_token(token, sizeof(token) / sizeof(token[0]));

  /* token parsing section */
  for (int i = 1; i < argc; i++) {
    /* fill the tokens with their initial values */
    token[i - 1].data.string_val = argv[i];
    token[i - 1].type = TOKEN_UNKNOWN;

    /* define the type of token */
    for (size_t j = 0; j < COMMANDS_COUNT; j++) {
      if (strcmp(argv[i], COMMANDS[j].name) == 0) {
        /* if the token type is found, break for the next one */
        token[i - 1].type = COMMANDS[j].type;
        break;
      }
    }
  }

  /* section on interpreting unknown tokens */
  uint8_t token_flags = 0;

  for (size_t i = 0; i < token_size; i++) {
    /* define the main file name */
    if (token[i].type == TOKEN_MAIN_FILE_CREATE) {
      if (token_flags & SEEN_MAIN_FILE_CREATE) {
        printf("you can create only one file\n");
        return EXIT_FAILURE;
      } else {
        token_flags |= SEEN_MAIN_FILE_CREATE;
      }
      if (i + 1 < token_size) {
        if (token[i + 1].type != TOKEN_UNKNOWN) {
          printf("file name cannot be a parameter\n");
          return EXIT_FAILURE;
        }
        if (token[i + 1].data.string_val != nullptr) {
          token_main_file_index = (int)(i + 1);
          token[i + 1].type = TOKEN_MAIN_FILE;
          token_flags |= SEEN_MAIN_FILE;
          i++;
          continue;
        }
      } else {
        printf("the name of the file to be created must be entered\n");
        return EXIT_FAILURE;
      }
    }

    if (token[i].type == TOKEN_FILE) {
      if (token_flags & SEEN_FILE) {
        printf("The --file parameter cannot be provided more than once\n");
        return EXIT_FAILURE;
      } else {
        token_flags |= SEEN_FILE;
      }

      if (i + 1 < token_size) {
        if (token[i + 1].type != TOKEN_UNKNOWN) {
          printf("file name cannot be a parameter\n");
          return EXIT_FAILURE;
        }
        char *tdata = token[i + 1].data.string_val;
        if (tdata == nullptr) {
          printf("No File Detected\n");
          return EXIT_FAILURE;
        }

        char *file_extension = strrchr(tdata, '.');
        if (file_extension == tdata) {
          printf("a file name cannot start with a dot\n");
          return EXIT_FAILURE;
        }
        if (file_extension == nullptr) {
          printf("the file does not have an extension\n");
          return EXIT_FAILURE;
        }
        if (strcmp(file_extension, ".txt") == 0) {
          token[i + 1].type = TOKEN_FILE_NAME;
          token_flags |= SEEN_FILE_NAME;
          i++;
        } else {
          printf("incompatible file extension\n");
          return EXIT_FAILURE;
        }
      } else {
        printf("an option argument must be provided for --file\n");
        return EXIT_FAILURE;
      }
    }
  }

#ifdef DEBUG
  print_token(token, token_size);
#endif

  if (token_flags & SEEN_MAIN_FILE_CREATE) {
    FILE *create_file = fopen(token[token_main_file_index].data.string_val, "w");
    if (create_file == nullptr) {
      printf("file could not be created\n");
      return EXIT_FAILURE;
    }

    fclose(create_file);
  }

  /* successfully log out */
  return EXIT_SUCCESS;
}

char *input_get_string(char *target, int size) {
  if (fgets(target, size, stdin) != nullptr) {
    /* retrieve data from the buffer and check that it is not a nullptr and check if there's a newline*/
    char *p = strchr(target, '\n');

    if (p == nullptr) {
      /* clear the buffer if there is no newline */
      int c = 0;
      while ((c = getchar()) != EOF && c != '\n') {
        continue;
      }
    } else {
      /* if there is a newline, set this value to null */
      *p = '\0';
    }
  }
  /* return a clean char* */
  return target;
}
#ifdef DEBUG
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
  constexpr int W_TYPE = 22;

  printf("%-*s | %-*s | %s\n", W_NUM, "TOKEN_NUMBER", W_TYPE, "TOKEN_TYPE",
         "TOKEN_VALUE");

  for (size_t i = 0; i < size; i++) {
    printf("%-*zu | %-*s | %s\n", W_NUM, i, W_TYPE,
           token_type_e_str[token[i].type],
           (token[i].data.string_val != nullptr) ? token[i].data.string_val
                                                 : "NULL");
  }
}
#endif

void clear_token(token_s *token, size_t size) {
  memset(token, 0, sizeof(token_s) * size);
  for (size_t i = 0; i < size; i++) {
    token[i].type = TOKEN_UNKNOWN;
  }
}
