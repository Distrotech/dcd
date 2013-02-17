/* $Revision: 1.6 $ */
/* I'll just lump all the MusicBrainz stuff here, make a nice interface,
 * and that will be that. No more needless mucking about with externs.
 */

#include "mbo.h"
#include "libcdplay.h"
#include "minilzo.h"
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

static musicbrainz_t mbo;
static int mbo_ok;

#define MBO_MAGIC "DCD1"
#define MBO_BAILOUT { fclose(f); unlink(mbo_filename()); return FALSE; }

int mbo_init (char *device) {
  #ifdef DEBUG
    fprintf (stderr, "mbo_init()\n");
  #endif
  
  mbo = mb_New();
  mb_SetDevice (mbo, device);
  mb_UseUTF8 (mbo, 0);
  #if DEBUG > 1
    mb_SetDebug (mbo, 1);
  #endif

  if (mbo_load_local()) { mbo_ok = TRUE; return TRUE; }
  if (mbo_load_net()) { mbo_ok = TRUE; return TRUE; }
  mbo_ok = FALSE;
  return FALSE;
}

/* convenience hack. 0 is disc name, others are track names. */
/* guess what, this too is XXX UNSAFE */
char *mbo_trackname (int i) {
  char *res = (char *)malloc(255);
  if (mbo_ok == FALSE) return "";
  memset(res, '\0', 255);
  if(i==0) mb_GetResultData(mbo, MBE_AlbumGetAlbumName, res, 255);
  else mb_GetResultData1(mbo, MBE_AlbumGetTrackName, res, 255, i);
  #ifdef DEBUG
    fprintf (stderr, "cd_get_trackname: track %i is %s\n", i, res);
  #endif
  return res;
}

/* 0 if local file found and loaded, -1 if something botched. */
/* new weird hack! -- if a disc wasn't found, we "cache" that, by storing */
/* an empty file. Hey, inodes are cheap. There's a random chance per query */
/* that we'll requery the master site just for kicks. */
static int mbo_load_local (void) {
  char *in, *rdf;
  char *fn = mbo_filename();
  struct stat statbuf;
  time_t whence;
  int staterr;
  lzo_uint isz, sz, osz;
  FILE *f;

  #ifdef DEBUG
    fprintf (stderr, "load_local: ");
  #endif
  staterr = stat(fn, &statbuf);
  if (staterr == -1) return FALSE;

  /* there's apparently a file there. */
  f = fopen (fn, "r");
  if (f == NULL) return FALSE; /* don't attempt to diagnose errors */
  #ifdef DEBUG
    fprintf (stderr, "found file, ");
  #endif

  /* if this is an old cdindex.org file, chuck it out thoughtfully */
  /* this is FAR from a precise and useful match, but it'll suffice */
  // if (strstr (rdf, "NumMatches: 1")) MBO_BAILOUT;

  /* is this really a DCD file? if not, chuck it away too */
  in = (char *)malloc(4);
  fread (in, sizeof(char), 4, f);
  /* the aforementioned ugly random hack. There's a 25% chance we requery. */
  srand((unsigned int)time(0));
  if(feof(f)) if((int)(4.0*rand()/(RAND_MAX+1.0))) MBO_BAILOUT;
  /* if (feof(f)) if (rand() % 4) { MBO_BAILOUT; } */
  /* the above piece of relative magick from Roman Mashirov */
  if (memcmp(MBO_MAGIC, in, 4) != 0) MBO_BAILOUT;
  free (in);
  #ifdef DEBUG
    fprintf (stderr, "magic, ");
  #endif

  fread (&whence, sizeof(time_t), 1, f);
  #ifdef DEBUG
    fprintf (stderr, "dt %i, ", (int) whence);
  #endif
  /* maybe do a staleness check - requery for data older than X time? */

  fread (&sz, sizeof(lzo_uint), 1, f);

  rdf = (char *)malloc(sz);
  memset(rdf, '\0', sz);

  #ifdef DEBUG
    fprintf (stderr, "sz %i, ", sz);
  #endif
  isz = statbuf.st_size - (sizeof(time_t) + sizeof(lzo_uint)
        + (4 * sizeof(char)));
  in = (char *)malloc(isz * sizeof(lzo_byte));
  fread (in, sizeof(lzo_byte), isz, f);
  #ifdef DEBUG
    fprintf (stderr, "data %i, ", isz);
  #endif

  if (lzo_init() != LZO_E_OK) MBO_BAILOUT;
  if (lzo1x_decompress(in,isz,rdf,&osz,NULL) != LZO_E_OK) MBO_BAILOUT;
  if (sz != osz) MBO_BAILOUT;
  #ifdef DEBUG
    fprintf (stderr, "LZO\n");
  #endif
  #if DEBUG > 1
    fprintf (stderr, "Loaded RDF:\n%s\n", rdf);
  #endif
  mb_SetResultRDF (mbo, rdf);
  mb_Select1(mbo, MBS_SelectArtist, 1);
  mb_Select1(mbo, MBS_SelectAlbum, 1);
  fflush (f);
  fclose (f);
  #ifdef DEBUG
    fprintf (stderr, "done.\n");
  #endif
  return TRUE;
}

static int mbo_load_net(void) {
  int i;
  /* the mb docco is unclear as to whether the mb object is "safe" to
   * use without first doing the query/select juggle below, but, well,
   * it SEEMS to work...
   */
  #ifdef DEBUG
    fprintf (stderr, "load_net: Querying the CyberPapacy\n");
  #endif
  mb_SetServer (mbo, MBO_SERVER, 80);
  mb_Query(mbo, MBQ_GetCDInfo);
  i = mb_GetResultInt(mbo, MBE_GetNumAlbums);
  if (i == 1) mbo_save_local();
     else {
       fprintf (stderr, "Warning: CD not found on %s\n", MBO_SERVER);
       mbo_save_null();
     }
  /* this looks weird. even if there are NO matches, we do this drill-down */
  /* thing, to select "empty" results. Otherwise, things go boom.          */

  mb_Select1(mbo, MBS_SelectArtist, 1);
  mb_Select1(mbo, MBS_SelectAlbum, 1);
  return i;
}

/* function to save an empty file. Yes, there is a reason for this. */
static int mbo_save_null(void) {
  FILE *f;
  f = fopen(mbo_filename(), "w+b");
  if (f == NULL) return FALSE;
  fclose(f);
}

static int mbo_save_local(void) {
  char wrkmem[LZO1X_1_MEM_COMPRESS];
  lzo_uint sz = mb_GetResultRDFLen(mbo);
  lzo_uint osz = sz * 1.2; /* should be sufficient for worst case? */
  int i;
  lzo_bytep rdf = (lzo_bytep)malloc(sz +1);
  lzo_bytep out = (lzo_bytep)malloc(osz +1);
  char hdr[4] = MBO_MAGIC;
  time_t now = time(NULL);
  FILE *f;

  memset(rdf, '\0', sz);
  memset(out, '\0', osz);
  mb_GetResultRDF (mbo, (char *)rdf, sz);
  if(lzo_init() != LZO_E_OK) return FALSE;

  i = lzo1x_1_compress(rdf,sz,out,&osz,wrkmem);
  if (i != LZO_E_OK) return FALSE;
  
  #ifdef DEBUG
    fprintf (stderr, "save_local: LZO, %i to %i bytes\n%s\n",
                      sz, osz, mbo_filename());
  #endif

  f = fopen (mbo_filename(), "wb");
  if (f == NULL) return FALSE;
  /* write stuff to file. format is: magic, timestamp, usz, data */
  if (fwrite ( &hdr, sizeof(char), 4, f) != 4) MBO_BAILOUT;
  if (fwrite ( &now, sizeof(time_t), 1, f) != 1) MBO_BAILOUT;
  if (fwrite ( &sz, sizeof(lzo_uint), 1, f) != 1) MBO_BAILOUT;
  if (fwrite ( out, sizeof(char), osz, f) != osz) MBO_BAILOUT;
  fclose(f);
  return TRUE;
}

static char *mbo_filename (void) {
  int i = strlen(getenv("HOME")) + strlen(CDI_HOME) + strlen(cd_discid()) + 1;
  char *fn = (char *)malloc(i);
  memset(fn, '\0', i);
  sprintf (fn, "%s/%s/%s", getenv("HOME"), CDI_HOME, cd_discid());
  return fn;
}
