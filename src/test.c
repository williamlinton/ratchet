#define TEST_COUNT 6

void getFileType_withHtmlExt_returnsHTML(int* failed)
{
    int fileType = getFileType("test.html");
    if (fileType != HTML)
    {
        *failed = 1;
    }
}

void getFileType_withHTMLExt_returnsOTHER(int* failed)
{
    int fileType = getFileType("test.HTML");
    if (fileType != OTHER)
    {
        printf("getFileType_withHTMLExt_returnsOTHER(): test.HTML was %d", fileType);
        *failed = 1;
    }
}

void getFileType_withExeExt_returnsOTHER(int* failed)
{
    int fileType = getFileType("test.exe");
    if (fileType != OTHER)
    {
        printf("getFileType_withHTMLExt_returnsOTHER(): test.exe was %d", fileType);
        *failed = 1;
    }
}

void getFileType_withRchExt_returnsTEMPLATE(int* failed)
{
    int fileType = getFileType("test.rch");
    if (fileType != TEMPLATE)
    {
        printf("getFileType_withRchExt_returnsTEMPLATE(): test.rch was %d", fileType);
        *failed = 1;
    }
}

void getFileType_withRchExtAndLeadingUnderscore_returnsLAYOUT(int* failed)
{
    int fileType = getFileType("_test.rch");
    if (fileType != LAYOUT)
    {
        printf("getFileType_withRchExtAndLeadingUnderscore_returnsLAYOUT(): _test.rch was %d", fileType);
        *failed = 1;
    }
}

void getTemplateOutputFilename_replaces_rch_with_html(int* failed)
{
    char outputFilename[MAX_PATH];
    getTemplateOutputFilename("test.rch", outputFilename);
    if (!stringsAreEqual("test.html", outputFilename, 10, 10))
    {
        printf("getTemplateOutputFilename_replaces_rch_with_html(): outputFilename was %s", outputFilename);
        *failed = 1;
    }
}

void getStringSize_withEmptyString_andNotIncludeNull_returns0(int* failed)
{
    char* str = "";
    int size = getStringSize(str, false);
    if (size != 0)
    {
        printf("getStringSize_withEmptyString_andNotIncludeNull_returns0(): size was %d", size);
        *failed = 1;
    }
}

void getStringSize_withEmptyString_andIncludeNull_returns0(int* failed)
{
    char* str = "";
    int size = getStringSize(str, true);
    if (size != 1)
    {
        printf("getStringSize_withEmptyString_andIncludeNull_returns0(): size was %d", size);
        *failed = 1;
    }
}

void getStringSize_withString_HelloWorld_andNotIncludeNull_returns10(int* failed)
{
    char* str = "";
    int size = getStringSize(str, false);
    if (size != 10)
    {
        printf("getStringSize_withString_HelloWorld_andNotIncludeNull_returns10(): size was %d", size);
        *failed = 1;
    }
}

void getStringSize_withString_HelloWorld_andIncludeNull_returns11(int* failed)
{
    char* str = "";
    int size = getStringSize(str, true);
    if (size != 11)
    {
        printf("getStringSize_withString_HelloWorld_andIncludeNull_returns11): size was %d", size);
        *failed = 1;
    }
}

int run_tests()
{
    int failed = 0;
    printf("Running tests...\n");

    void (*test_list[TEST_COUNT])(int*) = 
    {
        &getFileType_withHtmlExt_returnsHTML,
        &getFileType_withHTMLExt_returnsOTHER,
        &getFileType_withExeExt_returnsOTHER,
        &getFileType_withRchExt_returnsTEMPLATE,
        &getFileType_withRchExtAndLeadingUnderscore_returnsLAYOUT,
        &getTemplateOutputFilename_replaces_rch_with_html,
    };

    int t;
    for (t = 0; t < TEST_COUNT; t++)
    {
        int currentFailed = 0;
        test_list[t](&currentFailed);
        if (currentFailed)
        {
            failed = 1;
        }
        else
        {
            printf(".");
        }
    }

    if (failed)
    {
        printf("\nTests failed, exiting.\n");
    }
    else
    {
        printf("\nAll tests passed.\n");
    }
    
    return failed;
}