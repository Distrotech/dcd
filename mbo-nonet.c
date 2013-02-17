/* $Revision: 1.3 $ */
/* I'll just lump all the MusicBrainz stuff here, make a nice interface,
 * and that will be that. No more needless mucking about with externs.
 */

#include "mbo.h"
#include "libcdplay.h"
#include "dcd.h"
#include <sys/file.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

static int mbo_load_local(void);
static int mbo_save_local(void);
static int mbo_load_net(void);
static int mbo_save_null(void);
static char *mbo_filename(void);

static int mbo_ok;

#define MBO_MAGIC "DCD1"
#define MBO_BAILOUT { fclose(f); unlink(mbo_filename()); return FALSE; }

int mbo_init (char *device) {
  return FALSE;
}

/* convenience hack. 0 is disc name, others are track names. */
char *mbo_trackname (int i) {
  if(i == 0) return " " ;
  if(cd_data_track(i)) return "(Data)" ;
  return " " ; /* we have to return SOMETHING ... */
}

static int mbo_load_local (void) {
  return 0;
}

static int mbo_load_net(void) {
  return 1;
}

/* function to save an empty file. Yes, there is a reason for this. */
static int mbo_save_null(void) {
  FILE *f;
  f = fopen(mbo_filename(), "w+b");
  if (f == NULL) return FALSE;
  fclose(f);
}

static int mbo_save_local(void) {
  return TRUE;
}

static char *mbo_filename (void) {
  return NULL;
}
