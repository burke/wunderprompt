#ifdef ZSH
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
#else
  #define FMT_FG_RESET   "\x1b[0m"
  #define FMT_FG_BLACK   "\x1b[30m"
  #define FMT_FG_RED     "\x1b[31m"
  #define FMT_FG_GREEN   "\x1b[32m"
  #define FMT_FG_YELLOW  "\x1b[33m"
  #define FMT_FG_BLUE    "\x1b[34m"
  #define FMT_FG_MAGENTA "\x1b[35m"
  #define FMT_FG_WHITE   "\x1b[37m"
  #define FMT_FG_GRAY    "\x1b[93m"

  #define FMT_STAGED     "\x1b[30m"
  #define FMT_UNSTAGED   "\x1b[34m"
  #define FMT_STAGED_UNSTAGED "\x1b[31m"
#endif
