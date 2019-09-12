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
    JOIN
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
    char *channels;
    char *keys;
    int leave_all;
};

typedef struct irc_message {
    enum irc_message_type type;
    char *command;
    int parse_err;
    union {
        struct irc_message_nick nick;
        struct irc_message_user user;
        struct irc_message_privmsg privmsg;
        struct irc_message_lusers lusers;
        struct irc_message_whois whois;
        struct irc_message_join join;
    } message;
} irc_message_s;

#endif
