#include "fuk3.h"
void set_nonblock(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
}

void cfgStuff(int s, struct data *pb, char *com, char *text) {
    int x;
    int once = 1;
    char *pos;
    int replace = 0;
    char *list = strtok_r(text, " ", &pos);
    char *name = strtok_r(NULL, " ", &pos);
    if (strstr(list, "master")) {
        for (x=0; x < masterSz; x++) {
            if (!strcasecmp("list", com)) {
                Send(s, "master = %s\r\n", master[x].id);
                msleep(3000);
            } else if (!strcasecmp("rem", com) && name != NULL) {
                if (!strcasecmp(name, master[x].id))
                    replace = 1;
                if (replace == 1) {
                    memset(master[x].id, '\0', sizeof(master[x].id));
                    if (x < masterSz - 1) {
                        strcpy(master[x].id, master[x+1].id);
                    } else
                        master = reallocarray(master, --masterSz, sizeof(masterList));
                    if (once == 1) {
                        Send(s, "Success! %s %s from %s.\r\n", com, name, list);
                        once = 0;
                        msleep(3000);
                    }
                }
            } else if (!strcasecmp("add", com) && name != NULL) {
                if(!strcasecmp(master[x].id, name)) {
                    Send(s, "%s is already auto master.\r\n", name);
                    return;
                }
            }
        }
        if (!strcasecmp("add", com) && name !=NULL) {
            master = reallocarray(master, ++masterSz, sizeof(masterList));
            int ps = masterSz - 1;
            memset(master[ps].id, '\0', sizeof(master[ps].id));
            strcpy(master[ps].id, name);
            Send(s, "Success! %s %s to %s.\r\n", com, name, list);
            msleep(3000);
        } return;
    } else if (strstr(list, "safe")) {
        for (x=0; x < safeSz; x++) {
            if (!strcasecmp("list", com)) {
                Send(s, "safe = %s\r\n", safe[x].id);
                msleep(3000);
            } else if (!strcasecmp("rem", com) && name != NULL) {
                if (!strcasecmp(name, safe[x].id))
                    replace = 1;
                if (replace == 1) {
                    memset(safe[x].id, '\0', sizeof(safe[x].id));
                    if (x < safeSz - 1)
                        strcpy(safe[x].id, safe[x+1].id);
                    else
                        safe = reallocarray(safe, --safeSz, sizeof(safeList));
                    if (once == 1) {
                        Send(s, "Success! %s %s from %s.\r\n", com, name, list);
                        once = 0;
                        msleep(3000);
                    }
                }
            } else if (!strcasecmp("add", com) && name != NULL) {
                if(!strcasecmp(safe[x].id, name)) {
                    Send(s, "%s is already auto safe.\r\n", name);
                    return;
                }
            }
        }
        if (!strcasecmp("add", com) && name !=NULL) {
            safe = reallocarray(safe, ++safeSz, sizeof(safeList));
            int ps = safeSz - 1;
            memset(safe[ps].id, '\0', sizeof(safe[ps].id));
            strcpy(safe[ps].id, name);
            Send(s, "Success! %s %s to %s.\r\n", com, name, list);
            msleep(3000);
        } return;
    } else if (strstr(list, "shit")) {
        for (x=0; x < shitSz; x++) {
            if (!strcasecmp("list", com)) {
                Send(s, "shit = %s\r\n", shit[x].id);
                msleep(3000);
            } else if (!strcasecmp("rem", com) && name != NULL) {
                if (!strcasecmp(name, shit[x].id))
                    replace = 1;
                if (replace == 1) {
                    memset(shit[x].id, '\0', sizeof(shit[x].id));
                    if (x < shitSz - 1)
                        strcpy(shit[x].id, shit[x+1].id);
                    else
                        shit = reallocarray(shit, --shitSz, sizeof(shitList));
                    if (once == 1) {
                        Send(s, "Success! %s %s from %s.\r\n", com, name, list);
                        once = 0;
                        msleep(3000);
                    }
                }
            } else if (!strcasecmp("add", com) && name != NULL) {
                if (!strcasecmp(shit[x].id, name)) {
                    Send(s, "%s is already auto shit.\r\n", name);
                    return;
                }
            }
        }
        if (!strcasecmp("add", com) && name !=NULL) {
            shit = reallocarray(shit, ++shitSz, sizeof(shitList));
            int ps = shitSz - 1;
            memset(shit[ps].id, '\0', sizeof(shit[ps].id));
            strcpy(shit[ps].id, name);
            Send(s, "Success! %s %s to %s.\r\n", com, name, list);
            msleep(3000);
        } return;
    } else if (strstr(list, "des")) {
        for (x=0; x < desSz; x++) {
            if (!strcasecmp("list", com)) {
                Send(s, "Des = %s\r\n", des[x].id);
                msleep(3000);
            } else if (!strcasecmp("rem", com) && name != NULL) {
                if (!strcasecmp(name, des[x].id))
                    replace = 1;
                if (replace == 1) {
                    memset(des[x].id, '\0', sizeof(des[x].id));
                    if (x < desSz - 1)
                        strcpy(des[x].id, des[x+1].id);
                    else
                        des = reallocarray(des, --desSz, sizeof(desList));
                    if (once == 1) {
                        Send(s, "Success! %s %s from %s.\r\n", com, name, list);
                        once = 0;
                        msleep(3000);
                    }
                }
            } else if (!strcasecmp("add", com) && name != NULL) {
                if(!strcasecmp(des[x].id, name)) {
                    Send(s, "%s is already auto des.\r\n", name);
                    return;
                }
            }
        }
        if (!strcasecmp("add", com) && name !=NULL) {
            des = reallocarray(des, ++desSz, sizeof(desList));
            int ps = desSz - 1;
            memset(des[ps].id, '\0', sizeof(des[ps].id));
            strcpy(des[ps].id, name);
            Send(s, "Success! %s %s to %s.\r\n", com, name, list);
            msleep(3000);
        }
        return;
    } else {
        Send(s, "Options: list, add, rem | des, safe, shit, master.\r\n");
        msleep(3000);
        return;
    }
}
void OnJoin(int s, struct data *pb, char *szSpeaker) {
    if (pb->hasop==1 && time(NULL) - pb->lastTime < 5)
        pb->flood++;
    else
        pb->flood = 0;
    pb->lastTime = time(NULL);
    if (pb->hasop==1 && pb->flood < 500) {
        for (int d=0; d < desSz; d++) {
            if(!strcasecmp(szSpeaker, des[d].id)) {
                Send(s, "/designate %s\r\n", szSpeaker);
                msleep(3000);
		        return;
            }
        }
        for(int j=0; j < safeSz; j++) {
            if(strstr(szSpeaker, safe[j].id))
                return;
        }
        for(int k=0; k < masterSz; k++) {
            if(!strcasecmp(szSpeaker, master[k].id))
                return;
        }
        if (pb->tban==1) {
            if(strstr(szSpeaker, tag)) {
                Send(s, "/ban %s\r\n", szSpeaker);
                msleep(banWait * 1000);
                return;
            }
        }
        if (pb->lockdown == 1) {
            Send(s, "/ban %s\r\n", szSpeaker);
            msleep(banWait * 1000);
            return;
        }
        for(int l=0; l < shitSz; l++) {
            if(strstr(szSpeaker, shit[l].id)) {
                Send(s, "/ban %s\r\n", szSpeaker);
                msleep(banWait * 1000);
                return;
            }
        }
    } return;
}
void OnUserFlags(int s, struct data *pb, char *szSpeaker, u_long uFlags) {
    if (uFlags==18 && !strcasecmp(pb->username, szSpeaker)){
        pb->hasop=1;
        if(topic!=NULL) {
            Send(s, "/topic %s\r\n", topic);
            msleep(3000);
        }
        return;
    }
    if (uFlags==16 && !strcasecmp(pb->username, szSpeaker)){
        pb->hasop=0;
        pb->lockdown=0;
    } return;
}
void OnTalk(int s, struct data *pb, char *szSpeaker, char *szEventText) {
    int i = 0;
    int doStuff=0;
    char *text;
    char *com;
    char pi[512];
    FILE *fp1;
    char pingStr[250];
    for (i = 0; i < masterSz; i++) {
        if(!strcasecmp(szSpeaker,master[i].id))
            doStuff=1;
    }
    if (doStuff!=1)
        return;
    else {
        text=strdup(szEventText);
        if ( text[0] == pb->trigger[0] || text[0] == "?"[0] ) {
            text=strdup(text+1);
            com = strsep( &text, " " );
            if ( com != NULL) {
                if (!strcasecmp("list", com) && text != NULL) {
                    if (pb->botNum == 0)
                        cfgStuff(s, pb, com, text);
                    return;
                } else if (!strcasecmp("add", com) && text != NULL) {
                    if (pb->botNum == 0) {
                        cfgStuff(s, pb, com, text);
                        save_cfg(pb);
                    } return;
                } else if (!strcasecmp("rem", com) && text != NULL) {
                    if (pb->botNum == 0) {
                        cfgStuff(s, pb, com, text);
                        save_cfg(pb);
                    } return;
                } else if (!strcasecmp("trigger", com)) {
                    if (text == NULL) {
                        if(pb->botNum == 0) {
                            Send(s, "/msg %s Trigger: %s\r\n", szSpeaker, pb->trigger);
                            msleep(3000);
                        } else
                            return;
                    } else if (pb->botNum == 0) {
                        memset(pb->trigger, '\0', sizeof(pb->trigger));
                        strcpy(pb->trigger, text);
                        memset(trigger, '\0', sizeof(trigger));
                        strcpy(trigger, text);
                        save_cfg(pb);
                        Send(s, "/msg %s Trigger is now: %s\r\n", szSpeaker, pb->trigger);
                        msleep(3000);
                    } else {
                        memset(pb->trigger, '\0', sizeof(pb->trigger));
                        strcpy(pb->trigger, text);
                    } return;
                } else if (!strcasecmp("home", com)) {
                    if (text == NULL) {
                        if (pb->botNum == 0) {
                            Send(s, "/msg %s Home: %s\r\n", szSpeaker, pb->channel);
                            msleep(3000);
                        } else
                            return;
                    } else if (pb->botNum == 0) {
                        memset(pb->channel, '\0', sizeof(pb->channel));
                        strcpy(pb->channel, text);
                        memset(channel, '\0', sizeof(channel));
                        strcpy(channel, text);
                        save_cfg(pb);
                        Send(s, "/msg %s Home is now: %s\r\n", szSpeaker, pb->channel);
                        msleep(3000);
                    }else {
                        memset(pb->channel , '\0', sizeof(pb->channel));
                        strcpy(pb->channel, text);
                    } return;
                } else if (!strcasecmp("topic", com)) {
                    if (text == NULL) {
                        if (pb->hasop==1) {
                            Send(s, "/topic %s\r\n", topic);
                            msleep(3000);
                        } else if (pb->botNum == 0) {
                            Send(s, "/msg %s Topic: %s\r\n", szSpeaker, topic);
                            msleep(3000);
                        }
                    }else {
                        if (pb->hasop==1){
                            memset(topic, '\0', sizeof(topic));
                            strcpy(topic, text);
                            Send(s, "/topic %s\r\n", topic);
                            msleep(3000);
                        } else if (pb->botNum == 0) {
                            memset(topic, '\0', sizeof(topic));
                            strcpy(topic, text);
                            save_cfg(pb);
                            Send(s, "/msg %s Topic is now: %s\r\n", szSpeaker, topic);
                            msleep(3000);
                        }
                    } return;
                } else if (!strcasecmp("backup", com) && pb->botNum == 0) {
                    if(text == NULL) {
                        Send(s, "/msg %s Backup: %s\r\n", szSpeaker, backup);
                        msleep(3000);
                    }else {
                        memset(backup, '\0', sizeof(backup));
                        strcpy(backup, text);
                        save_cfg(pb);
                        Send(s, "/msg %s Backup is now: %s\r\n", szSpeaker, backup);
                        msleep(3000);
                    } return;
                } else if (!strcasecmp("server", com) && pb->botNum == 0) {
                    if(text == NULL) {
                        Send(s, "/msg %s Server: %s\r\n", szSpeaker, pb->server);
                        msleep(3000);
                    } return;
                } else if (!strcasecmp("threads",com) && pb->botNum == 0) {
                    if(text == NULL) {
                        Send(s, "/msg %s Threads: %d\r\n", szSpeaker, pb->threads);
                        msleep(3000);
                    } else {
                        threads = atoi(text);
                        save_cfg(pb);
                        Send(s, "/msg %s Threads are now: %d\r\n", szSpeaker, threads);
                        msleep(3000);
                    } return;
                } else if (!strcasecmp("port",com) && pb->botNum == 0) {
                    if(text == NULL) {
                        Send(s, "/msg %s Port: %d\r\n", szSpeaker, pb->port);
                        msleep(3000);
                    } else {
                        port = atoi(text);
                        save_cfg(pb);
                        Send(s, "/msg %s Port is now: %d\r\n", szSpeaker, port);
                        msleep(3000);
                    } return;
                } else if (!strcasecmp("delay",com) && pb->botNum == 0) {
                    if(text == NULL) {
                        Send(s, "/msg %s Delay: %d\r\n", szSpeaker, delay);
                        msleep(3000);
                    } else {
                        delay = atoi(text);
                        save_cfg(pb);
                        Send(s, "/msg %s Delay is now: %d\r\n", szSpeaker, delay);
                        msleep(3000);
                    } return;
                } else if (!strcasecmp("delay2",com)) {
                    if(text == NULL) {
                        Send(s, "/msg %s Actual delay (with scatter) %d\r\n", szSpeaker, pb->delay2);
                        msleep(3000);
                    } return;
                } else if (!strcasecmp("scatter",com) && pb->botNum == 0) {
                    if(text == NULL) {
                        Send(s, "/msg %s Scatter: %d\r\n", szSpeaker, scatter);
                        msleep(3000);
                    } else {
                        scatter = atoi(text);
                        save_cfg(pb);
                        Send(s, "/msg %s Scatter is now: %d\r\n", szSpeaker, scatter);
                        msleep(3000);
                    } return;
                } else if (!strcasecmp("banwait",com) && pb->botNum == 0) {
                    if (text == NULL) {
                        Send(s, "/msg %s Ban Wait: %d\r\n", szSpeaker, banWait);
                        msleep(3000);
                    } else {
                        banWait = atoi(text)*1000;
                        save_cfg(pb);
                        Send(s, "/msg %s Ban Wait is now: %d seconds.\r\n", szSpeaker, banWait / 1000);
                        msleep(3000);
                    } return;
                } else if (!strcasecmp("conwait",com) && pb->botNum == 0) {
                    if(text == NULL) {
                        Send(s, "/msg %s conWait: %d\r\n", szSpeaker, conWait);
                        msleep(3000);
                    }else {
                        conWait = atoi(text);
                        save_cfg(pb);
                        Send(s, "/msg %s conWait is now: %d\r\n", szSpeaker, conWait);
                        msleep(3000);
                    } return;
                } else if (!strcasecmp("say", com) && text!=NULL) {
                    Send( s, "%s\r\n", text);
                    msleep(3000);
                    return;
                } else if (!strcasecmp("ver",com) && pb->botNum == 0) {
                    Send(s, "/msg %s FuKeRy | v3.0\r\n", szSpeaker);
                    msleep(3000);
                    return;
                } else if (!strcasecmp("quit",com))
                    exit(0);
                else if (!strcasecmp("recon",com)) {
                    close(s);
                    return;
                } else if (!strcasecmp("place",com)) {
                    Send(s, "/msg %s %d to login to %s with actual delay of %d.\r\n", szSpeaker, pb->place, pb->server, pb->delay2);
                    msleep(3000);
                    return;
                } else if (!strcasecmp("ping",com) && pb->botNum == 0) {
                    memset(pingStr, '\0', sizeof(pingStr));
                    sprintf(pingStr, "ping -c1 %s", pb->server);
                    fp1 = popen(pingStr,"r");
                    fgets(pi,512,fp1);
                    fgets(pi,512,fp1);
                    Send(s, "%s\r\n",pi);
                    fclose(fp1);
                    msleep(3000);
                    return;
                } else if ( !strcasecmp( com, "ban" ) && text != NULL && pb->hasop==1) {
                    Send( s, "/ban %s\r\n", text);
                    msleep(3000);
                    return;
                } else if ( !strcasecmp( com, "unban" )&& text != NULL && pb->hasop==1) {
                    pb->lockdown=0;
                    Send( s, "/unban %s\r\n", text );
                    msleep(3000);
                    return;
                } else if ( !strcasecmp( com, "kick" ) && text != NULL && pb->hasop==1) {
                    Send( s, "/kick %s\r\n", text );
                    msleep(3000);
                    return;
                } else if ( !strcasecmp( com, "join" ) && text!=NULL ) {
                    Send( s, "/join %s\r\n", text );
                    msleep(3000);
                    return;
                } else if ( !strcasecmp( com, "rejoin") ) {
                    Send( s, "/rejoin\r\n");
                    msleep(3000);
                    return;
                } else if ( !strcasecmp( com, "des" ) && pb->hasop==1) {
                    pb->op=0;
                    if (text!=NULL) {
                        Send( s, "/designate %s\r\n", text );
                        msleep(3000);
                    } else {
                        Send(s, "/designate %s\r\n", szSpeaker);
                        msleep(3000);
                    } return;
                } else if ( !strcasecmp( com, "resign" )  && pb->hasop==1) {
                    if (pb->des!=0) {
                        pb->des=0;
                        Send( s, "/resign\r\n");
                        msleep(3000);
                    } else {
                        Send(s, "No heir to your throne.\r\n");
                        msleep(3000);
                    } return;
                } else if ( !strcasecmp( com, "uptime") && pb->botNum == 0) {
                    Send( s, "/uptime\r\n");
                    msleep(3000);
                    return;
                } else if ( !strcasecmp( com, "op" ) && pb->hasop==1) {
                    pb->op=1;
                    if(text!=NULL)
                        Send( s, "/designate %s\r\n", text );
                    else
                        Send( s, "/designate %s\r\n", szSpeaker );
                    msleep(3000);
                    return;
                } else if ( !strcasecmp( com, "lock" ) && pb->hasop==1) {
                    pb->lockdown=1;
                    Send(s, "/topic %s (Channel Locked)\r\n", topic);
                    msleep(3000);
                    Send( s, "/who %s\r\n", pb->currChan);
                    msleep(3000);
                    return;
                } else if ( !strcasecmp( com, "unlock" )) {
                    pb->lockdown=0;
                    if (pb->hasop==1) {
                        Send(s, "/topic %s\r\n", topic);
                        msleep(3000);
                    } return;
                } else if ( !strcasecmp( com, "clean" ) && pb->hasop==1) {
                    Send( s, "/who %s\r\n", pb->currChan);
                    msleep(3000);
                    return;
                } else if ( !strcasecmp( com, "tag" ) && pb->hasop==1) {
                    if (text != NULL) {
                        pb->tban = 1;
                        memset(tag, '\0', sizeof(tag));
                        strcpy(tag, text);
                        Send( s, "/who %s\r\n", pb->currChan);
                        msleep(3000);
                    } else {
                        pb->tban = 0;
                        Send(s, "Tagban disabled.\r\n");
                        msleep(3000);
                    } return;
                } else if ( !strcasecmp( com, "contime") && pb->botNum == 0) {
                    Send( s, "/msg %s %d seconds to connect.\r\n",szSpeaker, pb->conTime);
                    msleep(3000);
                    return;
                } else if (!strcasecmp(com, "mem") && pb->botNum == 0) {
                    getrusage(RUSAGE_SELF,&r_usage);
                    Send(s, "/msg %s Memory usage: %ld bytes\r\n", szSpeaker, r_usage.ru_maxrss);
                    msleep(3000);
                    return;
                } else
                    return;
            }
        }
    }
}
void OnChannel(int s, struct data *pb, char *szEventText) {
    while ((*szEventText == ' '))
    	++szEventText;
    memset(pb->currChan, '\0', sizeof(pb->currChan));
    strcpy(pb->currChan, szEventText);
    if (!strcasecmp(szEventText, pb->channel)) {
        pb->chanham=0;
        Send(s, "/place\r\n");
        msleep(3000);
        return;
    }  else if (!strcasecmp(szEventText, "The Void")) {
        pb->chanham = 1;
        Send(s, "/place\r\n");
        msleep(3000);
        Send(s, "/join %s\r\n", backup);
        msleep(3000);
        return;
    } else
    	return;
}
void OnInfo(int s, struct data *pb, char *szEventText) {
    if(strstr(szEventText, "init 6"))
        return;
    else if(strstr(szEventText, "Topic:"))
        return;
    else if(strstr(szEventText, "set the topic"))
        return;
    else if(strstr(szEventText, "was kicked out of the channel"))
        return;
    else if(strstr(szEventText, "was banned by"))
        return;
    else if(strstr(szEventText, "was unbanned by"))
        return;
    else if(strstr(szEventText, "This channel does not have chat privileges"))
        return;
    else if(strstr(szEventText, "This is a chat channel"))
        return;
    else if(strstr(szEventText, "users in channel"))
        return;
    else if(strstr(szEventText, "kicked you out of the channel"))
        return;
    else if(strstr(szEventText, "is your new designated heir")) {
        pb->des=1;
        if(pb->op!=0 && pb->hasop!=0) {
            pb->op=0;
            Send(s, "/resign\r\n");
            msleep(3000);
        } return;
    } else if (strstr(szEventText,"You placed")) {
        char *tmp;
        char *pos;
        tmp=strtok_r(szEventText, " ", &pos);
        tmp=strtok_r(NULL, " ", &pos);
        tmp=strtok_r(NULL, " ", &pos);
        pb->place=atoi(tmp);
        return;
    } else if (strstr(szEventText,"Uptime")) {
        char *tmp;
        char *pos;
        tmp=strtok_r(szEventText, "\r\n", &pos);
        Send(s, "%s\r\n", tmp);
        msleep(3000);
        return;
    } else {
        char *pos;
        char *user1;
        char *user2;
        user1 = strtok_r(szEventText, ", ", &pos);
        if(user1!=NULL) {
            while ((*user1 == ' '))
                ++user1;
            OnJoin(s, pb, user1);
        }
        user2 = strtok_r(NULL, "\r\n", &pos);
        if(user2!=NULL) {
            while ((*user2 == ' '))
                ++user2;
            OnJoin(s, pb, user2);
        } return;
    }
}
void OnError(int s, struct data *pb, char *szEventText) {
    if (strstr(szEventText, "That channel is restricted"))
        return;
    if (strstr(szEventText, "Invalid user")){
        pb->des=0;
        Send(s, "Invalid User.\r\n");
        msleep(3000);
    } return;
}
void OnPing(int s, struct data *pb, char *szEventText) {
    if (pb->chanham==1) {
        Send(s, "/join %s\r\n", pb->channel);
        msleep(3000);
        return;
    } else {
        Send(s, "/PONG %s\r\n", szEventText);
        msleep(3000);
    } return;
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
        } return;
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
    } return;
}
int Send(int s, char *lpszFmt, ...) {
    char szOutStr[256];
    int try;
    va_list argptr;
    va_start(argptr, lpszFmt);
    vsprintf(szOutStr, lpszFmt, argptr);
    va_end(argptr);
    try = send(s, szOutStr, strlen(szOutStr), 0);
    return try;
}
void message_loop(int s, struct data *pb) {
    int n;
    int nBufLen=0;
    int nBufPos=0;
    char stageBuf[BUFFSIZE];
    struct timeval tv;
    if (s == INVALID_SOCKET)
        return;
    for (;;) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(s, &fds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        n = select(s+1, &fds, 0, 0, &tv);
        if (n) {
            int nNumToRead = BUFFSIZE-nBufLen-nBufPos;
            if (nNumToRead == 0) {
                memmove(stageBuf, stageBuf+nBufPos, nBufLen);
                nBufPos = 0;
                nNumToRead = BUFFSIZE-nBufLen;
            }
            n = recv(s, stageBuf+nBufPos+nBufLen, nNumToRead, 0);
            if (n <= 0) {
                return;
            }
            nBufLen += n;
            while (nBufLen > 0) {
                unsigned char *m = stageBuf+nBufPos;
                int nMsgLen=0;
                while (nMsgLen < nBufLen) {
                    if (m[nMsgLen] == '\n')
                        break;
                    nMsgLen++;
                }
                nMsgLen++;
                if (nMsgLen > nBufLen)
                    break;
                m[nMsgLen-1] = '\0';
                Dispatch(s, pb, m);
                nBufLen -= nMsgLen;
                nBufPos += nMsgLen;
            }
            if (!nBufLen)
                nBufPos = 0;
        }
        msleep(500);
    } return;
}
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
    FILE *cfg;
    int i;
    cfg = fopen("fuk.cfg", "w+");
    fprintf(cfg, "username=%s\n", username);
    fprintf(cfg, "password=%s\n", password);
    fprintf(cfg, "home=%s\n", channel);
    fprintf(cfg, "server=%s\n", server);
    fprintf(cfg, "port=%d\n", port);
    fprintf(cfg, "bind=%s\n", bindaddr);
    fprintf(cfg, "trigger=%s\n", trigger);
    fprintf(cfg, "threads=%d\n", threads);
    fprintf(cfg, "delay=%d\n", delay);
    fprintf(cfg, "scatter=%d\n", scatter);
    fprintf(cfg, "conwait=%d\n", conWait);
    fprintf(cfg, "banwait=%d\n", banWait);
    fprintf(cfg, "numbots=%d\n", numBots);
    fprintf(cfg, "backupchan=%s\n", backup);
    fprintf(cfg, "topic=%s\n", topic);
    fprintf(cfg, "\n");
    for (i = 0; i < masterSz; i++) {
        fprintf(cfg, "master=%s\n", master[i].id);
    }
    fprintf(cfg, "\n");
    for (i = 0; i < safeSz; i++) {
        fprintf(cfg, "safe=%s\n", safe[i].id);
    }
    fprintf(cfg, "\n");
    for (i = 0; i < desSz; i++) {
        fprintf(cfg, "des=%s\n", des[i].id);
    }
    fprintf(cfg, "\n");
    for (i = 0; i < shitSz; i++) {
        fprintf(cfg, "shit=%s\n", shit[i].id);
    }
    fclose(cfg);
}
int read_config() {
	FILE *cfg;
	int i = 0;
	int j = 0;
	int k = 0;
	int d = 0;
	int line = 0;
	cfg = fopen("fuk.cfg", "rb");
	if(!cfg) {
	    printf("Can't find fuk.cfg!\n");
        return -1;
	} else {
		char r[1024];
		while(fgets(r, 1024, cfg)) {
			line++;
			strtok(r, "\r\n");
            if(!memcmp(r, "username=", 9)) {
                memset(username, '\0', sizeof(username));
                strcpy(username, strdup(r+9));
            } else if(!memcmp(r, "password=", 9)) {
                memset(password, '\0', sizeof(password));
                strcpy(password, strdup(r+9));
            } else if(!memcmp(r, "home=", 5)) {
                memset(channel, '\0', sizeof(channel));
                strcpy(channel, strdup(r+5));
            } else if(!memcmp(r, "server=", 7)) {
                memset(server, '\0', sizeof(server));
                strcpy(server, strdup(r+7));
            } else if(!memcmp(r, "port=", 5)) {
                port = atoi(r+5);
            } else if(!memcmp(r, "bind=", 5)) {
                memset(bindaddr, '\0', sizeof(bindaddr));
                strcpy(bindaddr, strdup(r+5));
            } else if(!memcmp(r, "trigger=", 8)) {
                memset(trigger, '\0', sizeof(trigger));
                strcpy(trigger, strdup(r+8));
            } else if(!memcmp(r, "threads=", 8)) {
                threads = atoi(r+8);
            } else if(!memcmp(r, "delay=", 6)) {
                delay = atoi(r+6);
            } else if(!memcmp(r, "scatter=", 8)) {
                scatter = atoi(r+8);
            } else if (!memcmp(r, "backupchan=", 11)) {
                memset(backup, '\0', sizeof(backup));
                strcpy(backup, strdup(r+11));
            } else if (!memcmp(r, "topic=", 6)) {
                memset(topic, '\0', sizeof(topic));
                strcpy(topic, strdup(r+6));
            } else if(!memcmp(r, "banwait=", 8)) {
                banWait = atoi(r+8);
            } else if(!memcmp(r, "conwait=", 8)) {
                conWait = atoi(r+8);
            } else if(!memcmp(r, "numbots=", 8)) {
                numBots = atoi(r+8);
            } else if (!memcmp(r, "master=", 7)) {
                master = reallocarray(master, ++masterSz, sizeof(masterList));
                memset(master[i].id, '\0', sizeof(master[i].id));
                strcpy(master[i].id, strdup(r+7));
                i++;
            } else if (!memcmp(r, "safe=", 5)) {
                safe = reallocarray(safe, ++safeSz, sizeof(safeList));
                memset(safe[j].id, '\0', sizeof(safe[j].id));
                strcpy(safe[j].id, strdup(r+5));
                j++;
            } else if (!memcmp(r, "shit=", 5)) {
                shit = reallocarray(shit, ++shitSz, sizeof(shitList));
                memset(shit[k].id, '\0', sizeof(shit[k].id));
                strcpy(shit[k].id, strdup(r+5));
                k++;
            } else if (!memcmp(r, "des=", 4)) {
                des = reallocarray(des, ++desSz, sizeof(desList));
                memset(des[d].id, '\0', sizeof(des[d].id));
                strcpy(des[d].id, strdup(r+4));
                d++;
            }
		}
		fclose(cfg);
	} return 0;
}
int Connect(int s, struct timeval tv, struct data* pb) {
	fd_set fdr, fdw;
	int on=1, err=0, errlen=4;
	int sockinlen=16;
	struct sockaddr_in sockin;
	struct sockaddr_in name2;
	struct sockaddr_in name;
	name2.sin_family = AF_INET;
	name2.sin_port = INADDR_ANY;
	name.sin_family = AF_INET;
	name.sin_port = htons(pb->port);
	inet_pton(AF_INET, pb->server, &(name.sin_addr));
	inet_pton(AF_INET, bindaddr, &(name2.sin_addr));
	if (bind(s, (struct sockaddr *)&name2, sizeof(name2)) == -1)
	    return -1;
	set_nonblock(s);
	FD_ZERO(&fdr); FD_SET(s,&fdr); fdw=fdr;
	connect(s,(struct sockaddr *)&name,sizeof(name));
	select(s+1,&fdr,&fdw,NULL,(struct timeval *)&tv);
	if (FD_ISSET(s,&fdw)) {
		getsockopt(s,SOL_SOCKET,SO_ERROR,&err,&errlen);
	} else
	    err= -1;
	return err;
}
void *thread_conn(void *arg) {
    struct data* pb=(struct data *)arg;
	struct timeval tv;
	int s, off = 0;
	tv.tv_sec=0;
	startTime = time (NULL);
	if (delay > 0) {
		pb->delay2 = rand() % scatter + delay;
	}else {
		pb->delay2 = delay;
	}
	tv.tv_usec = pb->delay2 * 1000;
    while (__sync_bool_compare_and_swap(&pb->connected, 0, 0)) {
		s=socket(AF_INET, SOCK_STREAM, 0);
		int con = Connect(s, tv, pb);
		if (con == 0)
		    break;
		else
			close(s);
	}
    if (!__sync_bool_compare_and_swap(&pb->connected, 0, 1)) {
        close(s);
        pthread_exit(NULL);
	}
    Send(s, pb->logonPacket, sizeof(pb->logonPacket), 0);
    pb->conTime = time(NULL) - startTime;
    message_loop(s, pb);
    close(s);
    __sync_bool_compare_and_swap(&pb->connected, 1, 0);
    pthread_exit(NULL);
}
void create_threads(struct data *pb) {
    int err, numThreads;
    int i = 0;
    numThreads = numBots * threads;
    pthread_t thread[numThreads];
    for (int t = 0; t < numBots; t++, pb++) {
        char *thisBotsName = strdup(replace_str(username, "#", t));
        if (strcmp(pb->username, thisBotsName) != 0) {
            strcpy(pb->username, thisBotsName);
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
	    for (int z = 0; z < threads; z++, i++) {
            err = pthread_create(&thread[i], NULL, thread_conn, pb);
            if (err < 0)
                perror("pthread_create");
	    }
    }
    for (int t = 0; t < numThreads; t++) {
        err = pthread_join(thread[t], NULL);
        if (err < 0)
            perror("pthread_exit");
    }
    msleep(conWait * 1000);
}
int main() {
    printf("FuKeRy | v3.0\n");
    printf("PID: %d\n",getpid() + 1);
    if ((main_pid = fork())==-1) {
        printf("shutting down: unable to fork\n");
        exit(1);
    }
    if(main_pid!=0)
        return 0;
    int try_read = read_config();
    if (try_read != 0) {
        perror("Read config error.");
        exit(0);
    }
    pb = (struct data *)calloc(numBots, sizeof(struct data));
    while (1) {
        startTime = time (NULL);
        create_threads(pb);
    }
    free(pb);
}