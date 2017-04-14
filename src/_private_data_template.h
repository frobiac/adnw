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
 * Twik uses 36 character uuids with capital letters, but any string should do here.
 *
 * Possible sources:
 *     cat /proc/sys/kernel/random/uuid
 *     tr -dc 'a-zA-Z0-9' </dev/urandom | tr -d 'lIO' | head -c 36 | sed 's/./-/9; s/./-/14; s/./-/19; s/./-/24'
 *
 * Put the private key enclosed in "" below and store a copy somewhere safe!
 */
#define PH_PRIVATE_KEY "secret"

/*
 * Test vector to verify correct entry of password upon unlock.
 * If defined, the passhash creation is only activated if the expected result matches after entry of the master password.
 *
 * Format: "tag", length,  mode, "expected hash"
 *
 * Example for a private key="secret" defined above and
 * tag "a" with length 4 and mode 3 (numeric) should result in 4616
 * when password "key" is entered.
 */

// #define PH_TEST_DATA "a", 4, 3, "4616"
