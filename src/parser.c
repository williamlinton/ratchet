#include <string.h>
#include <stdio.h>
#include <io.h>
#include <windows.h>
#include "types.h"
#include "test.c"

bool stringsAreEqual(char* s1, char* s2, int length1, int length2)
{
    int index = 0;
    while (index < length1 && index < length2)
    {
        if (s1[index] != s2[index])
        {
            return false;
        }
        if (s1[index] == NULL || s2[index] == NULL) break;
        ++index;
    }
    return true;
}

int getFileType(char* filename)
{
    int indexPeriod = -1;
    int extensionLength = 0;
    for (int i = 0; i < MAX_PATH; i++)
    {
        if (filename[i] == NULL) break;
        if (filename[i] == '.') indexPeriod = i;
    }

    if (indexPeriod > -1)
    {
        char extension[MAX_PATH];
        int extensionCurrIndex = 0;
        for (int i = indexPeriod + 1; i < MAX_PATH; i++)
        {
            extension[extensionCurrIndex] = filename[i];
            ++extensionCurrIndex;
            if (filename[i] == NULL) break;
        }
        
        if (stringsAreEqual("html", extension, extensionCurrIndex, 5))
        {
            return HTML;
        }

        if (stringsAreEqual("js", extension, extensionCurrIndex, 5))
        {
            return JAVASCRIPT;
        }

        if (stringsAreEqual("css", extension, extensionCurrIndex, 5))
        {
            return CSS;
        }

        if (stringsAreEqual("rch", extension, extensionCurrIndex, 5))
        {
            if (filename[0] == '_') return LAYOUT;
            return TEMPLATE;
        }
    }
    else
    {
        return OTHER;
    }

    return OTHER;
}

int main(int argc, char *argv[])
{
    run_tests();

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