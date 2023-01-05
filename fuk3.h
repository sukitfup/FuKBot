#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <limits.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>

#define MAX_THREADS 16
#define BUFFSIZE 1024
#define INVALID_SOCKET -1

typedef struct {
	char id[21];
}masterList;
typedef struct {
	char id[21];
}safeList;
typedef struct {
	char id[21];
}shitList;
typedef struct {
	char id[21];
}desList;

shitList* shit;
desList* des;
safeList* safe;
masterList* master;

int main_pid, masterSz, safeSz, shitSz, desSz, threadSz;
int port, threads, delay, scatter, numBots, banWait, conWait, randGreet, startTime;

char username[21];
char password[21];
char server[21];
char bindaddr[21];
char channel[21];
char trigger[2];
char backup[21];
char topic[41];
char tag[11];

struct rusage r_usage;

struct data{
	time_t lastTime;
	unsigned int flood;
	int conTime, tban, op, des, greet, botNum;
	int port, place, threads, delay2, hasop, lockdown, chanham;
	volatile int connected;
	char password[21];
	char username[21];
	char server[21];
	char bindaddr[21];
	char channel[21];
	char trigger[2];
	char logonPacket[100];
	char currChan[21];
};

struct data *pb;

void set_nonblock(int fd);
void set_block(int fd);
void cfgStuff(int s, struct data *pb, char *com, char *text);
void OnJoin(int s, struct data *pb, char *szSpeaker);
void OnUserFlags(int s, struct data *pb, char *szSpeaker, u_long uFlags);
void OnTalk(int s, struct data *pb, char *szSpeaker, char *szEventText);
void OnChannel(int s, struct data *pb, char *szEventText);
void OnInfo(int s, struct data *pb, char *szEventText);
void OnError(int s, struct data *pb, char *szEventText);
void OnPing(int s, struct data *pb, char *szEventText);
void Dispatch(int s, struct data *pb, char *szEventText);
int Send(int s, char *lpszFmt, ...);
void message_loop(int s, struct data *pb);
void msleep(unsigned long milisec);
char *replace_str(char *str, char *orig, int rep);
int save_cfg(struct data* pb);
int read_config();
int Connect(int s, struct timeval tv, struct data* pb);
void *thread_conn(void *arg);
int thread_conf(struct data *pb);
void create_threads(struct data *pb);