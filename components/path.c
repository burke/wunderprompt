#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "../colors.h"

void generate_hostname_info(char *output) {
  char hostname[256];
  gethostname(hostname, 256);
  if (!strncmp(hostname, "burke", 5)) {
    sprintf(output, "%s", FMT_FG_BLUE);
  } else if (!strcmp(hostname, "shopify-dev")) {
    sprintf(output, "%s", FMT_FG_YELLOW);
  } else {
    sprintf(output, "%s%s:", FMT_FG_RED, hostname);
  }
}

void generate_path_info(char *output) {
  char hostname_info[256];
  char cwd[256];
  char *ptr = cwd;
  int i;
  generate_hostname_info(hostname_info);
  getcwd(cwd, 256);

  for (i = strlen(cwd); i > 0; i--) {
    if (cwd[i] == '/') {
      ptr = cwd + i + 1;
      break;
    }
  }
  sprintf(output, "%s%s", hostname_info, ptr);
}
