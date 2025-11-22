#ifndef CONSTANTS_H
#define CONSTANTS_H
#define NUMSESSIONS 4
#define SECRET "secret"
#define FILENAME_PATTERN "[a-zA-Z\\.]{1, 10}"
#endif

struct client {
    int sessionindex;
    unsigned long IPaddr;//should be in network order (converted to fit into addr.sin_addr.saddr)
    unsigned short pn;
    unsigned short blocksize;
};
