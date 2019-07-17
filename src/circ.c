#include <stdio.h>
#include "circ.h"
#include "msgtok.h"

typedef int circ_parse_msg_fn(struct circ_msg *);

static int parse_msg_nick(struct circ_msg *msg);
static int parse_msg_user(struct circ_msg *msg);
static int parse_msg_privmsg(struct circ_msg *msg);

static circ_parse_msg_fn *parse_msg_fns[2] = {
    parse_msg_user,
    parse_msg_nick
};

int parse_msg_nick(struct circ_msg *msg)
{
    size_t toklen;
    char *tok = msgtok(NULL, &toklen);

    msg->msgType = MSGNICK;

    // If no nickname parameter given, then reply with
    // ERR_NONICKNAMEGIVEN
    if (tok == NULL || toklen == 0) {
        return ERR_NONICKNAMEGIVEN;
    } else if (toklen > NICKNAME_SIZE) {
        return ERR_ERRONEUSNICKNAME;
    }

    msg->msgNick.nick = tok;

    return 0;
}

static int parse_msg_user(struct circ_msg *msg)
{
    return 0;
}


static int parse_user_cmd(struct circ_msg *msg)
{
    size_t toklen;
    char *tok = msgtok(NULL, &toklen);
    int err;

    msg->msgType = MSGUSER;

    if (tok == NULL || toklen == 0) {
        err = ERR_NEEDMOREPARAMS;
        goto err;
    }

//    strncpy(username, tok, toklen);
//    username[toklen + 1] = '\0';
    msg->msgUser.username = tok;

    if (msgtok(NULL, &toklen) == NULL) {
        err = ERR_NEEDMOREPARAMS;
        goto err;
    }

    if (msgtok(NULL, &toklen) == NULL) {
        err = ERR_NEEDMOREPARAMS;
        goto err;
    }

    if ((tok = msgtok(NULL, &toklen)) == NULL) {
        err = ERR_NEEDMOREPARAMS;
        goto err;
    }

//    strncpy(fullname, tok, toklen);
//    fullname[toklen + 1] = '\0';
    msg->msgUser.fullname = tok;
    return 0;

    err:
        return err;

}

static int parse_msg_privmsg(struct circ_msg *msg)
{
    size_t toklen;
    char *tok = msgtok(NULL, &toklen);
    int err;

    msg->msgType = MSGPRIVMSG;





    return 0;
}


int parse_msg(char *buf, struct circ_msg *msg)
{
    size_t toklen;
    char *tok = msgtok(buf, &toklen);
    if (tok != NULL && toklen != 0) {
        if (strncasecmp(tok, "NICK", toklen) == 0) {
            return parse_msg_nick(msg);
        } else if (strncasecmp(tok, "USER", toklen) == 0) {
            return parse_user_cmd(msg);
        } else {
            msg->msgType = MSGUNKNOWN;
            return 0;
        }
    } else {
        fprintf(stderr, "failed to parse\n");
    }


    return 0;
}
