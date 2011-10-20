#include "git-prompt-helper.h"

int main() {
  char refname[128];
  char git_d_info[512];
  char time_elapsed[32];
  char git_dir[512];
  char stash_info[32];
  char refname_color[32];

  int dirty;

  git_commit_time_elapsed(time_elapsed);
  dirty = git_dirty_info(git_d_info);
  get_git_dir(git_dir);
  get_refname(git_dir, refname);
  get_refname_color(git_dir, refname, dirty, refname_color);

  if (!strcmp(refname, "master")) {
    strcpy(refname, "*");
  }

  if (strlen(git_d_info) != 0) {
    strcat(stash_info, " ");
  }

  printf("%s%s(%s%s)%s",
      refname_color,
      refname,
      time_elapsed,
      refname_color,
      FMT_FG_RESET);
  return 0;
}

