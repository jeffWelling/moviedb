#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

void logProgram ( char *progName )
{
#if LOGFILE
  { 
    int logfile;
    logfile = open(LOGFILENAME, O_RDWR | O_CREAT | O_APPEND, 0666);
    if (logfile != -1) 
    {
      char StrBuf[80];
      time_t temp;
      time(&temp);
      sprintf(StrBuf, "%s\t%s\t%s\n",
        ctime(&temp), progName, getpwuid(getuid())->pw_name);
      StrBuf[strlen(ctime(&temp))-1] = ' ';
      write(logfile, StrBuf, strlen(StrBuf));
      close(logfile);
    }
  }
#endif
  return ;
}
