#include <string.h>
#include <stdio.h>
#include <io.h>
#include <windows.h>
#include <shlwapi.h>
#include "types.h"
#include "parser.h"
#include "test.c"

#define COMMAND_BLOCK_SIZE 1000

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

void getTemplateOutputFilename(char* oldFilename, char* newFilename)
{
    int indexPeriod = -1;
    int extensionLength = 0;
    for (int i = 0; i < MAX_PATH; i++)
    {
        if (oldFilename[i] == NULL) break;
        if (oldFilename[i] == '.') indexPeriod = i;
    }

    if (indexPeriod > -1)
    {
        int i;
        for (i = 0; i < indexPeriod; i++)
        {
            newFilename[i] = oldFilename[i];
        }

        newFilename[i] = NULL;

        sprintf(newFilename, "%s.html", newFilename);
    }
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

void generate(char* sourceDir, char* outputDir)
{
    Commands commands;
    commands.oldPath = (char *)malloc(sizeof(char) * MAX_PATH * COMMAND_BLOCK_SIZE);
    commands.newPath = (char *)malloc(sizeof(char) * MAX_PATH * COMMAND_BLOCK_SIZE);
    commands.fileType = (int *)malloc(sizeof(int) * COMMAND_BLOCK_SIZE);
    commands.commandCount = 0;
    memset(commands.oldPath, 0, sizeof(char) * MAX_PATH * COMMAND_BLOCK_SIZE);
    memset(commands.newPath, 0, sizeof(char) * MAX_PATH * COMMAND_BLOCK_SIZE);
    memset(commands.fileType, 0, sizeof(int) * COMMAND_BLOCK_SIZE);

    scan(sourceDir, outputDir, &commands);

    int i;
    for (i = 0; i < commands.commandCount; i++)
    {
        printf("Command\n");
        printf("-------\n");
        printf("Old path: %s\n", commands.oldPath[i * MAX_PATH]);
        printf("New path: %s\n", commands.newPath[i * MAX_PATH]);
        printf("File type: %d\n", commands.fileType[i]);
    }
}

void scan(char* sourceDir, char* outputDir, Commands* commands)
{
    printf("Scanning %s\n", sourceDir);
    TCHAR szDir[MAX_PATH];
    sprintf(szDir, "%s\\*", sourceDir);

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
        char oldPath[MAX_PATH];
        char newPath[MAX_PATH];
        sprintf(oldPath, "%s\\%s", sourceDir, ffd.cFileName);
        sprintf(newPath, "%s\\%s", outputDir, ffd.cFileName);
        printf("found file or dir...\n");
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            printf("%s <DIR>\n", ffd.cFileName);
            if (!stringsAreEqual(".", ffd.cFileName, 2, 2) && 
                !stringsAreEqual("..", ffd.cFileName, 3, 3))
            {
                commands->oldPath[commands->commandCount * MAX_PATH] = oldPath;
                commands->newPath[commands->commandCount * MAX_PATH] = newPath;
                commands->fileType[commands->commandCount] = DIRECTORY;
                scan(oldPath, newPath, commands);
            }
        }
        else
        {
            commands->oldPath[commands->commandCount * MAX_PATH] = oldPath;
            commands->newPath[commands->commandCount * MAX_PATH] = newPath;
            commands->fileType[commands->commandCount] = getFileType(ffd.cFileName);
        }
        commands->commandCount++;
        
    } while (FindNextFile(hFind, &ffd) != 0);
}

int parse(char* sourceDir, char* outputDir)
{
    printf("Parsing %s\n", sourceDir);
    TCHAR szDir[MAX_PATH];
    sprintf(szDir, "%s\\*", sourceDir);

    if (!PathIsDirectory(outputDir))
    {
        if (!CreateDirectory(outputDir, NULL))
        {
            printf("Could not create output folder %s\n", outputDir);
            exit(1);
        }
        else
        {
            printf("Successfully created output directory!\n");
        }
    }
    else
    {
        printf("Output directory exists!\n");
    }

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
        char oldPath[MAX_PATH];
        char newPath[MAX_PATH];
        sprintf(oldPath, "%s\\%s", sourceDir, ffd.cFileName);
        sprintf(newPath, "%s\\%s", outputDir, ffd.cFileName);
        printf("found file or dir...\n");
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            printf("%s <DIR>\n", ffd.cFileName);
            if (!stringsAreEqual(".", ffd.cFileName, 2, 2) && 
                !stringsAreEqual("..", ffd.cFileName, 3, 3))
            {
                parse(oldPath, newPath);
            }
        }
        else
        {
            switch (getFileType(ffd.cFileName))
            {
                case HTML:
                {
                    printf("%s <HTML>\n", ffd.cFileName);
                    if (!CopyFile(oldPath, newPath, FALSE))
                    {
                        printf("Failed to copy to %s\n", newPath);
                    }
                } break;
                case JAVASCRIPT:
                {
                    printf("%s <JAVASCRIPT>\n", ffd.cFileName);
                    if (!CopyFile(oldPath, newPath, FALSE))
                    {
                        printf("Failed to copy to %s\n", newPath);
                    }
                } break;
                case CSS:
                {
                    printf("%s <CSS>\n", ffd.cFileName);
                    if (!CopyFile(oldPath, newPath, FALSE))
                    {
                        printf("Failed to copy to %s\n", newPath);
                    }
                } break;
                case TEMPLATE:
                {
                    printf("%s <TEMPLATE>\n", ffd.cFileName);
                    char outputFilename[MAX_PATH];
                    getTemplateOutputFilename(ffd.cFileName, outputFilename);
                    sprintf(newPath, "%s%s", outputDir, outputFilename);
                    printf("New path for template file: %s\n", newPath);
                    FILE* existingFile = fopen(oldPath, "r");
                    FILE* newfile = fopen(newPath, "w");
                } break;
                case LAYOUT:
                {
                    printf("%s <LAYOUT>\n", ffd.cFileName);
                } break;
            }
        }
        
    } while (FindNextFile(hFind, &ffd) != 0);
}