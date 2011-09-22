#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define FMT_FG_RESET   "\x1b[0m"
#define FMT_FG_BLACK   "\x1b[30m"
#define FMT_FG_RED     "\x1b[31m"
#define FMT_FG_GREEN   "\x1b[32m"
#define FMT_FG_YELLOW  "\x1b[33m"
#define FMT_FG_BLUE    "\x1b[34m"
#define FMT_FG_MAGENTA "\x1b[35m"
#define FMT_FG_CYAN    "\x1b[36m"
#define FMT_FG_WHITE   "\x1b[37m"

int inp_exit_status;

char *get_git_dir() {
  FILE *fd;
  char *git_dir = (char *)malloc(1024 * sizeof(char));

  fd = popen("/usr/local/bin/git rev-parse --git-dir", "r");
  while (fgets(git_dir, 1023, fd));
  git_dir[strlen(git_dir)-1] = 0;
  pclose(fd);
  return git_dir;
}

char *status_color() {
  if (inp_exit_status == 0) {
    return FMT_FG_GREEN;
  } else {
    return FMT_FG_RED;
  }
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

  fp = popen("/usr/local/bin/git status --porcelain", "r");
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

  sprintf(git_info, "%s%s(%s)%s%s",
      FMT_FG_MAGENTA,
      headfile,
      commit_hash,
      stats_part,
      FMT_FG_RESET);

  free(orig_headfile);

  return git_info;
}

char *ruby_info() {
  return "";
}

int main(int argc, char *argv[]) {
  inp_exit_status = atoi(argv[1]);

  printf("%s%%~ %s%s %s▸%s ",
      FMT_FG_CYAN,
      git_info(),
      ruby_info(),
      status_color(),
      FMT_FG_RESET);

  return 0;
}

