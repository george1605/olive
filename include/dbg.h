#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define expect(thing, tests, fails) do { \
                tests++; \
                if(!(thing)) \
                        printf("Test %i failed at line %i, file %s", tests, __LINE__, __FILE__); \
                        fails++; \
                } while(0)

#define TEST(name, thing)  struct test name; \
                     name.file = malloc(sizeof(__FILE__)); \
                     memcpy(name.file, __FILE__, sizeof(__FILE__)); \
                     name.line = __LINE__; \
                     name.failed = !(thing); \
                     name.expression = #thing;

struct test
{
        int failed;
        int line;
        char* file;
        char* expression;
};

void ol_save_test(char* fname, struct test t)
{
        FILE* f = fopen(fname, "w");
        fprintf(f, "Test %s at line %i, file %s: %s", t.expression, t.line, t.file, (t.failed ? "failed": "passed"));
        fclose(f);
}

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
/*
* Prints the error specified.
* @param char* if NULL, prints GetLastError() code
*/
void ol_show_error(char* error)
{
    if(error == NULL)
        printf("[Error]: %i\n", GetLastError());
    else
        printf("[Error]: %s\n", error);
}

void ol_term_color(int c)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
}

uint64_t ol_mem_usage()
{
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    return (uint64_t)pmc.PrivateUsage;
}

#elif defined(__linux__)
#include <unistd.h>
#include <sys/resource.h>

void ol_show_error(char* error)
{
    if(error == NULL)
        printf("[Error]: %i\n", errno);
    else
        printf("[Error]: %s\n", error);

}

/// may or may not work 
uint64_t ol_mem_usage()
{
    struct rusage r;
    getrusage(RUSAGE_SELF, &r);
    return r.ru_maxrss;
}

#endif