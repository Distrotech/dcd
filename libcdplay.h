/*
 * This is libcdplay, a Linux-specific CD-ROM playing library.
 *
 * This code is (C) 1998-2001 David E. Smith <dave@technopagan.org>
 * and released under the GNU GPL. See `COPYING' for details.
 *
 * Older versions were available under the LGPL; this is no longer true.
 */

#ifndef __CD_PLAYER_LIB
#define __CD_PLAYER_LIB

#include <sys/types.h>
#include <linux/cdrom.h>

/* Initialization. Takes the drive to set up (like "/dev/cdrom") and
 * returns the drive's file descriptor. Note that you shouldn't ever
 * need that, but it's there just in case. (You can also get the
 * drive's file descriptor with the cd_fd() function.)
 */
int cd_init_player (char *device);


/* Playing operations - tracks, disc, etc.
 * These operations return the track number you fed them, or -1 in case of
 * an error (like trying to play a data track in cd_play_track).
 */
u_char cd_play_track (u_char trknum);
u_char cd_play_sequence (u_char trk1, u_char trk2);
u_char cd_play_disc (u_char trknum);


/* Informational operations */
u_char cd_current_track(void);
unsigned long cd_disc_length_frames (void);
unsigned long cd_track_length_frames (u_char trknum);

/* rounded to the nearest second */
int cd_disc_length (void);
int cd_track_length (u_char trknum);
int cd_data_track (u_char trknum);

u_char cd_first_track(void); /* almost always `1' */
u_char cd_last_track(void);


/* Simple commands. */
void cd_stop(void);
void cd_eject(void);
void cd_pause(void);    /* toggles pause/resume status internally */
int cd_active(void);    /* if playing or paused, return TRUE */
int cd_paused(void);    /* boolean for pause(TRUE) or not */
int cd_present(void);   /* is there a disc in the drive? */

/* The RAW frame count is really only useful for cddb calculations. */
unsigned long raw_track_length (u_char trknum);

int cd_fd (void); /* returns the CD file descriptor in case you want to
                     do your own raw IOCTLs on the device */
int cd_hw_status (void); /* raw CD status, from cdrom.h - never use this */

char *cd_discid(void);

#endif /* __CD_PLAYER_LIB */
