#ifndef IRC_H
#define IRC_H

// Specifies the type of the IRC message from the client.
enum irc_message_type {
    INVALID = -1,
    UNKNOWN,
    USER,
    NICK,
    PRIVMSG,
    NOTICE,
    PING,
    PONG,
    MOTD,
    LUSERS,
    WHOIS,
    JOIN,
    PART,
    TOPIC,
    LIST,
    NAMES,
    AWAY,
};

struct irc_message_nick {
    const char *nick;
};

struct irc_message_user {
    const char *username;
    const char *fullname;
};

struct irc_message_privmsg {
    char *msgtarget;
    char *contents;
};

struct irc_message_lusers {
    char *mask;
    char *target;
};

struct irc_message_whois {
    char *target;
    char *mask;
};

struct irc_message_join {
    char *  channels;   /* Comma-delimited list of channels. */
    char *  keys;
    int     leave_all;
};

struct irc_message_part {
    char *channels;     /* Comma-delimited list of channels. */
    char *part_message;
};

struct irc_message_list {
    char *channels; /* Comma-delimited list of channels. */
    char *server;
};

struct irc_message_topic {
    char *channel;
    char *topic;
};

struct irc_names {
    char *channels; /* Comma-delimited list of channels. */
};

struct irc_away {
    char *text;
};

typedef struct irc_message {
    enum irc_message_type           type;
    char *                          command;
    int                             parse_err;
    union {
        struct irc_message_nick     nick;
        struct irc_message_user     user;
        struct irc_message_privmsg  privmsg;
        struct irc_message_lusers   lusers;
        struct irc_message_whois    whois;
        struct irc_message_join     join;
        struct irc_message_part     part;
        struct irc_message_list     list;
        struct irc_message_topic    topic;
        struct irc_names            names;
        struct irc_away away;
    }                               message;
} irc_message_s;

#endif
