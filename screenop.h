/* screenop.h - screen/display bits for dcd
 * (C) 1999 David E. Smith (dave@technopagan.org)
 * Released under GNU GPL, see `COPYING' for details.
 */


#ifndef __DCD_SCREEN
#define __DCD_SCREEN

void disk_directory(void); 	/* writes a disk dir */
void print_status(void);  	/* guess what, writes the status */
void print_help (void);		/* writes keen help stuff */
void print_discid (void);	/* just the CDI discid */
void print_sub_url (void);	/* complete submission URL */
void dcd_version(void);		/* it's more than a version... */
void short_help(void);		/* abbreviated help page */
#endif
