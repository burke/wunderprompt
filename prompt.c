#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "colors.h"

int get_git_dir(char *git_dir) {
  FILE *fd;

  fd = popen("git rev-parse --git-dir 2>/dev/null", "r");
  fgets(git_dir, 1023, fd);

  if (strlen(git_dir) == 0) return 1;

  git_dir[strlen(git_dir)-1] = 0;
  pclose(fd);
  return 0;
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

#define WORKING_DELETED   0x0001
#define WORKING_MODIFIED  0x0004
#define WORKING_ADDED     0x0010
#define WORKING_UNMERGED  0x0040
#define WORKING_RENAMED   0x0100
#define WORKING_COPIED    0x0400
#define WORKING_UNTRACKED 0x1000
#define WORKING_IGNORED   0x4000

#define INDEX_DELETED     0x00010000
#define INDEX_MODIFIED    0x00040000
#define INDEX_ADDED       0x00100000
#define INDEX_UNMERGED    0x00400000
#define INDEX_RENAMED     0x01000000
#define INDEX_COPIED      0x04000000
#define INDEX_UNTRACKED   0x10000000
#define INDEX_IGNORED     0x40000000

#define MARKER_DELETED    "D"
#define MARKER_MODIFIED   "M"
#define MARKER_ADDED      "A"
#define MARKER_UNMERGED   "U"
#define MARKER_RENAMED    "R"
#define MARKER_COPIED     "C"
#define MARKER_UNTRACKED  "?"
#define MARKER_IGNORED    "!"

#define FMT_STAGED     "%{\x1b[32m%}"
#define FMT_UNSTAGED   "%{\x1b[31m%}"
#define FMT_STAGED_UNSTAGED "%{\x1b[34m%}"

#define ADD_FLAG_IF_MARKER(_flag, _marker) case _flag: stats |= _marker; break

int append_git_status_info(char *stats_part, int stats, int working_mask, int index_mask, char *output) {
  if (stats & working_mask) {
    if (stats & index_mask) {
      strcat(stats_part, FMT_STAGED_UNSTAGED);
    } else {
      strcat(stats_part, FMT_UNSTAGED);
    }
    strcat(stats_part, output);
    return 1;
  } else if (stats & index_mask) {
    strcat(stats_part, FMT_STAGED);
    strcat(stats_part, output);
    return 1;
  }
  return 0;
}

int git_dirty_info(char *stats_part) {
  FILE *fp;
  int stats = 0;
  char line[1024];
  int output_size = 0;
  int numlines = 0;

  fp = popen("git status --porcelain", "r");

  while (fgets(line, 1023, fp)) {
    numlines++;
    switch(line[1]) {
      ADD_FLAG_IF_MARKER('D', WORKING_DELETED);
      ADD_FLAG_IF_MARKER('M', WORKING_MODIFIED);
      ADD_FLAG_IF_MARKER('A', WORKING_ADDED);
      ADD_FLAG_IF_MARKER('U', WORKING_UNMERGED);
      ADD_FLAG_IF_MARKER('R', WORKING_RENAMED);
      ADD_FLAG_IF_MARKER('C', WORKING_COPIED);
      ADD_FLAG_IF_MARKER('?', WORKING_UNTRACKED);
      ADD_FLAG_IF_MARKER('!', WORKING_IGNORED);
    }
    switch(line[0]) {
      ADD_FLAG_IF_MARKER('D', INDEX_DELETED);
      ADD_FLAG_IF_MARKER('M', INDEX_MODIFIED);
      ADD_FLAG_IF_MARKER('A', INDEX_ADDED);
      ADD_FLAG_IF_MARKER('U', INDEX_UNMERGED);
      ADD_FLAG_IF_MARKER('R', INDEX_RENAMED);
      ADD_FLAG_IF_MARKER('C', INDEX_COPIED);
      ADD_FLAG_IF_MARKER('?', INDEX_UNTRACKED);
      ADD_FLAG_IF_MARKER('!', INDEX_IGNORED);
    }
  }
  pclose(fp);

  output_size += append_git_status_info(stats_part, stats, WORKING_DELETED, INDEX_DELETED, MARKER_DELETED);
  output_size += append_git_status_info(stats_part, stats, WORKING_MODIFIED, INDEX_MODIFIED, MARKER_MODIFIED);
  output_size += append_git_status_info(stats_part, stats, WORKING_ADDED, INDEX_ADDED, MARKER_ADDED);
  output_size += append_git_status_info(stats_part, stats, WORKING_UNMERGED, INDEX_UNMERGED, MARKER_UNMERGED);
  output_size += append_git_status_info(stats_part, stats, WORKING_RENAMED, INDEX_RENAMED, MARKER_RENAMED);
  output_size += append_git_status_info(stats_part, stats, WORKING_COPIED, INDEX_COPIED, MARKER_COPIED);
  output_size += append_git_status_info(stats_part, stats, WORKING_UNTRACKED, INDEX_UNTRACKED, MARKER_UNTRACKED);
  output_size += append_git_status_info(stats_part, stats, WORKING_IGNORED, INDEX_IGNORED, MARKER_IGNORED);

  strcat(stats_part, FMT_FG_RESET);
  if (output_size > 0) {
    sprintf(stats_part + strlen(stats_part), "%d ", numlines);
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
  FILE *fp;
  char buf[2000];
  int counter = 0;
  char *filename = strdup(git_dir);
  strcat(filename, "/logs/refs/stash");

  if (access(filename, F_OK)) {
    strcpy(output, " ");
  } else {
    fp = fopen(filename, "r");
    while (fgets(buf, 2000, fp) != NULL) counter++;
    sprintf(output, "%s%d ", FMT_FG_WHITE, counter);
  }
}

int generate_git_prompt(char *git_info) {
  char refname[128];
  char git_d_info[512];
  char time_elapsed[32];
  char git_dir[512];
  char stash_info[32];
  char refname_color[32];

  int dirty;

  if (get_git_dir(git_dir)) {
    return 1;
  }
  git_commit_time_elapsed(time_elapsed);
  dirty = git_dirty_info(git_d_info);
  get_refname(git_dir, refname);
  get_refname_color(git_dir, refname, dirty, refname_color);
  get_stash_info(git_dir, stash_info);

  if (!strcmp(refname, "master")) {
    strcpy(refname, "*");
  }

  sprintf(git_info,
      "%s %s%s%s%s%s",
      time_elapsed,
      refname_color,
      refname,
      stash_info,
      git_d_info,
      FMT_FG_RESET);

  return 0;
}

void generate_hostname_color(char *output) {
  char hostname[256];
  gethostname(hostname, 256);
  if (!strncmp(hostname, "burke", 5)) {
    sprintf(output, "%s", FMT_FG_BLUE);
  } else if (!strcmp(hostname, "hoth")) {
    sprintf(output, "%s", FMT_FG_YELLOW);
  } else {
    sprintf(output, "%s", FMT_FG_RED);
  }
}

void generate_status_and_prompt(char *output, char *prev_exit) {
  if (strncmp(prev_exit, "0", 1)) {
    sprintf(output, "%s%s%%%%", FMT_FG_RED, prev_exit);
  } else {
    sprintf(output, "%s%%%%", FMT_FG_GREEN);
  }
}

void generate_path_info(char *output) {
  char cwd[256];
  char *ptr = cwd;
  int i;
  getcwd(cwd, 256);

  for (i = strlen(cwd); i > 0; i--) {
    if (cwd[i] == '/') {
      ptr = cwd + i + 1;
      break;
    }
  }
  sprintf(output, "%s", ptr);
}

void generate_ruby_info(char *output) {
  char *env = getenv("RUBY_HEAP_MIN_SLOTS");
  int turbo = (env != NULL);

  if (turbo) {
    sprintf(output, "%s+ ", FMT_FG_RED);
  } else {
    sprintf(output, "%s- ", FMT_FG_RED);
  }
}


int main(int argc, char *argv[]) {
  char *prev_exit;
  if (argc < 2) {
    prev_exit = "0";
  } else {
    prev_exit = argv[1];
  }
  char hostname_color[16];
  char path_info[256];
  char git_info[512];
  char ruby_info[32];
  char status_and_prompt[32];
  generate_hostname_color(hostname_color);
  generate_path_info(path_info);
  generate_ruby_info(ruby_info);
  generate_git_prompt(git_info);
  generate_status_and_prompt(status_and_prompt, prev_exit);
  printf("%s%s %s%s%s%s ", hostname_color, path_info, ruby_info, git_info, status_and_prompt, FMT_FG_RESET);
  return 0;
}
