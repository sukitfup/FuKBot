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

/*
	Set your version text here
*/
#define FUK_VERSION "FuKeRy | v3.0"

#pragma region "CFG_STUFF Defines"
	#define CFGSTUFF_LIST       "list"
	#define CFGSTUFF_REM        "rem"
	#define CFGSTUFF_ADD        "add"
	#define CFGSTUFF_FROM       "from"
	#define CFGSTUFF_TO         "to"

	#define CFGSTUFF_MASTER     "master"
	#define CFGSTUFF_SAFE       "safe"
	#define CFGSTUFF_SHIT       "shit"
	#define CFGSTUFF_DES        "des"

	#define CFGSTUFF_USERNAME	"username"
	#define CFGSTUFF_HOME		"home"
	#define CFGSTUFF_SERVER		"server"
	#define CFGSTUFF_PORT		"port"
	#define CFGSTUFF_PASSWORD	"password"
	#define CFGSTUFF_TRIGGER	"trigger"
	#define CFGSTUFF_THREADS	"threads"
	#define CFGSTUFF_DELAY		"delay"
	#define CFGSTUFF_SCATTER	"scatter"
	#define CFGSTUFF_BACKUPCHAN	"backupchan"
	#define CFGSTUFF_TOPIC		"topic"
	#define CFGSTUFF_BANWAIT	"banwait"
	#define CFGSTUFF_CONWAIT	"conwait"
	#define CFGSTUFF_NUMBOTS	"numbots"
	#define CFGSTUFF_BIND		"bind"

	#define CFGSTUFF_FORMATTING_X_EQUILS_X      "%s = %s\r\n"
	#define CFGSTUFF_FORMATTING_TO_FROM_SECTION "Success! %s %s %s %s.\r\n"
	#define CFGSTUFF_FORMATTING_ADD_SECTION     "%s is already a %s.\r\n"
	#define CFGSTUFF_FORMATTING_OPTIONS         "Options: %s, %s, %s | %s, %s, %s, %s.\r\n"
#pragma endregion

#pragma region "Base messages (this should get larger lol)"
	#define BASE_DESIGNATE      "designate"
	#define BASE_BAN            "ban"
	#define BASE_TOPIC          "topic"
	#define BASE_TRIGGER        "trigger"
	#define BASE_HOME           "home"
	#define BASE_BACKUP         "backup"
	#define BASE_SERVER         "server"
	#define BASE_THREADS        "threads"
	#define BASE_PORT           "port"
	#define BASE_DELAY          "delay"
	#define BASE_DELAY_2        "delay2"
	#define BASE_SCATTER        "scatter"
	#define BASE_BANWAIT        "banwait"
	#define BASE_BANWAIT_TEXT   "ban wait"
	#define BASE_CONWAIT        "conwait"
	#define BASE_SAY            "say"
	#define BASE_VER            "ver"
	#define BASE_QUIT           "quit"
	#define BASE_RECON          "recon"
	#define BASE_PLACE          "place"
	#define BASE_PING           "ping"
	#define BASE_DES            CFGSTUFF_DES    
	#define BASE_OP             "op"
	#define BASE_LOCK           "lock"
	#define BASE_UNLOCK         "unlock"
	#define BASE_CLEAN          "clean"
	#define BASE_TAG            "tag"
	#define BASE_CONTIME        "contime"
	#define BASE_MEM            "mem"

	#define BASE_COLON              ": "
	#define BASE_ISNOW              " is now: "
	#define BASE_NO_HEIR_TEXT       "No heir to your throne."
	#define BASE_CHANLOCKED         "(Channel Locked)"
	#define BASE_TAGBAN_DISABLED    "Tagban disabled."
#pragma endregion

#pragma region "Server Commands"
	#define SERVER_WHISPER      "w"
	#define SERVER_BAN          "ban"
	#define SERVER_UNBAN        "unban"
	#define SERVER_KICK         "kick"
	#define SERVER_JOIN         "join"
	#define SERVER_REJOIN       "rejoin"    /* Note: Non existant command, this will not work on a properly emulated server */
	#define SERVER_DESIGNATE    BASE_DESIGNATE
	#define SERVER_TOPIC        BASE_TOPIC  /* Note: Non existant command, this will not work on a properly emulated server */
	#define SERVER_RESIGN       "resign"
	#define SERVER_UPTIME       "uptime"    /* Note: Non existant command, this will not work on a properly emulated server */
	#define SERVER_WHO          "who"

	#define SERVER_BASE_SPEAK   "%s\r\n"
	#define SERVER_COMMAND_0    "/%s\r\n"
	#define SERVER_COMMAND_1    "/%s %s\r\n"
	#define SERVER_COMMAND_2    "/%s %s %s\r\n"
#pragma endregion

#pragma region "Join,Flags,Talk formatting"
	#define ON_COMMAND_REPLY_0  "/%s %s %s%s%s\r\n"
	#define ON_COMMAND_REPLY_1  "/%s %s Actual %s (with %s): %d\r\n"
	#define ON_COMMAND_REPLY_2  SERVER_BASE_SPEAK
	#define ON_COMMAND_REPLY_3  "/%s %s %d seconds to connect.\r\n"
	#define ON_COMMAND_REPLY_4  "/%s %s Memory usage: %ld bytes\r\n"
#pragma endregion

#pragma region "Config Constants"
	/*
		should probably make an actual config.c
		and maybe pass load a data structure pointer.
	*/
	#define FUK_CFG_MAXCOUNT	1024
	#define FUK_CFG_READBINARY	"rb"
	#define FUK_CFG_WRITE		"w+"
	#define FUK_CFG				"fuk.cfg"
	#define FUK_CFG_ERROR		"Can't find %s!\n" /* if you ever deside to change the FUK_CFG file */
	#define FUK_CFG_ERROR_O		"Can't open %s!\n" /* if you ever deside to change the FUK_CFG file */

	#define FUK_CFG_PRINTFS		"%s=%s\n"
	#define FUK_CFG_PRINTFI		"%s=%d\n"

	#define FUK_CFG_EQUIL	"="
	/*
		small note: if this dosent compile for you, ill re write this entire section
	*/
	#define FUK_CFG_USERNAME	(CFGSTUFF_USERNAME FUK_CFG_EQUIL)
	#define FUK_CFG_HOME		(CFGSTUFF_HOME FUK_CFG_EQUIL)
	#define FUK_CFG_SERVER		(CFGSTUFF_SERVER FUK_CFG_EQUIL)
	#define FUK_CFG_PORT		(CFGSTUFF_PORT FUK_CFG_EQUIL)
	#define FUK_CFG_PASSWORD	(CFGSTUFF_PASSWORD FUK_CFG_EQUIL)
	#define FUK_CFG_TRIGGER		(CFGSTUFF_TRIGGER FUK_CFG_EQUIL)
	#define FUK_CFG_THREADS		(CFGSTUFF_THREADS FUK_CFG_EQUIL)
	#define FUK_CFG_DELAY		(CFGSTUFF_DELAY FUK_CFG_EQUIL)
	#define FUK_CFG_SCATTER		(CFGSTUFF_SCATTER FUK_CFG_EQUIL)
	#define FUK_CFG_MASTER		(CFGSTUFF_MASTER FUK_CFG_EQUIL)
	#define FUK_CFG_SAFE		(CFGSTUFF_SAFE FUK_CFG_EQUIL)
	#define FUK_CFG_SHIT		(CFGSTUFF_SHIT FUK_CFG_EQUIL)
	#define FUK_CFG_DES			(CFGSTUFF_DES FUK_CFG_EQUIL)
	#define FUK_CFG_BACKUPCHAN	(CFGSTUFF_BACKUPCHAN FUK_CFG_EQUIL)
	#define FUK_CFG_TOPIC		(CFGSTUFF_TOPIC FUK_CFG_EQUIL)
	#define FUK_CFG_BANWAIT		(CFGSTUFF_BANWAIT FUK_CFG_EQUIL)
	#define FUK_CFG_CONWAIT		(CFGSTUFF_CONWAIT FUK_CFG_EQUIL)
	#define FUK_CFG_NUMBOTS		(CFGSTUFF_NUMBOTS FUK_CFG_EQUIL)

#pragma endregion

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
