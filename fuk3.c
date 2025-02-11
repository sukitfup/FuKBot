
#include "fuk3.h"

// Global pointers for lists (unchanged)
masterList* master = NULL;
safeList*   safe   = NULL;
shitList*   shit   = NULL;
desList*    des    = NULL;

// Global pointer to the bot array
struct data *pb = NULL;

// Global sizes and configuration variables
int masterSz = 0;
int safeSz   = 0;
int shitSz   = 0;
int desSz    = 0;
int numBots  = 0;
int main_pid, threadSz, port, threads, delay, scatter, banWait, conWait, randGreet, startTime;

// Global strings for configuration
char username[MAX_CFG_LEN];
char password[MAX_CFG_LEN];
char server[MAX_CFG_LEN];
char bindaddr[MAX_CFG_LEN];
char channel[MAX_CFG_LEN];
char trigger[MAX_CFG_LEN];
char backup[MAX_CFG_LEN];
char topic[MAX_CFG_LEN];
char tag[MAX_CFG_LEN];

void clean_exit(int status) {
    free_lists();
    if (pb) {
        free(pb);
    }
    exit(status);
}

int Send(int s, const char* lpszFmt, ...) {
    char szOutStr[SEND_BUFFER_SIZE];
    va_list argptr;
    va_start(argptr, lpszFmt);
    
    // Prevent buffer overflow
    vsnprintf(szOutStr, sizeof(szOutStr), lpszFmt, argptr);
    
    va_end(argptr);
    
    return send(s, szOutStr, strnlen(szOutStr, sizeof(szOutStr)), 0);
}

void msleep(unsigned long milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

void set_nonblock(int s) {
    int flags = fcntl(s, F_GETFL, 0);
    if (flags != -1) {
        fcntl(s, F_SETFL, flags | O_NONBLOCK);
    }
}

void allocate_lists() {
    if (masterSz > 0) {
        master = calloc(masterSz, sizeof(masterList));
        if (!master) { perror("calloc failed for master"); clean_exit(EXIT_FAILURE); }
    } else {
        master = NULL;
    }

    if (safeSz > 0) {
        safe = calloc(safeSz, sizeof(safeList));
        if (!safe) { perror("calloc failed for safe"); clean_exit(EXIT_FAILURE); }
    } else {
        safe = NULL;
    }

    if (shitSz > 0) {
        shit = calloc(shitSz, sizeof(shitList));
        if (!shit) { perror("calloc failed for shit"); clean_exit(EXIT_FAILURE); }
    } else {
        shit = NULL;
    }

    if (desSz > 0) {
        des = calloc(desSz, sizeof(desList));
        if (!des) { perror("calloc failed for des"); clean_exit(EXIT_FAILURE); }
    } else {
        des = NULL;
    }

    //printf("Allocated lists: masterSz=%d, safeSz=%d, shitSz=%d, desSz=%d\n", masterSz, safeSz, shitSz, desSz);
}

void free_lists() {
    if (master) { free(master); master = NULL; masterSz = 0; }
    if (safe) { free(safe); safe = NULL; safeSz = 0; }
    if (shit) { free(shit); shit = NULL; shitSz = 0; }
    if (des) { free(des); des = NULL; desSz = 0; }
}

CommandID resolve_command(const char* com) {
    if (!com) return CMD_UNKNOWN;

    if (!strcasecmp(com, CFGSTUFF_LIST))      return CMD_CFGSTUFF_LIST;
    if (!strcasecmp(com, CFGSTUFF_ADD))       return CMD_CFGSTUFF_ADD;
    if (!strcasecmp(com, CFGSTUFF_REM))       return CMD_CFGSTUFF_REM;
    if (!strcasecmp(com, BASE_TRIGGER))       return CMD_BASE_TRIGGER;
    if (!strcasecmp(com, BASE_HOME))          return CMD_BASE_HOME;
    if (!strcasecmp(com, BASE_TOPIC))         return CMD_BASE_TOPIC;
    if (!strcasecmp(com, BASE_BACKUP))        return CMD_BASE_BACKUP;
    if (!strcasecmp(com, BASE_SERVER))        return CMD_BASE_SERVER;
    if (!strcasecmp(com, BASE_THREADS))       return CMD_BASE_THREADS;
    if (!strcasecmp(com, BASE_PORT))          return CMD_BASE_PORT;
    if (!strcasecmp(com, BASE_DELAY))         return CMD_BASE_DELAY;
    if (!strcasecmp(com, BASE_DELAY_2))       return CMD_BASE_DELAY_2;
    if (!strcasecmp(com, BASE_SCATTER))       return CMD_BASE_SCATTER;
    if (!strcasecmp(com, BASE_BANWAIT))       return CMD_BASE_BANWAIT;
    if (!strcasecmp(com, BASE_CONWAIT))       return CMD_BASE_CONWAIT;
    if (!strcasecmp(com, BASE_SAY))           return CMD_BASE_SAY;
    if (!strcasecmp(com, BASE_VER))           return CMD_BASE_VER;
    if (!strcasecmp(com, BASE_QUIT))          return CMD_BASE_QUIT;
    if (!strcasecmp(com, BASE_RECON))         return CMD_BASE_RECON;
    if (!strcasecmp(com, BASE_PLACE))         return CMD_BASE_PLACE;
    if (!strcasecmp(com, BASE_PING))          return CMD_BASE_PING;
    if (!strcasecmp(com, SERVER_BAN))         return CMD_SERVER_BAN;
    if (!strcasecmp(com, SERVER_UNBAN))       return CMD_SERVER_UNBAN;
    if (!strcasecmp(com, SERVER_KICK))        return CMD_SERVER_KICK;
    if (!strcasecmp(com, SERVER_JOIN))        return CMD_SERVER_JOIN;
    if (!strcasecmp(com, SERVER_REJOIN))      return CMD_SERVER_REJOIN;
    if (!strcasecmp(com, BASE_DES))           return CMD_BASE_DES;
    if (!strcasecmp(com, SERVER_RESIGN))      return CMD_SERVER_RESIGN;
    if (!strcasecmp(com, SERVER_UPTIME))      return CMD_SERVER_UPTIME;
    if (!strcasecmp(com, BASE_OP))            return CMD_BASE_OP;
    if (!strcasecmp(com, BASE_LOCK))          return CMD_BASE_LOCK;
    if (!strcasecmp(com, BASE_UNLOCK))        return CMD_BASE_UNLOCK;
    if (!strcasecmp(com, BASE_CLEAN))         return CMD_BASE_CLEAN;
    if (!strcasecmp(com, BASE_TAG))           return CMD_BASE_TAG;
    if (!strcasecmp(com, BASE_CONTIME))       return CMD_BASE_CONTIME;
    if (!strcasecmp(com, BASE_MEM))           return CMD_BASE_MEM;

    // If none matched, return unknown
    return CMD_UNKNOWN;
}

void processList(int s, char* com, char* name, void **pArray, int *pSize, const char* type) {
    masterList *listArray = (masterList *)(*pArray);
    int x;
    //printf("Processing list: %s\n", list);  // ✅ Debug print
    // Check if command is LIST
    if (!strcasecmp(CFGSTUFF_LIST, com )) {
        //printf("Listing entries for %s (Size: %d)\n", type, *pSize);
        if (*pSize == 0) {
            Send(s, "The %s list is empty.\n", type);
            msleep(1000);
            return;
        }
        for (x = 0; x < *pSize; x++) {
            //printf("Listing: %s\n", listArray[x].id);  // ✅ Debug print
            Send(s, "%s\n", listArray[x].id);
            msleep(1000);
        }
        return;
    }
    // REMOVE
    else if (!strcasecmp(CFGSTUFF_REM, com) && (*name)) {
        for (x = 0; x < *pSize; x++) {
            if (!strcasecmp(listArray[x].id, name)) {
                break;
            }
        }
        if (x == *pSize) {
            // Item not found; send error and return
            Send(s, "Name %s not found in %s list.\n", name, type);
            msleep(1000);
            return;
        }
        // Once you find the entry, remove it:
        memmove(&listArray[x], &listArray[x + 1],
                ((*pSize) - x - 1) * sizeof(masterList));
        (*pSize)--;
    
        masterList *temp = realloc(listArray, (*pSize) * sizeof(masterList));
        if (temp || *pSize == 0) {  // Allow for NULL if list is empty
            listArray = temp;
            *pArray = listArray; // update pointer
        } else {
            Send(s, "realloc failed in processList removal");
            msleep(1000);
            return;
        }
        Send(s, "Removed %s from %s list.\n", name, type);
        msleep(1000);
        return;
    }
    // ADD
    else if (!strcasecmp(CFGSTUFF_ADD, com) && (*name)) {
        // Check if the entry already exists
        for (x = 0; x < *pSize; x++) {
            if (!strcasecmp(listArray[x].id, name)) {
                Send(s, CFGSTUFF_FORMATTING_ADD_SECTION, name, type);
                return;
            }
        }
        // If not found, add a new entry
        masterList *temp = realloc(listArray, (*pSize + 1) * sizeof(masterList));
        if (temp) {
            listArray = temp;
            memset(listArray[*pSize].id, 0, sizeof(listArray[*pSize].id));
            snprintf(listArray[*pSize].id, sizeof(listArray[*pSize].id), "%s", name);
            (*pSize)++;
            Send(s, "Added %s to %s list.\n", name, type);
            *pArray = listArray; // update the pointer
            msleep(1000);
        } else {
            perror("realloc failed in processList addition");
        }
        return;
    }
    else {
        Send(s, "Invalid command or name.\n");
        msleep(1000);
    }
}

void cfgStuff(int s, char* com, char* text) {
    char* pos;
    char textBuffer[FUK_CFG_MAXCOUNT];
    snprintf(textBuffer, sizeof(textBuffer), "%s", text);

    char* list = strtok_r(textBuffer, " ", &pos);
    if (!list) return;
    char* name = strtok_r(NULL, " ", &pos);
    if (!name) {
        name = "";  // ✅ Ensures name is always a valid pointer
    }
    //printf("Processing cfgStuff: list=%s, name=%s\n", list, name);  // ✅ Debug print
    if (strstr(list, CFGSTUFF_MASTER)) {
        processList(s, com, name, (void**)&master, &masterSz, CFGSTUFF_MASTER);
    }
    else if (strstr(list, CFGSTUFF_SAFE)) {
        processList(s, com, name, (void**)&safe, &safeSz, CFGSTUFF_SAFE);
    }
    else if (strstr(list, CFGSTUFF_SHIT)) {
        processList(s, com, name, (void**)&shit, &shitSz, CFGSTUFF_SHIT);
    }
    else if (strstr(list, CFGSTUFF_DES)) {
        processList(s, com, name, (void**)&des, &desSz, CFGSTUFF_DES);
    }
}

void OnJoin(int s, struct data* pb, char* szSpeaker) {
    if (pb->hasop == 1 && time(NULL) - pb->lastTime < 5)
        pb->flood++;
    else
        pb->flood = 0;
    pb->lastTime = time(NULL);
    if (pb->hasop == 1 && pb->flood < 500) {
        for (int d = 0; d < desSz; d++) {
            if (!strcasecmp(szSpeaker, des[d].id)) {
                Send(s, SERVER_COMMAND_1, BASE_DESIGNATE, szSpeaker);
                msleep(1000);
                return;
            }
        }
        for (int j = 0; j < safeSz; j++) {
            if (strstr(szSpeaker, safe[j].id))
                return;
        }
        for (int k = 0; k < masterSz; k++) {
            if (!strcasecmp(szSpeaker, master[k].id))
                return;
        }
        if (pb->tban == 1) {
            if (strstr(szSpeaker, tag)) {
                Send(s, SERVER_COMMAND_1, BASE_BAN, szSpeaker);
                msleep(banWait * 1000);
                return;
            }
        }
        if (pb->lockdown == 1) {
            Send(s, SERVER_COMMAND_1, BASE_BAN, szSpeaker);
            msleep(banWait * 1000);
            return;
        }
        for (int l = 0; l < shitSz; l++) {
            if (strstr(szSpeaker, shit[l].id)) {
                Send(s, SERVER_COMMAND_1, BASE_BAN, szSpeaker);
                msleep(banWait * 1000);
                return;
            }
        }
    } 
}

void OnUserFlags(int s, struct data* pb, char* szSpeaker, u_long uFlags) {
    if (uFlags == 18 && !strcasecmp(pb->username, szSpeaker)) {
        pb->hasop = 1;
        if (topic[0] != '\0') {
            Send(s, SERVER_COMMAND_1, BASE_TOPIC, topic);
            msleep(1000);
        }
        return;
    }
    if (uFlags == 16 && !strcasecmp(pb->username, szSpeaker)) {
        pb->hasop = 0;
        pb->lockdown = 0;
    }
}

// The OnTalk function
void OnTalk(int s, struct data* pb, char* szSpeaker, char* szEventText)
{
    int i = 0;
    int doStuff = 0;

    // Basic sanity checks
    if (!pb || !szSpeaker || !szEventText) return;

    // Check if speaker is in master list
    for (i = 0; i < masterSz; i++) {
        //printf("master[%d].id = %s\n", i, master[i].id);
        if (!strcasecmp(szSpeaker, master[i].id)) {
            doStuff = 1;
            break;
        }
    }

    // If speaker not recognized, return
    if (!doStuff) return;
    //printf("is in master list\n");
    // Check if the message starts with the bot's trigger or a '?' 
    if (szEventText[0] == pb->trigger[0] || szEventText[0] == '?') {
        // Create a buffer for the command string (everything after the first character)
        char tmpBuf[SEND_BUFFER_SIZE] = {0};
        strncpy(tmpBuf, szEventText + 1, sizeof(tmpBuf) - 1);

        // Use strsep to separate the first token (command) from the rest (arguments)
        char* ptrDat = tmpBuf;
        char* com = strsep(&ptrDat, " ");
        if (!com) {
            return;  // no command found
        }

        // Resolve the command to an enum
        CommandID cmd = resolve_command(com);

        // Print the command ID for verification
        //printf("Resolved command ID: %d\n", cmd);
        
        // Switch on the enum for clearer code
        switch (cmd)
        {
            case CMD_CFGSTUFF_LIST:
                if (ptrDat != NULL && pb->botNum == 0) {
                    //printf("Calling cfgStuff for list\n");
                    cfgStuff(s, com, ptrDat);
                }
                break;

            case CMD_CFGSTUFF_ADD:
            {
                if (ptrDat != NULL && pb->botNum == 0) {
                    //printf("Calling cfgStuff for add\n");
                    cfgStuff(s,com, ptrDat);
                    save_cfg();
                }
                break;
            }

            case CMD_CFGSTUFF_REM:
            {
                if (ptrDat != NULL && pb->botNum == 0) {
                    //printf("Calling cfgStuff for remove\n");
                    cfgStuff(s, com, ptrDat);
                    save_cfg();
                }
                break;
            }

            case CMD_BASE_TRIGGER:
            {
                if (ptrDat == NULL) {
                    if (pb->botNum == 0) {
                        Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker,
                             BASE_TRIGGER, BASE_COLON, pb->trigger);
                        msleep(1000);
                    }
                } else {
                    // Set new trigger
                    if (pb->botNum == 0) {
                        memset(pb->trigger, 0, sizeof(pb->trigger));
                        strncpy(pb->trigger, ptrDat, sizeof(pb->trigger) - 1);

                        // Possibly also update global trigger
                        memset(trigger, 0, sizeof(trigger));
                        strncpy(trigger, ptrDat, sizeof(trigger) - 1);

                        save_cfg();
                        Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker,
                             BASE_TRIGGER, BASE_ISNOW, pb->trigger);
                        msleep(1000);
                    } else {
                        memset(pb->trigger, 0, sizeof(pb->trigger));
                        strncpy(pb->trigger, ptrDat, sizeof(pb->trigger) - 1);
                    }
                }
                break;
            }

            case CMD_BASE_HOME:
            {
                if (ptrDat == NULL) {
                    if (pb->botNum == 0) {
                        Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker,
                             BASE_HOME, BASE_COLON, pb->channel);
                        msleep(1000);
                    }
                } else {
                    if (pb->botNum == 0) {
                        memset(pb->channel, 0, sizeof(pb->channel));
                        strncpy(pb->channel, ptrDat, sizeof(pb->channel) - 1);

                        memset(channel, 0, sizeof(channel));
                        strncpy(channel, ptrDat, sizeof(channel) - 1);

                        save_cfg();
                        Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker,
                             BASE_HOME, BASE_ISNOW, pb->channel);
                        msleep(1000);
                    } else {
                        memset(pb->channel, 0, sizeof(pb->channel));
                        strncpy(pb->channel, ptrDat, sizeof(pb->channel) - 1);
                    }
                }
                break;
            }

            case CMD_BASE_TOPIC:
            {
                if (ptrDat == NULL) {
                    if (pb->hasop == 1) {
                        Send(s, SERVER_COMMAND_1, SERVER_TOPIC, topic);
                        msleep(1000);
                    } else if (pb->botNum == 0) {
                        Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker,
                             BASE_TOPIC, BASE_COLON, topic);
                        msleep(1000);
                    }
                } else {
                    if (pb->hasop == 1) {
                        memset(topic, 0, sizeof(topic));
                        strncpy(topic, ptrDat, sizeof(topic) - 1);
                        Send(s, SERVER_COMMAND_1, SERVER_TOPIC, topic);
                        msleep(1000);
                    } else if (pb->botNum == 0) {
                        memset(topic, 0, sizeof(topic));
                        strncpy(topic, ptrDat, sizeof(topic) - 1);
                        save_cfg();
                        Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker,
                             BASE_TOPIC, BASE_ISNOW, topic);
                        msleep(1000);
                    }
                }
                break;
            }

            case CMD_BASE_BACKUP:
            {
                if (pb->botNum == 0) {
                    if (ptrDat == NULL) {
                        Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker,
                             BASE_BACKUP, BASE_COLON, backup);
                        msleep(1000);
                    } else {
                        memset(backup, 0, sizeof(backup));
                        strncpy(backup, ptrDat, sizeof(backup) - 1);
                        save_cfg();
                        Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker,
                             BASE_BACKUP, BASE_ISNOW, backup);
                        msleep(1000);
                    }
                }
                break;
            }

            case CMD_BASE_SERVER:
            {
                if (pb->botNum == 0) {
                    if (ptrDat == NULL) {
                        Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker,
                             BASE_SERVER, BASE_COLON, pb->server);
                        msleep(1000);
                    }
                }
                break;
            }

            case CMD_BASE_THREADS:
            {
                if (pb->botNum == 0) {
                    if (ptrDat == NULL) {
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker,
                             BASE_THREADS, BASE_COLON, pb->threads);
                        msleep(1000);
                    } else {
                        threads = atoi(ptrDat);
                        save_cfg();
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker,
                             BASE_THREADS, BASE_ISNOW, pb->threads);
                        msleep(1000);
                    }
                }
                break;
            }

            case CMD_BASE_PORT:
            {
                // Original note: "arent you forgetting to update pb->port at the save? lol"
                if (pb->botNum == 0) {
                    if (ptrDat == NULL) {
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker,
                             BASE_PORT, BASE_COLON, pb->port);
                        msleep(1000);
                    } else {
                        // Update both global and pb->port if that is your intention
                        port = atoi(ptrDat);
                        pb->port = port;  // so it stays in sync
                        save_cfg();
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker,
                             BASE_PORT, BASE_ISNOW, port);
                        msleep(1000);
                    }
                }
                break;
            }

            case CMD_BASE_DELAY:
            {
                if (pb->botNum == 0) {
                    if (ptrDat == NULL) {
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker,
                             BASE_DELAY, BASE_COLON, delay);
                        msleep(1000);
                    } else {
                        delay = atoi(ptrDat);
                        save_cfg();
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker,
                             BASE_DELAY, BASE_ISNOW, delay);
                        msleep(1000);
                    }
                }
                break;
            }

            case CMD_BASE_DELAY_2:
            {
                if (ptrDat == NULL) {
                    Send(s, ON_COMMAND_REPLY_1, SERVER_WHISPER, szSpeaker,
                         BASE_DELAY, BASE_SCATTER, pb->delay2);
                    msleep(1000);
                }
                break;
            }

            case CMD_BASE_SCATTER:
            {
                if (pb->botNum == 0) {
                    if (ptrDat == NULL) {
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker,
                             BASE_SCATTER, BASE_COLON, scatter);
                        msleep(1000);
                    } else {
                        scatter = atoi(ptrDat);
                        save_cfg();
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker,
                             BASE_SCATTER, BASE_ISNOW, scatter);
                        msleep(1000);
                    }
                }
                break;
            }

            case CMD_BASE_BANWAIT:
            {
                if (pb->botNum == 0) {
                    if (ptrDat == NULL) {
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker,
                             BASE_BANWAIT, BASE_COLON, banWait);
                        msleep(1000);
                    } else {
                        banWait = atoi(ptrDat) * 1000;
                        save_cfg();
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker,
                             BASE_BANWAIT, BASE_ISNOW, (banWait / 1000));
                        msleep(1000);
                    }
                }
                break;
            }

            case CMD_BASE_CONWAIT:
            {
                if (pb->botNum == 0) {
                    if (ptrDat == NULL) {
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker,
                             BASE_CONWAIT, BASE_COLON, conWait);
                        msleep(1000);
                    } else {
                        conWait = atoi(ptrDat);
                        save_cfg();
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker,
                             BASE_CONWAIT, BASE_ISNOW, conWait);
                        msleep(1000);
                    }
                }
                break;
            }

            case CMD_BASE_SAY:
            {
                if (ptrDat != NULL) {
                    Send(s, ON_COMMAND_REPLY_2, ptrDat);
                    msleep(1000);
                }
                break;
            }

            case CMD_BASE_VER:
            {
                if (pb->botNum == 0) {
                    Send(s, SERVER_COMMAND_2, SERVER_WHISPER, szSpeaker, FUK_VERSION);
                    msleep(1000);
                }
                break;
            }

            case CMD_BASE_QUIT:
            {
                shutDown = 1;
                close(s);
                s = -1;
                break;
            }

            case CMD_BASE_RECON:
            {
                recon = 1;
                close(s);
                s = -1;
                break;
            }

            case CMD_BASE_PLACE:
            {
                // Original code snippet
                Send(s, "/%s %s %d to login to %s with actual delay of %d.\r\n",
                     SERVER_WHISPER, szSpeaker, pb->place, pb->server, pb->delay2);
                msleep(1000);
                break;
            }

            case CMD_BASE_PING:
            {
                // Only if pb->botNum == 0
                if (pb->botNum == 0) {
                    char pingStr[250];
                    snprintf(pingStr, sizeof(pingStr), "%s -c1 %s", BASE_PING, pb->server);

                    FILE* fp1 = popen(pingStr, "r");
                    if (fp1) {
                        char pi[512];
                        // read two lines
                        if (fgets(pi, sizeof(pi), fp1) != NULL) {
                            // do something with pi
                        }
                        if (fgets(pi, sizeof(pi), fp1) != NULL) {
                            Send(s, SERVER_BASE_SPEAK, pi);
                        }
                        pclose(fp1);
                    }
                    msleep(1000);
                }
                break;
            }

            case CMD_SERVER_BAN:
            {
                if (ptrDat != NULL && pb->hasop == 1) {
                    Send(s, SERVER_COMMAND_1, SERVER_BAN, ptrDat);
                    msleep(1000);
                }
                break;
            }

            case CMD_SERVER_UNBAN:
            {
                if (ptrDat != NULL && pb->hasop == 1) {
                    pb->lockdown = 0;
                    Send(s, SERVER_COMMAND_1, SERVER_UNBAN, ptrDat);
                    msleep(1000);
                }
                break;
            }

            case CMD_SERVER_KICK:
            {
                if (ptrDat != NULL && pb->hasop == 1) {
                    Send(s, SERVER_COMMAND_1, SERVER_KICK, ptrDat);
                    msleep(1000);
                }
                break;
            }

            case CMD_SERVER_JOIN:
            {
                if (ptrDat != NULL) {
                    Send(s, SERVER_COMMAND_1, SERVER_JOIN, ptrDat);
                    msleep(1000);
                }
                break;
            }

            case CMD_SERVER_REJOIN:
            {
                Send(s, SERVER_COMMAND_0, SERVER_REJOIN);
                msleep(1000);
                break;
            }

            case CMD_BASE_DES:
            {
                if (pb->hasop == 1) {
                    pb->op = 0;
                    if (ptrDat != NULL) {
                        Send(s, SERVER_COMMAND_1, SERVER_DESIGNATE, ptrDat);
                    } else {
                        Send(s, SERVER_COMMAND_1, SERVER_DESIGNATE, szSpeaker);
                    }
                    msleep(1000);
                }
                break;
            }

            case CMD_SERVER_RESIGN:
            {
                if (pb->hasop == 1) {
                    if (pb->des != 0) {
                        pb->des = 0;
                        Send(s, SERVER_COMMAND_0, SERVER_RESIGN);
                        msleep(1000);
                    } else {
                        Send(s, ON_COMMAND_REPLY_2, BASE_NO_HEIR_TEXT);
                        msleep(1000);
                    }
                }
                break;
            }

            case CMD_SERVER_UPTIME:
            {
                if (pb->botNum == 0) {
                    Send(s, SERVER_COMMAND_0, SERVER_UPTIME);
                    msleep(1000);
                }
                break;
            }

            case CMD_BASE_OP:
            {
                if (pb->hasop == 1) {
                    pb->op = 1;
                    if (ptrDat != NULL) {
                        Send(s, SERVER_COMMAND_1, SERVER_DESIGNATE, ptrDat);
                    } else {
                        Send(s, SERVER_COMMAND_1, SERVER_DESIGNATE, szSpeaker);
                    }
                    msleep(1000);
                }
                break;
            }

            case CMD_BASE_LOCK:
            {
                if (pb->hasop == 1) {
                    pb->lockdown = 1;
                    Send(s, SERVER_COMMAND_2, SERVER_TOPIC, topic, BASE_CHANLOCKED);
                    msleep(1000);
                    Send(s, SERVER_COMMAND_1, SERVER_WHO, pb->currChan);
                    msleep(1000);
                }
                break;
            }

            case CMD_BASE_UNLOCK:
            {
                pb->lockdown = 0;
                if (pb->hasop == 1) {
                    Send(s, SERVER_COMMAND_1, SERVER_TOPIC, topic);
                    msleep(1000);
                }
                break;
            }

            case CMD_BASE_CLEAN:
            {
                if (pb->hasop == 1) {
                    Send(s, SERVER_COMMAND_1, SERVER_WHO, pb->currChan);
                    msleep(1000);
                }
                break;
            }

            case CMD_BASE_TAG:
            {
                if (pb->hasop == 1) {
                    if (ptrDat != NULL) {
                        pb->tban = 1;
                        memset(tag, 0, sizeof(tag));
                        strcpy(tag, ptrDat);
                        Send(s, SERVER_COMMAND_1, SERVER_WHO, pb->currChan);
                        msleep(1000);
                    } else {
                        pb->tban = 0;
                        Send(s, ON_COMMAND_REPLY_2, BASE_TAGBAN_DISABLED);
                        msleep(1000);
                    }
                }
                break;
            }

            case CMD_BASE_CONTIME:
            {
                if (pb->botNum == 0) {
                    Send(s, ON_COMMAND_REPLY_3, SERVER_WHISPER, szSpeaker, pb->conTime);
                    msleep(1000);
                }
                break;
            }

            case CMD_BASE_MEM:
            {
                if (pb->botNum == 0) {
                    struct rusage r_usage;
                    getrusage(RUSAGE_SELF, &r_usage);
                    // r_usage.ru_maxrss is often in KB on Linux, but usage may vary by system
                    Send(s, ON_COMMAND_REPLY_4, SERVER_WHISPER, szSpeaker, r_usage.ru_maxrss);
                    msleep(1000);
                }
                break;
            }

            case CMD_UNKNOWN:
            default:
                // No recognized command - do nothing or handle an error
                break;
        } // end switch
    } // end if (starts with trigger)
}

void OnChannel(int s, struct data* pb, char* szEventText) {
    if (!pb || !szEventText) return;  // Prevent NULL pointer dereference

    // Trim leading spaces safely
    while (*szEventText == ' ') ++szEventText;

    // Ensure bounded copy to avoid overflow
    memset(pb->currChan, 0, sizeof(pb->currChan));
    strncpy(pb->currChan, szEventText, sizeof(pb->currChan) - 1);
    pb->currChan[sizeof(pb->currChan) - 1] = '\0';  // Ensure null termination

    if (!strcasecmp(szEventText, pb->channel)) {
        pb->chanham = 0;
        Send(s, SERVER_COMMAND_0, SERVER_PLACE, NULL);
        msleep(1000);
    }
    else if (!strcasecmp(szEventText, CHANNEL_VOID)) {
        pb->chanham = 1;
        Send(s, SERVER_COMMAND_0, SERVER_PLACE, NULL);
        msleep(1000);

        // Ensure 'backup' is properly initialized before use
        extern char backup[MAX_CFG_LEN];  // Assumption: backup is declared elsewhere
        if (backup[0] != '\0') {  // Ensure backup is not empty before using it
            Send(s, SERVER_COMMAND_1, SERVER_JOIN, backup);
            msleep(1000);
        }
    }
}

void OnInfo(int s, struct data *pb, char *szEventText) {
    if (!szEventText) return;
    char szEventTextCopy[EVENT_TEXT_SIZE];  // Allocate fixed buffer
    strncpy(szEventTextCopy, szEventText, sizeof(szEventTextCopy) - 1);
    szEventTextCopy[sizeof(szEventTextCopy) - 1] = '\0';

    if (strstr(szEventTextCopy, BASE_INIT6) || strstr(szEventTextCopy, BASE_SETTHETOPIC) ||
        strstr(szEventTextCopy, BASE_WASKICKED) || strstr(szEventTextCopy, BASE_WASBANNED) ||
        strstr(szEventTextCopy, BASE_WASUNBANNED) || strstr(szEventTextCopy, BASE_NOCHATPRIV) ||
        strstr(szEventTextCopy, BASE_CHATCHANNEL) || strstr(szEventTextCopy, BASE_CHANNELUSERS)) {
        return;
    }
    else if (strstr(szEventTextCopy, BASE_NEWHEIR)) {
        pb->des = 1;
        if (pb->op != 0 && pb->hasop != 0) {
            pb->op = 0;
            Send(s, SERVER_COMMAND_0, SERVER_RESIGN);
            msleep(1000);
        }
        return;
    }
    else if (strstr(szEventTextCopy, BASE_PLACED)) {
        char *tmp, *pos;
        tmp = strtok_r(szEventTextCopy, " ", &pos);
        if (tmp) tmp = strtok_r(NULL, " ", &pos);
        if (tmp) tmp = strtok_r(NULL, " ", &pos);
        if (tmp) {
            char *endptr;
            pb->place = strtol(tmp, &endptr, 10);
            if (*endptr != '\0') {
                // Handle error if conversion fails
            }
        }
        return;
    }
    else if (strstr(szEventTextCopy, BASE_CUPTIME)) {
        char *tmp, *pos;
        tmp = strtok_r(szEventTextCopy, "\r\n", &pos);
        if (tmp) {
            Send(s, SERVER_BASE_SPEAK, tmp);
            msleep(1000);
        }
        return;
    }
    else if (strstr(szEventTextCopy, BASE_YOUWEREKICKED)) {
        pb->chanham = 1;
        Send(s, SERVER_COMMAND_1, SERVER_JOIN, backup);
        msleep(1000);
        return;
    }
    else {
        char *pos, *user1, *user2;
        user1 = strtok_r(szEventTextCopy, ", ", &pos);
        if (user1) {
            while (*user1 == ' ') {
                ++user1;
            }
            OnJoin(s, pb, user1);
        }
        user2 = strtok_r(NULL, "\r\n", &pos);
        if (user2) {
            while (*user2 == ' ') {
                ++user2;
            }
            OnJoin(s, pb, user2);
        }
    }
}

void OnError(struct data *pb, char *szEventText) {
    if (!pb || !szEventText) {
        return;  // Prevent NULL dereferences
    }

    if (strstr(szEventText, BASE_CHANRESTRICTED_TEXT)) {
        return;
    }
    if (strstr(szEventText, BASE_INVALIDUSER_TEXT)) {
        pb->des = 0;
        return;
    }
    if (strstr(szEventText, BASE_USERNOTLOGGEDON_TEXT)) {
        pb->des = 0;
    }
}

void OnPing(int s, struct data *pb, char *szEventText) {
    if (!pb || !szEventText) {
        return;  // Prevent NULL dereferences
    }

    if (pb->chanham == 1) {
        Send(s, SERVER_COMMAND_1, SERVER_JOIN, pb->channel);
    } else {
        Send(s, SERVER_COMMAND_1, SERVER_PONG, szEventText);
    }
    
    msleep(1000);  // Ensure `pb` remains valid if this runs in a multi-threaded environment
}

void Dispatch(int s, struct data *pb, char *szEventText) {
    if (!pb || !szEventText) {
        return;  // Prevent NULL dereference
    }

    char *eventType, *pos;
    char *USER_CMD, *USER_FLAGS, *USER_PING, *USER_NAME, *USER_DIRECTION, *USER_MSG;
    char *CHANNEL_CMD, *CHANNEL_NAME;
    char *SERVER_MSG, *SERVER_CMD;
    char *PING;

    eventType = strtok_r(szEventText, " ", &pos);
    if (!eventType) return;  // Ensure we have an event type
    if (!strcasecmp(eventType, "USER")) {
        USER_CMD = strtok_r(NULL, " ", &pos);
        //printf("USER CMD: %s\n", USER_CMD);
        if (!USER_CMD) return;

        if (!strcasecmp(USER_CMD, "IN") || !strcasecmp(USER_CMD, "UPDATE") || !strcasecmp(USER_CMD, "JOIN")) {
            USER_FLAGS = strtok_r(NULL, " ", &pos);
            USER_PING = strtok_r(NULL, " ", &pos);
            USER_NAME = strtok_r(NULL, " ", &pos);

            if (!USER_FLAGS || !USER_PING || !USER_NAME) return;  // Validate required tokens

            OnUserFlags(s, pb, USER_NAME, atoi(USER_FLAGS));
            return;
        } else if (!strcasecmp(USER_CMD, "EMOTE")) {
            USER_NAME = strtok_r(NULL, " ", &pos);
            USER_MSG = strtok_r(NULL, " ", &pos);

            if (!USER_NAME || !USER_MSG) return;

            OnTalk(s, pb, USER_NAME, USER_MSG);
            return;
        } else if (!strcasecmp(USER_CMD, "WHISPER") || !strcasecmp(USER_CMD, "TALK")) {
            USER_DIRECTION = strtok_r(NULL, " ", &pos);
            USER_NAME = strtok_r(NULL, " ", &pos);
            USER_MSG = strtok_r(NULL, "\r\n", &pos);

            if (!USER_DIRECTION || !USER_NAME || !USER_MSG) return;

            OnTalk(s, pb, USER_NAME, USER_MSG);
            return;
        }
        return;
    } else if (!strcasecmp(eventType, "CHANNEL")) {
        CHANNEL_CMD = strtok_r(NULL, " ", &pos);
        if (!CHANNEL_CMD) return;

        if (!strcasecmp(CHANNEL_CMD, "JOIN")) {
            CHANNEL_NAME = strtok_r(NULL, "\r\n", &pos);
            if (!CHANNEL_NAME) return;

            OnChannel(s, pb, CHANNEL_NAME);
        }
        return;
    } else if (!strcasecmp(eventType, "SERVER")) {
        SERVER_CMD = strtok_r(NULL, " ", &pos);
        if (!SERVER_CMD) return;

        if (!strcasecmp(SERVER_CMD, "INFO") || !strcasecmp(SERVER_CMD, "ERROR")) {
            SERVER_MSG = strtok_r(NULL, "\r\n", &pos);
            if (!SERVER_MSG) return;

            if (!strcasecmp(SERVER_CMD, "INFO")) {
                OnInfo(s, pb, SERVER_MSG);
            } else {
                OnError(pb, SERVER_MSG);
            }
            return;
        }
        return;
    } else if (!strcasecmp(eventType, "PING")) {
        PING = strtok_r(NULL, " ", &pos);
        if (!PING) return;

        OnPing(s, pb, PING);
        return;
    }
}

void message_loop(int s, struct data* pb) {
    int n;
    int nBufLen = 0;
    int nBufPos = 0;
    char stageBuf[BUFFSIZE];
    struct timeval tv;

    if (s == INVALID_SOCKET) {
        return;
    }

    for (;;) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(s, &fds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        if (shutDown == 1 || recon == 1) {
            break;
        }
        n = select(s + 1, &fds, NULL, NULL, &tv);
        if (n > 0) {
            int nNumToRead = BUFFSIZE - nBufLen - nBufPos;

            // Ensure we don't read more than the buffer can hold
            if (nNumToRead <= 0) {
                if (nBufPos + nBufLen > BUFFSIZE) {
                    // Prevent memory corruption
                    nBufLen = BUFFSIZE - nBufPos;
                }
                memmove(stageBuf, stageBuf + nBufPos, nBufLen);
                nBufPos = 0;
                nNumToRead = BUFFSIZE - nBufLen;
            }

            n = recv(s, stageBuf + nBufPos + nBufLen, nNumToRead, 0);
            if (n <= 0) {
                return ; // Handle disconnection or error
            }

            nBufLen += n;

            while (nBufLen > 0) {
                unsigned char* m = (unsigned char*) stageBuf + nBufPos;
                int nMsgLen = 0;

                // Find newline character as a message delimiter
                while (nMsgLen < nBufLen && nMsgLen < BUFFSIZE) {
                    if (m[nMsgLen] == '\n')
                        break;
                    nMsgLen++;
                }

                if (nMsgLen >= nBufLen) {
                    break; // Incomplete message, wait for more data
                }

                // Ensure the message is null-terminated
                m[nMsgLen] = '\0';

                // Dispatch the complete message
                Dispatch(s, pb, (char*) m);

                // Adjust buffer to remove processed message
                nBufLen -= (nMsgLen + 1);
                nBufPos += (nMsgLen + 1);

                // Reset buffer if empty
                if (nBufLen == 0)
                    nBufPos = 0;
            }
        }
        msleep(500); // Small delay to prevent CPU overuse
    }
    //printf(" %s Exiting message loop\n", pb->username);
}

int save_cfg() {
    FILE* cfg = fopen("config.json", "w");
    if (!cfg) {
        perror("Error opening config.json\n");
        return -1;
    }
    int i;
    // Create JSON object
    cJSON *json = cJSON_CreateObject();

    // Add string values
    cJSON_AddStringToObject(json, CFGSTUFF_USERNAME, username);
    cJSON_AddStringToObject(json, CFGSTUFF_PASSWORD, password);
    cJSON_AddStringToObject(json, CFGSTUFF_HOME, channel);
    cJSON_AddStringToObject(json, CFGSTUFF_SERVER, server);
    cJSON_AddNumberToObject(json, CFGSTUFF_PORT, port);
    cJSON_AddStringToObject(json, CFGSTUFF_BIND, bindaddr);
    cJSON_AddStringToObject(json, CFGSTUFF_TRIGGER, trigger);
    cJSON_AddNumberToObject(json, CFGSTUFF_THREADS, threads);
    cJSON_AddNumberToObject(json, CFGSTUFF_DELAY, delay);
    cJSON_AddNumberToObject(json, CFGSTUFF_SCATTER, scatter);
    cJSON_AddNumberToObject(json, CFGSTUFF_CONWAIT, conWait);
    cJSON_AddNumberToObject(json, CFGSTUFF_BANWAIT, banWait);
    cJSON_AddNumberToObject(json, CFGSTUFF_NUMBOTS, numBots);
    cJSON_AddStringToObject(json, CFGSTUFF_BACKUPCHAN, backup);
    cJSON_AddStringToObject(json, CFGSTUFF_TOPIC, topic);
    
    cJSON *masterArray = cJSON_CreateArray();
    cJSON *safeArray = cJSON_CreateArray();
    cJSON *desArray = cJSON_CreateArray();
    cJSON *shitArray = cJSON_CreateArray();

    for (i = 0; i < masterSz; i++) {
        cJSON_AddItemToArray(masterArray, cJSON_CreateString(master[i].id));
    }
    for (i = 0; i < safeSz; i++) {
        cJSON_AddItemToArray(safeArray, cJSON_CreateString(safe[i].id));
    }
    for (i = 0; i < desSz; i++) {
        cJSON_AddItemToArray(desArray, cJSON_CreateString(des[i].id));
    }
    for (i = 0; i < shitSz; i++) {
        cJSON_AddItemToArray(shitArray, cJSON_CreateString(shit[i].id));
    }

    cJSON_AddItemToObject(json, "master", masterArray);
    cJSON_AddItemToObject(json, "safe", safeArray);
    cJSON_AddItemToObject(json, "des", desArray);
    cJSON_AddItemToObject(json, "shit", shitArray);

    // Convert JSON object to a string
    char *json_string = cJSON_Print(json);

    // Save to file
    fprintf(cfg, "%s", json_string);
    fclose(cfg);

    // Cleanup
    free(json_string);
    cJSON_Delete(json);

    //printf("Config saved to config.json\n");
    return 0;
}

void json_safe_copy(cJSON *json, const char *key, char *dest, size_t dest_size) {
    if (!json || !key || !dest || dest_size == 0) {
        return;
    }

    cJSON *item = cJSON_GetObjectItem(json, key);
    if (item && cJSON_IsString(item) && item->valuestring) {
        strncpy(dest, item->valuestring, dest_size - 1);
        dest[dest_size - 1] = '\0'; // Ensure null termination
    }
}

int read_config() {
    FILE* cfg = fopen("config.json", "r");
    if (!cfg) {
        perror("Error opening config.json\n");
        return -1;
    }
    //printf("Reading config.json...\n");

    // Read file content
    fseek(cfg, 0, SEEK_END);
    long length = ftell(cfg);
    fseek(cfg, 0, SEEK_SET);
    char *data = (char*) malloc(length + 1);
    if (!data) {
        fclose(cfg);
        perror("Memory allocation failed");
        return -1;
    }
    fread(data, 1, length, cfg);
    fclose(cfg);
    data[length] = '\0';

    // Parse JSON
    cJSON *json = cJSON_Parse(data);
    if (!json) {
        perror("Error parsing JSON\n");
        free(data);
        return -1;
    }
    //printf("JSON has been parsed\n");

    // Copy individual values safely
    json_safe_copy(json, "username", username, sizeof(username));
    json_safe_copy(json, "password", password, sizeof(password));
    json_safe_copy(json, "home", channel, sizeof(channel));
    json_safe_copy(json, "server", server, sizeof(server));
    json_safe_copy(json, "bind", bindaddr, sizeof(bindaddr));
    json_safe_copy(json, "trigger", trigger, sizeof(trigger));
    json_safe_copy(json, "backupchan", backup, sizeof(backup));
    json_safe_copy(json, "topic", topic, sizeof(topic));

    // Read integer values safely
    cJSON *portItem = cJSON_GetObjectItem(json, "port");
    port = (portItem && cJSON_IsNumber(portItem)) ? portItem->valueint : 6112;

    cJSON *threadsItem = cJSON_GetObjectItem(json, "threads");
    threads = (threadsItem && cJSON_IsNumber(threadsItem)) ? threadsItem->valueint : 1;

    cJSON *numBotsItem = cJSON_GetObjectItem(json, "numbots");
    numBots = (numBotsItem && cJSON_IsNumber(numBotsItem)) ? numBotsItem->valueint : 1;

    cJSON *delayItem = cJSON_GetObjectItem(json, "delay");
    delay = (delayItem && cJSON_IsNumber(delayItem)) ? delayItem->valueint : 100;

    cJSON *scatterItem = cJSON_GetObjectItem(json, "scatter");
    scatter = (scatterItem && cJSON_IsNumber(scatterItem)) ? scatterItem->valueint : 50;

    cJSON *conwaitItem = cJSON_GetObjectItem(json, "conwait");
    conWait = (conwaitItem && cJSON_IsNumber(conwaitItem)) ? conwaitItem->valueint : 5;

    cJSON *banwaitItem = cJSON_GetObjectItem(json, "banwait");
    banWait = (banwaitItem && cJSON_IsNumber(banwaitItem)) ? banwaitItem->valueint : 0;

    // Read and allocate arrays
    cJSON *masterArray = cJSON_GetObjectItem(json, "master");
    cJSON *safeArray = cJSON_GetObjectItem(json, "safe");
    cJSON *desArray = cJSON_GetObjectItem(json, "des");
    cJSON *shitArray = cJSON_GetObjectItem(json, "shit");

    masterSz = masterArray ? cJSON_GetArraySize(masterArray) : 0;
    safeSz   = safeArray   ? cJSON_GetArraySize(safeArray)   : 0;
    desSz    = desArray    ? cJSON_GetArraySize(desArray)    : 0;
    shitSz   = shitArray   ? cJSON_GetArraySize(shitArray)   : 0;

    allocate_lists();  // Allocate memory for master, safe, des, and shit arrays

    for (int i = 0; i < masterSz; i++) {
        cJSON *item = cJSON_GetArrayItem(masterArray, i);
        if (item && cJSON_IsString(item)) {
            strncpy(master[i].id, item->valuestring, sizeof(master[i].id) - 1);
            master[i].id[sizeof(master[i].id) - 1] = '\0';
        }
    }

    for (int i = 0; i < safeSz; i++) {
        cJSON *item = cJSON_GetArrayItem(safeArray, i);
        if (item && cJSON_IsString(item)) {
            strncpy(safe[i].id, item->valuestring, sizeof(safe[i].id) - 1);
            safe[i].id[sizeof(safe[i].id) - 1] = '\0';
        }
    }

    for (int i = 0; i < desSz; i++) {
        cJSON *item = cJSON_GetArrayItem(desArray, i);
        if (item && cJSON_IsString(item)) {
            strncpy(des[i].id, item->valuestring, sizeof(des[i].id) - 1);
            des[i].id[sizeof(des[i].id) - 1] = '\0';
        }
    }

    for (int i = 0; i < shitSz; i++) {
        cJSON *item = cJSON_GetArrayItem(shitArray, i);
        if (item && cJSON_IsString(item)) {
            strncpy(shit[i].id, item->valuestring, sizeof(shit[i].id) - 1);
            shit[i].id[sizeof(shit[i].id) - 1] = '\0';
        }
    }

    // Free memory safely
    cJSON_Delete(json);
    free(data);

    return 0;
}

// This function ensures that if the cached DNS results are
// older than DNS_CACHE_TTL seconds, we refresh them.
struct addrinfo* get_cached_dns(const char *portStr) {
    time_t now = time(NULL);

    pthread_mutex_lock(&g_dns_mutex);

    // If no cache or it's older than 5 seconds, do a fresh lookup
    if (!g_cached_dns || (now - g_last_dns_update) > DNS_CACHE_TTL) {
        // If we already had a cached list, free it
        if (g_cached_dns) {
            freeaddrinfo(g_cached_dns);
            g_cached_dns = NULL;
        }

        struct addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family   = AF_INET;   // or AF_INET if you only want IPv4
        hints.ai_socktype = SOCK_STREAM;

        int ret = getaddrinfo(pb->server, portStr, &hints, &g_cached_dns);
        if (ret != 0) {
            fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(ret));
            g_cached_dns = NULL;  // ensure it's not left half‐initialized
        } else {
            // Successfully refreshed
            g_last_dns_update = now;
        }
    }

    // You might want to *return a copy* if you need full control
    // outside the lock. But in many cases, returning the pointer
    // is enough, as long as you keep the lock for usage or only
    // read it quickly.
    struct addrinfo* result = g_cached_dns;
    pthread_mutex_unlock(&g_dns_mutex);

    return result;
}

/**
 * connect_nonblock()
 *   - Sets the socket to O_NONBLOCK.
 *   - Calls connect().
 *   - If connect() returns EINPROGRESS, uses select() with the given `tv`
 *     to wait for the socket to become writable.
 *   - Returns 0 on success, -1 on failure.
 */
static int connect_nonblock(int s,
                            const struct sockaddr* addr,
                            socklen_t addrlen,
                            struct timeval tv)
{
    set_nonblock(s);

    if (connect(s, addr, addrlen) == 0) {
        // Connected immediately (rare but possible)
        return 0;
    }
    if (errno != EINPROGRESS) {
        // Some other error
        return -1;
    }

    // Wait for writeability
    fd_set wfds;
    FD_ZERO(&wfds);
    FD_SET(s, &wfds);

    int ret = select(s + 1, NULL, &wfds, NULL, &tv);
    if (ret <= 0) { 
        // 0 => timeout, -1 => error
        return -1;
    }
    if (!FD_ISSET(s, &wfds)) {
        // Not writable for some reason
        return -1;
    }

    // Double-check for connect() success
    int so_error = 0;
    socklen_t len = sizeof(so_error);
    getsockopt(s, SOL_SOCKET, SO_ERROR, &so_error, &len);
    if (so_error != 0) {
        errno = so_error;
        return -1;
    }

    return 0; // success
}

/**
 * Connect() tries exactly ONE address from the DNS cache
 * using the already-created socket 's'.
 *   - Returns 0 if connect succeeded,
 *     -1 if bind or connect failed,
 *     or if no addresses exist.
 */
int Connect(int s, struct timeval tv, struct data* pb) {
    // Convert port to string for getaddrinfo
    char portStr[16];
    snprintf(portStr, sizeof(portStr), "%d", pb->port);

    // Obtain cached DNS results
    struct addrinfo* ai = get_cached_dns(portStr);
    if (!ai) {
        return -1; // no addresses
    }

    // We'll just take the FIRST address from the list:
    // If you want to attempt multiple, you'd need multiple attempts,
    // but then you'd also need multiple sockets, because once you've
    // called connect() on 's', you can't just reconnect it to a different address.
    struct addrinfo* p = ai; 
    if (!p) {
        return -1; 
    }

    // BIND to your local IP if desired
    if (bindaddr[0]) {
        struct sockaddr_in local_addr;
        memset(&local_addr, 0, sizeof(local_addr));
        local_addr.sin_family = AF_INET;
        local_addr.sin_port   = 0; // ephemeral local port
        inet_pton(AF_INET, bindaddr, &local_addr.sin_addr);

        if (bind(s, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
            perror("bind failed");
            return -1;
        }
    }

    // Nonblocking connect on the one address
    if (connect_nonblock(s, p->ai_addr, p->ai_addrlen, tv) == 0) {
        // success
        return 0;
    }
    // else fail
    return -1;
}


void* thread_conn(void* arg) {
    int s;
    struct data* pb = (struct data*)arg;
    struct timeval tv = { .tv_sec = 0 };

    startTime = time(NULL);
    pb->delay2 = (scatter > 0) ? (rand() % scatter + delay) : delay;
    tv.tv_usec = pb->delay2 * 1000;

    //printf("Thread attempting to connect as %s\n", pb->username);
    while (!atomic_load(&pb->connected)) {
        s = socket(AF_INET, SOCK_STREAM, 0);
        if (Connect(s, tv, pb) == 0) {
            break;
        } else {
            //printf("Connection failed. Retrying...\n");
            close(s);
        }   
    }

    // Ensure only one thread establishes the connection
    bool expected = false;
    if (!atomic_compare_exchange_strong(&pb->connected, &expected, true)) {
        //printf("Wasn't firt thread. Closing socket.\n");
        close(s);
        pthread_exit(NULL);
    }

    // First successful thread does the work
    //printf("Sending logon packet: %s\n", pb->logonPacket);
    Send(s, pb->logonPacket, sizeof(pb->logonPacket), 0);
    pb->conTime = time(NULL) - startTime;
    
    message_loop(s, pb);
    
    // Cleanup and discont if not already disconnected by remote command.
    if (s != -1) {
        close(s);
    }

    //printf("%s disconnected.\n", pb->username);
    atomic_store(&pb->connected, false);
    pthread_exit(NULL);
}

void reinitialize_bot(struct data* pb) {
    // Reset connection state and timing information
    pb->flood = 0;
    pb->conTime = 0;
    pb->lockdown = 0;
    pb->op = 0;
    pb->des = 0;
    pb->tban = 0;
    pb->chanham = 0;
    pb->hasop = 0;
    recon = 0;
    memset(pb->logonPacket, '\0', sizeof(pb->logonPacket));
    memset(pb->currChan, '\0', sizeof(pb->currChan));
    memset(pb->username, '\0', sizeof(pb->username));
    memset(pb->password, '\0', sizeof(pb->password));
    memset(pb->channel, '\0', sizeof(pb->channel));
    memset(pb->server, '\0', sizeof(pb->server));
    memset(pb->trigger, '\0', sizeof(pb->trigger));
}

char *replace_str(char *str, char *orig, int rep) {
    char *p = strstr(str, orig);
    if (!p) return strdup(str);

    int len = snprintf(NULL, 0, "%.*s%d%s", (int)(p - str), str, rep, p + strlen(orig));
    char *buffer = malloc(len + 1);
    if (!buffer) return NULL;

    snprintf(buffer, len + 1, "%.*s%d%s", (int)(p - str), str, rep, p + strlen(orig));
    return buffer;
}

void create_threads(struct data* pb) {
    int err;
    int i = 0;
    int numThreads = numBots * threads;
    pthread_t *thread = malloc(sizeof(pthread_t) * numThreads);
    if (!thread) {
        perror("Thread allocation failed");
        clean_exit(EXIT_FAILURE);
    }

    //printf("Creating %d bots...\n", numBots);

    // Create and configure each bot using indexed access
    for (int t = 0; t < numBots; t++) {
        char *replaced = replace_str(username, "#", t);
        char locName[MAX_CFG_LEN] = { 0 };  // Ensure safe buffer size
        
        if (replaced) {
            strncpy(locName, replaced, sizeof(locName) - 1);
            locName[sizeof(locName) - 1] = '\0';  // Ensure null termination
            free(replaced);  // Ensure no memory leaks
        } else {
            printf("Failed to replace username. Need # in username in config.json");
            strncpy(locName, username, sizeof(locName) - 1);
            locName[sizeof(locName) - 1] = '\0';  // Ensure null termination
            numBots = 1;  // Ensure only one bot is created
        }
        // Ensure pb[t].username has valid allocated memory before writing
        if (pb[t].username) {
            snprintf(pb[t].username, sizeof(pb[t].username), "%s", locName);
            pb[t].username[sizeof(pb[t].username) - 1] = '\0';
        }
        
        // Repeat for other fields
        if (pb[t].password) {
            snprintf(pb[t].password, sizeof(pb[t].password), "%s", password);
            pb[t].password[sizeof(pb[t].password) - 1] = '\0';
        }
        
        if (pb[t].channel) {
            snprintf(pb[t].channel, sizeof(pb[t].channel), "%s", channel);
            pb[t].channel[sizeof(pb[t].channel) - 1] = '\0';
        }
        
        if (pb[t].server) {
            snprintf(pb[t].server, sizeof(pb[t].server), "%s", server);
            pb[t].server[sizeof(pb[t].server) - 1] = '\0';
        }
        
        if (pb[t].trigger) {
            snprintf(pb[t].trigger, sizeof(pb[t].trigger), "%s", trigger);
            pb[t].trigger[sizeof(pb[t].trigger) - 1] = '\0';
        }        

        pb[t].botNum = t;
        pb[t].port = port;
        pb[t].threads = threads;

        memset(pb[t].logonPacket, '\0', sizeof(pb[t].logonPacket));
        snprintf(pb[t].logonPacket, sizeof(pb[t].logonPacket),
            "C1\r\nACCT %s\r\nPASS %s\r\nHOME %s\r\nLOGIN\r\n",
            pb[t].username, pb[t].password, pb[t].channel);
    
        pb[t].logonPacket[sizeof(pb[t].logonPacket) - 1] = '\0';

        //printf("Bot %d\n%s\n", t, pb[t].logonPacket);
        // Create threads for each bot
        for (int z = 0; z < threads; z++, i++) {
            if (pthread_create(&thread[i], NULL, thread_conn, &pb[t]) != 0) {
                perror("Failed to create thread");
            }
        }
    }
    //printf(" %d threads created.\n", i);

    // Join threads, we compare to i because it's the total number of threads
    for (int z = 0; z < i; z++) {
        err = pthread_join(thread[z], NULL);
        if (err != 0) {
            perror("pthread_join failed");
        }
    }

    free(thread);
}

int main() {
    srand((unsigned)time(NULL));

    printf("%s\n" , FUK_VERSION);
    // Fork the process to run in the background.
    printf("PID: %d\n", getpid() + 1);
    if ((main_pid = fork()) == -1) {
        printf("shutting down: unable to fork\n");
        exit(1);
        return 1;
    }
    // If the PID is not 0, then we are the parent process.
    if (main_pid != 0) {
        return 0;
    }

    // Read configuration from file.
    if (read_config() != 0) {
        perror("Read config error.");
        clean_exit(EXIT_FAILURE);
    }

    // Allocate the bot array AFTER numBots is set by read_config().
    pb = calloc(numBots, sizeof(struct data));
    if (!pb) {
        perror("Memory allocation failed for pb");
        clean_exit(EXIT_FAILURE);
    }

    // Main loop: run indefinitely until the process is killed remotely.
    while (shutDown != 1) {
        // Reinitialize each bot's data before starting threads.
        for (int i = 0; i < numBots; i++) {
            reinitialize_bot(&pb[i]);
        }
        startTime = time(NULL);

        // Create threads that perform bot tasks.
        create_threads(pb);

        // Sleep for a specified connection wait time.
        msleep(conWait * 1000);
    }

    // Cleanup and exit
    clean_exit(EXIT_SUCCESS);
}
