#ifndef uint8_t
    #include <stdint.h>
#endif

#define PH_MIN_LEN       4
#define PH_MAX_LEN      26

#define PH_TYPE_NUM      3
#define PH_TYPE_ALNUM    2
#define PH_TYPE_ALNUMSYM 1

///  password array must be one longer then requested length!
uint8_t passHash(char * password, uint8_t len, uint8_t type, char * secret, char * key, char * tag);

