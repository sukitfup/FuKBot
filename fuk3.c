#include "fuk3.h"

int Send(int s, const char* lpszFmt, ...) {
    char szOutStr[SEND_BUFFER_SIZE];
    va_list argptr;
    va_start(argptr, lpszFmt);
    
    // Prevent buffer overflow
    vsnprintf(szOutStr, sizeof(szOutStr), lpszFmt, argptr);
    
    va_end(argptr);
    
    return send(s, szOutStr, strlen(szOutStr), 0);
}

char *replace_str(const char *str, const char *orig, int rep) {
    char *p = strstr(str, orig);
    if (!p) return str ? strdup(str) : strdup("");  // Always return a valid pointer

    // Ensure buffer size does not exceed predefined MAX_USERNAME_LEN
    size_t new_size = MAX_USERNAME_LEN; // Use the defined size instead of guessing
    char *buffer = calloc(new_size, sizeof(char));
    if (!buffer) return NULL;  // Handle allocation failure

    size_t prefix_len = p - str;
    snprintf(buffer, new_size, "%.*s%d%s", (int)prefix_len, str, rep, p + strlen(orig));

    return buffer;  // Must be freed by caller
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
    // Ensure minimum allocation of 1 element
    masterSz = masterSz > 0 ? masterSz : 1;
    safeSz = safeSz > 0 ? safeSz : 1;
    shitSz = shitSz > 0 ? shitSz : 1;
    desSz = desSz > 0 ? desSz : 1;

    master = calloc(masterSz, sizeof(masterList));
    safe = calloc(safeSz, sizeof(safeList));
    shit = calloc(shitSz, sizeof(shitList));
    des = calloc(desSz, sizeof(desList));

    if (!master || !safe || !shit || !des) {
        perror("Memory allocation failed for lists");
        exit(EXIT_FAILURE);
    }

    pb = calloc(numBots, sizeof(struct data));
    if (!pb) {
        perror("Memory allocation failed for pb");
        exit(EXIT_FAILURE);
    }
}

void free_config() {
    if (master) { free(master); master = NULL; }
    if (safe) { free(safe); safe = NULL; }
    if (shit) { free(shit); shit = NULL; }
    if (des) { free(des); des = NULL; }
}

void clean_exit(int status) {
    free_config();
    if (pb) {
        free(pb);
    }
    exit(status);
}

void setup_signal_handlers() {
    signal(SIGINT, clean_exit);
    signal(SIGTERM, clean_exit);
}

int try_connect(struct data* pb, struct timeval tv) {
    int s;
    while (!atomic_load(&pb->connected)) {
        s = socket(AF_INET, SOCK_STREAM, 0);
        if (Connect(s, tv, pb) == 0)
            return s;  // Successful connection
        close(s);
    }
    return -1;
}

void processList(int s, char* com, char* name, char* list, void **pArray, int *pSize, const char* type) {
    masterList *listArray = (masterList *)(*pArray);
    int x, once = 1, replace = 0;

    // Check if command is LIST
    for (x = 0; x < *pSize; x++) {
        if (!strcasecmp(CFGSTUFF_LIST, com)) {
            Send(s, CFGSTUFF_FORMATTING_X_EQUILS_X, type, listArray[x].id);
            msleep(3000);
        }
        else if (!strcasecmp(CFGSTUFF_REM, com) && name != NULL) {
            if (!strcasecmp(name, listArray[x].id)) {
                replace = 1;
            }
            if (replace) {
                if (x < *pSize - 1) {
                    memmove(&listArray[x], &listArray[x + 1], ((*pSize) - x - 1) * sizeof(masterList));
                }
                masterList *temp = realloc(listArray, (*pSize - 1) * sizeof(masterList));
                if (temp || *pSize == 1) {  // realloc can return NULL for size 0
                    *pArray = temp;
                    (*pSize)--;
                } else {
                    perror("realloc failed in processList");
                }
                break;
            }
        }
        else if (!strcasecmp(CFGSTUFF_ADD, com) && name != NULL) {
            if (!strcasecmp(listArray[x].id, name)) {
                Send(s, CFGSTUFF_FORMATTING_ADD_SECTION, name, type);
                return;
            }
        }
    }

    // Handle adding a new entry
    if (!strcasecmp(CFGSTUFF_ADD, com) && name != NULL) {
        masterList *temp = realloc(listArray, (*pSize + 1) * sizeof(masterList));
        if (temp) {
            listArray = temp;
            size_t ps = *pSize;
            memset(listArray[ps].id, 0, sizeof(listArray[ps].id));
            strncpy(listArray[ps].id, name, sizeof(listArray[ps].id) - 1);
            (*pSize)++;
            Send(s, CFGSTUFF_FORMATTING_TO_FROM_SECTION, com, name, CFGSTUFF_TO, list);
            msleep(3000);
        } else {
            perror("realloc failed to add entry");
        }
    }

    *pArray = listArray;
}

void cfgStuff(int s, struct data* pb, char* com, char* text) {
    char* pos;
    char textBuffer[FUK_CFG_MAXCOUNT];
    strncpy(textBuffer, text, sizeof(textBuffer) - 1);
    textBuffer[sizeof(textBuffer) - 1] = '\0';

    char* list = strtok_r(textBuffer, " ", &pos);
    if (!list) return;
    
    char* name = strtok_r(NULL, " ", &pos);
    if (!name) return;

    if (strstr(list, CFGSTUFF_MASTER)) {
        processList(s, com, name, list, (void**)&master, &masterSz, CFGSTUFF_MASTER);
    }
    else if (strstr(list, CFGSTUFF_SAFE)) {
        processList(s, com, name, list, (void**)&safe, &safeSz, CFGSTUFF_SAFE);
    }
    else if (strstr(list, CFGSTUFF_SHIT)) {
        processList(s, com, name, list, (void**)&shit, &shitSz, CFGSTUFF_SHIT);
    }
    else if (strstr(list, CFGSTUFF_DES)) {
        processList(s, com, name, list, (void**)&des, &desSz, CFGSTUFF_DES);
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
                msleep(3000);
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
        if (topic != NULL) {
            Send(s, SERVER_COMMAND_1, BASE_TOPIC, topic);
            msleep(3000);
        }
        return;
    }
    if (uFlags == 16 && !strcasecmp(pb->username, szSpeaker)) {
        pb->hasop = 0;
        pb->lockdown = 0;
    }
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

// The OnTalk function
void OnTalk(int s, struct data* pb, char* szSpeaker, char* szEventText)
{
    int i = 0;
    int doStuff = 0;

    // Basic sanity checks
    if (!pb || !szSpeaker || !szEventText) return;

    // Check if speaker is in master list
    for (i = 0; i < masterSz; i++) {
        if (!strcasecmp(szSpeaker, master[i].id)) {
            doStuff = 1;
            break;
        }
    }

    // If speaker not recognized, return
    if (!doStuff) return;

    // Check if the message starts with the bot's trigger or a '?' 
    if (szEventText[0] == pb->trigger[0] || szEventText[0] == '?') {
        // Create a buffer for the command string (everything after the first character)
        char tmpBuf[256] = {0};
        strncpy(tmpBuf, szEventText + 1, sizeof(tmpBuf) - 1);

        // Use strsep to separate the first token (command) from the rest (arguments)
        char* ptrDat = tmpBuf;
        char* com = strsep(&ptrDat, " ");
        if (!com) {
            return;  // no command found
        }

        // Resolve the command to an enum
        CommandID cmd = resolve_command(com);

        // Switch on the enum for clearer code
        switch (cmd)
        {
            case CMD_CFGSTUFF_LIST:
                if (ptrDat != NULL && pb->botNum == 0) {
                    cfgStuff(s, pb, com, ptrDat);
                }
                break;

            case CMD_CFGSTUFF_ADD:
            {
                if (ptrDat != NULL && pb->botNum == 0) {
                    cfgStuff(s, pb, com, ptrDat);
                    save_cfg(pb);
                }
                break;
            }

            case CMD_CFGSTUFF_REM:
            {
                if (ptrDat != NULL && pb->botNum == 0) {
                    cfgStuff(s, pb, com, ptrDat);
                    save_cfg(pb);
                }
                break;
            }

            case CMD_BASE_TRIGGER:
            {
                if (ptrDat == NULL) {
                    if (pb->botNum == 0) {
                        Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker,
                             BASE_TRIGGER, BASE_COLON, pb->trigger);
                        msleep(3000);
                    }
                } else {
                    // Set new trigger
                    if (pb->botNum == 0) {
                        memset(pb->trigger, 0, sizeof(pb->trigger));
                        strncpy(pb->trigger, ptrDat, sizeof(pb->trigger) - 1);

                        // Possibly also update global trigger
                        memset(trigger, 0, sizeof(trigger));
                        strncpy(trigger, ptrDat, sizeof(trigger) - 1);

                        save_cfg(pb);
                        Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker,
                             BASE_TRIGGER, BASE_ISNOW, pb->trigger);
                        msleep(3000);
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
                        msleep(3000);
                    }
                } else {
                    if (pb->botNum == 0) {
                        memset(pb->channel, 0, sizeof(pb->channel));
                        strncpy(pb->channel, ptrDat, sizeof(pb->channel) - 1);

                        memset(channel, 0, sizeof(channel));
                        strncpy(channel, ptrDat, sizeof(channel) - 1);

                        save_cfg(pb);
                        Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker,
                             BASE_HOME, BASE_ISNOW, pb->channel);
                        msleep(3000);
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
                        msleep(3000);
                    } else if (pb->botNum == 0) {
                        Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker,
                             BASE_TOPIC, BASE_COLON, topic);
                        msleep(3000);
                    }
                } else {
                    if (pb->hasop == 1) {
                        memset(topic, 0, sizeof(topic));
                        strncpy(topic, ptrDat, sizeof(topic) - 1);
                        Send(s, SERVER_COMMAND_1, SERVER_TOPIC, topic);
                        msleep(3000);
                    } else if (pb->botNum == 0) {
                        memset(topic, 0, sizeof(topic));
                        strncpy(topic, ptrDat, sizeof(topic) - 1);
                        save_cfg(pb);
                        Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker,
                             BASE_TOPIC, BASE_ISNOW, topic);
                        msleep(3000);
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
                        msleep(3000);
                    } else {
                        memset(backup, 0, sizeof(backup));
                        strncpy(backup, ptrDat, sizeof(backup) - 1);
                        save_cfg(pb);
                        Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker,
                             BASE_BACKUP, BASE_ISNOW, backup);
                        msleep(3000);
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
                        msleep(3000);
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
                        msleep(3000);
                    } else {
                        threads = atoi(ptrDat);
                        save_cfg(pb);
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker,
                             BASE_THREADS, BASE_ISNOW, pb->threads);
                        msleep(3000);
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
                        msleep(3000);
                    } else {
                        // Update both global and pb->port if that is your intention
                        port = atoi(ptrDat);
                        pb->port = port;  // so it stays in sync
                        save_cfg(pb);
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker,
                             BASE_PORT, BASE_ISNOW, port);
                        msleep(3000);
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
                        msleep(3000);
                    } else {
                        delay = atoi(ptrDat);
                        save_cfg(pb);
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker,
                             BASE_DELAY, BASE_ISNOW, delay);
                        msleep(3000);
                    }
                }
                break;
            }

            case CMD_BASE_DELAY_2:
            {
                if (ptrDat == NULL) {
                    Send(s, ON_COMMAND_REPLY_1, SERVER_WHISPER, szSpeaker,
                         BASE_DELAY, BASE_SCATTER, pb->delay2);
                    msleep(3000);
                }
                break;
            }

            case CMD_BASE_SCATTER:
            {
                if (pb->botNum == 0) {
                    if (ptrDat == NULL) {
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker,
                             BASE_SCATTER, BASE_COLON, scatter);
                        msleep(3000);
                    } else {
                        scatter = atoi(ptrDat);
                        save_cfg(pb);
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker,
                             BASE_SCATTER, BASE_ISNOW, scatter);
                        msleep(3000);
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
                        msleep(3000);
                    } else {
                        banWait = atoi(ptrDat) * 1000;
                        save_cfg(pb);
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker,
                             BASE_BANWAIT, BASE_ISNOW, (banWait / 1000));
                        msleep(3000);
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
                        msleep(3000);
                    } else {
                        conWait = atoi(ptrDat);
                        save_cfg(pb);
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker,
                             BASE_CONWAIT, BASE_ISNOW, conWait);
                        msleep(3000);
                    }
                }
                break;
            }

            case CMD_BASE_SAY:
            {
                if (ptrDat != NULL) {
                    Send(s, ON_COMMAND_REPLY_2, ptrDat);
                    msleep(3000);
                }
                break;
            }

            case CMD_BASE_VER:
            {
                if (pb->botNum == 0) {
                    Send(s, SERVER_COMMAND_2, SERVER_WHISPER, szSpeaker, FUK_VERSION);
                    msleep(3000);
                }
                break;
            }

            case CMD_BASE_QUIT:
            {
                exit(0);
                break;  // unreachable, but keep for clarity
            }

            case CMD_BASE_RECON:
            {
                close(s);
                break;
            }

            case CMD_BASE_PLACE:
            {
                // Original code snippet
                Send(s, "/%s %s %d to login to %s with actual delay of %d.\r\n",
                     SERVER_WHISPER, szSpeaker, pb->place, pb->server, pb->delay2);
                msleep(3000);
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
                    msleep(3000);
                }
                break;
            }

            case CMD_SERVER_BAN:
            {
                if (ptrDat != NULL && pb->hasop == 1) {
                    Send(s, SERVER_COMMAND_1, SERVER_BAN, ptrDat);
                    msleep(3000);
                }
                break;
            }

            case CMD_SERVER_UNBAN:
            {
                if (ptrDat != NULL && pb->hasop == 1) {
                    pb->lockdown = 0;
                    Send(s, SERVER_COMMAND_1, SERVER_UNBAN, ptrDat);
                    msleep(3000);
                }
                break;
            }

            case CMD_SERVER_KICK:
            {
                if (ptrDat != NULL && pb->hasop == 1) {
                    Send(s, SERVER_COMMAND_1, SERVER_KICK, ptrDat);
                    msleep(3000);
                }
                break;
            }

            case CMD_SERVER_JOIN:
            {
                if (ptrDat != NULL) {
                    Send(s, SERVER_COMMAND_1, SERVER_JOIN, ptrDat);
                    msleep(3000);
                }
                break;
            }

            case CMD_SERVER_REJOIN:
            {
                Send(s, SERVER_COMMAND_0, SERVER_REJOIN);
                msleep(3000);
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
                    msleep(3000);
                }
                break;
            }

            case CMD_SERVER_RESIGN:
            {
                if (pb->hasop == 1) {
                    if (pb->des != 0) {
                        pb->des = 0;
                        Send(s, SERVER_COMMAND_0, SERVER_RESIGN);
                        msleep(3000);
                    } else {
                        Send(s, ON_COMMAND_REPLY_2, BASE_NO_HEIR_TEXT);
                        msleep(3000);
                    }
                }
                break;
            }

            case CMD_SERVER_UPTIME:
            {
                if (pb->botNum == 0) {
                    Send(s, SERVER_COMMAND_0, SERVER_UPTIME);
                    msleep(3000);
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
                    msleep(3000);
                }
                break;
            }

            case CMD_BASE_LOCK:
            {
                if (pb->hasop == 1) {
                    pb->lockdown = 1;
                    Send(s, SERVER_COMMAND_2, SERVER_TOPIC, topic, BASE_CHANLOCKED);
                    msleep(3000);
                    Send(s, SERVER_COMMAND_1, SERVER_WHO, pb->currChan);
                    msleep(3000);
                }
                break;
            }

            case CMD_BASE_UNLOCK:
            {
                pb->lockdown = 0;
                if (pb->hasop == 1) {
                    Send(s, SERVER_COMMAND_1, SERVER_TOPIC, topic);
                    msleep(3000);
                }
                break;
            }

            case CMD_BASE_CLEAN:
            {
                if (pb->hasop == 1) {
                    Send(s, SERVER_COMMAND_1, SERVER_WHO, pb->currChan);
                    msleep(3000);
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
                        msleep(3000);
                    } else {
                        pb->tban = 0;
                        Send(s, ON_COMMAND_REPLY_2, BASE_TAGBAN_DISABLED);
                        msleep(3000);
                    }
                }
                break;
            }

            case CMD_BASE_CONTIME:
            {
                if (pb->botNum == 0) {
                    Send(s, ON_COMMAND_REPLY_3, SERVER_WHISPER, szSpeaker, pb->conTime);
                    msleep(3000);
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
                    msleep(3000);
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
        msleep(3000);
    }
    else if (!strcasecmp(szEventText, CHANNEL_VOID)) {
        pb->chanham = 1;
        Send(s, SERVER_COMMAND_0, SERVER_PLACE, NULL);
        msleep(3000);

        // Ensure 'backup' is properly initialized before use
        extern char backup[MAX_CHANNEL_LEN];  // Assumption: backup is declared elsewhere
        if (backup[0] != '\0') {  // Ensure backup is not empty before using it
            Send(s, SERVER_COMMAND_1, SERVER_JOIN, backup);
            msleep(3000);
        }
    }
}

void OnInfo(int s, struct data *pb, char *szEventText) {
    if (!szEventText) return;
    char szEventTextCopy[512];  // Allocate fixed buffer
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
            msleep(3000);
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
            msleep(3000);
        }
        return;
    }
    else if (strstr(szEventTextCopy, BASE_YOUWEREKICKED)) {
        pb->chanham = 1;
        Send(s, SERVER_COMMAND_1, SERVER_JOIN, backup);
        msleep(3000);
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

void OnError(int s, struct data *pb, char *szEventText) {
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
    
    msleep(3000);  // Ensure `pb` remains valid if this runs in a multi-threaded environment
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
                OnError(s, pb, SERVER_MSG);
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
                return; // Handle disconnection or error
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
}

int save_cfg(struct data* pb) {
    FILE* cfg;
    int i;
    cfg = fopen(FUK_CFG, FUK_CFG_WRITE);
    if (!cfg) {
        printf(FUK_CFG_ERROR_O, FUK_CFG);
        return -1;
    }

    fprintf(cfg, FUK_CFG_PRINTFS, CFGSTUFF_USERNAME, username);
    fprintf(cfg, FUK_CFG_PRINTFS, CFGSTUFF_PASSWORD, password);
    fprintf(cfg, FUK_CFG_PRINTFS, CFGSTUFF_HOME, channel);
    fprintf(cfg, FUK_CFG_PRINTFS, CFGSTUFF_SERVER, server);
    fprintf(cfg, FUK_CFG_PRINTFI, CFGSTUFF_PORT, port);
    fprintf(cfg, FUK_CFG_PRINTFS, CFGSTUFF_BIND, bindaddr);
    fprintf(cfg, FUK_CFG_PRINTFS, CFGSTUFF_TRIGGER, trigger);
    fprintf(cfg, FUK_CFG_PRINTFI, CFGSTUFF_THREADS, threads);
    fprintf(cfg, FUK_CFG_PRINTFI, CFGSTUFF_DELAY, delay);
    fprintf(cfg, FUK_CFG_PRINTFI, CFGSTUFF_SCATTER, scatter);
    fprintf(cfg, FUK_CFG_PRINTFI, CFGSTUFF_CONWAIT, conWait);
    fprintf(cfg, FUK_CFG_PRINTFI, CFGSTUFF_BANWAIT, banWait);
    fprintf(cfg, FUK_CFG_PRINTFI, CFGSTUFF_NUMBOTS, numBots);
    fprintf(cfg, FUK_CFG_PRINTFS, CFGSTUFF_BACKUPCHAN, backup);
    fprintf(cfg, FUK_CFG_PRINTFS, CFGSTUFF_TOPIC, topic);
    fprintf(cfg, "\n");
    for (i = 0; i < masterSz; i++) {
        fprintf(cfg, FUK_CFG_PRINTFS, CFGSTUFF_MASTER, master[i].id);
    }
    fprintf(cfg, "\n");
    for (i = 0; i < safeSz; i++) {
        fprintf(cfg, FUK_CFG_PRINTFS, CFGSTUFF_SAFE, safe[i].id);
    }
    fprintf(cfg, "\n");
    for (i = 0; i < desSz; i++) {
        fprintf(cfg, FUK_CFG_PRINTFS, CFGSTUFF_DES, des[i].id);
    }
    fprintf(cfg, "\n");
    for (i = 0; i < shitSz; i++) {
        fprintf(cfg, FUK_CFG_PRINTFS, CFGSTUFF_SHIT, shit[i].id);
    }
    fclose(cfg);

    return 1; /* apparently i havent fixed this yet */
}

int read_config() {
    FILE *cfg = fopen(FUK_CFG, FUK_CFG_READBINARY);
    if (!cfg) {
        printf(FUK_CFG_ERROR, FUK_CFG);
        return -1;
    }

    char r[FUK_CFG_MAXCOUNT];
    size_t i = 0, j = 0, k = 0, d = 0;

    while (fgets(r, FUK_CFG_MAXCOUNT, cfg)) {
        strtok(r, "\r\n"); // Remove newlines

        if (!memcmp(r, FUK_CFG_USERNAME, strlen(FUK_CFG_USERNAME))) {
            strncpy(username, r + strlen(FUK_CFG_USERNAME), sizeof(username) - 1);
            username[sizeof(username) - 1] = '\0';
        }
        else if (!memcmp(r, FUK_CFG_PASSWORD, strlen(FUK_CFG_PASSWORD))) {
            strncpy(password, r + strlen(FUK_CFG_PASSWORD), sizeof(password) - 1);
            password[sizeof(password) - 1] = '\0';
        }
        else if (!memcmp(r, FUK_CFG_HOME, strlen(FUK_CFG_HOME))) {
            strncpy(channel, r + strlen(FUK_CFG_HOME), sizeof(channel) - 1);
            channel[sizeof(channel) - 1] = '\0';
        }
        else if (!memcmp(r, FUK_CFG_SERVER, strlen(FUK_CFG_SERVER))) {
            strncpy(server, r + strlen(FUK_CFG_SERVER), sizeof(server) - 1);
            server[sizeof(server) - 1] = '\0';
        }
        else if (!memcmp(r, FUK_CFG_PORT, strlen(FUK_CFG_PORT))) {
            port = atoi(r + strlen(FUK_CFG_PORT));
        }
        else if (!memcmp(r, FUK_CFG_BIND, strlen(FUK_CFG_BIND))) {
            strncpy(bindaddr, r + strlen(FUK_CFG_BIND), sizeof(bindaddr) - 1);
            bindaddr[sizeof(bindaddr) - 1] = '\0';
        }
        else if (!memcmp(r, FUK_CFG_TRIGGER, strlen(FUK_CFG_TRIGGER))) {
            strncpy(trigger, r + strlen(FUK_CFG_TRIGGER), sizeof(trigger) - 1);
            trigger[sizeof(trigger) - 1] = '\0';
        }
        else if (!memcmp(r, FUK_CFG_THREADS, strlen(FUK_CFG_THREADS))) {
            threads = atoi(r + strlen(FUK_CFG_THREADS));
        }
        else if (!memcmp(r, FUK_CFG_DELAY, strlen(FUK_CFG_DELAY))) {
            delay = atoi(r + strlen(FUK_CFG_DELAY));
        }
        else if (!memcmp(r, FUK_CFG_SCATTER, strlen(FUK_CFG_SCATTER))) {
            scatter = atoi(r + strlen(FUK_CFG_SCATTER));
        }
        else if (!memcmp(r, FUK_CFG_BACKUPCHAN, strlen(FUK_CFG_BACKUPCHAN))) {
            strncpy(backup, r + strlen(FUK_CFG_BACKUPCHAN), sizeof(backup) - 1);
            backup[sizeof(backup) - 1] = '\0';
        }
        else if (!memcmp(r, FUK_CFG_TOPIC, strlen(FUK_CFG_TOPIC))) {
            strncpy(topic, r + strlen(FUK_CFG_TOPIC), sizeof(topic) - 1);
            topic[sizeof(topic) - 1] = '\0';
        }
        else if (!memcmp(r, FUK_CFG_BANWAIT, strlen(FUK_CFG_BANWAIT))) {
            banWait = atoi(r + strlen(FUK_CFG_BANWAIT));
        }
        else if (!memcmp(r, FUK_CFG_CONWAIT, strlen(FUK_CFG_CONWAIT))) {
            conWait = atoi(r + strlen(FUK_CFG_CONWAIT));
        }
        else if (!memcmp(r, FUK_CFG_NUMBOTS, strlen(FUK_CFG_NUMBOTS))) {
            numBots = atoi(r + strlen(FUK_CFG_NUMBOTS));
        }
        else if (!memcmp(r, FUK_CFG_MASTER, strlen(FUK_CFG_MASTER))) {
            masterList *temp = (masterList *)realloc(master, (masterSz + 1) * sizeof(masterList));
            if (!temp) break; // Memory allocation failed
            master = temp;
            strncpy(master[masterSz].id, r + strlen(FUK_CFG_MASTER), sizeof(master[masterSz].id) - 1);
            master[masterSz].id[sizeof(master[masterSz].id) - 1] = '\0';
            masterSz++;
        }
        else if (!memcmp(r, FUK_CFG_SAFE, strlen(FUK_CFG_SAFE))) {
            safeList *temp = (safeList *)realloc(safe, (safeSz + 1) * sizeof(safeList));
            if (!temp) break;
            safe = temp;
            strncpy(safe[safeSz].id, r + strlen(FUK_CFG_SAFE), sizeof(safe[safeSz].id) - 1);
            safe[safeSz].id[sizeof(safe[safeSz].id) - 1] = '\0';
            safeSz++;
        }
        else if (!memcmp(r, FUK_CFG_SHIT, strlen(FUK_CFG_SHIT))) {
            shitList *temp = (shitList *)realloc(shit, (shitSz + 1) * sizeof(shitList));
            if (!temp) break;
            shit = temp;
            strncpy(shit[shitSz].id, r + strlen(FUK_CFG_SHIT), sizeof(shit[shitSz].id) - 1);
            shit[shitSz].id[sizeof(shit[shitSz].id) - 1] = '\0';
            shitSz++;
        }
        else if (!memcmp(r, FUK_CFG_DES, strlen(FUK_CFG_DES))) {
            desList *temp = (desList *)realloc(des, (desSz + 1) * sizeof(desList));
            if (!temp) break;
            des = temp;
            strncpy(des[desSz].id, r + strlen(FUK_CFG_DES), sizeof(des[desSz].id) - 1);
            des[desSz].id[sizeof(des[desSz].id) - 1] = '\0';
            desSz++;
        }
    }

    fclose(cfg);
    return 0;
}
      
int Connect(int s, struct timeval tv, struct data* pb) {
    struct sockaddr_in bind_addr = { .sin_family = AF_INET };
    struct sockaddr_in server_addr;
    struct addrinfo hints = {0}, *res = NULL;
    fd_set fdr, fdw;
    int err = 0, errlen = sizeof(err);

    // Resolve server address
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(pb->server, NULL, &hints, &res) != 0) {
        return -1;  // Address resolution failed
    }

    // Prepare server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(pb->port);
    server_addr.sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;

    // Bind socket (optional, only if `bindaddr` is set)
    if (bindaddr && inet_pton(AF_INET, bindaddr, &bind_addr.sin_addr) > 0) {
        if (bind(s, (struct sockaddr*)&bind_addr, sizeof(bind_addr)) < 0) {
            if (res) freeaddrinfo(res); // Ensure res is valid before freeing
            return -2;  // Binding failed
        }
    }

    set_nonblock(s);

    // Start non-blocking connection
    connect(s, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // Use select to wait for connection
    FD_ZERO(&fdr);
    FD_ZERO(&fdw);
    FD_SET(s, &fdr);
    FD_SET(s, &fdw);

    if (select(s + 1, &fdr, &fdw, NULL, &tv) <= 0) {
        if (res) freeaddrinfo(res);
        return -3;  // Timeout or error
    }

    // Check if connection was successful
    if (FD_ISSET(s, &fdw)) {
        getsockopt(s, SOL_SOCKET, SO_ERROR, &err, &errlen);
        if (err != 0) {
            if (res) freeaddrinfo(res);
            return -4;  // Connection failed
        }
    } else {
        if (res) freeaddrinfo(res);
        return -5;  // Unexpected failure
    }

    // Free memory only once before returning successfully
    if (res) freeaddrinfo(res);
    return 0;  // Successfully connected
}

void* thread_conn(void* arg) {
    struct data* pb = (struct data*)arg;
    struct timeval tv = { .tv_sec = 0 };

    startTime = time(NULL);
    pb->delay2 = (scatter > 0) ? (rand() % scatter + delay) : delay;
    tv.tv_usec = pb->delay2 * 1000;

    int s = try_connect(pb, tv);
    if (s == -1) pthread_exit(NULL);

    // Ensure only one thread establishes the connection
    if (!atomic_compare_exchange_strong(&pb->connected, &(int){0}, 1)) {
        close(s);
        pthread_exit(NULL);
    }

    // First successful thread does the work
    Send(s, pb->logonPacket, sizeof(pb->logonPacket), 0);
    pb->conTime = time(NULL) - startTime;
    
    message_loop(s, pb);
    
    // Cleanup after disconnection
    close(s);
    atomic_store(&pb->connected, 0);
    pthread_exit(NULL);
}

void create_threads(struct data* pb) {
    int err;
    int numThreads = numBots * threads;

    pthread_t *thread = malloc(numThreads * sizeof(pthread_t));
    if (!thread) {
        perror("malloc failed for thread array");
        return;
    }
    struct data *p = pb;
    for (int t = 0; t < numBots; t++, pb_ptr++)
        char *replaced = replace_str(username, "#", t);
        if (!replaced) {
            perror("Memory allocation failed in replace_str");
            free(thread);
            return;
        }

        // Ensure `username` is correctly bounded
        size_t len = strnlen(replaced, MAX_USERNAME_LEN - 1);
        strncpy(pb->username, replaced, len);
        pb->username[len] = '\0';

        free(replaced); // Free dynamically allocated memory

        // Copy other fields safely
        strncpy(pb->password, password, MAX_PASSWORD_LEN - 1);
        pb->password[MAX_PASSWORD_LEN - 1] = '\0';

        strncpy(pb->channel, channel, MAX_CHANNEL_LEN - 1);
        pb->channel[MAX_CHANNEL_LEN - 1] = '\0';

        strncpy(pb->server, server, MAX_SERVER_LEN - 1);
        pb->server[MAX_SERVER_LEN - 1] = '\0';

        strncpy(pb->trigger, trigger, MAX_TRIGGER_LEN - 1);
        pb->trigger[MAX_TRIGGER_LEN - 1] = '\0';

        pb->botNum = t;
        pb->port = port;
        pb->threads = threads;

        // Ensure `logonPacket` stays within `MAX_LOGON_PACKET_LEN`
        snprintf(pb->logonPacket, MAX_LOGON_PACKET_LEN - 1,
                 "C1\r\nACCT %s\r\nPASS %s\r\nHOME %s\r\nLOGIN\r\n",
                 pb->username, pb->password, pb->channel);
        pb->logonPacket[MAX_LOGON_PACKET_LEN - 1] = '\0';
    }

    // Start threads safely
    for (int t = 0; t < numThreads; t++) {
        err = pthread_create(&thread[t], NULL, thread_conn, pb);
        if (err != 0) {
            fprintf(stderr, "pthread_create failed: %s\n", strerror(err));
        }
    }

    for (int t = 0; t < numThreads; t++) {
        pthread_join(thread[t], NULL);
    }

    free(thread);  // Free allocated thread memory
}

int main() {
    srand((unsigned)time(NULL));  // Initialize srand with explicit unsigned cast
    setup_signal_handlers();
    printf("%s\n", FUK_VERSION);
    printf("PID: %d\n", getpid());

    if ((main_pid = fork()) == -1) {
        perror("shutting down: unable to fork");
        return EXIT_FAILURE;
    }

    if (main_pid != 0) {
        return EXIT_SUCCESS;  // Parent process exits cleanly
    }

    allocate_lists();
    
    if (read_config() != 0) {
        perror("Read config error");
        clean_exit(EXIT_FAILURE);
    }

    // Main event loop
    while (1) {
        startTime = time(NULL);
        create_threads(pb);
        sleep(1);  // Prevent high CPU usage
    }

    clean_exit(EXIT_SUCCESS);
}
