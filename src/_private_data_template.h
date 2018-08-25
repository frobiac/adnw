/*
 * This is a template file for private data that must not be checked into git.
 *
 * It is copied to _private_data.h during the first run of make.
 * This copy is ignored in git and can be freely edited.
 *
 * You should make sure that neither the edited header nor the compiled artifacts are distributed
 * as they contain your secret key. Without knowledge of your master password which is not stored
 * anywhere, however, that should still be only of limited use.
 *
 */

/* This is the private key of the passhash implementation.
 *
 * It must be 20 characters long:
 * Possible sources:
 *     sha1sum directly/openssl
 *     echo -n 'MySuperRandomPassword' | sha1sum | cut -f 1 -d \  | sed 's/\(..\)/\\x\1/g'
 *     tr -dc 'a-f0-9' </dev/urandom | head -c 40 | sed 's/\(..\)/\\x\1/g'
 *
 * Put the private key enclosed in "" below and store a copy somewhere safe!
 */

// Must be 20 characters!
#define HMAC_TAG_BASE_LEN 20
const char * HMAC_TAG_BASE = "\x00\x32\xa7\x9c\xe5\x96\x2e\x20\x25\x0b\x1a\xf3\x85\x97\x7f\x0a\x8e\x72\x57\xff";

