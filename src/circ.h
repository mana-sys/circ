#ifndef CIRC_H
#define CIRC_H


#define RPL_WELCOME 1
#define ERR_ALREADYREGISTRED 462
#define ERR_NONICKNAMEGIVEN 431
#define ERR_ERRONEUSNICKNAME 432
#define ERR_NEEDMOREPARAMS 461
#define NICKNAME_SIZE 9

enum circ_msg_type {
    MSGUNKNOWN,
    MSGUSER,
    MSGNICK,
    MSGPRIVMSG,
};

struct circ_msg_nick {
    char *nick;
};

struct circ_msg_user {
    char *username;
    char *fullname;
};

struct circ_msg_privmsg {
    char *msgtarget;
    char *text;
};

struct circ_msg {
    enum circ_msg_type msgType;
    union {
        struct circ_msg_nick msgNick;
        struct circ_msg_user msgUser;
        struct circ_msg_privmsg msgPrivmsg;
    };
};

int parse_msg(char *buf, struct circ_msg *msg);


#endif
