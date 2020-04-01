#include <windows.h>
#include <shlwapi.h>
#include "parser.c"

int main(int argc, char *argv[])
{
    run_tests();
    char szDir[] = "C:\\Users\\William\\Projects\\nnj\\nononsensejavascript.com\\templates";
    char outputDir[] = "C:\\Users\\William\\Projects\\nnj\\nononsensejavascript.com\\build";
    parse(szDir, outputDir);
}