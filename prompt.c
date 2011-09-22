#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FMT_FG_RESET   "\x1b[0m"
#define FMT_FG_BLACK   "\x1b[30m"
#define FMT_FG_RED     "\x1b[31m"
#define FMT_FG_GREEN   "\x1b[32m"
#define FMT_FG_YELLOW  "\x1b[33m"
#define FMT_FG_BLUE    "\x1b[34m"
#define FMT_FG_MAGENTA "\x1b[35m"
#define FMT_FG_WHITE   "\x1b[37m"
#define FMT_FG_GRAY    "\x1b[93m"

char *get_git_dir() {
  FILE *fd;
  char *git_dir = (char *)malloc(1024 * sizeof(char));

  fd = popen("git rev-parse --git-dir", "r");
  while (fgets(git_dir, 1023, fd));
  git_dir[strlen(git_dir)-1] = 0;
  pclose(fd);
  return git_dir;
}

long git_last_commit() {
  FILE *fd;
  char timestamp[1024];

  fd = popen("git log -n1 --format=%ct", "r");
  while (fgets(timestamp, 1023, fd));
  pclose(fd);

  return atol(timestamp);
}

char *git_commit_time_elapsed() {
  char *ret = (char *)malloc(sizeof(char) * 32);
  long last_commit = git_last_commit();

  long diff = time(NULL) - last_commit;
  int diff_min = (int)(diff / 60);

  if (diff_min < 10) {
    sprintf(ret, "%s%dm%s", FMT_FG_GREEN, diff_min, FMT_FG_RESET);
  } else if (diff_min < 30) {
    sprintf(ret, "%s%dm%s", FMT_FG_YELLOW, diff_min, FMT_FG_RESET);
  } else {
    sprintf(ret, "%s%dm%s", FMT_FG_RED, diff_min, FMT_FG_RESET);
  }

  return ret;
}

char *git_info() {
  char *git_dir = get_git_dir();
  FILE *fp;
  char *fname = strdup(git_dir);
  char *headfile    = (char *)malloc(1024 * sizeof(char));
  char *git_info    = (char *)malloc(1024 * sizeof(char));
  char *commit_hash = (char *)malloc(1024 * sizeof(char)); 
  char *line        = (char *)malloc(1024 * sizeof(char)); 
  int stats = 0;
  int lstats = 0;
  char *stats_part = (char *)malloc(8 * sizeof(char));

  char *orig_headfile = headfile;

  strcat(fname, "/HEAD");

  fp = fopen(fname, "r");
  sleep(0.2);
  while (fgets(headfile, 1023, fp));
  headfile[strlen(headfile)-1] = 0;
  fclose(fp);

  if (strncmp(headfile, "ref: refs/heads/", 16) == 0) {
    headfile += 16;
    fname = strdup(git_dir);
    strcat(fname, "/refs/heads/");
    strcat(fname, headfile);
    fp = fopen(fname, "r");
    while (fgets(commit_hash, 1023, fp));
    commit_hash[7] = 0;
    fclose(fp);
  } else {
    headfile[7] = 0;
    commit_hash = strdup(headfile);
  }

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

  strcat(stats_part, FMT_FG_BLUE);
  if (stats & 0x0001) {
    strcat(stats_part, "D");
  }
  if (stats & 0x0004) {
    strcat(stats_part, "M");
  }
  if (stats & 0x0010) {
    strcat(stats_part, "?");
  }
  strcat(stats_part, FMT_FG_GREEN);
  if (stats & 0x0040) {
    strcat(stats_part, "D");
  }
  if (stats & 0x0100) {
    strcat(stats_part, "M");
  }
  if (stats & 0x0400) {
    strcat(stats_part, "?");
  }
  strcat(stats_part, FMT_FG_RESET);

  sprintf(git_info, "%s:%s%s%s(%s%s%s)%s%s",
      git_commit_time_elapsed(),
      FMT_FG_MAGENTA,
      headfile,
      FMT_FG_BLACK,
      FMT_FG_GRAY,
      commit_hash,
      FMT_FG_BLACK,
      stats_part,
      FMT_FG_RESET);

  free(orig_headfile);

  return git_info;
}

int main() {
  printf("%s", git_info());
  return 0;
}

