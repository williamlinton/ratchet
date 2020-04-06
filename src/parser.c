#include <string.h>
#include <stdio.h>
#include <io.h>
#include <windows.h>
#include <shlwapi.h>
#include "types.h"
#include "parser.h"
#include "test.c"

#define COMMAND_BLOCK_SIZE 1000
#define MAX_LAYOUT_FILE_SIZE 4096000
#define MAX_TEMPLATE_FILE_SIZE 4096000
#define TD_CONTENT "{{CONTENT}}"
#define MAX_TD_SIZE 20

enum TemplateDirective
{
    TDContent
};

char** substr(char* input, int startIndex, int length)
{
    int i;
    char* output = malloc(length);
    for (i = startIndex; i < startIndex + length; i++)
    {
        output[i] = input[i];
        if (input[i] == NULL) break;
    }
    return &output;
}

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

int getStringSize(char* str, bool includeNullTerminator)
{
    int size = 0;
    int index = 0;
    while (str[index++] != NULL)
    {
        size++;
    }
    return includeNullTerminator ? size + 1 : size;
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
    TemplateSettings settings;
    if (!validate(&commands, &settings))
    {
        exit(1);
    }
    
    if (!PathIsDirectory(outputDir))
    {
        if (!CreateDirectory(outputDir, NULL))
        {
            printf("Could not create output folder %s\n", outputDir);
            exit(1);
        }
        else printf("Successfully created output directory!\n");
    }
    else printf("Output directory exists!\n");

    int i;
    for (i = 0; i < commands.commandCount; i++)
    {
        int address = i * MAX_PATH;
        printf("\nCommand\n");
        printf("-------\n");
        printf("Old path: %s\n", commands.oldPath + address);
        printf("New path: %s\n", commands.newPath + address);
        printf("File type: %d\n", commands.fileType[i]);
    }
    printf("\n(end of commands)\n");

    printf("Layout file path: %s\n", settings.layoutFilePath);
    FILE* layoutFile = fopen(settings.layoutFilePath, "r");

    settings.layoutFileContents = malloc(MAX_LAYOUT_FILE_SIZE);
    settings.layoutFileContentsLength = fread(settings.layoutFileContents, sizeof(char), MAX_LAYOUT_FILE_SIZE, layoutFile);

    parse(&commands, &settings);
}

typedef struct DirectiveSearchResult
{
    bool found;
    int startIndex;
    int endIndex;
    int fileType;
} DirectiveSearchResult;

DirectiveSearchResult* findDirective(char* content, char* directive)
{

}

void parseTemplate(char* templateContent, char* layoutContent, char** outputContent, int* outputContentLength)
{ 
    int templateContentLength = getStringSize(templateContent, false);
    int layoutContentLength = getStringSize(templateContent, false);
    *outputContent = malloc(templateContentLength + layoutContentLength);

    char* td_content = TD_CONTENT;
    int td_content_size = getStringSize(td_content, false);
    printf("td_content: %s\n", td_content);

    char found_td[MAX_TD_SIZE];
    int found_td_index = -1;
    int found_td_index_start = -1;
    bool found_full_td = false;
    int li;
    for (li = 0; li < layoutContentLength; li++)
    {
        // Start found td
        char currentChar = layoutContent[li];
        if (found_td_index == -1)
        {
            if (td_content[0] == currentChar)
            {
                found_full_td = true;
                break;
            }
        }
        else
        {
            // Replace and reset found td
            if ((found_td_index + 1) == td_content_size)
            {
                found_td_index = -1;
                found_td_index_start = -1;
            }
            // Continue found td
            else if (td_content[found_td_index + 1] == currentChar)
            {
                found_td[++found_td_index] = currentChar;
            }
            // Reset found td
            else
            {
                found_td_index = -1;
                found_td_index_start = -1;
            }
            
        }
    }

    if (found_full_td)
    {
        char* preContent = substr(layoutContent, 0, found_td_index_start);
        int tdEndIndex = found_td_index_start + td_content_size;
        char* postContent = substr(layoutContent, tdEndIndex, layoutContentLength - tdEndIndex);
        printf("Templated:\n%s%s%s\n", preContent, templateContent, postContent);
    }

#if 1
    sprintf(*outputContent, templateContent);
    printf("output content: %s\n", *outputContent);
#endif


    *outputContentLength = getStringSize(*outputContent, false);
    printf("output content length: %d\n", *outputContentLength);
}

bool validate(Commands* commands, TemplateSettings* settings)
{
    bool success = true;
    int layoutFileCount = 0;
    int layoutFileIndex = -1;
    int i;
    for (i = 0; i < commands->commandCount; i++)
    {
        if (commands->fileType[i] == LAYOUT)
        {
            layoutFileCount++;
            layoutFileIndex = i;
        }
    }
    if (layoutFileCount < 1)
    {
        printf("Error: Missing layout file!\n");
        success = false;
    }
    else if (layoutFileCount > 1)
    {
        printf("Error: Can only have layout file!\n");
        success = false;
    }
    else
    {
        sprintf(settings->layoutFilePath, commands->oldPath + layoutFileIndex * MAX_PATH);
    }
    
    return success;
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
        int arrayOffset = commands->commandCount * MAX_PATH;
        char* oldAddress = commands->oldPath + arrayOffset;
        char* newAddress = commands->newPath + arrayOffset;
        int fileType = -1;
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            printf("%s <DIR>\n", ffd.cFileName);
            if (!stringsAreEqual(".", ffd.cFileName, 2, 2) && 
                !stringsAreEqual("..", ffd.cFileName, 3, 3))
            {
                fileType = DIRECTORY;
            }
        }
        else
        {
            fileType = getFileType(ffd.cFileName);
        }

        if (fileType != -1)
        {
            sprintf(oldAddress, oldPath);
            sprintf(newAddress, newPath);
            commands->fileType[commands->commandCount] = fileType;
            commands->commandCount++;

            if (fileType == DIRECTORY)
            {
                scan(oldPath, newPath, commands);
            }
        }
        
    } while (FindNextFile(hFind, &ffd) != 0);
}

int parse(Commands* commands, TemplateSettings* settings)
{
    int i;
    for (i = 0; i < commands->commandCount; i++)
    {
        int pathOffset = i * MAX_PATH;
        switch (commands->fileType[i])
        {
            case DIRECTORY:
            {
                char* outputDir = commands->newPath + pathOffset;
                if (!PathIsDirectory(outputDir))
                {
                    if (!CreateDirectory(outputDir, NULL))
                    {
                        printf("Could not create output folder %s\n", outputDir);
                        exit(1);
                    }
                    else printf("Successfully created output directory!\n");
                }
                else printf("Output directory exists!\n");
            } break;

            case JAVASCRIPT:
            case CSS:
            case HTML:
            {
                char* oldPath = commands->oldPath + pathOffset;
                char* newPath = commands->newPath + pathOffset;
                if (!CopyFile(oldPath, newPath, FALSE))
                {
                    printf("Failed to copy to %s\n", newPath);
                }
            } break;
            case TEMPLATE:
            {
                // char templateContent[MAX_TEMPLATE_FILE_SIZE + 1];
                char* templateContent = malloc(MAX_TEMPLATE_FILE_SIZE);
                FILE* sourceFile = fopen(commands->oldPath + pathOffset, "r");
                int templateFileSize = fread(templateContent, sizeof(char), MAX_TEMPLATE_FILE_SIZE, sourceFile);
                printf("TEMPLATE CONTENT:%s\n", templateContent);
                fclose(sourceFile);

                char* outputContent;
                int outputFileSize;
                parseTemplate(templateContent, settings->layoutFileContents, &outputContent, &outputFileSize);
                printf("output content in parse(): %s\n", outputContent);
                printf("output content length in parse(): %d\n", outputFileSize);

                FILE* destFile = fopen(commands->newPath + pathOffset, "w");
                fwrite(outputContent, sizeof(char), outputFileSize, destFile);
                fclose(destFile);
            } break;
        }

    }
}

int parseOld(char* sourceDir, char* outputDir)
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