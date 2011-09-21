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
  FILE *fp;
  int status;
  char path[1035];
  char *git_dir;

  fp = popen("/usr/local/bin/git rev-parse --git-dir", "r");
  if (fp == NULL) {
    /* failed to run command */
  }
  while (fgets(path, sizeof(path)-1, fp) != NULL) {
    git_dir = path;
  }
  git_dir[strlen(git_dir)-1] = 0;
  pclose(fp);
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
  FILE *fp;
  char *git_dir = get_git_dir();
  char *fname = strdup(git_dir);
  char *headfile = (char *)malloc(1024 * sizeof(char));
  char *git_info = (char *)malloc(1024 * sizeof(char));

  char *orig_headfile = headfile;

  strcat(fname, "/HEAD");

  fp = fopen(fname, "r");
  fgets(headfile, 1023, fp);
  headfile[strlen(headfile)-1] = 0;
  fclose(fp);

  if (strncmp(headfile, "ref: refs/heads/", 16) == 0) {
    headfile += 16;
  } else {
    headfile[7] = 0; 
  }

  sprintf(git_info, "%s%s%s",
      FMT_FG_MAGENTA,
      headfile,
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

