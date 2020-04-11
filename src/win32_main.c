#include <windows.h>
#include <shlwapi.h>
#include "parser.c"

int RchGetFileSize(char* filename, long* fileSize)
{
    LPCSTR lpFileName = filename;
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
    *fileSize = layoutFileSizeRaw.QuadPart;
}
int RchScanDirectory(char** flatFileList, int* fileCount) { }
int RchCopyFile(char* sourceFile, char* outputFile) { }

int main(int argc, char *argv[])
{
    run_tests();
    char sourceDir[] = "C:\\Users\\William\\Projects\\nnj\\nononsensejavascript.com\\templates";
    char outputDir[] = "C:\\Users\\William\\Projects\\nnj\\nononsensejavascript.com\\build";
    PlatformApi api;
    api.RchCopyFile = RchCopyFile;
    api.RchGetFileSize = RchGetFileSize;
    api.RchScanDirectory = RchScanDirectory;
    generate(api, sourceDir, outputDir);
}

