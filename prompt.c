#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "colours.h"

void get_git_dir(char *git_dir) {
  FILE *fd;

  fd = popen("git rev-parse --git-dir", "r");
  fgets(git_dir, 1023, fd);
  git_dir[strlen(git_dir)-1] = 0;
  pclose(fd);
}

long git_last_commit() {
  FILE *fd;
  char timestamp[1024];

  fd = popen("git log -n1 --format=%ct", "r");
  fgets(timestamp, 1023, fd);
  pclose(fd);

  return atol(timestamp);
}

void git_commit_time_elapsed(char *ret) {
  long last_commit = git_last_commit();

  long diff = time(NULL) - last_commit;
  int diff_min = (int)(diff / 60);

  if (diff_min < 10) {
    sprintf(ret, "%s%d%s", FMT_FG_GREEN, diff_min, FMT_FG_RESET);
  } else if (diff_min < 30) {
    sprintf(ret, "%s%d%s", FMT_FG_YELLOW, diff_min, FMT_FG_RESET);
  } else if (diff_min < 120) {
    sprintf(ret, "%s%d%s", FMT_FG_RED, diff_min, FMT_FG_RESET);
  } else if (diff_min < 1440) {
    sprintf(ret, "%s%dh%s", FMT_FG_RED, diff_min/60, FMT_FG_RESET);
  } else {
    sprintf(ret, "%s%dd%s", FMT_FG_RED, diff_min/1440, FMT_FG_RESET);
  }
}

int git_dirty_info(char *stats_part) {
  FILE *fp;
  int stats = 0;
  char line[1024];

  fp = popen("git status --porcelain", "r");

  while (fgets(line, 1023, fp)) {
    switch(line[1]) {
    case 'D':
      stats |= 0x0001;
      break;
    case 'M':
      stats |= 0x0004;
      break;
    case '?':
      stats |= 0x0010;
      break;
    }
    switch(line[0]) {
    case 'D':
      stats |= 0x0040;
      break;
    case 'M':
      stats |= 0x0100;
      break;
    case '?':
      stats |= 0x0400;
      break;
    }
  }
  pclose(fp);

  if (stats & 0x0001) {
    strcat(stats_part, FMT_UNSTAGED);
    if (stats & 0x0040) {
      strcat(stats_part, FMT_STAGED_UNSTAGED);
    }
    strcat(stats_part, "D");
  strcat(stats_part, FMT_FG_RESET);
  }
  if (stats & 0x0004) {
    strcat(stats_part, FMT_UNSTAGED);
    if (stats & 0x0100) {
      strcat(stats_part, FMT_STAGED_UNSTAGED);
    }
    strcat(stats_part, "M");
  strcat(stats_part, FMT_FG_RESET);
  }
  if (stats & 0x0010) {
    strcat(stats_part, FMT_UNSTAGED);
    if (stats & 0x0400) {
      strcat(stats_part, FMT_STAGED_UNSTAGED);
    }
    strcat(stats_part, "?");
  strcat(stats_part, FMT_FG_RESET);
  }
  if (stats & 0x0040) {
  strcat(stats_part, FMT_STAGED);
    if (!(stats & 0x0001))
      strcat(stats_part, "D");
  strcat(stats_part, FMT_FG_RESET);
  }
  if (stats & 0x0100) {
  strcat(stats_part, FMT_STAGED);
    if (!(stats & 0x0004))
      strcat(stats_part, "M");
  strcat(stats_part, FMT_FG_RESET);
  }
  if (stats & 0x0400) {
  strcat(stats_part, FMT_STAGED);
    if (!(stats & 0x0010))
      strcat(stats_part, "?");
  strcat(stats_part, FMT_FG_RESET);
  }

  return stats;
}

void get_refname(const char *git_dir, char *refname) {
  FILE *fp;
  char *filename = strdup(git_dir);
  strcat(filename, "/HEAD");

  fp = fopen(filename, "r");
  fgets(refname, 1023, fp);
  refname[strlen(refname)-1] = 0;
  fclose(fp);

  if (strncmp(refname, "ref: refs/heads/", 16) == 0) {
    strcpy(refname, refname + 16);
    filename = strdup(git_dir);
  }
}

void get_refname_color(const char *git_dir, const char *refname, const int dirty, char *refname_color) {
  if (dirty) {
    sprintf(refname_color, "%s", FMT_FG_MAGENTA);
  } else {
    sprintf(refname_color, "%s", FMT_FG_GREEN);
  }
}

void get_stash_info(const char *git_dir, char *output) {
  char *stashfile = strdup(git_dir);
  strcat(stashfile, "/refs/stash");
  if (! access(stashfile, F_OK)) {
    sprintf(output, "%s+", FMT_FG_WHITE);
  } else {
    output[0] = 0;
  }
}

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
  get_stash_info(git_dir, stash_info);

  if (!strcmp(refname, "master")) {
    strcpy(refname, "*");
  }

  if (strlen(git_d_info) != 0) {
    strcat(stash_info, " ");
  }

  printf("%s %s%s%s%s%s",
      time_elapsed,
      refname_color,
      refname,
      stash_info,
      git_d_info,
      FMT_FG_RESET);
  return 0;
}


