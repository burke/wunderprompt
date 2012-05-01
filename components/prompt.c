#include <stdio.h>
#include <string.h>

#include "../colors.h"

void generate_status_and_prompt(char *output, char *prev_exit) {
  if (strncmp(prev_exit, "0", 1)) {
    sprintf(output, "%s%s%%%%", FMT_FG_RED, prev_exit);
  } else {
    sprintf(output, "%s%%%%", FMT_FG_GREEN);
  }
}

