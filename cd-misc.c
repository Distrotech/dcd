/* $Revision: 1.2 $ */

/*
 * This is part of DCD, Dave's CD Player.
 * (C) 1998-2001 David E. Smith <dave@technopagan.org>
 * Released under GNU GPL v2. (See 'COPYING' for details.)
 *
 * These are weird miscellaneous bits of CD handling code that don't
 * really fit anywhere else...
 */

#include "libcdplay.h"
#include "cd-misc.h"
#include <sys/types.h>

u_char cd_play_next_track(void) {
  u_char target_track;
  u_char cur_track = cd_current_track();
  if (cur_track >= cd_last_track()) target_track = cd_first_track();
     else target_track = (cur_track + 1);
  cd_play_disc(target_track);
  return target_track;
}

u_char cd_play_prev_track(void) {
  u_char target_track;
  u_char cur_track = cd_current_track();
  if (cur_track <= cd_first_track()) target_track = cd_last_track();
     else target_track = (cur_track - 1);
  cd_play_disc(target_track);
  return target_track;
}
