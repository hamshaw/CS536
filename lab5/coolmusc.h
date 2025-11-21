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
};
int pipefd[2];
//static snd_pcm_t *mulawdev;
//static snd_pcm_uframes_t mulawfrms;

#ifndef LOAD_PARAMS_H
#define LOAD_PARAMS_H
    int load_params(const char *filename, struct client_params *params);
#endif

#define mulawwrite(x) snd_pcm_writei(mulawdev, x, mulawfrms)

#ifndef PLAY_H
#define PLAY_H
    int play(int fd, int slptime);
#endif

#ifndef PLAYER_THREAD_H
#define PLAYER_THREAD_H
    void *player_thread(void *arg);
#endif

#ifndef MULAWCLOSE_H
#define MULAWCLOSE_HI
    void mulawclose(void);
#endif

#ifndef MULAWOPEN_H
#define MULAWOPEN_H
    void mulawopen(size_t *bufsiz);
#endif

