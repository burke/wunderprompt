#include <stdio.h>

#include "colors.h"
#include "components/path.h"
#include "components/ruby.h"
#include "components/git.h"
#include "components/prompt.h"

int main(int argc, char *argv[]) {
  char *prev_exit = argc < 2 ? "0" : argv[1];
  char path_info[256], git_info[512], ruby_info[32], status_and_prompt[32];

  generate_path_info(path_info);
  generate_ruby_info(ruby_info);
  generate_git_prompt(git_info);
  generate_status_and_prompt(status_and_prompt, prev_exit);

  //printf("%s %s%s%s%s ", path_info, ruby_info, git_info, status_and_prompt, FMT_FG_RESET);
  printf("%s %s%s%s ", path_info, git_info, status_and_prompt, FMT_FG_RESET);

  return 0;
}
