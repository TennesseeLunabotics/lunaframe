// resets all formatting
#define RESET    "\x1b[m"

// foreground
#define F_BLACK       "\x1b[30m"
#define F_RED         "\x1b[31m"
#define F_GREEN       "\x1b[32m"
#define F_YELLOW      "\x1b[33m"
#define F_BLUE        "\x1b[34m"
#define F_MAGENTA     "\x1b[35m"
#define F_CYAN        "\x1b[36m"
#define F_WHITE       "\x1b[37m"

// background
#define B_BLACK       "\x1b[40m"
#define B_RED         "\x1b[41m"
#define B_GREEN       "\x1b[42m"
#define B_YELLOW      "\x1b[43m"
#define B_BLUE        "\x1b[44m"
#define B_MAGENTA     "\x1b[45m"
#define B_CYAN        "\x1b[46m"
#define B_WHITE       "\x1b[47m"

// cursor movement
#define UP            "\x1b[1a"
#define DOWN          "\x1b[1b"
#define LEFT          "\x1b[1c"
#define RIGHT         "\x1b[1d"

// text modes
#define BOLD          "\x1b[1m"
#define DIM           "\x1b[2m"
#define ITALIC        "\x1b[3m"
#define UNDERLINE     "\x1b[4m"
#define BLINKING      "\x1b[5m"
#define REVERSE       "\x1b[7m"
#define HIDDEN        "\x1b[8m"
#define STRIKE        "\x1b[9m"

// remove specific text modes (nicer lines)
#define RM_BOLD       "\x1b[22m"
#define RM_DIM        "\x1b[22m"
#define RM_ITALIC     "\x1b[23m"
#define RM_UNDERLINE  "\x1b[24m"
#define RM_BLINKING   "\x1b[25m"
#define RM_REVERSE    "\x1b[27m"
#define RM_HIDDEN     "\x1b[28m"
#define RM_STRIKE     "\x1b[29m"


#define LUNA_ERROR(msg, ...) do \
        {fprintf(stderr, F_RED BOLD "ERROR" RESET ": %s : %d : " BOLD msg RESET, __FILE__, __LINE__, ##__VA_ARGS__); }while(0);

#define LUNA_DEBUG(msg, ...) do \
        {printf( F_BLUE BOLD "DEBUG" RESET ": " msg, ##__VA_ARGS__); }while(0);
        