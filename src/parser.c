#include <string.h>
#include <stdio.h>
#include <io.h>
#include <windows.h>

main()
{
    char* dirname = "C:\\Users\\William\\Projects\\nnj\\nononsensejavascript.com";
    TCHAR szDir[] = "C:\\Users\\William\\Projects\\nnj\\nononsensejavascript.com\\*";
    HANDLE hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA ffd;
    hFind = FindFirstFile(szDir, &ffd);
    if (INVALID_HANDLE_VALUE == hFind)
    {
        printf("invalid handle\n");
        exit(1);
    }

    do
    {
        printf("found file or dir...");
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            printf("%s <DIR>\n", ffd.cFileName);
        }
        else
        {
            if (ffd.cFileName[0] == '_')
            {
                printf("%s <LAYOUT>\n", ffd.cFileName);
            }
            else
            {
                printf("%s <FILE>\n", ffd.cFileName);
            }
        }
        
    } while (FindNextFile(hFind, &ffd) != 0);
    
    printf("Hello!");
}