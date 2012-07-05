#include <stdio.h>
#include <string.h>

#include "../colors.h"

#ifdef ZSH
#define PROMPT_CHARS(prefix) #prefix "%%#"
#else
#define PROMPT_CHARS(prefix) #prefix "%%"
#endif

void generate_status_and_prompt(char *output, char *prev_exit) {
  if (strncmp(prev_exit, "0", 1)) {
    sprintf(output, PROMPT_CHARS(%s%s), FMT_FG_RED, prev_exit);
  } else {
    sprintf(output, PROMPT_CHARS(%s), FMT_FG_GREEN);
  }
}

