#define SIMPLE_TEST(x) int main(){ x; return 0; }

#ifdef VSNPRINTF_WORKS
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

int test_vsnprintf(const char *fmt,...)
{
    va_list     ap;
    char *s = malloc(16);
    int ret;

    va_start(ap, fmt);
    ret=vsnprintf(s,16,"%s",ap);
    va_end(ap);

    return(ret!=42 ? 1 : 0);
}

int main(void)
{
    return(test_vsnprintf("%s","A string that is longer than 16 characters"));
}
#endif


#ifdef STDC_HEADERS
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <float.h>
int main() { return 0; }
#endif /* STDC_HEADERS */


#ifdef HAVE_ATTRIBUTE

#if 0
static void test int __attribute((unused)) var)
{
  int __attribute__((unused)) x = var;
}

int main(void)
{
  test(19);
}

#else
int
main ()
{
int __attribute__((unused)) x
  ;
  return 0;
}
#endif


#endif /* HAVE_ATTRIBUTE */

#ifdef SYSTEM_SCOPE_THREADS
#include <stdlib.h>
#include <pthread.h>

int main(void)
{
    pthread_attr_t attribute;
    int ret;

    pthread_attr_init(&attribute);
    ret=pthread_attr_setscope(&attribute, PTHREAD_SCOPE_SYSTEM);
    if (ret==0)
        return 0;
    return 1;
}

#endif /* SYSTEM_SCOPE_THREADS */

#ifdef HAVE_OFF64_T
#include <sys/types.h>
int main()
{
  off64_t n = 0;
  return (int)n;
}
#endif

#ifdef HAVE_STAT64_STRUCT
#include <sys/types.h>
#include <sys/stat.h>],
struct stat64 sb;
int main()
{
  return 0;
}
#endif

#ifdef TEST_LFS_WORKS
/* Return 0 when LFS is available and 1 otherwise.  */
#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE
#define _LARGE_FILES
#define _FILE_OFFSET_BITS 64
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char **argv)
{
  /* check that off_t can hold 2^63 - 1 and perform basic operations... */
#define OFF_T_64 (((off_t) 1 << 62) - 1 + ((off_t) 1 << 62))
  if (OFF_T_64 % 2147483647 != 1)
    return 1;

  /* stat breaks on SCO OpenServer */
  struct stat buf;
  stat( argv[0], &buf );
  if (!S_ISREG(buf.st_mode))
    return 2;

  FILE *file = fopen( argv[0], "r" );
  off_t offset = ftello( file );
  fseek( file, offset, SEEK_CUR );
  fclose( file );
  return 0;
}
#endif

#ifdef CXX_HAVE_OFFSETOF

#include <stdio.h>
#include <stddef.h>

#ifdef FC_DUMMY_MAIN
#ifndef FC_DUMMY_MAIN_EQ_F77
#  ifdef __cplusplus
extern "C"
#  endif
int FC_DUMMY_MAIN()
{ return 1;}
#endif
#endif
int
main ()
{

  struct index_st
  {
    unsigned char type;
    unsigned char num;
    unsigned int len;
  };
  typedef struct index_st index_t;
  int x,y;
  x = offsetof(struct index_st, len);
  y = offsetof(index_t, num)

  ;
  return 0;
}

#endif
