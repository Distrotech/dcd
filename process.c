/* $Revision: 1.4 $ */
/* process.c - PID handling bits for dcd
 * (C) 1999 David E. Smith (dave@technopagan.org)
 * Released under GNU GPL, see `COPYING' for details.
 */

#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
/* the above is for PATH_MAX that is supposed to be in unistd but ain't */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <signal.h>

#define DIRSIZE ((PATH_MAX) - 8)
#define SPACE ' '
/* something really should be done with these hard-coded numbers */

/* This does a malloc() so be sure to have a matching free() later on. */
static char * fname (void) {
  char *home = getenv("HOME");
  char *ret = malloc(DIRSIZE);
  memset (ret, '\0', sizeof(ret));
  snprintf (ret, DIRSIZE, "%s/%s", home, PIDFILE);
  return ret;
}

int dcd_getpid (void) {
  int pid;
  FILE *fd = fopen (fname(), "r");
  if (NULL == fd) return 0;
  fscanf (fd, "%i", &pid);
  #ifdef DEBUG
    if (pid == 0)
      fprintf (stderr, "dcd_getpid couldn't find an old PID\n");
    else
      fprintf (stderr, "dcd_getpid found old pid %i\n", pid);
  #endif
  fclose(fd);
  return pid;
}

int dcd_setpid (void) {
  int pid = getpid();
  FILE *fd = fopen (fname(), "w+");
  if (NULL == fd) return -1;
  fprintf (fd, "%i\n", pid);
  fclose (fd);
  #ifdef DEBUG
    fprintf (stderr, "dcd_setpid wrote %i\n", pid);
  #endif
  return pid;
}

int dcd_delpid (void) {
  unlink (fname()); /* not worried about errors */
  #ifdef DEBUG
    fprintf (stderr, "dcd_delpid just went off\n");
  #endif
  return 0;
}

int dcd_kill (int signal) {
  int pid = dcd_getpid();
  int k = 0;
  #ifdef DEBUG
    fprintf (stderr, "dcd_kill: sending signal %i to process %i\n",
             signal, pid);
  #endif
  if (pid != 0) k = kill (pid, signal);
  /* kill() is intentionally not error checked. If it fails, it will do
     so silently. This is, believe it or not, a good thing. */
  return pid;
}
