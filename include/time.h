#ifndef __TIME__
#define __TIME__
#include <time.h>
#include <string.h>
#include <stdlib.h>
#define OL_INTSEC    0x1
#define OL_INTMSEC   0x2
#define OL_INTUSEC   0x4

typedef struct {
  uint64_t start;
  uint32_t inter;
} OlInterval;

typedef struct {
  uint64_t start;
  uint32_t inter;
  void(*f)(void*);
} OlFunInterval;

int ol_elapsed(OlInterval int, int flags)
{
  switch(flags)
  {
    case OL_INTSEC:
    return ((time(NULL) - int.start) % int.inter == 0);
    // TO DO: add next cases
  }
  return 1;
}

size_t ol_get_year()
{
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  size_t year = tm.tm_year + 1900;
  return year;
}
