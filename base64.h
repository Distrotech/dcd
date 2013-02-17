#ifndef BASE64_H
#define BASE64_H

void bin_to_base64 (unsigned char *out, const unsigned char *in,
                    size_t len, size_t olen);

#endif
