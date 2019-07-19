#ifndef IRC_H
#define IRC_H

// Specifies the type of the IRC message from the client.
enum irc_message_type {
    UNKNOWN = 0,
    USER,
    NICK,
    PRIVMSG,
    NOTICE,
    PING,
    PONG,
    MOTD,
    LUSERS,
    WHOIS
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

struct irc_message {
    enum irc_message_type type;
    union {
        struct irc_message_nick nick;
        struct irc_message_user user;
        struct irc_message_privmsg privmsg;
    } message;
};

struct irc_reply {

};



#endif IRC_H
