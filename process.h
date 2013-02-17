/* process.h - PID handling bits for dcd.
 * (C) 1999 David E. Smith (dave@technopagan.org)
 * Released under GNU GPL, see `COPYING' for details.
 */

/* dcd process.h - internal process management toys */

#ifndef __DCD_PROCESS
#define __DCD_PROCESS

#ifndef PIDFILE
#define PIDFILE ".dcdpid"
#endif

int dcd_getpid(void);
int dcd_setpid(void);
int dcd_delpid(void);
int dcd_kill(int signal);

#endif
