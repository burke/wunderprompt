#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>

#include "../colors.h"

int get_git_dir(char *git_dir) {
  int i;
  char cwd[1024];
  getcwd(cwd, 1024);
  strcat(cwd, "/");

  for (i = strlen(cwd); i >= 0; i--) {
    if (cwd[i] == '/') {
      strcpy(cwd + i + 1, ".git");
      if (!access(cwd, F_OK)) {
        struct stat st_buf;
        stat (cwd, &st_buf);
        if (S_ISDIR (st_buf.st_mode)) {
          strcpy(git_dir, cwd);
          return 0;
        }
        else {
          // Assume the .git file tells the location of the gitdir
          FILE *fd;
          char line[1024];
          fd = fopen (cwd, "r");
          while(fgets(line, 1023, fd) != NULL);
          line[strlen(line)-1] = 0;
          fclose(fd);

          if (strncmp(line, "gitdir: ", 8) == 0) {
            strcpy(cwd + strlen(cwd)-4, line + 8);
          }
          strcpy(git_dir, cwd);
          return 0;
        }
      }
    }
  }

  return 1;
}

long git_last_activity(char *git_dir) {
  FILE *fd;
  char line[1024];
  char filename[1024];
  char *timestamp;

  strcpy(filename, git_dir);
  strcat(filename, "/logs/HEAD");

  fd = fopen(filename, "r");
  while(fgets(line, 1023, fd) != NULL);
  fclose(fd);

  timestamp = (index(line, '>') + 2);
  timestamp[10] = 0;

  return atol(timestamp);
}

void git_activity_time_elapsed(char *ret, char *git_dir) {
  long last_activity = git_last_activity(git_dir);

  long diff = time(NULL) - last_activity;
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

#define ADD_FLAG_IF_MARKER(_flag, _marker) case _flag: stats |= _marker; break

int append_git_status_info(char *stats_part, int stats, int working_mask, int index_mask, char *output) {
  if (stats & working_mask) {
    if (stats & index_mask) {
      strcat(stats_part, FMT_FG_GREEN);
    } else {
      strcat(stats_part, FMT_FG_RED);
    }
    strcat(stats_part, output);
    return 1;
  } else if (stats & index_mask) {
    strcat(stats_part, FMT_FG_BLUE);
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

  fp = popen("git status --porcelain 2>/dev/null", "r");

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
  char filename[strlen(git_dir)+6];
  strcpy(filename, git_dir);
  strcat(filename, "/HEAD");

  fp = fopen(filename, "r");
  fgets(refname, 1023, fp);
  refname[strlen(refname)-1] = 0;
  fclose(fp);

  if (strncmp(refname, "ref: refs/heads/", 16) == 0) {
    strcpy(refname, refname + 16);
    strcpy(filename, git_dir);
  }

  if (strlen(refname) == 40) {
    refname[8] = 0;
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
  char filename[strlen(git_dir)+18];
  strcpy(filename, git_dir);
  strcat(filename, "/logs/refs/stash");

  if (access(filename, F_OK)) {
    strcpy(output, " ");
  } else {
    fp = fopen(filename, "r");
    while (fgets(buf, 2000, fp) != NULL) counter++;
    fclose(fp);
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

  git_d_info[0]='\0';
  git_info[0]='\0';

  int dirty;

  if (get_git_dir(git_dir)) {
    return 1;
  }
  git_activity_time_elapsed(time_elapsed, git_dir);
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
