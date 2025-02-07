#include "fuk3.h"

#if defined(WINDOWS_CPP_BUILD)
    char* mystrsep(char** stringp, const char* delim)
    {
        char* start = *stringp;
        char* p;

        p = (start != NULL) ? strpbrk(start, delim) : NULL;

        if (p == NULL)
        {
            *stringp = NULL;
        }
        else
        {
            *p = '\0';
            *stringp = p + 1;
        }

        return start;
    }
#endif

#if defined(WINDOWS_CPP_BUILD)
    void* ReAllocArray(void* ptr, size_t nmemb, size_t size) {
        const size_t zerobase = 1;
        size_t xsize = ((nmemb + zerobase) * size);
        void* newpointer;
        if ((newpointer = realloc(ptr, xsize)) == NULL) { /* i havent looked up realloc in some time but this should be the equiv */
            free(ptr);
            ptr = NULL;
            size = 0;
            return (NULL);
        }
        return newpointer;
    }

    int __cdecl getrusage(int who, struct rusage *r_usageval)
    {
        PROCESS_MEMORY_COUNTERS pmc;
        FILETIME        starttime;
        FILETIME        exittime;
        FILETIME        KernelTime;
        FILETIME        UserTime;
        ULARGE_INTEGER  li;

        if (!&r_usageval)
        {
            errno = EFAULT;
            return -1;
        }

        if (who != RUSAGE_SELF)
        {
            errno = EINVAL;
            return -1;
        }

        memset(r_usageval, 0, sizeof(struct rusage));
        if (GetProcessTimes(GetCurrentProcess(),
            &starttime, &exittime, &KernelTime, &UserTime) == 0)
        {
            return -1;
        }

        if (!GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
            return -1;
        }

        /* Convert FILETIMEs (0.1 us) to struct timeval */
        memcpy(&li, &KernelTime, sizeof(FILETIME));
        li.QuadPart /= 10L;         /* Convert to microseconds */
        r_usageval->ru_stime.tv_sec = li.QuadPart / 10000000L;
        r_usageval->ru_stime.tv_usec = li.QuadPart % 10000000L;

        memcpy(&li, &UserTime, sizeof(FILETIME));
        li.QuadPart /= 10L;         /* Convert to microseconds */
        r_usageval->ru_utime.tv_sec = li.QuadPart / 10000000L;
        r_usageval->ru_utime.tv_usec = li.QuadPart % 10000000L;

        r_usageval->ru_majflt = pmc.PageFaultCount;
        r_usageval->ru_maxrss = pmc.PeakWorkingSetSize / 1024;

        return 0;
    }
#endif

#if !defined(WINDOWS_CPP_BUILD) /* windows build running ioctl equivilant */
    void set_nonblock(int fd) {
        int old_option = fcntl(fd, F_GETFL);
        int new_option = old_option | O_NONBLOCK;
        fcntl(fd, F_SETFL, new_option);
    }
#endif

void cfgStuff(int s, struct data* pb, char* com, char* text) {
    int x;
    int once = 1;
    char* pos;
    int replace = 0;
    char* list = strtok_r(text, " ", &pos);
    char* name = strtok_r(NULL, " ", &pos);
    if (strstr(list, CFGSTUFF_MASTER)) {
        for (x = 0; x < masterSz; x++) {
            if (!strcasecmp(CFGSTUFF_LIST, com)) {
                Send(s, CFGSTUFF_FORMATTING_X_EQUILS_X, CFGSTUFF_MASTER, master[x].id);
                msleep(3000);
            }
            else if (!strcasecmp(CFGSTUFF_REM, com) && name != NULL) {
                if (!strcasecmp(name, master[x].id)) {
                    replace = 1;
                }
                if (replace == 1) {
                    memset(master[x].id, '\0', sizeof(master[x].id));
                    if (x < masterSz - 1) {
                        strcpy(master[x].id, master[x + 1].id);
                    } else { /* i know you can skip the brackets, however it makes it less readable */
                        master = (masterList*)reallocarray(master, --masterSz, sizeof(masterList));
                    }
                    if (once == 1) {
                        Send(s, CFGSTUFF_FORMATTING_TO_FROM_SECTION, com, name, CFGSTUFF_FROM, list);
                        once = 0;
                        msleep(3000);
                    }
                }
            }
            else if (!strcasecmp(CFGSTUFF_ADD, com) && name != NULL) {
                if (!strcasecmp(master[x].id, name)) {
                    Send(s, CFGSTUFF_FORMATTING_ADD_SECTION, name, CFGSTUFF_MASTER);
                    return;
                }
            }
        }
        if (!strcasecmp(CFGSTUFF_ADD, com) && name != NULL) {
            master = (masterList*)reallocarray(master, ++masterSz, sizeof(masterList));
            int ps = masterSz - 1;
            memset(master[ps].id, '\0', sizeof(master[ps].id));
            strcpy(master[ps].id, name);
            Send(s, CFGSTUFF_FORMATTING_TO_FROM_SECTION, com, name, CFGSTUFF_TO, list);
            msleep(3000);
        } return;
    }
    else if (strstr(list, CFGSTUFF_SAFE)) {
        for (x = 0; x < safeSz; x++) {
            if (!strcasecmp(CFGSTUFF_LIST, com)) {
                Send(s, CFGSTUFF_FORMATTING_X_EQUILS_X, CFGSTUFF_SAFE, safe[x].id);
                msleep(3000);
            }
            else if (!strcasecmp(CFGSTUFF_REM, com) && name != NULL) {
                if (!strcasecmp(name, safe[x].id))
                    replace = 1;
                if (replace == 1) {
                    memset(safe[x].id, '\0', sizeof(safe[x].id));
                    if (x < safeSz - 1)
                        strcpy(safe[x].id, safe[x + 1].id);
                    else
                        safe = (safeList*)reallocarray(safe, --safeSz, sizeof(safeList));
                    if (once == 1) {
                        Send(s, CFGSTUFF_FORMATTING_TO_FROM_SECTION, com, name, CFGSTUFF_FROM, list);
                        once = 0;
                        msleep(3000);
                    }
                }
            }
            else if (!strcasecmp(CFGSTUFF_ADD, com) && name != NULL) {
                if (!strcasecmp(safe[x].id, name)) {
                    Send(s, CFGSTUFF_FORMATTING_ADD_SECTION, name, CFGSTUFF_SAFE);
                    return;
                }
            }
        }
        if (!strcasecmp(CFGSTUFF_ADD, com) && name != NULL) {
            safe = (safeList*)reallocarray(safe, ++safeSz, sizeof(safeList));
            int ps = safeSz - 1;
            memset(safe[ps].id, '\0', sizeof(safe[ps].id));
            strcpy(safe[ps].id, name);
            Send(s, CFGSTUFF_FORMATTING_TO_FROM_SECTION, com, name, CFGSTUFF_TO,list);
            msleep(3000);
        } return;
    }
    else if (strstr(list, CFGSTUFF_SHIT)) {
        for (x = 0; x < shitSz; x++) {
            if (!strcasecmp(CFGSTUFF_LIST, com)) {
                Send(s, CFGSTUFF_FORMATTING_X_EQUILS_X, CFGSTUFF_SHIT, shit[x].id);
                msleep(3000); 
            }
            else if (!strcasecmp(CFGSTUFF_REM, com) && name != NULL) {
                if (!strcasecmp(name, shit[x].id))
                    replace = 1;
                if (replace == 1) {
                    memset(shit[x].id, '\0', sizeof(shit[x].id));
                    if (x < shitSz - 1)
                        strcpy(shit[x].id, shit[x + 1].id);
                    else
                        shit = (shitList*)reallocarray(shit, --shitSz, sizeof(shitList));
                    if (once == 1) {
                        Send(s, CFGSTUFF_FORMATTING_TO_FROM_SECTION, com, name, CFGSTUFF_FROM, list);
                        once = 0;
                        msleep(3000);
                    }
                }
            }
            else if (!strcasecmp(CFGSTUFF_ADD, com) && name != NULL) {
                if (!strcasecmp(shit[x].id, name)) {
                    Send(s, CFGSTUFF_FORMATTING_ADD_SECTION, name, CFGSTUFF_SHIT);
                    return;
                }
            }
        }
        if (!strcasecmp(CFGSTUFF_ADD, com) && name != NULL) {
            shit = (shitList*)reallocarray(shit, ++shitSz, sizeof(shitList));
            int ps = shitSz - 1;
            memset(shit[ps].id, '\0', sizeof(shit[ps].id));
            strcpy(shit[ps].id, name);
            Send(s, CFGSTUFF_FORMATTING_TO_FROM_SECTION, com, name, CFGSTUFF_TO, list);
            msleep(3000);
        } return;
    }
    else if (strstr(list, CFGSTUFF_DES)) {
        for (x = 0; x < desSz; x++) {
            if (!strcasecmp(CFGSTUFF_LIST, com)) {
                Send(s, CFGSTUFF_FORMATTING_X_EQUILS_X, CFGSTUFF_DES, des[x].id);
                msleep(3000);
            }
            else if (!strcasecmp(CFGSTUFF_REM, com) && name != NULL) {
                if (!strcasecmp(name, des[x].id))
                    replace = 1;
                if (replace == 1) {
                    memset(des[x].id, '\0', sizeof(des[x].id));
                    if (x < desSz - 1)
                        strcpy(des[x].id, des[x + 1].id);
                    else
                        des = (desList*)reallocarray(des, --desSz, sizeof(desList));
                    if (once == 1) {
                        Send(s, CFGSTUFF_FORMATTING_TO_FROM_SECTION, com, name, CFGSTUFF_FROM, list);
                        once = 0;
                        msleep(3000);
                    }
                }
            }
            else if (!strcasecmp(CFGSTUFF_ADD, com) && name != NULL) {
                if (!strcasecmp(des[x].id, name)) {
                    Send(s, CFGSTUFF_FORMATTING_ADD_SECTION, name, CFGSTUFF_DES);
                    return;
                }
            }
        }
        if (!strcasecmp(CFGSTUFF_ADD, com) && name != NULL) {
            des = (desList*)reallocarray(des, ++desSz, sizeof(desList));
            int ps = desSz - 1;
            memset(des[ps].id, '\0', sizeof(des[ps].id));
            strcpy(des[ps].id, name);
            Send(s, CFGSTUFF_FORMATTING_TO_FROM_SECTION, com, name, CFGSTUFF_TO, list);
            msleep(3000);
        }
        return;
    }
    else {
        Send(s, CFGSTUFF_FORMATTING_OPTIONS, CFGSTUFF_LIST, CFGSTUFF_ADD, CFGSTUFF_REM, CFGSTUFF_DES, CFGSTUFF_SAFE, CFGSTUFF_SHIT, CFGSTUFF_MASTER);
        msleep(3000);
        return;
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
} /* dont need to return at the end of a void function */

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

void OnTalk(int s, struct data* pb, char* szSpeaker, char* szEventText) {
    int i = 0;
    int doStuff = 0;
    char* com;
    char pi[512];
    FILE* fp1;
    char pingStr[250];
    for (i = 0; i < masterSz; i++) {
        if (!strcasecmp(szSpeaker, master[i].id))
            doStuff = 1;
    }
    if (doStuff != 1)
        return;
    else {
        if (szEventText[0] == pb->trigger[0] || szEventText[0] == "?"[0]) {
            /*
                removed the strdup
            */
            char tmpBuf[256] = { 0 };
            memcpy(tmpBuf, szEventText + 1, strlen(szEventText + 1));
            char* ptrDat = tmpBuf;
            com = strsep(&ptrDat, " ");

            if (com != NULL) {
                if (!strcasecmp(CFGSTUFF_LIST, com) && ptrDat != NULL) {
                    if (pb->botNum == 0)
                        cfgStuff(s, pb, com, ptrDat);
                    return;
                }
                else if (!strcasecmp(CFGSTUFF_ADD, com) && ptrDat != NULL) {
                    if (pb->botNum == 0) {
                        cfgStuff(s, pb, com, ptrDat);
                        save_cfg(pb);
                    } return;
                }
                else if (!strcasecmp(CFGSTUFF_REM, com) && ptrDat != NULL) {
                    if (pb->botNum == 0) {
                        cfgStuff(s, pb, com, ptrDat);
                        save_cfg(pb);
                    } return;
                }
                else if (!strcasecmp(BASE_TRIGGER, com)) {
                    if (ptrDat == NULL) {
                        if (pb->botNum == 0) {
                            Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker, BASE_TRIGGER, BASE_COLON, pb->trigger);
                            msleep(3000);
                        }
                        else
                            return;
                    }
                    else if (pb->botNum == 0) {
                        memset(pb->trigger, '\0', sizeof(pb->trigger));
                        strcpy(pb->trigger, ptrDat);
                        memset(trigger, '\0', sizeof(trigger));
                        strcpy(trigger, ptrDat);
                        save_cfg(pb);
                        Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker, BASE_TRIGGER, BASE_ISNOW, pb->trigger);
                        msleep(3000);
                    }
                    else {
                        memset(pb->trigger, '\0', sizeof(pb->trigger));
                        strcpy(pb->trigger, ptrDat);
                    } return;
                }
                else if (!strcasecmp(BASE_HOME, com)) {
                    if (ptrDat == NULL) {
                        if (pb->botNum == 0) {
                            Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker, BASE_HOME, BASE_COLON, pb->channel);
                            msleep(3000);
                        }
                        else
                            return;
                    }
                    else if (pb->botNum == 0) {
                        memset(pb->channel, '\0', sizeof(pb->channel));
                        strcpy(pb->channel, ptrDat);
                        memset(channel, '\0', sizeof(channel));
                        strcpy(channel, ptrDat);
                        save_cfg(pb);
                        Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker, BASE_HOME, BASE_ISNOW, pb->channel);
                        msleep(3000);
                    }
                    else {
                        memset(pb->channel, '\0', sizeof(pb->channel));
                        strcpy(pb->channel, ptrDat);
                    } return;
                }
                else if (!strcasecmp(BASE_TOPIC, com)) {
                    if (ptrDat == NULL) {
                        if (pb->hasop == 1) {
                            Send(s, SERVER_COMMAND_1, SERVER_TOPIC, topic);
                            msleep(3000);
                        }
                        else if (pb->botNum == 0) {
                            Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker, BASE_TOPIC, BASE_COLON, topic);
                            msleep(3000);
                        }
                    }
                    else {
                        if (pb->hasop == 1) {
                            memset(topic, '\0', sizeof(topic));
                            strcpy(topic, ptrDat);
                            Send(s, SERVER_COMMAND_1, SERVER_TOPIC, topic);
                            msleep(3000);
                        }
                        else if (pb->botNum == 0) {
                            memset(topic, '\0', sizeof(topic));
                            strcpy(topic, ptrDat);
                            save_cfg(pb);
                            Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker, BASE_TOPIC, BASE_ISNOW, topic);
                            msleep(3000);
                        }
                    } return;
                }
                else if (!strcasecmp(BASE_BACKUP, com) && pb->botNum == 0) {
                    if (ptrDat == NULL) {
                        Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker, BASE_BACKUP, BASE_COLON, backup);
                        msleep(3000);
                    }
                    else {
                        memset(backup, '\0', sizeof(backup));
                        strcpy(backup, ptrDat);
                        save_cfg(pb);
                        Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker, BASE_BACKUP, BASE_ISNOW, backup);
                        msleep(3000);
                    } return;
                }
                else if (!strcasecmp(BASE_SERVER, com) && pb->botNum == 0) {
                    if (ptrDat == NULL) {
                        Send(s, ON_COMMAND_REPLY_0S, SERVER_WHISPER, szSpeaker, BASE_SERVER, BASE_COLON, pb->server);
                        msleep(3000);
                    } return;
                }
                else if (!strcasecmp(BASE_THREADS, com) && pb->botNum == 0) {
                    if (ptrDat == NULL) {
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker, BASE_THREADS, BASE_COLON, pb->threads);
                        msleep(3000);
                    }
                    else {
                        threads = atoi(ptrDat);
                        save_cfg(pb);
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker, BASE_THREADS, BASE_ISNOW, pb->threads);
                        msleep(3000);
                    } return;
                }
                else if (!strcasecmp(BASE_PORT, com) && pb->botNum == 0) { /* arent you forgetting to update pb->port at the save? lol */
                    if (ptrDat == NULL) {
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker, BASE_PORT, BASE_COLON, pb->port);
                        msleep(3000);
                    }
                    else {
                        port = atoi(ptrDat); // <----- pb->port
                        save_cfg(pb);
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker, BASE_PORT, BASE_ISNOW, port);
                        msleep(3000);
                    } return;
                }
                else if (!strcasecmp(BASE_DELAY, com) && pb->botNum == 0) {
                    if (ptrDat == NULL) {
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker, BASE_DELAY, BASE_COLON, delay);
                        msleep(3000);
                    }
                    else {
                        delay = atoi(ptrDat);
                        save_cfg(pb);
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker, BASE_DELAY, BASE_ISNOW, delay);
                        msleep(3000);
                    } return;
                }
                else if (!strcasecmp(BASE_DELAY_2, com)) {
                    if (ptrDat == NULL) {
                        Send(s, ON_COMMAND_REPLY_1, SERVER_WHISPER, szSpeaker, BASE_DELAY, BASE_SCATTER, pb->delay2);
                        msleep(3000);
                    } return;
                }
                else if (!strcasecmp(BASE_SCATTER, com) && pb->botNum == 0) {
                    if (ptrDat == NULL) {
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker, BASE_SCATTER, BASE_COLON, scatter);
                        msleep(3000);
                    }
                    else {
                        scatter = atoi(ptrDat);
                        save_cfg(pb);
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker, BASE_SCATTER, BASE_ISNOW, scatter);
                        msleep(3000);
                    } return;
                }
                else if (!strcasecmp(BASE_BANWAIT, com) && pb->botNum == 0) {
                    if (ptrDat == NULL) {
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker, BASE_BANWAIT, BASE_COLON, banWait);
                        msleep(3000);
                    }
                    else {
                        banWait = atoi(ptrDat) * 1000;
                        save_cfg(pb);
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker, BASE_BANWAIT, BASE_ISNOW, (banWait / 1000));
                        msleep(3000);
                    } return;
                }
                else if (!strcasecmp(BASE_CONWAIT, com) && pb->botNum == 0) {
                    if (ptrDat == NULL) {
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker, BASE_CONWAIT, BASE_COLON, conWait);
                        msleep(3000);
                    }
                    else {
                        conWait = atoi(ptrDat);
                        save_cfg(pb);
                        Send(s, ON_COMMAND_REPLY_0I, SERVER_WHISPER, szSpeaker, BASE_CONWAIT, BASE_ISNOW, conWait);
                        msleep(3000);
                    } return;
                }
                else if (!strcasecmp(BASE_SAY, com) && ptrDat != NULL) {
                    Send(s, ON_COMMAND_REPLY_2, ptrDat);
                    msleep(3000);
                    return;
                }
                else if (!strcasecmp(BASE_VER, com) && pb->botNum == 0) {
                    Send(s, SERVER_COMMAND_2, SERVER_WHISPER, szSpeaker, FUK_VERSION);
                    msleep(3000);
                    return;
                }
                else if (!strcasecmp(BASE_QUIT, com)) {
                    exit(0);
                }
                else if (!strcasecmp(BASE_RECON, com)) {
                    close(s);
                    return;
                }
                else if (!strcasecmp(BASE_PLACE, com)) {
                    Send(s, "/%s %s %d to login to %s with actual delay of %d.\r\n", SERVER_WHISPER, szSpeaker, pb->place, pb->server, pb->delay2);
                    msleep(3000);
                    return;
                }
                else if (!strcasecmp(BASE_PING, com) && pb->botNum == 0) {
                    memset(pingStr, '\0', sizeof(pingStr));
#if defined(WINDOWS_CPP_BUILD)
                    sprintf(pingStr, "%s %s", BASE_PING, pb->server);
#else
                    sprintf(pingStr, "%s -c1 %s", BASE_PING, pb->server);
#endif
                    fp1 = popen(pingStr, "r");
                    fgets(pi, 512, fp1);
                    fgets(pi, 512, fp1);
#if defined(WINDOWS_CPP_BUILD)
                    fgets(pi, 512, fp1);
#endif
                    Send(s, SERVER_BASE_SPEAK, pi);
                    pclose(fp1); /* if your going to use popen, use pclose */
                    msleep(3000);
                    return;
                }
                else if (!strcasecmp(com, SERVER_BAN) && ptrDat != NULL && pb->hasop == 1) {
                    Send(s, SERVER_COMMAND_1, SERVER_BAN, ptrDat);
                    msleep(3000);
                    return;
                }
                else if (!strcasecmp(com, SERVER_UNBAN) && ptrDat != NULL && pb->hasop == 1) {
                    pb->lockdown = 0;
                    Send(s, SERVER_COMMAND_1, SERVER_UNBAN, ptrDat);
                    msleep(3000);
                    return;
                }
                else if (!strcasecmp(com, SERVER_KICK) && ptrDat != NULL && pb->hasop == 1) {
                    Send(s, SERVER_COMMAND_1, SERVER_KICK, ptrDat);
                    msleep(3000);
                    return;
                }
                else if (!strcasecmp(com, SERVER_JOIN) && ptrDat != NULL) {
                    Send(s, SERVER_COMMAND_1, SERVER_JOIN, ptrDat);
                    msleep(3000);
                    return;
                }
                else if (!strcasecmp(com, SERVER_REJOIN)) {
                    Send(s, SERVER_COMMAND_0, SERVER_REJOIN);
                    msleep(3000);
                    return;
                }
                else if (!strcasecmp(com, BASE_DES) && pb->hasop == 1) {
                    pb->op = 0;
                    if (ptrDat != NULL) {
                        Send(s, SERVER_COMMAND_1, SERVER_DESIGNATE, ptrDat);
                    } else {
                        Send(s, SERVER_COMMAND_1, SERVER_DESIGNATE, szSpeaker);
                    } 
                    msleep(3000); 
                    return;
                }
                else if (!strcasecmp(com, SERVER_RESIGN) && pb->hasop == 1) {
                    if (pb->des != 0) {
                        pb->des = 0;
                        Send(s, SERVER_COMMAND_0, SERVER_RESIGN);
                        msleep(3000);
                    }
                    else {
                        Send(s, ON_COMMAND_REPLY_2, BASE_NO_HEIR_TEXT);
                        msleep(3000);
                    } return;
                }
                else if (!strcasecmp(com, SERVER_UPTIME) && pb->botNum == 0) {
                    Send(s, SERVER_COMMAND_0, SERVER_UPTIME);
                    msleep(3000);
                    return;
                }
                else if (!strcasecmp(com, BASE_OP) && pb->hasop == 1) { /* confused by the second designate command here */
                    pb->op = 1;
                    if (ptrDat != NULL) {
                        Send(s, SERVER_COMMAND_1, SERVER_DESIGNATE, ptrDat);
                    } else {
                        Send(s, SERVER_COMMAND_1, SERVER_DESIGNATE, szSpeaker);
                    } 
                    msleep(3000); 
                    return;
                }
                else if (!strcasecmp(com, BASE_LOCK) && pb->hasop == 1) { //BASE_CHANLOCKED
                    pb->lockdown = 1;
                    Send(s, SERVER_COMMAND_2, SERVER_TOPIC, topic, BASE_CHANLOCKED);
                    msleep(3000);
                    Send(s, SERVER_COMMAND_1, SERVER_WHO, pb->currChan);
                    msleep(3000);
                    return;
                }
                else if (!strcasecmp(com, BASE_UNLOCK)) {
                    pb->lockdown = 0;
                    if (pb->hasop == 1) {
                        Send(s, SERVER_COMMAND_1, SERVER_TOPIC, topic);
                        msleep(3000);
                    } return;
                }
                else if (!strcasecmp(com, BASE_CLEAN) && pb->hasop == 1) {
                    Send(s, SERVER_COMMAND_1, SERVER_WHO, pb->currChan);
                    msleep(3000);
                    return;
                }
                else if (!strcasecmp(com, BASE_TAG) && pb->hasop == 1) {
                    if (ptrDat != NULL) {
                        pb->tban = 1;
                        memset(tag, '\0', sizeof(tag));
                        strcpy(tag, ptrDat);
                        Send(s, SERVER_COMMAND_1, SERVER_WHO, pb->currChan);
                        msleep(3000);
                    }
                    else {
                        pb->tban = 0;
                        Send(s, ON_COMMAND_REPLY_2, BASE_TAGBAN_DISABLED);
                        msleep(3000);
                    } return;
                }
                else if (!strcasecmp(com, BASE_CONTIME) && pb->botNum == 0) {
                    Send(s, ON_COMMAND_REPLY_3, SERVER_WHISPER, szSpeaker, pb->conTime);
                    msleep(3000);
                    return;
                }
                else if (!strcasecmp(com, BASE_MEM) && pb->botNum == 0) {
                    getrusage(RUSAGE_SELF, &r_usage);
                    Send(s, ON_COMMAND_REPLY_4, SERVER_WHISPER, szSpeaker, r_usage.ru_maxrss);
                    msleep(3000);
                    return;
                }
                else
                    return;
            }
        }
    }
}

void OnChannel(int s, struct data* pb, char* szEventText) {
    while ((*szEventText == ' '))
        ++szEventText;
    memset(pb->currChan, '\0', sizeof(pb->currChan));
    strcpy(pb->currChan, szEventText);
    if (!strcasecmp(szEventText, pb->channel)) {
        pb->chanham = 0;
        Send(s, SERVER_COMMAND_0, SERVER_PLACE);
        msleep(3000);
        return;
    }
    else if (!strcasecmp(szEventText, CHANNEL_VOID)) {
        pb->chanham = 1;
        Send(s, SERVER_COMMAND_0, SERVER_PLACE);
        msleep(3000);
        Send(s, SERVER_COMMAND_1, SERVER_JOIN, backup);
        msleep(3000);
        return;
    }
} /* you dont need a return at the end of a void function [remove this note if you ever revisit it] */

void OnInfo(int s, struct data *pb, char *szEventText) {
    if (strstr(szEventText, BASE_INIT6)) {
        return;
    }
    else if (strstr(szEventText, BASE_SETTHETOPIC)) {
        return;
    }
    else if (strstr(szEventText, BASE_WASKICKED)) {
        return;
    }
    else if (strstr(szEventText, BASE_WASBANNED)) {
        return;
    }
    else if (strstr(szEventText, BASE_WASUNBANNED)) {
        return;
    }
    else if (strstr(szEventText, BASE_NOCHATPRIV)) {
        return;
    }
    else if (strstr(szEventText, BASE_CHATCHANNEL)) {
        return;
    }
    else if (strstr(szEventText, BASE_CHANNELUSERS)) {
        return;
    }
    else if(strstr(szEventText, BASE_NEWHEIR)) {
        pb->des=1;
        if(pb->op!=0 && pb->hasop!=0) {
            pb->op=0;
            Send(s, SERVER_COMMAND_0, SERVER_RESIGN);
            msleep(3000);
        }
        return;
    }
    else if(strstr(szEventText, BASE_PLACED)) {
        char *tmp;
        char *pos;
        tmp=strtok_r(szEventText, " ", &pos);
        tmp=strtok_r(NULL, " ", &pos);
        tmp=strtok_r(NULL, " ", &pos);
        pb->place=atoi(tmp);
        return;
    }
    else if(strstr(szEventText, BASE_CUPTIME)) {
        char *tmp;
        char *pos;
        tmp=strtok_r(szEventText, "\r\n", &pos);
        Send(s, SERVER_BASE_SPEAK, tmp);
        msleep(3000);
        return;
    }
    else if(strstr(szEventText, BASE_YOUWEREKICKED)){
        pb->chanham=1;
        Send(s, SERVER_COMMAND_1, SERVER_JOIN, backup);
        msleep(3000);
        return;
    } 
    else {
        char *pos;
        char *user1;
        char *user2;
        user1 = strtok_r(szEventText, ", ", &pos);
        if(user1!=NULL) {
            while ((*user1 == ' ')) {
                ++user1;
            }
            OnJoin(s, pb, user1);
        }
        user2 = strtok_r(NULL, "\r\n", &pos);
        if(user2!=NULL) {
            while ((*user2 == ' ')) {
                ++user2;
            }
            OnJoin(s, pb, user2);
        }
        return; /* this return dosent need to be here but whatever lol */
    }
}

void OnError(int s, struct data *pb, char *szEventText) {
    if (strstr(szEventText, BASE_CHANRESTRICTED_TEXT)) {
        return;
    }
    if (strstr(szEventText, BASE_INVALIDUSER_TEXT)){
        pb->des=0;
        return;
    }
    if (strstr(szEventText, BASE_USERNOTLOGGEDON_TEXT)) {
        pb->des=0;
        return;
    }
}

void OnPing(int s, struct data *pb, char *szEventText) {
    if (pb->chanham==1) {
        Send(s, SERVER_COMMAND_1, SERVER_JOIN, pb->channel);
        msleep(3000);
        return;
    } else {
        Send(s, SERVER_COMMAND_1, SERVER_PONG, szEventText);
        msleep(3000);
        return;
    }
}

void Dispatch(int s, struct data *pb, char *szEventText) {
    char *eventType;
    char *pos;
    char *USER_CMD;
    char *USER_FLAGS;
    char *USER_PING;
    char *USER_NAME;
    char *USER_DIRECTION;
    char *USER_MSG;
    char *CHANNEL_CMD;
    char *CHANNEL_NAME;
    char *SERVER_MSG;
    char *SERVER_CMD;
    char *PING;
    eventType = strtok_r(szEventText, " ", &pos);
    if(!strcasecmp(eventType, "USER")) {
        USER_CMD = strtok_r(NULL, " ", &pos);
        if(!strcasecmp(USER_CMD, "IN")) {
            USER_FLAGS = strtok_r(NULL, " ", &pos);
            USER_PING = strtok_r(NULL, " ", &pos);
            USER_NAME = strtok_r(NULL, " ", &pos);
            OnUserFlags(s, pb, USER_NAME, atoi(USER_FLAGS));
            return;
        } else if(!strcasecmp(USER_CMD, "UPDATE")) {
            USER_FLAGS = strtok_r(NULL, " ", &pos);
            USER_PING = strtok_r(NULL, " ", &pos);
            USER_NAME = strtok_r(NULL, " ", &pos);
            OnUserFlags(s, pb, USER_NAME, atoi(USER_FLAGS));
            return;
        } else if(!strcasecmp(USER_CMD, "EMOTE")) {
            USER_NAME = strtok_r(NULL, " ", &pos);
            USER_MSG = strtok_r(NULL, " ", &pos);
            OnTalk(s, pb, USER_NAME, USER_MSG);
            return;
        } else if(!strcasecmp(USER_CMD, "JOIN")) {
            USER_FLAGS = strtok_r(NULL, " ", &pos);
            USER_PING = strtok_r(NULL, " ", &pos);
            USER_NAME = strtok_r(NULL, " ", &pos);
            OnJoin(s, pb, USER_NAME);
            return;
        } else if(!strcasecmp(USER_CMD, "WHISPER")) {
            USER_DIRECTION = strtok_r(NULL, " ", &pos);
            USER_NAME = strtok_r(NULL, " ", &pos);
            USER_MSG = strtok_r(NULL, "\r\n", &pos);
            OnTalk(s, pb, USER_NAME, USER_MSG);
            return;
        } else if(!strcasecmp(USER_CMD, "TALK")) {
            USER_DIRECTION = strtok_r(NULL, " ", &pos);
            USER_NAME = strtok_r(NULL, " ", &pos);
            USER_MSG = strtok_r(NULL, "\r\n", &pos);
            OnTalk(s , pb, USER_NAME, USER_MSG);
            return;
        }else
            return;
    } else if(!strcasecmp(eventType, "CHANNEL")) {
        CHANNEL_CMD = strtok_r(NULL, " ", &pos);
        if(!strcasecmp(CHANNEL_CMD, "JOIN")) {
            CHANNEL_NAME = strtok_r(NULL, "\r\n", &pos);
            OnChannel(s, pb, CHANNEL_NAME);
        }
        return;
    } else if(!strcasecmp(eventType, "SERVER")) {
        SERVER_CMD = strtok_r(NULL, " ", &pos);
        if(!strcasecmp(SERVER_CMD, "INFO")) {
            SERVER_MSG = strtok_r(NULL, "\r\n", &pos);
            OnInfo(s, pb, SERVER_MSG);
            return;
        } else if(!strcasecmp(SERVER_CMD, "ERROR")) {
            SERVER_MSG = strtok_r(NULL, "\r\n", &pos);
            OnError(s, pb, SERVER_MSG);
            return;
        } else
            return;
    } else if(!strcasecmp(eventType, "PING")) {
        PING = strtok_r(NULL, " ", &pos);
        OnPing(s, pb, PING);
        return;
    } else
        return;
}

/*
    local variable "int try" was getting caught as a (try/catch) block
    wasent needed regardless just return the send().
*/
int Send(int s, const char* lpszFmt, ...) {
    char szOutStr[256];
    va_list argptr;
    va_start(argptr, lpszFmt);
    vsprintf(szOutStr, lpszFmt, argptr);
    va_end(argptr);
    return send(s, szOutStr, strlen(szOutStr), 0);
}

/*
    type casting, though this shouldent matter to C
    so the changes made should still compile.
*/
void message_loop(int s, struct data* pb) {
    int n;
    int nBufLen = 0;
    int nBufPos = 0;
    char stageBuf[BUFFSIZE];
    struct timeval tv;
    if (s == INVALID_SOCKET) {
        return;
    }
    for (;;) { /* ... */
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(s, &fds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        n = select(s + 1, &fds, 0, 0, &tv);
        if (n) {
            int nNumToRead = BUFFSIZE - nBufLen - nBufPos;
            if (nNumToRead == 0) {
                memmove(stageBuf, stageBuf + nBufPos, nBufLen);
                nBufPos = 0;
                nNumToRead = BUFFSIZE - nBufLen;
            }
            n = recv(s, stageBuf + nBufPos + nBufLen, nNumToRead, 0);
            if (n <= 0) {
                return;
            }
            nBufLen += n;
            while (nBufLen > 0) {
#ifdef _WIN32
                unsigned char* m = (unsigned char*)stageBuf+nBufPos;
#else
                unsigned char* m = reinterpret_cast<unsigned char*>(stageBuf + nBufPos);
#endif
                int nMsgLen = 0;
                while (nMsgLen < nBufLen) {
                    if (m[nMsgLen] == '\n')
                        break;
                    nMsgLen++;
                }
                nMsgLen++;
                if (nMsgLen > nBufLen)
                    break;
                m[nMsgLen - 1] = '\0';
#ifdef _WIN32
                Dispatch(s, pb, (char*)m);
#else
                Dispatch(s, pb, m);
#endif
                nBufLen -= nMsgLen;
                nBufPos += nMsgLen;
            }
            if (!nBufLen)
                nBufPos = 0;
        }
        msleep(500);
    }
    return;
}

#if !defined(WINDOWS_CPP_BUILD)
    void msleep(unsigned long milisec) {
        struct timespec req={0};
        time_t sec=(int)(milisec/1000);
        milisec=milisec-(sec*1000);
        req.tv_sec=sec;
        req.tv_nsec=milisec*1000000L;
        while(nanosleep(&req,&req)==-1)
             continue;
        return;
    }
#endif

char *replace_str(char *str, char *orig, int rep) {
  static char buffer[20];
  char *p;
  if(!(p = strstr(str, orig)))
    return str;
  strncpy(buffer, str, p-str);
  buffer[p-str] = '\0';
  sprintf(buffer+(p-str), "%d%s", rep, p+strlen(orig));
  return buffer;
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
    FILE* cfg;
    int i = 0;
    int j = 0;
    int k = 0;
    int d = 0;
    int line = 0;
    cfg = fopen(FUK_CFG, FUK_CFG_READBINARY);
    if (!cfg) {
        printf(FUK_CFG_ERROR, FUK_CFG);
        return -1;
    }
    else {
        char r[1024];
        while (fgets(r, 1024, cfg)) {
            line++;
            strtok(r, "\r\n");
            if (!memcmp(r, FUK_CFG_USERNAME, strlen(FUK_CFG_USERNAME))) {
                memset(username, '\0', sizeof(username));
                strcpy(username, (r + strlen(FUK_CFG_USERNAME)));
            }
            else if (!memcmp(r, FUK_CFG_PASSWORD, strlen(FUK_CFG_PASSWORD))) {
                memset(password, '\0', sizeof(password));
                strcpy(password, (r + strlen(FUK_CFG_PASSWORD)));
            }
            else if (!memcmp(r, FUK_CFG_HOME, strlen(FUK_CFG_HOME))) {
                memset(channel, '\0', sizeof(channel));
                strcpy(channel, (r + strlen(FUK_CFG_HOME)));
            }
            else if (!memcmp(r, FUK_CFG_SERVER, strlen(FUK_CFG_SERVER))) {
                memset(server, '\0', sizeof(server));
                strcpy(server, (r + strlen(FUK_CFG_SERVER)));
            }
            else if (!memcmp(r, FUK_CFG_PORT, strlen(FUK_CFG_PORT))) {
                port = atoi(r + strlen(FUK_CFG_PORT));
            }
            else if (!memcmp(r, FUK_CFG_BIND, strlen(FUK_CFG_BIND))) {
                memset(bindaddr, '\0', sizeof(server));
                strcpy(bindaddr, (r + strlen(FUK_CFG_BIND)));
            }
            else if (!memcmp(r, FUK_CFG_TRIGGER, strlen(FUK_CFG_TRIGGER))) {
                memset(trigger, '\0', sizeof(trigger));
                strcpy(trigger, (r + strlen(FUK_CFG_TRIGGER)));
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
                memset(backup, '\0', sizeof(backup));
                strcpy(backup, (r + strlen(FUK_CFG_BACKUPCHAN)));
            }
            else if (!memcmp(r, FUK_CFG_TOPIC, strlen(FUK_CFG_TOPIC))) {
                memset(topic, '\0', sizeof(topic));
                strcpy(topic, (r + strlen(FUK_CFG_TOPIC)));
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
                master = (masterList*)reallocarray(master, ++masterSz, sizeof(masterList));
                memset(master[i].id, '\0', sizeof(master[i].id));
                strcpy(master[i].id, (r + strlen(FUK_CFG_MASTER)));
                i++;
            }
            else if (!memcmp(r, FUK_CFG_SAFE, strlen(FUK_CFG_SAFE))) {
                safe = (safeList*)reallocarray(safe, ++safeSz, sizeof(safeList));
                memset(safe[j].id, '\0', sizeof(safe[j].id));
                strcpy(safe[j].id, (r + strlen(FUK_CFG_SAFE)));
                j++;
            }
            else if (!memcmp(r, FUK_CFG_SHIT, strlen(FUK_CFG_SHIT))) {
                shit = (shitList*)reallocarray(shit, ++shitSz, sizeof(shitList));
                memset(shit[k].id, '\0', sizeof(shit[k].id));
                strcpy(shit[k].id, (r + strlen(FUK_CFG_SHIT)));
                k++;
            }
            else if (!memcmp(r, FUK_CFG_DES, strlen(FUK_CFG_DES))) {
                des = (desList*)reallocarray(des, ++desSz, sizeof(desList));
                memset(des[d].id, '\0', sizeof(des[d].id));
                strcpy(des[d].id, (r + strlen(FUK_CFG_DES)));
                d++;
            }
        }
        fclose(cfg);
    } 
    return 0;
}

#if defined(WINDOWS_CPP_BUILD)
    int Connect(int s, struct timeval tv, struct data* pb) {
	    fd_set fdr, fdw;
	    int on=1, err=0, errlen=4;
	    int sockinlen=16;
        struct sockaddr_in name;
        struct sockaddr_in name2 = { AF_INET, 0, INADDR_ANY };

        name.sin_family = AF_INET;
	    name.sin_port = htons(pb->port);
	    inet_pton(AF_INET, pb->server, &(name.sin_addr));

        inet_pton(AF_INET, bindaddr, &(name2.sin_addr)); //pb->bindaddr
        if (bind(s, (PSOCKADDR)&name2, sizeof(name2)) == INVALID_SOCKET) {
            return INVALID_SOCKET;
        }

        ioctl(s, FIONBIO, (u_long*)&on);

        FD_ZERO(&fdr); FD_SET(s,&fdr); fdw=fdr;
	    connect(s,(struct sockaddr *)&name,sizeof(name));
	    select(s+1,&fdr,&fdw,NULL,(struct timeval *)&tv);

	    if (FD_ISSET(s,&fdw)) {
		    getsockopt(s, SOL_SOCKET, SO_ERROR, (char*)&err, &errlen);
	    }else {
	        err= -1;
	    }
	    return err;
    }
#else
    int Connect(int s, struct timeval tv, struct data* pb) {
        fd_set fdr, fdw;
        int on = 1, err = 0, errlen = 4;
        int sockinlen = 16;
        struct sockaddr_in sockin;
        struct sockaddr_in name2 = { AF_INET, 0, INADDR_ANY };
        struct sockaddr_in name;
        name.sin_family = AF_INET;
        name.sin_port = htons(pb->port);
        inet_pton(AF_INET, pb->server, &(name.sin_addr));
        inet_pton(AF_INET, bindaddr, &(name2.sin_addr));
        if (bind(s, (const struct sockaddr*)&name2, sizeof(name2)) == INVALID_SOCKET) {
            return INVALID_SOCKET;
        }
        set_nonblock(s);
        FD_ZERO(&fdr); FD_SET(s, &fdr); fdw = fdr;
        connect(s, (struct sockaddr*)&name, sizeof(name));
        //printf("%s Attempting to connect on socket %d\n", pb->username, s);
        select(s + 1, &fdr, &fdw, NULL, (struct timeval*)&tv);
        if (FD_ISSET(s, &fdw)) {
            getsockopt(s, SOL_SOCKET, SO_ERROR, &err, &errlen);
        }
        else {
            err = -1;
        }
        return err;
    }
#endif

#if defined(WINDOWS_CPP_BUILD)
    void *thread_conn(void *arg) {
        struct data* pb=(struct data *)arg;
	    struct timeval tv;
	    int s = 0, off = 0;
	    tv.tv_sec=0;
	    startTime = time (NULL);
	    if (delay > 0) {
		    pb->delay2 = rand() % scatter + delay;
	    }else {
		    pb->delay2 = delay;
	    }
	    tv.tv_usec = pb->delay2 * 1000;

        //Recon Loop
        while (pb->connected == 0) {
		    s=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		    int con = Connect(s, tv, pb);
		    if (con == 0)
		        break;
		    else
			    close(s);
	    }

        //Not first so close socket and kill thread. /* these __sync_bool_compare_and_swap are fuckin with my eyeballs */
        if (pb->connected == 1) {
            //printf("%s Not first :( Closing socket %d\n", pb->username, s);
            close(s);
            return (NULL);
	    }
        pb->connected = 1;
        //First thread to connect. Do stuff.
        Send(s, pb->logonPacket, sizeof(pb->logonPacket), 0);
        pb->conTime = time(NULL) - startTime;
        //printf("%s first to connect! On socket %d\n", pb->username, s);
        message_loop(s, pb);
        //Disconnected. Wrap it up.
        //printf("Disconnected... Closing socket %d\n", s);
        close(s);
    
        pb->connected = 0;
    }
#else
    void* thread_conn(void* arg) {
        struct data* pb = (struct data*)arg;
        struct timeval tv;
        int s, off = 0;
        tv.tv_sec = 0;
        startTime = time(NULL);
        if (delay > 0) {
            pb->delay2 = rand() % scatter + delay;
        }
        else {
            pb->delay2 = delay;
        }
        tv.tv_usec = pb->delay2 * 1000;
        //Recon Loop
        while (__sync_bool_compare_and_swap(&pb->connected, 0, 0)) {
            s = socket(AF_INET, SOCK_STREAM, 0);
            int con = Connect(s, tv, pb);
            if (con == 0)
                break;
            else
                close(s);
        }
        //Not first so close socket and kill thread.
        if (!__sync_bool_compare_and_swap(&pb->connected, 0, 1)) {
            //printf("%s Not first :( Closing socket %d\n", pb->username, s);
            close(s);
            pthread_exit(NULL);
        }
        //First thread to connect. Do stuff.
        Send(s, pb->logonPacket, sizeof(pb->logonPacket), 0);
        pb->conTime = time(NULL) - startTime;
        //printf("%s first to connect! On socket %d\n", pb->username, s);
        message_loop(s, pb);
        //Disconnected. Wrap it up.
        //printf("Disconnected... Closing socket %d\n", s);
        close(s);
        __sync_bool_compare_and_swap(&pb->connected, 1, 0);
        pthread_exit(NULL);
    }
#endif

#if defined(WINDOWS_CPP_BUILD)
    void create_threads(struct data *pb) {
        int i = 0;

        //pthread_t thread[8]; //numThreads
        int numThreads = numBots * threads;
        std::vector<std::thread> thread(numThreads);

        //Create and configure each bot.
        for (int t = 0; t < numBots; t++, pb++) {
            char *replaced = replace_str(username, (char*)"#", t);
            char locName[20] = { 0 };
            memcpy(locName, replaced, strlen(replaced));
            //char *thisBotsName = strdup(replace_str(username, (char*)"#", t));
            //There's no need to do this more than once.
            if (strcmp(pb->username, locName) != 0) {
                strcpy(pb->username, locName);
                strcpy(pb->password, password);
                strcpy(pb->channel, channel);
                strcpy(pb->server, server);
                strcpy(pb->trigger, trigger);
            }
            pb->botNum = t;
            pb->port = port;
            pb->threads = threads;
            memset(pb->logonPacket, '\0', sizeof(pb->logonPacket));
            sprintf(pb->logonPacket, "C1\r\nACCT %s\r\nPASS %s\r\nHOME %s\r\nLOGIN\r\n", pb->username, pb->password, pb->channel);
            //Create threads for each bot.
	        for (int z = 0; z < threads; z++, i++) {
                thread[i] = std::thread(thread_conn, pb);
	        }
        }
        //Join threads if they haven't already kilt themselves.
        for (int t = 0; t < numThreads; t++) {
            thread[t].join();
        }
        msleep(conWait * 1000);
    }
#else
    void create_threads(struct data* pb) {
        int err;
        int i = 0;
        int numThreads = numBots * threads;
        pthread_t thread[numThreads];
        //Create and configure each bot.
        for (int t = 0; t < numBots; t++, pb++) {
            char *replaced = replace_str(username, (char*)"#", t);
            char locName[20] = { 0 };
            memcpy(locName, replaced, strlen(replaced));
            //char *thisBotsName = strdup(replace_str(username, (char*)"#", t));
            //There's no need to do this more than once.
            if (strcmp(pb->username, locName) != 0) {
                strcpy(pb->username, locName);
                strcpy(pb->password, password);
                strcpy(pb->channel, channel);
                strcpy(pb->server, server);
                strcpy(pb->trigger, trigger);
            }
            pb->botNum = t;
            pb->port = port;
            pb->threads = threads;
            memset(pb->logonPacket, '\0', sizeof(pb->logonPacket));
            sprintf(pb->logonPacket, "C1\r\nACCT %s\r\nPASS %s\r\nHOME %s\r\nLOGIN\r\n", pb->username, pb->password, pb->channel);
            //Create threads for each bot.
            for (int z = 0; z < threads; z++, i++) {
                err = pthread_create(&thread[i], NULL, thread_conn, pb);
                if (err < 0)
                    perror("pthread_create");
            }
        }
        //Join threads if they haven't already kilt themselves.
        for (int t = 0; t < numThreads; t++) {
            err = pthread_join(thread[t], NULL);
            if (err < 0)
                perror("pthread_exit");
        }
        msleep(conWait * 1000);
    }
#endif

#if defined(WINDOWS_CPP_BUILD)
    /*
        we cant use pthread directly in windows nor can we use fork()
        so we need to rely on the thread work.
        also windows requires wsa to be initialized and destroyed to use sockets
    */
    int main() {
        srand(time(NULL)); /* must initialize srand */
        int mainresult = 0;
        WSADATA mainSdata;
        int err = WSAStartup(WINSOCK_VERSION, &mainSdata); /* WINSOCK_VERSION is defined in the tcpip head */

        printf("%s\n", FUK_VERSION);
        printf("PID: %d\n", getpid() + 1);

        if (err == 0) {
            int try_read = read_config();
            if (try_read != 0) {
                printf("Read config error: %i\n", try_read);
                mainresult = 2;
            } else {
                pb = (struct data*)calloc(numBots, sizeof(struct data));

                //Launching numBots threads.
                while (1) { /* this right here already worrys me lol */
                    startTime = time(NULL);
                    create_threads(pb);
                }
                free(pb);
            }
            /* WSA had a clean start and is initalized clean it all up now */
            WSACleanup();
        } else {
            printf("WSAStartup failed with error: %d\n", err);
            mainresult = 1;
        }

        return mainresult; /* 0 = ran normaly, 1 = wsa error, 2 = failed read cfg */
    }
#else
    int main() {
        srand(time(NULL)); /* must initialize srand */
        printf("%s\n", FUK_VERSION);
        printf("PID: %d\n", getpid() + 1);
        if ((main_pid = fork()) == -1) {
            printf("shutting down: unable to fork\n");
            exit(1);
            return 1; /* if fork failed shouldent need to exit() though i could be wrong lol */
        }
        if (main_pid != 0) {
            return 0;
        }
        if (read_config() != 0) {
            perror("Read config error.");
            exit(0);
            return 0; 
        }
        pb = (struct data*)calloc(numBots, sizeof(struct data));
        while (1) {
            startTime = time(NULL);
            create_threads(pb);
        }
        free(pb);
        return 0; /* need to return an integer lol */
    }
#endif
