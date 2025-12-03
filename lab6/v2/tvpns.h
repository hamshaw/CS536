int NUMSESSIONS = 8;
struct tunneltab {
    unsigned long destaddr[2];
    unsigned short destpt[2];
    unsigned long sourceaddr;
    unsigned short sourcept;
    unsigned short tunnelpt;
    struct sockaddr_in clientaddr;
    struct sockaddr_in sendingaddrs[2];
    int count;
};


