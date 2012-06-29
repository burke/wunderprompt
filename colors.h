#ifdef ZSH
#define FMT_CODE(i) "%{\x1b[" #i "m%}"
#else
#define FMT_CODE(i) "\x1b[" #i "m"
#endif

#define FMT_FG_RESET   FMT_CODE(0)
#define FMT_FG_BLACK   FMT_CODE(30)
#define FMT_FG_RED     FMT_CODE(31)
#define FMT_FG_GREEN   FMT_CODE(32)
#define FMT_FG_YELLOW  FMT_CODE(33)
#define FMT_FG_BLUE    FMT_CODE(34)
#define FMT_FG_MAGENTA FMT_CODE(35)
#define FMT_FG_WHITE   FMT_CODE(37)
#define FMT_FG_GRAY    FMT_CODE(93)

