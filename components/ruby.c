#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "../colors.h"

void generate_ruby_info(char *output) {
  char *env = getenv("RUBY_HEAP_MIN_SLOTS");
  int turbo = (env != NULL);

  if (turbo) {
    sprintf(output, "%s+ ", FMT_FG_RED);
  } else {
    sprintf(output, "%s- ", FMT_FG_RED);
  }
}

