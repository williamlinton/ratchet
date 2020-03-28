#define TEST_COUNT 3

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

int run_tests()
{
    int failed = 0;
    printf("Running tests...\n");

    void (*test_list[TEST_COUNT])(int*) = 
    {
        &getFileType_withHtmlExt_returnsHTML,
        &getFileType_withHTMLExt_returnsOTHER,
        &getFileType_withExeExt_returnsOTHER
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