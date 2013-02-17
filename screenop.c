/* $Revision: 1.4 $ */
/* screenop.c - screen/display bits for dcd
 * (C) 1999 David E. Smith (dave@technopagan.org)
 * Released under GNU GPL, see `COPYING' for details.
 */

#include "screenop.h"
#include "version.h"
#include "libcdplay.h"
#include "dcd.h"
#include "mbo.h"
#include "minilzo.h"
#include <unistd.h>
#include <stdio.h>

void disk_directory(void) {
  u_char ct = cd_current_track();
  int tl;
  char outline[80];
  int disc_length = cd_disc_length();
  int i;
  #ifdef DEBUG
    fprintf (stderr, "Entering directory. tz is %i\n", cd_last_track());
  #endif

  sprintf (outline, "Track  Time  (%i tracks / %2i:%02i) %30s",
          cd_last_track(), disc_length/60, disc_length%60, mbo_trackname(0));
  printf ("%s\n", outline);
  
  for (i=1; i<=cd_last_track(); i++) {
    tl = cd_track_length(i);
    sprintf (outline, "%s %2i  %2i:%02i  %-45s", (i==ct ? "*" : " "),
            i, (tl/60), (tl%60), mbo_trackname(i));
    printf ("%s\n", outline);
  } /* for */
}

void print_status(void) {
  int disc_length, track_length, current_track, last_track, disc_status;
  disc_length = cd_disc_length();
  last_track = cd_last_track();
  current_track = cd_current_track();
  disc_status = cd_hw_status();
  if (0 == current_track) {
    printf ("Not playing (%i tracks, %i:%02i)\n", last_track,
             disc_length/60, disc_length%60);
    return;
  }
  track_length = cd_track_length(current_track);
  printf ("Playing track %i (of %i), length %i:%02i (of %i:%02i)",
           current_track, last_track, track_length/60, track_length%60,
           disc_length/60, disc_length%60);
  printf ("\n");
}

void short_help (void) {
  printf ("%s\n", VERSION);
  printf ("\n");
  printf ("For a complete list of options, try `%s help'.\n", PROGNAME);
  printf ("Options include: Back-track, Dir, Eject, Forward-track, Help,\n");
  printf ("   Info, Loop, Pause, Random, Stop, Version.\n");
  printf ("(For the above commands, only the first letter is required.\n");
}

void print_help (void) {
  dcd_version();
  printf ("\n");
  printf ("This program is released to the public with NO WARRANTY under the\n");
  printf ("GNU General Public License. See `COPYING' in the %s distribution\n", PROGNAME);
  printf ("for details.\n\n");
  printf ("Usage: %s [options] [tracklist]\n", PROGNAME);
  printf ("The following options cause %s to exit immediately:\n", PROGNAME);
  printf ("      d   Dir      lists the CD directory\n");
  printf ("      e   Eject    opens the CD tray\n");
  printf ("      h   Help     displays this message :-)\n");
  printf ("      i   Info     minimal info/status of current CD\n");
  printf ("      p   Pause    pause/resume the CD\n");
  printf ("      s   Stop     stops the CD player\n");
  printf ("      v   Version  displays software version\n");
  printf ("      f   Forward  play the next track on CD\n");
  printf ("      b   Back     play the previous track on CD\n");
  printf ("      k   Kill     kill all known instances of %s and stop CD\n", PROGNAME);
  printf ("\n");
  printf ("These options are for programming %s like a CD player:\n", PROGNAME);
  printf ("      l   Loop    loop the listed tracks (or CD if none given)\n");  
  printf ("      r   Random  plays random track from the CD\n");
  printf ("      q   Quit    Don't detach from console until finished\n");
  printf ("\n");
  printf ("These are miscellaneous options, intended mainly for shell scripting:\n");
  printf ("      a   firstrk  Echo the first track number to stdout\n");
  printf ("      z   last-trk Echo the last track number to stdout\n");
  printf ("      x   cdindeX  Echo the CD Index disc ID (deprecated?)\n");
} /* help */

void dcd_version (void) {
  printf ("%s\n", VERSION);
  printf ("Compiled on %s with default drive %s\n", __DATE__,CDROM_DEVICE);
  #ifdef NETWORK
    printf ("Includes MiniLZO %s by Markus F.X.J. Oberhumer\n", LZO_VERSION_STRING);
  #endif
}

void print_discid(void) {
  printf ("%s\n", cd_discid());
}
