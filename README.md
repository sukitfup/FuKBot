# Bot Commands Documentation

## Overview

This document provides an overview of the available commands for the bot. The bot operates in an IRC-like environment and responds to specific commands that allow configuration, user management, and interaction.

## Command List

### Configuration Commands

| Command | Description |
|---------|-------------|
| `list (master, des, safe, shit)` | Lists entries. |
| `add (master, des, safe, shit)` | Adds an entry to a list. |
| `remove (master, des, safe, shit)` | Removes an entry from a list. |
| `trigger (set, show)` | Sets or displays the bot's trigger. |
| `home (set, show)` | Sets or displays the bot's home channel. |
| `topic (set, show)` | Sets or displays the channel topic. |
| `backup (set, show)` | Sets or displays the backup channel. |
| `server (show)` | Displays the current server. |
| `threads (set, show)` | Sets or displays the number of threads. |
| `port (set, show)` | Sets or displays the connection port. |
| `delay (set, show)` | Sets or displays the timeout for each connection attempt. |
| `scatter (set, show)` | Scatter randomly adds 0-x to the delay. |
| `banwait (set, show)` | Sets or displays the time the bot waits after each ban/kick. |
| `conwait (set, show)` | Sets or displays the time that the bot waits to reconnect on disconnect. |
| `ver (show)` | Displays the bot version. |
| `quit (bot exit)` | Commands the bot to quit. |
| `recon (bot reconnect)` | Commands the bot to reconnect. |

### Interaction Commands

| Command | Description |
|---------|-------------|
| `say (message)` | Makes the bot say a message. |
| `ping (server latency check)` | Pings the server to check latency. |
| `place (show login status)` | Displays the bot's login status. |
| `contime (show time it took to connect)` | Displays the bot's connection time in seconds. |
| `mem (show usage)` | Displays the bot's memory usage. |

### User Management Commands

| Command | Description |
|---------|-------------|
| `ban (user)` | Bans a user from the server. |
| `unban (user)` | Unbans a user from the server. |
| `kick (user)` | Kicks a user from the server. |
| `join (channel)` | Joins a new channel. |
| `rejoin (channel)` | Rejoins the current channel. |
| `resign (bot control)` | Resigns bot control. |
| `uptime (show)` | Displays the server uptime. |
| `op (give op to someone)` | Designates and resigns. |
| `lock (channel)` | Locks the channel. |
| `unlock (channel)` | Unlocks the channel. |
| `clean (configurations)` | Cleans up channel without locking. |
| `tag (set, remove for bans)` | Sets or removes a tag for quick temp bans. |

## Usage

Commands can be issued by prefixing them with the bot's trigger (default: `?`).


