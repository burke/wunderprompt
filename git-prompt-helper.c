#include "git-prompt-helper.h"

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

void git_commit_time_elapsed(char *ret, size_t len)
{
  long last_commit = git_last_commit();
  long min         = (time(NULL) - last_commit) / 60;
  char suffix;
  char *color;

  if (min < 10)
  {
    suffix = 'm';
    color  = FMT_FG_GREEN;
  }
  else if (min < 30)
  {
    suffix = 'm';
    color  = FMT_FG_YELLOW;
  }
  else if (min < 120)
  {
    suffix = 'm';
    color  = FMT_FG_RED;
  }
  else if (min < 1440)
  {
    suffix = 'h';
    color  = FMT_FG_RED;
    min   /= 60;
  }
  else
  {
    suffix = 'd';
    color  = FMT_FG_RED;
    min   /= 1440;
  }

  snprintf(ret, len, "%s%ld%c%s", color, min, suffix, FMT_FG_RESET);
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
    snprintf(refname_color,sizeof(refname_color), "%s", FMT_FG_MAGENTA);
  } else {
    snprintf(refname_color,sizeof(refname_color), "%s", FMT_FG_GREEN);
  }
}

void get_stash_info(const char *git_dir, char *output) {
  char *stashfile = strdup(git_dir);
  strcat(stashfile, "/refs/stash");
  if (! access(stashfile, F_OK)) {
    snprintf(output,sizeof(output), "%s+", FMT_FG_WHITE);
  } else {
    output[0] = 0;
  }
}
