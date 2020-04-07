#define ASSERT(x, msg, ...) if (!x) { printf(msg, __VA_ARGS__); *failed = 1; }
#define TEST(name) void name(int* failed)

#define TEST_COUNT 11

TEST(getFileType_withHtmlExt_returnsHTML)
{
    int fileType = getFileType("test.html");
    ASSERT(fileType == HTML, "wrong file type: %d", fileType);
}

TEST(getFileType_withHTMLExt_returnsOTHER)
{
    int fileType = getFileType("test.HTML");
    ASSERT(fileType == OTHER, "getFileType_withHTMLExt_returnsOTHER(): test.HTML was %d", fileType);
}

TEST(getFileType_withExeExt_returnsOTHER)
{
    int fileType = getFileType("test.exe");
    ASSERT(fileType == OTHER, "getFileType_withHTMLExt_returnsOTHER(): test.exe was %d", fileType);
}

TEST(getFileType_withRchExt_returnsTEMPLATE)
{
    int fileType = getFileType("test.rch");
    ASSERT(fileType == TEMPLATE, "getFileType_withRchExt_returnsTEMPLATE(): test.rch was %d", fileType);
}

TEST(getFileType_withRchExtAndLeadingUnderscore_returnsLAYOUT)
{
    int fileType = getFileType("_test.rch");
    ASSERT(fileType == LAYOUT, "getFileType_withRchExtAndLeadingUnderscore_returnsLAYOUT(): _test.rch was %d", fileType);
}

TEST(getTemplateOutputFilename_replaces_rch_with_html)
{
    char outputFilename[MAX_PATH];
    getTemplateOutputFilename("test.rch", outputFilename);
    ASSERT(stringsAreEqual("test.html", outputFilename, 10, 10), 
        "getTemplateOutputFilename_replaces_rch_with_html(): outputFilename was %s", outputFilename);
}

TEST(getStringSize_withEmptyString_andNotIncludeNull_returns0)
{
    char* str = "";
    int size = getStringSize(str, false);
    ASSERT(size == 0, 
        "getStringSize_withEmptyString_andNotIncludeNull_returns0(): size was %d", size);
}

TEST(getStringSize_withEmptyString_andIncludeNull_returns0)
{
    char* str = "";
    int size = getStringSize(str, true);
    ASSERT(size == 1, "getStringSize_withEmptyString_andIncludeNull_returns0(): size was %d", size);
}

TEST(getStringSize_withString_HelloWorld_andNotIncludeNull_returns10)
{
    char* str = "HelloWorld";
    int size = getStringSize(str, false);
    ASSERT(size == 10, "getStringSize_withString_HelloWorld_andNotIncludeNull_returns10(): size was %d", size);
}

TEST(getStringSize_withString_HelloWorld_andIncludeNull_returns11)
{
    char* str = "HelloWorld";
    int size = getStringSize(str, true);
    ASSERT(size == 11, "getStringSize_withString_HelloWorld_andIncludeNull_returns11): size was %d", size);
}

TEST(findDFirstirective_withDirectivePresent_returnsPopulatedSearchResult)
{
    #if 0
    char* directive = "{{CONTENT}}";
    char* content = "sldjkf sdflkjs kldf sdflkjs\n lkdfjs d{{CONTENT}} asdfkljsldkjf";
    DirectiveSearchResult result = findFirstDirective(content, directive);
    ASSERT(result.found == true, "couldn't find directive %s in %s", directive, content);
    ASSERT(result.startIndex == 38, "wrong startIndex: %d", result.startIndex);
    ASSERT(result.endIndex == 48, "wrong endIndex: %d", result.endIndex);
    #endif
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
        &getStringSize_withEmptyString_andNotIncludeNull_returns0,
        &getStringSize_withEmptyString_andIncludeNull_returns0,
        &getStringSize_withString_HelloWorld_andNotIncludeNull_returns10,
        &getStringSize_withString_HelloWorld_andIncludeNull_returns11,
        &findDFirstirective_withDirectivePresent_returnsPopulatedSearchResult,
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