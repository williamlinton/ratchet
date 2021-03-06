#define ASSERT(x, msg, ...) if (!x) { printf("\n"); printf(msg, __VA_ARGS__); *failed = 1; }
#define TEST(name) void name(int* failed)

#define TEST_COUNT 17

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
    #if 1
    char* directive = "{{CONTENT}}";
    char* content = "sldjkf sdflkjs kldf sdflkjs\n lkdfjs d{{CONTENT}} asdfkljsldkjf";
    DirectiveSearchResult result = findFirstDirective(content, directive);
    ASSERT(result.found == true, "\ncouldn't find directive %s in %s", directive, content);
    ASSERT(result.startIndex == 38, "wrong startIndex: %d", result.startIndex);
    ASSERT(result.endIndex == 48, "wrong endIndex: %d", result.endIndex);
    #endif
}

TEST(findDFirstirective_withDirectiveAtEndOfString_returnsPopulatedSearchResult)
{
    #if 1
    char* directive = "{{CONTENT}}";
    char* content = "sldjkf sdflkjs kldf sdflkjs\n lkdfjs d{{CONTENT}}";
    DirectiveSearchResult result = findFirstDirective(content, directive);
    ASSERT(result.found == true, "\ncouldn't find directive %s in %s", directive, content);
    ASSERT(result.startIndex == 38, "wrong startIndex: %d", result.startIndex);
    ASSERT(result.endIndex == 48, "wrong endIndex: %d", result.endIndex);
    #endif
}

TEST(findDFirstirective_withNoDirectiveButCurlyBraces_returnsPopulatedSearchResult)
{
    #if 1
    char* directive = "{{CONTENT}}";
    char* content = "sldjkf sdflkjs kldf sdflkjs\n lkdfjs d{{ASDF}}";
    DirectiveSearchResult result = findFirstDirective(content, directive);
    ASSERT(result.found == false, "\nfalse positive: found directive %s in %s", directive, content);
    ASSERT(result.startIndex == -1, "erroneously set startIndex: %d", result.startIndex);
    ASSERT(result.endIndex == -1, "erroneously endIndex: %d", result.endIndex);
    #endif
}

TEST(substr_atBeginningOfString)
{
    char* str = "HelloWorld and then some";
    char* result;
    substr(str, &result, 0, 10);
    int length = strlen(result);
    ASSERT(stringsAreEqual2("HelloWorld", result), "Expected \"HelloWorld\", got %s", result);
}

TEST(substr_atEndOfString)
{
    char* str = "HelloWorld and then some";
    char* result;
    substr(str, &result, 14, 10);
    int length = strlen(result);
    ASSERT(stringsAreEqual2(" then some", result), "Expected \" then some\", got %s", result);
}

TEST(substr_inMiddleOfString)
{
    char* str = "HelloWorld and then some";
    char* result;
    substr(str, &result, 10, 10);
    int length = strlen(result);
    ASSERT(stringsAreEqual2(" and then ", result), "Expected \" and then \", got %s", result);
}

TEST(substr_pastEndOfString)
{
    char* str = "HelloWorld and then some";
    char* result;
    substr(str, &result, 20, 10);
    int length = strlen(result);
    ASSERT(stringsAreEqual2("some", result), "Expected \"some\", got %s", result);
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
        &findDFirstirective_withDirectiveAtEndOfString_returnsPopulatedSearchResult,
        &findDFirstirective_withNoDirectiveButCurlyBraces_returnsPopulatedSearchResult,
        &substr_atBeginningOfString,
        &substr_atEndOfString,
        &substr_inMiddleOfString,
        &substr_pastEndOfString,
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