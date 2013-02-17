/*
 * infinite.c - part of dcd, (C)1998-2001 David E. Smith <dave@technopagan.org>
 *
 * This file is CD operations that, by design, never exit. It may be a
 * good idea if you install a SIGTERM handler before you call one of these.
 *
 * GNU General Public License. See `COPYING' for details.
 */

#include "libcdplay.h"
#include "infinite.h"
#include "dcd.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void loop_disc (void) {
  while (TRUE) {
    int len = cd_disc_length();
    cd_stop();
    cd_play_disc(cd_first_track());
    alarm (len+EXTRA_SLEEP_TIME+1);
    pause();
  } /* while */
} /* loop_disc */


void shuffle_disc (void) {
  int tn = 0, playing = 0;
  while (TRUE) {
   while ((tn == playing) & (!cd_data_track(tn)))
      tn = 1+ (int) (rand() % cd_last_track());
    #ifdef DEBUG
      fprintf (stderr, "shuffle_disc now playing track %i\n", tn);
    #endif
    cd_play_track ((u_char)tn);
    playing = tn;
    alarm (cd_track_length(tn) + EXTRA_SLEEP_TIME);
    pause();
  } /* while */
} /* func */
