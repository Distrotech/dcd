/* $Revision: 1.2 $ */
/*
 * This is DCD, Dave's CD Player.
 * (C)1998-2001 David E. Smith <dave@technopagan.org>
 * Released under GNU GPL. (See 'COPYING' for details.)
 *
 * This is dcd.c -- where all the fun begins.
 */

#include "libcdplay.h"
#include "screenop.h"
#include "dcd.h"
#include "infinite.h"
#include "version.h"
#include "process.h"
#include "cd-misc.h"
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>

#define ZERO 0
#define MAX_TRACKS 103

static int tracklist[MAX_TRACKS];
/* the above should, OC, be dynamic to save memory and avoid wacky
   buffer overflows. I'll fix it later. Maybe a linked list... */
static int trk_ptr, trksz, randomflag, loopflag, quitflag;
/* trk_ptr is an index in the track array; trksz is the size of that array.
   the flags, hopefully, are obvious. */

int sigterm_handler (int sig) {
  cd_stop();
  exit(EXIT_SUCCESS);
}

int dcd_sighandler (int sig) {
  signal (sig, (void *)dcd_sighandler);
  return 0;
}

#define DCD_TOAST() { dcd_kill(SIGTERM); dcd_delpid(); } 

/* this is just ugly enough that I don't want to make it a macro */
static void dcd_check(void) {
  if(!cd_present()) {
    fprintf (stderr, "No disc in drive %s, aborting.\n", CDROM_DEVICE);
    exit(EXIT_FAILURE);
  }
}

static u_char dcd_next_track (u_char old) {
  int ret = 0;
  if (randomflag == TRUE) {
    ret = rand() % trksz;
    while (tracklist[ret] == old) ret = rand() % trksz;
    return tracklist[ret];
  } /* rand */
  trk_ptr++;
  if (loopflag == TRUE) {
    if (tracklist[trk_ptr] == ZERO) trk_ptr = 0;
    return tracklist[trk_ptr];
  } /* loop */
  else if (quitflag == TRUE) {
    if (tracklist[trk_ptr] == ZERO) return 0;
    return tracklist[trk_ptr];
  } /* quit */
  #ifdef DEBUG
    fprintf (stderr, "dcd_next_track: this shouldn't be reached\n");
  #endif
  return 0;
}

/* opt_handler and do_opt should possibly be combined, to make things
 * like x:: possible (CD Index, then pick a mode). It gets ugly though.
 */

void opt_handler (char c) {
  switch (c) {
    case 'h': print_help(); exit(EXIT_SUCCESS); break;
    case 'd': disk_directory(); exit(EXIT_SUCCESS); break;
    case 'i': print_status(); exit(EXIT_SUCCESS); break;
    case 'k': DCD_TOAST(); cd_stop();  exit(EXIT_SUCCESS); break;
    case 's': DCD_TOAST(); cd_stop();  exit(EXIT_SUCCESS); break;
    case 'e': DCD_TOAST(); cd_eject(); exit(EXIT_SUCCESS); break;
    case 'u': /* for "unpause", to make some random debian user happy */
    case 'p': cd_pause(); exit (EXIT_SUCCESS); break;
    case 'v': dcd_version(); exit(EXIT_SUCCESS); break;
    case 'r': randomflag = TRUE; break;
    case 'l': loopflag = TRUE; break;
    case 'q': 
    case 'o': quitflag = TRUE; break; /* q and o are the same */
    case 'b': DCD_TOAST(); cd_play_prev_track(); exit(EXIT_SUCCESS); break;
    case 'f': DCD_TOAST(); cd_play_next_track(); exit(EXIT_SUCCESS); break;
    case 'x': print_discid(); exit(EXIT_SUCCESS); break;
    case 'a': printf ("%i\n", cd_first_track()); exit(EXIT_SUCCESS); break;
    case 'z': printf ("%i\n", cd_last_track()); exit(EXIT_SUCCESS); break;
    default : fprintf (stderr, "Unknown option: %c\n", c);
              short_help(); exit (EXIT_FAILURE); break;
  }
}

int do_opt (int argc, char **argv) {
  int optc, i;
  int tz = cd_last_track();
  extern char *optarg;
  extern int optind;
  /* char *shortopts = "hedrilnoqvsxubfpc"; */
  char *shortopts = "bcdefhilnopqrsuvx";
  while ((optc = getopt(argc, argv, shortopts)) != EOF) opt_handler (optc);

  /* Now load a tracklist, and do a really ugly backward-compat hack.
   * This has the unintended, but maybe-useful, side effect of allowing 
   * tracks and options to be listed in any order and not requiring the
   * GNU format for options (loop instead of -loop). Ponder.
   */

  for ( ; optind < argc ; optind++)
    if (isdigit(argv[optind][0]))
       tracklist[trk_ptr++] = atoi(argv[optind]);
      else opt_handler(argv[optind][0]);
  trksz = trk_ptr;
  trk_ptr = 0;

  /* sanity check on params */

  #ifdef DEBUG
    fprintf (stderr, "flags: %s%s%s\n",
             (quitflag == TRUE) ? "quit " : "",
             (loopflag == TRUE) ? "loop " : "",
             (randomflag == TRUE) ? "random" : "");
    if ((quitflag==FALSE) && (loopflag==FALSE) && (randomflag==FALSE))
        fprintf (stderr, "none");
  #endif
  if ((quitflag == TRUE) & ((loopflag == TRUE) | (randomflag == TRUE))) {
    fprintf (stderr, "%s\n", VERSION);
    fprintf (stderr,
             "Incompatible options selected. You may not use quit and %s together.\n",
             (loopflag == TRUE) ? "loop" : "random");
    exit (EXIT_FAILURE);
  } /* if */

  #ifdef DEBUG
    fprintf (stderr, "Running sanity check on tracklist.\n");
  #endif
  for (i = 0; i < trksz; i++) {
    if (cd_data_track(tracklist[i])) {
      printf ("Track %i is a data track and can't be played. Aborting.\n", tracklist[i]);
      exit (EXIT_FAILURE);
    } /* if */
    if (tracklist[i] > tz) {
      printf ("Track %i doesn't seem to be on this disk. Aborting.\n", tracklist[i]);
      exit (EXIT_FAILURE);
    } /* if */
  } /* for */
  return trksz;
}

void init_global(void) {
  int i=0;
  if (cd_init_player(CDROM_DEVICE) == -1) {
    fprintf (stderr, "Unable to open %s, aborting.\n", CDROM_DEVICE);
    exit (EXIT_FAILURE);
  }

  randomflag = loopflag = quitflag = FALSE;
  trk_ptr = 0;
  for (i=0; i<MAX_TRACKS; i++) tracklist[i] = ZERO;
  #ifdef DEBUG
    srand(42);
  #else
    srand((unsigned int)time(NULL));
  #endif
}

int main (int argc, char **argv) {
  pid_t pid1, pid2, waitstatus;
  int i, playing;

  init_global();
  if(!cd_present()) {
    fprintf (stderr, "No disc in drive %s, aborting.\n", CDROM_DEVICE);
    exit(EXIT_FAILURE);
  }
  i = do_opt (argc, argv);

  /* "free" sanity check, rarely enabled. */
  #if DEBUG > 1
    if (i != trksz) {
      printf ("Uhoh, i != trksz, call the guru.\n");
      exit (EXIT_FAILURE);
    }
  #endif

  /* fork to have a process for actual work */
  pid1 = fork();
  switch (pid1) {
    case -1:
      perror ("unable to fork");
      exit (EXIT_FAILURE);
      break;
    case 0: break;	/* child  */
    default:
      wait (&waitstatus);
      #ifdef DEBUG
        fprintf (stderr, "parent exiting with status code %i\n", waitstatus);
      #endif
      _exit (EXIT_SUCCESS);
      break;
  } /* switch */

  /* do it again to detach from console */

  if (FALSE == quitflag) {
    pid2 = fork();
    switch (pid2) {
      case -1:
        perror ("unable to fork");
        exit (EXIT_FAILURE);
        break;
      case 0: break;
      default: exit (EXIT_SUCCESS);
    } /* switch */
  } /* if */

  /* unless quitflag set, we've detached now. */

  dcd_kill(SIGTERM);
  signal (SIGTERM, (void *)sigterm_handler);
  signal (SIGUSR1, (void *)dcd_sighandler);
  signal (SIGALRM, (void *)dcd_sighandler);
  dcd_setpid();

  /* now step through various permutations. First up: no tracklist. */

  if (tracklist[0] == ZERO) {
    if (randomflag == TRUE) shuffle_disc();   /* these will */
    else if (loopflag == TRUE) loop_disc();   /* never exit */
    else {
      cd_play_disc(cd_first_track());
      exit (EXIT_SUCCESS);
    }
  }

  /* handle the `normal' behaviour of starting from a track */

  if ((loopflag == FALSE) & (randomflag == FALSE) &
      (quitflag == FALSE) & (argc == 2)) {
     cd_play_disc(tracklist[0]);
     exit (EXIT_SUCCESS);
  } /* if */

  /* we must have a track list programmed in, let's rock. */

  #ifdef DEBUG
    fprintf (stderr, "Tracklist: ");
    for (i=0; i<MAX_TRACKS; i++)
      if (tracklist[i] != ZERO)
        fprintf (stderr, "%i  ", tracklist[i]);
    fprintf (stderr, "\n");
  #endif
  trk_ptr = -1; playing = 0; /* trust me on this. */
  while (TRUE) {
    cd_stop();
    playing = dcd_next_track(playing);
    if (playing == 0) break; /* to cover quitflag behaviour */
    cd_play_track(playing);
    #ifdef DEBUG
      fprintf (stderr, "main loop now playing track %i\n", playing);
    #endif
    alarm(cd_track_length(playing) + EXTRA_SLEEP_TIME);
    pause();
  } /* while */

  #ifdef DEBUG
    fprintf(stderr,"Exiting at the end of main()\n");
  #endif
  exit (EXIT_SUCCESS); /* This should only be reached if quitflag is set. */
  return 0; /* not reached, but makes -pedantic happy */
} /* main */
