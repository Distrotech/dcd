/* this base64 variant was "borrowed" from MUTT, which in turn
   borrowed from fetchmail.

   Dave stripped it down and added the cdindex-specific charmap.

   You are in a twisty maze of copyright notices, all different.

   (This is GPL'd, by the way.)

*/


/* 
 * This code is heavily modified from fetchmail (also GPL'd, of
 * course) by Brendan Cully <brendan@kublai.com>.
 * 
 * Original copyright notice:
 * 
 * The code in the fetchmail distribution is Copyright 1997 by Eric
 * S. Raymond.  Portions are also copyrighted by Carl Harris, 1993
 * and 1995.  Copyright retained for the purpose of protecting free
 * redistribution of source. 
 * 
 */


#include <sys/types.h>
#include <unistd.h>

/* raw bytes to null-terminated base 64 string */
void bin_to_base64 (unsigned char *out, const unsigned char *in, size_t len,
		     size_t olen) {
  const char B64Chars[65] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789._-" ;

  while (len >= 3 && olen > 10) {
    *out++ = B64Chars[in[0] >> 2];
    *out++ = B64Chars[((in[0] << 4) & 0x30) | (in[1] >> 4)];
    *out++ = B64Chars[((in[1] << 2) & 0x3c) | (in[2] >> 6)];
    *out++ = B64Chars[in[2] & 0x3f];
    olen  -= 4;
    len   -= 3;
    in    += 3;
  }

  /* clean up remainder */
  if (len > 0 && olen > 4) {
    unsigned char fragment;
    *out++ = B64Chars[in[0] >> 2];
    fragment = (in[0] << 4) & 0x30;
    if (len > 1)
      fragment |= in[1] >> 4;
    *out++ = B64Chars[fragment];
    *out++ = (len < 2) ? '=' : B64Chars[(in[1] << 2) & 0x3c];
    *out++ = '-';
  }
  *out = '\0';
}

