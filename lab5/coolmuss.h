int NUMSESSIONS = 4;
char SECRET[] = "secret";
char A = 'A';
char E = 'E';
char FILENAME_PATTERN[] = "[a-zA-Z\\.]{1, 10}";

struct client {
    int sessionindex;
    unsigned long IPaddr;//should be in network order (converted to fit into addr.sin_addr.saddr)
    unsigned short pn;
    unsigned short blocksize;
};
