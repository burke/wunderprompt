#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "colours.h"

void get_git_dir(char *git_dir);
long git_last_commit();
void git_commit_time_elapsed(char *ret);
int  git_dirty_info(char *stats_part);
void get_refname(const char *git_dir, char *refname);
void get_refname_color(const char *git_dir, const char *refname, const int dirty, char *refname_color);
void get_stash_info(const char *git_dir, char *output);
