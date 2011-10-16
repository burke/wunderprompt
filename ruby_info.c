#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colors.h"

#define RUBY_IS(x) ((!strncmp(ruby, x, strlen(x))))

int main() {
  char *gem_home = getenv("GEM_HOME");
  char *ruby_with_gemset = strrchr(gem_home, '/')+1;
  char *ruby;
  char *gemset;

  char *atptr = strrchr(ruby_with_gemset, '@');
  if (atptr == NULL) {
    gemset = "@";
    ruby = ruby_with_gemset;
  } else {
    gemset = strdup(atptr + 1);
    ruby = strdup(ruby_with_gemset);
    ruby[atptr - ruby_with_gemset] = 0;
  }

  if RUBY_IS("ruby-1.9.2-") {
    ruby = FMT_FG_YELLOW;
  } else if RUBY_IS("ree-1.8.7-") {
    ruby = FMT_FG_MAGENTA;
  } else if RUBY_IS("rbx-") {
    ruby = FMT_FG_BLUE;
  } else if RUBY_IS("jruby") {
    ruby = FMT_FG_GREEN;
  } else if RUBY_IS("ruby-1.9.3") {
    ruby = FMT_FG_WHITE;
  } else {
    ruby = FMT_FG_RED;
  }

  printf("%s%s%s", ruby, gemset, FMT_FG_RESET);

  return 0;
}
