/*
	Windows only section
*/
#if defined( _WIN32 )

	#define WINDOWS_CPP_BUILD

	#define BASE_WIN	"Win"
	#if defined( _WIN64 )
		#define BASE_WINTEXT	BASE_WIN "64"
		#define WINDOWS_CPP_64BIT
	#elif defined( _WIN32 )
		#define BASE_WINTEXT	BASE_WIN "32"
		#define WINDOWS_CPP_32BIT
	#endif

	#if defined( _DEBUG )
		#define WIN_BUILD	" (" BASE_WINTEXT " debug)"
	#else
		#define WIN_BUILD	" (" BASE_WINTEXT ")"
	#endif

#endif

#if defined(WINDOWS_CPP_BUILD)
	#define _CRT_SECURE_NO_WARNINGS
	#pragma comment(lib, "ws2_32.lib") 
	#include <winsock2.h>
	#include <Windows.h>
	#include <ws2tcpip.h>
	#include <WinInet.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <stdarg.h>
	#include <chrono>
	#include <thread>
	#include <vector>
	#include <stdint.h>
	#include <errno.h>
	#include <psapi.h>

	#define strncasecmp _strnicmp 
	#define snprintf _snprintf
	#define vsnprintf _vsnprintf
	#define strcasecmp _stricmp
	#define strdup _strdup
	#define ioctl ioctlsocket
	#define close closesocket
	#define popen _popen
	#define pclose _pclose
	#define usleep sleepextra
	#define msleep sleepextra
	#define getpid GetCurrentProcessId

	inline void sleepextra(int v) {
	    std::this_thread::sleep_for(std::chrono::milliseconds(v));
	}

	struct rusage {
		struct timeval ru_utime; /* user CPU time used */
		struct timeval ru_stime; /* system CPU time used */
		long   ru_maxrss;        /* maximum resident set size */
		long   ru_ixrss;         /* integral shared memory size */
		long   ru_idrss;         /* integral unshared data size */
		long   ru_isrss;         /* integral unshared stack size */
		long   ru_minflt;        /* page reclaims (soft page faults) */
		long   ru_majflt;        /* page faults (hard page faults) */
		long   ru_nswap;         /* swaps */
		long   ru_inblock;       /* block input operations */
		long   ru_oublock;       /* block output operations */
		long   ru_msgsnd;        /* IPC messages sent */
		long   ru_msgrcv;        /* IPC messages received */
		long   ru_nsignals;      /* signals received */
		long   ru_nvcsw;         /* voluntary context switches */
		long   ru_nivcsw;        /* involuntary context switches */
	};

	enum __rusage_who
	{
		/* The calling process.  */
		RUSAGE_SELF = 0,
		#define RUSAGE_SELF RUSAGE_SELF
		/* All of its terminated child processes.  */
		RUSAGE_CHILDREN = -1
		#define RUSAGE_CHILDREN RUSAGE_CHILDREN
		#ifdef __USE_GNU
			,
			/* The calling thread.  */
			RUSAGE_THREAD = 1
			# define RUSAGE_THREAD RUSAGE_THREAD
			/* Name for the same functionality on Solaris.  */
			# define RUSAGE_LWP RUSAGE_THREAD
		#endif
	};

	int getrusage(int who, struct rusage* r_usageval);
	void* ReAllocArray(void* ptr, size_t nmemb, size_t size);
	#define reallocarray ReAllocArray
	char* mystrsep(char** stringp, const char* delim);
	#define strsep mystrsep
#else
#include "cjson/cJSON.h"
#include <stdatomic.h>
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
#include <signal.h>
#include <stdbool.h>
#endif

#define BUILD_DATE	" (Built " __DATE__ " " __TIME__ ")"
/*
	Set your version text here
*/
#define VERSION_TEXT "FuKeRy | v3.666"

#if defined(WINDOWS_CPP_BUILD)
	#define FUK_VERSION (VERSION_TEXT WIN_BUILD BUILD_DATE)
#else
	#define FUK_VERSION (VERSION_TEXT BUILD_DATE)
#endif

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

	/*
		basic text
	*/
	#define BASE_INIT6					"init 6"
	#define BASE_SETTHETOPIC			"set the topic"
	#define BASE_WASKICKED				"was kicked out of the channel"
	#define BASE_WASBANNED				"was banned by"
	#define BASE_WASUNBANNED			"was unbanned by"
	#define BASE_NOCHATPRIV				"This channel does not have chat privileges"
	#define BASE_CHATCHANNEL			"This is a chat channel"
	#define BASE_CHANNELUSERS			"users in channel"
	#define BASE_NEWHEIR				"is your new designated heir"
	#define BASE_PLACED					"You placed"
	#define BASE_CUPTIME				"Uptime"
	#define BASE_YOUWEREKICKED			"kicked you out of the channel"

	#define BASE_CHANRESTRICTED_TEXT	"That channel is restricted"
	#define BASE_INVALIDUSER_TEXT		"Invalid user"
	#define BASE_USERNOTLOGGEDON_TEXT	"That user is not logged on"

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
	#define SERVER_PLACE		"place"		/* Note: Non existant command, this will not work on a properly emulated server */
	#define SERVER_PONG			"PONG"		/* Note: Non existant command, this will not work on a properly emulated server */

	#define SERVER_BASE_SPEAK   "%s\r\n"
	#define SERVER_COMMAND_0    "/%s\r\n"
	#define SERVER_COMMAND_1    "/%s %s\r\n"
	#define SERVER_COMMAND_2    "/%s %s %s\r\n"
#pragma endregion

#pragma region "Server Channels"
	#define CHANNEL_VOID	"The Void"
#pragma endregion

#pragma region "Join,Flags,Talk formatting"
	#define ON_COMMAND_REPLY_0S  "/%s %s %s%s%s\r\n"
	#define ON_COMMAND_REPLY_0I  "/%s %s %s%s%d\r\n"
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
	#define FUK_CFG				"config.json"
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
	#define FUK_CFG_BIND		(CFGSTUFF_BIND FUK_CFG_EQUIL)

#pragma endregion

/* 
	Defines used by both or should be at some point
*/
#define MAX_THREADS			8
#define BUFFSIZE			1024
#if defined(WINDOWS_CPP_BUILD)
	#define INVALID_SOCKET		SOCKET_ERROR
#else
	#define INVALID_SOCKET		-1
#endif

#define MAX_LOGON_PACKET_LEN 128
#define MAX_CFG_LEN 32
#define MAX_LIST_LEN 32
#define MAX_TOPIC_LEN 128
#define SEND_BUFFER_SIZE 256
#define EVENT_TEXT_SIZE 256

typedef struct {
    char id[MAX_LIST_LEN];
} masterList;

typedef struct {
    char id[MAX_LIST_LEN];
} safeList;

typedef struct {
    char id[MAX_LIST_LEN];
} shitList;

typedef struct {
    char id[MAX_LIST_LEN];
} desList;

// Declare pointers for dynamically allocated lists
masterList* master;
safeList* safe;
shitList* shit;
desList* des;

int main_pid, masterSz, safeSz, shitSz, desSz, threadSz, shutDown, recon;
int port, threads, delay, scatter, numBots, banWait, conWait, randGreet, startTime;

// Declare global variables using the same length constraints
char username[MAX_CFG_LEN];
char password[MAX_CFG_LEN];
char server[MAX_CFG_LEN];
char bindaddr[MAX_CFG_LEN];
char channel[MAX_CFG_LEN];
char trigger[MAX_CFG_LEN];
char backup[MAX_CFG_LEN];
char topic[MAX_TOPIC_LEN];
char tag[MAX_CFG_LEN];

struct data {
    time_t lastTime;
    unsigned int flood;
    int conTime, tban, op, des, greet, botNum;
    int port, place, threads, delay2, hasop, lockdown, chanham;
    atomic_bool connected;
    
    char password[MAX_CFG_LEN];
    char username[MAX_CFG_LEN];
    char server[MAX_CFG_LEN];
    char bindaddr[MAX_CFG_LEN];
    char channel[MAX_CFG_LEN];
    char trigger[MAX_CFG_LEN];
    char logonPacket[MAX_LOGON_PACKET_LEN];
    char currChan[MAX_CFG_LEN];
};
extern struct data *pb;
struct rusage r_usage;  // Keep this unchanged

#if !defined(WINDOWS_CPP_BUILD)

typedef enum {
    CMD_UNKNOWN,
    CMD_CFGSTUFF_LIST,
    CMD_CFGSTUFF_ADD,
    CMD_CFGSTUFF_REM,
    CMD_BASE_TRIGGER,
    CMD_BASE_HOME,
    CMD_BASE_TOPIC,
    CMD_BASE_BACKUP,
    CMD_BASE_SERVER,
    CMD_BASE_THREADS,
    CMD_BASE_PORT,
    CMD_BASE_DELAY,
    CMD_BASE_DELAY_2,
    CMD_BASE_SCATTER,
    CMD_BASE_BANWAIT,
    CMD_BASE_CONWAIT,
    CMD_BASE_SAY,
    CMD_BASE_VER,
    CMD_BASE_QUIT,
    CMD_BASE_RECON,
    CMD_BASE_PLACE,
    CMD_BASE_PING,
    CMD_SERVER_BAN,
    CMD_SERVER_UNBAN,
    CMD_SERVER_KICK,
    CMD_SERVER_JOIN,
    CMD_SERVER_REJOIN,
    CMD_BASE_DES,
    CMD_SERVER_RESIGN,
    CMD_SERVER_UPTIME,
    CMD_BASE_OP,
    CMD_BASE_LOCK,
    CMD_BASE_UNLOCK,
    CMD_BASE_CLEAN,
    CMD_BASE_TAG,
    CMD_BASE_CONTIME,
    CMD_BASE_MEM
} CommandID;

void set_nonblock(int fd);
void allocate_lists();
void free_lists();
void clean_exit(int status);
void setup_signal_handlers();
void processList(int s, char* com, char* name, void **pArray, int *pSize, const char* type);
CommandID resolve_command(const char* com);
#endif
void cfgStuff(int s, char* com, char* text);
void OnJoin(int s, struct data* pb, char* szSpeaker);
void OnUserFlags(int s, struct data* pb, char* szSpeaker, u_long uFlags);
void OnTalk(int s, struct data* pb, char* szSpeaker, char* szEventText);
void OnChannel(int s, struct data* pb, char* szEventText);
void OnInfo(int s, struct data* pb, char* szEventText);
void OnError(int s, struct data* pb, char* szEventText);
void OnPing(int s, struct data* pb, char* szEventText);
void Dispatch(int s, struct data* pb, char* szEventText);
int Send(int s, const char* lpszFmt, ...); /* match vars */
void message_loop(int s, struct data* pb);
#if defined(WINDOWS_CPP_BUILD)
void msleep(unsigned long milisec);
#endif
char *replace_str(char *str, char *orig, int rep);
int save_cfg();
int read_config();
int Connect(int s, struct timeval tv, struct data* pb);
void* thread_conn(void* arg);
void create_threads(struct data* pb);
