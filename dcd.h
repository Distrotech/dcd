/* 
 * This is DCD, Dave's CD Player.
 * (C)1998 David E. Smith <dave@technopagan.org> - released under GNU GPL.
 * See 'COPYING' for details.
 */

#ifndef __DCD_H
#define __DCD_H

#define TRUE 1
#define FALSE (!TRUE)

#ifndef CDROM_DEVICE
#define CDROM_DEVICE "/dev/cdrom"
#endif

#ifndef EXTRA_SLEEP_TIME
#define EXTRA_SLEEP_TIME 0
#endif

int sigterm_handler (int sig);
int sigusr1_handler (int sig);

#endif /* __DCD_H */
