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
#define LOGGING 0

enum TemplateDirective
{
    TDContent
};

substr(char* input, char** output, int startIndex, int length)
{
    int i;
    int o;
    *output = malloc(length + 1);
    for (i = startIndex, o = 0; i < startIndex + length; i++, o++)
    {
        if (input[i] == NULL)
        {
            (*output)[o] = NULL;
            break;
        }
        (*output)[o] = input[i];
    }
    (*output)[o] = NULL;
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

bool stringsAreEqual2(char* s1, char* s2)
{
    int index = 0;
    while (true)
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

void generate(PlatformApi api, char* sourceDir, char* outputDir)
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
        else { 
            #if LOGGING
            printf("Successfully created output directory!\n");
            #endif
        }
    }
    else { 
        #if LOGGING
        printf("Output directory exists!\n");
        #endif
    }

    #if LOGGING
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
    #endif



#if 0
    LPCSTR lpFileName = settings.layoutFilePath;
    HANDLE layoutFileHandle = CreateFileA(lpFileName, 
                       FILE_READ_ATTRIBUTES,
                       FILE_SHARE_READ,
                       0,
                       OPEN_EXISTING,
                       0,
                       0);
    LARGE_INTEGER layoutFileSizeRaw = {0};
    if (!GetFileSizeEx(layoutFileHandle, &layoutFileSizeRaw))
    {
        DWORD error = GetLastError();
    }
    if (!CloseHandle(layoutFileHandle))
    {
        printf("Could not close layout file handle!\n");
    }
    printf("Quad part: %d\n", layoutFileSizeRaw.QuadPart);
    long layoutFileSize = layoutFileSizeRaw.QuadPart;
    #endif
    
    long layoutFileSize;
    api.RchGetFileSize(settings.layoutFilePath, &layoutFileSize);

    FILE* layoutFile = fopen(settings.layoutFilePath, "r");
    printf("Layout file actual size: %d\n", layoutFileSize);

    settings.layoutFileContents = malloc(layoutFileSize + 1);
    memset(settings.layoutFileContents, 0, layoutFileSize);
    printf("LAYOUT FILE CONTENTS, pre-read:\n%s\n----------\n", settings.layoutFileContents);
    settings.layoutFileContentsLength = fread(settings.layoutFileContents, sizeof(char), layoutFileSize, layoutFile);
    fclose(layoutFile);
    printf("LAYOUT FILE CONTENTS:\n%s\n----------\n", settings.layoutFileContents);

    parse(&commands, &settings);

    printf("\n----------\n");
    printf("Succeeded!\n");
}

DirectiveSearchResult findFirstDirective(char* content, char* directive)
{
    int td_content_size = getStringSize(directive, false);
    int layoutContentLength = getStringSize(content, false);
    DirectiveSearchResult result;
    result.startIndex = -1;
    result.endIndex = -1;
    result.found = false;

    int found_td_index = -1;
    int found_td_index_start = -1;
    int li;
    for (li = 0; li < layoutContentLength; li++)
    {
        // Nothing found yet
        char currentChar = content[li];
        if (found_td_index == -1)
        {
            // Start found td
            if (directive[0] == currentChar)
            {
                found_td_index = 0;
                found_td_index_start = li;
            }
        }
        // Already found something
        else
        {
            // Middle of td
            if (directive[found_td_index + 1] == currentChar)
            {
                ++found_td_index;
                // End of td
                if ((found_td_index + 1) == td_content_size)
                {
                    result.found = true;
                    result.startIndex = found_td_index_start;
                    result.endIndex = found_td_index_start + td_content_size;
                    break;
                }
            }
            // Discover this is not td
            else
            {
                found_td_index = -1;
                found_td_index_start = -1;
            }
            
        }
    }
   
    return result;
}

void parseTemplate(char* templateContent, char* layoutContent, char** outputContent, int* outputContentLength)
{ 
    int templateContentLength = getStringSize(templateContent, false);
    int layoutContentLength = getStringSize(layoutContent, false);
    *outputContent = malloc(templateContentLength + layoutContentLength);
    printf("Allocation size for output content: %d\n", templateContentLength + layoutContentLength);

    DirectiveSearchResult contentResult = findFirstDirective(layoutContent, TD_CONTENT);

    if (contentResult.found)
    {
        char* preContent;
        substr(layoutContent, &preContent, 0, contentResult.startIndex);
        printf("preContent:\n%s\n", preContent);
        char* postContent;
        substr(layoutContent, &postContent, contentResult.endIndex, layoutContentLength - contentResult.endIndex);
        printf("postContent:\n%s\n", postContent);
        printf("Templated:\n%s%s%s\nEnd Templated\n", preContent, templateContent, postContent);

        sprintf(*outputContent, "%s%s%s", preContent, templateContent, postContent);
        printf("Output content:\n%s", *outputContent);
        *outputContentLength = strlen(*outputContent);
    }

    // *outputContentLength = getStringSize(*outputContent, false);

    // #if LOGGING
    // printf("output content length: %d\n", *outputContentLength);
    // #endif
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
    #if LOGGING
    printf("Scanning %s\n", sourceDir);
    #endif
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
        #if LOGGING
        printf("found file or dir...\n");
        #endif
        int arrayOffset = commands->commandCount * MAX_PATH;
        char* oldAddress = commands->oldPath + arrayOffset;
        char* newAddress = commands->newPath + arrayOffset;
        int fileType = -1;
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            #if LOGGING
            printf("%s <DIR>\n", ffd.cFileName);
            #endif
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
            if (fileType == TEMPLATE)
            {
                char newFilename[MAX_PATH];
                getTemplateOutputFilename(ffd.cFileName, newFilename);
                sprintf(newPath, "%s\\%s", outputDir, newFilename);
            }
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
                    else
                    {
                        #if LOGGING
                        printf("Successfully created output directory!\n");
                        #endif
                    }
                }
                else 
                {
                    #if LOGGING
                    printf("Output directory exists!\n");
                    #endif
                }
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
                
                #if LOGGING
                printf("TEMPLATE CONTENT:%s\n", templateContent);
                #endif

                fclose(sourceFile);

                char* outputContent;
                int outputFileSize;
                parseTemplate(templateContent, settings->layoutFileContents, &outputContent, &outputFileSize);

                #if LOGGING
                printf("output content in parse(): %s\n", outputContent);
                printf("output content length in parse(): %d\n", outputFileSize);
                #endif

                FILE* destFile = fopen(commands->newPath + pathOffset, "w");
                fwrite(outputContent, sizeof(char), outputFileSize, destFile);
                fclose(destFile);
            } break;
        }

    }
}
