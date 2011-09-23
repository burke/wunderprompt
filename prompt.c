#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FMT_FG_RESET   "%{\x1b[0m%}"
#define FMT_FG_BLACK   "%{\x1b[30m%}"
#define FMT_FG_RED     "%{\x1b[31m%}"
#define FMT_FG_GREEN   "%{\x1b[32m%}"
#define FMT_FG_YELLOW  "%{\x1b[33m%}"
#define FMT_FG_BLUE    "%{\x1b[34m%}"
#define FMT_FG_MAGENTA "%{\x1b[35m%}"
#define FMT_FG_WHITE   "%{\x1b[37m%}"
#define FMT_FG_GRAY    "%{\x1b[93m%}"

#define FMT_STAGED     "%{\x1b[30m%}"
#define FMT_UNSTAGED   "%{\x1b[34m%}"
#define FMT_STAGED_UNSTAGED "%{\x1b[31m%}"

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
  }
  if (stats & 0x0004) {
    strcat(stats_part, FMT_UNSTAGED);
    if (stats & 0x0100) {
      strcat(stats_part, FMT_STAGED_UNSTAGED);
    }
    strcat(stats_part, "M");
  }
  if (stats & 0x0010) {
    strcat(stats_part, FMT_UNSTAGED);
    if (stats & 0x0400) {
      strcat(stats_part, FMT_STAGED_UNSTAGED);
    }
    strcat(stats_part, "?");
  }
  strcat(stats_part, FMT_FG_RESET);
  strcat(stats_part, FMT_STAGED);
  if (stats & 0x0040) {
    if (!(stats & 0x0001))
      strcat(stats_part, "D");
  }
  if (stats & 0x0100) {
    if (!(stats & 0x0004))
      strcat(stats_part, "M");
  }
  if (stats & 0x0400) {
    if (!(stats & 0x0010))
      strcat(stats_part, "?");
  }
  strcat(stats_part, FMT_FG_RESET);

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

char *git_info() {
  char *refname      = (char *)malloc(1024 * sizeof(char));
  char *git_info     = (char *)malloc(1024 * sizeof(char));
  char *git_d_info   = (char *)malloc(1024 * sizeof(char)); 
  char *time_elapsed = (char *)malloc(1024 * sizeof(char)); 
  char *git_dir      = (char *)malloc(1024 * sizeof(char)); 
  char *refname_color= (char *)malloc(1024 * sizeof(char)); 

  int dirty;

  git_commit_time_elapsed(time_elapsed);
  dirty = git_dirty_info(git_d_info);
  get_git_dir(git_dir);
  get_refname(git_dir, refname);
  get_refname_color(git_dir, refname, dirty, refname_color);

  sprintf(git_info, "%s %s%s %s%s",
      time_elapsed,
      refname_color,
      refname,
      git_d_info,
      FMT_FG_RESET);

  return git_info;
}

int main() {
  printf("%s", git_info());
  return 0;
}

