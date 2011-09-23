#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RESET   "%{\x1b[0m%}"
#define BLACK   "%{\x1b[30m%}"
#define RED     "%{\x1b[31m%}"
#define GREEN   "%{\x1b[32m%}"
#define YELLOW  "%{\x1b[33m%}"
#define BLUE    "%{\x1b[34m%}"
#define MAGENTA "%{\x1b[35m%}"
#define WHITE   "%{\x1b[37m%}"
#define GRAY    "%{\x1b[93m%}"

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
    ruby = YELLOW;
  } else if RUBY_IS("ree-1.8.7-") {
    ruby = MAGENTA;
  } else if RUBY_IS("rbx-") {
    ruby = BLUE;
  } else if RUBY_IS("jruby") {
    ruby = GREEN;
  } else if RUBY_IS("ruby-1.9.3") {
    ruby = WHITE;
  } else {
    ruby = RED;
  }

  printf("%s%s%s", ruby, gemset, RESET);

  return 0;
}
