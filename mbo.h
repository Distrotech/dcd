#ifndef __MBO_H
#define __MBO_H

#ifdef NETWORK
#include <musicbrainz/mb_c.h>
#endif

int mbo_init(char *device);
char *mbo_trackname (int i);

#define CDI_HOME ".cdindex" 	/* for historical reasons */

#define MBO_SERVER "www.musicbrainz.org"

#endif
