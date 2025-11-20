char SECRET[] = "secret";
char A = 'A';
char E = 'E';
unsigned short PORTNUM = 55555;
struct client_params{
    unsigned short BLOCKSIZE;
    unsigned long BUFFERSIZE;
    unsigned long TARGETBUF;
    float INVLAMBDA;
    float EPSILON;
    float BETA;
}
#ifndef LOAD_PARAMS_H
#define LOAD_PARAMS_H
    int load_params(const char *filename, struct client_params *params)
#endif
