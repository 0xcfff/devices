#define IS_FLAG_SET(flag, x) ((x &flag) == flag)
#define SET_FLAG(flag, x) (x|=flag)
#define RESET_FLAG(flag, x) (x=(x & ~flag))
#define SET_FLAG_VALUE(flag, val, x) (val ? SET_FLAG(flag, x) : RESET_FLAG(flag, x))