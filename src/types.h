enum FILE_TYPE
{
    HTML = 1,
    JAVASCRIPT,
    CSS,
    LAYOUT,
    TEMPLATE,
    DIRECTORY,
    OTHER
};

#define true (1)
#define false (0)

typedef int bool;

#define TEMPLATE_EXTENSION "rch"

typedef struct Commands
{
    char* oldPath;
    char* newPath;
    int* fileType;
    int commandCount;
} Commands;