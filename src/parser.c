#include <stdio.h>
#include <irc.h>

#include "codes.h"
#include "msgtok.h"
#include "parser.h"
#include "log.h"

#define NICK_MAX_SIZE 9

static int parse_message_nick    (struct irc_message *message, char *saveptr);
static int parse_message_user    (struct irc_message *message, char *saveptr);
static int parse_message_ping    (struct irc_message *message, char *saveptr);
static int parse_message_pong    (struct irc_message *message, char *saveptr);
static int parse_message_privmsg (struct irc_message *message, char *saveptr);
static int parse_message_motd    (struct irc_message *message, char *saveptr);
static int parse_message_lusers  (struct irc_message *message, char *saveptr);
static int parse_message_whois   (struct irc_message *message, char *saveptr);
static int parse_message_join    (irc_message_s *message, char *saveptr);
static int parse_message_part    (irc_message_s *message, char *saveptr);
static int parse_message_list    (irc_message_s *message, char *saveptr);
static int parse_message_topic   (irc_message_s *message, char *saveptr);
static int parse_message_names   (irc_message_s *message, char *saveptr);

static int parse_message_away(irc_message_s *, char *);


int parse_message(char *buf, struct irc_message *message)
{
    size_t toklen;
    char *tok, *saveptr;

    tok = msgtok_r(buf, &toklen, &saveptr);
    if (tok != NULL && toklen != 0) {
        message->command = tok;
        if (strcmp(tok, "NICK") == 0) {
            return parse_message_nick(message, saveptr);
        } else if (strcmp(tok, "USER") == 0) {
            return parse_message_user(message, saveptr);
        } else if (strcmp(tok, "PING") == 0) {
            return parse_message_ping(message, saveptr);
        } else if (strcmp(tok, "PONG") == 0) {
            return parse_message_pong(message, saveptr);
        } else if (strcmp(tok, "MOTD") == 0) {
            return parse_message_motd(message, saveptr);
        } else if (strcmp(tok, "PRIVMSG") == 0) {
            return parse_message_privmsg(message, saveptr);
        } else if (strcmp(tok, "LUSERS") == 0) {
            return parse_message_lusers(message, saveptr);
        } else if (strcmp(tok, "WHOIS") == 0) {
            return parse_message_whois(message, saveptr);
        } else if (strcmp(tok, "JOIN") == 0) {
            return parse_message_join(message, saveptr);
        } else if (strcmp(tok, "PART") == 0) {
            return parse_message_part(message, saveptr);
        } else if (strcmp(tok, "LIST") == 0) {
            return parse_message_list(message, saveptr);
        } else if (strcmp(tok, "TOPIC") == 0) {
            return parse_message_topic(message, saveptr);
        } else if (strcmp(tok, "NAMES") == 0) {
            return parse_message_names(message, saveptr);
        } else if (strcmp(tok, "AWAY") == 0) {
            return parse_message_away(message, saveptr);
        } else {
            message->type = UNKNOWN;
            return 0;
        }
    }

    fprintf(stderr, "Error parsing message: invalid message buffer.\n");

    return -1;
}

/**
 * Parses a NICK message into the specified message struct. This function
 * expects saveptr to be pointing to the first token in the message after
 * the NICK keyword.
 *
 * @param message
 * @param saveptr
 * @return
 */
static int parse_message_nick(struct irc_message *message, char *saveptr) {
    size_t toklen;
    char *tok;

    message->type = NICK;

    // Parse the <nickname> parameter.
    tok = msgtok_r(NULL, &toklen, &saveptr);
    if (tok == NULL || toklen == 0) {
        message->parse_err = ERR_NONICKNAMEGIVEN;
        return ERR_NONICKNAMEGIVEN;
    } else if (toklen > NICK_MAX_SIZE) {
        message->parse_err = ERR_ERRONEUSNICKNAME;
        return ERR_ERRONEUSNICKNAME;
    }

    Log_Trace("Parsed nick is %s", tok);
    message->message.nick.nick = tok;
    message->parse_err = 0;

    return 0;

}


/**
 * Parses a USER message into the specified message struct. This function
 * expects saveptr to be pointing to the first token in the message after
 * the USER keyword.
 *
 * @param message
 * @param saveptr
 * @return
 */
static int parse_message_user(struct irc_message *message, char *saveptr)
{
    size_t toklen;
    char * tok;

    message->type = USER;

    // Parse the <user> parameter.
    tok = msgtok_r(NULL, &toklen, &saveptr);

    if (tok == NULL || toklen == 0) {
        message->parse_err = ERR_NEEDMOREPARAMS;
        return ERR_NEEDMOREPARAMS;
    }

    message->message.user.username = tok;

    // Skip the <mode> parameter.
    if ((msgtok_r(NULL, &toklen, &saveptr)) == NULL) {
        message->parse_err = ERR_NEEDMOREPARAMS;
        return ERR_NEEDMOREPARAMS;
    }

    // Skip the <user> parameter.
    if ((msgtok_r(NULL, &toklen, &saveptr)) == NULL) {
        message->parse_err = ERR_NEEDMOREPARAMS;
        return ERR_NEEDMOREPARAMS;
    }

    // Parse the <realname> parameter.
    if ((tok = msgtok_r(NULL, &toklen, &saveptr)) == NULL) {
        message->parse_err = ERR_NEEDMOREPARAMS;
        return ERR_NEEDMOREPARAMS;
    }

    message->message.user.fullname = tok;
    message->parse_err = 0;

    return 0;

}

static int parse_message_privmsg(struct irc_message *message, char *saveptr)
{
    size_t toklen;
    char * tok;

    message->type = PRIVMSG;

    // Parse the <msgtarget> parameter.
    tok = msgtok_r(NULL, &toklen, &saveptr);

    if (tok == NULL || toklen == 0) {
        message->parse_err = ERR_NORECIPIENT;
        return ERR_NORECIPIENT;
    }

    message->message.privmsg.msgtarget = tok;

    if ((tok = msgtok_r(NULL, &toklen, &saveptr)) == NULL) {
        message->parse_err = ERR_NOTEXTTOSEND;
        return ERR_NOTEXTTOSEND;
    }

    message->message.privmsg.contents = tok;
    message->parse_err = 0;

    return 0;
}


static int parse_message_ping(struct irc_message *message, char *saveptr)
{
    message->type = PING;
    return 0;
}

static int parse_message_pong(struct irc_message *message, char *saveptr)
{
    message->type = PONG;
    return 0;
}

static int parse_message_motd(struct irc_message *message, char *saveptr)
{
    message->type = MOTD;
    return 0;
}

static int parse_message_lusers(struct irc_message *message, char *saveptr)
{
    message->type = LUSERS;
    return 0;
}

static int parse_message_whois(struct irc_message *message, char *saveptr)
{
    size_t toklen;
    char * tok;

    message->type = WHOIS;

    // Parse the <mask> parameter.
    message->message.whois.mask = msgtok_r(NULL, &toklen, &saveptr);

    return 0;
}


static int parse_message_join(irc_message_s *message, char *saveptr)
{
    size_t toklen;
    char *tok;

    message->type = JOIN;

    // Parse the list of channels.
    tok = msgtok_r(NULL, &toklen, &saveptr);

    if (tok == NULL || toklen == 0) {
        message->parse_err = ERR_NEEDMOREPARAMS;
        return ERR_NEEDMOREPARAMS;
    }

    if (strcmp(tok, "0") == 0) {
        message->message.join.leave_all = 1;
        return 0;
    }

    message->message.join.channels = tok;
    message->parse_err = 0;
    return 0;

}


static int parse_message_part(irc_message_s *message, char *saveptr)
{
    size_t toklen;
    char * tok;

    message->type = PART;

    /*
     * Parse the list of <channel> parameters.
     */
    tok = msgtok_r(NULL, &toklen, &saveptr);

    if (tok == NULL || toklen == 0) {
        message->parse_err = ERR_NEEDMOREPARAMS;
        return ERR_NEEDMOREPARAMS;
    }

    /*
     * Parse the optional <Part Message> parameter.
     */
    message->message.part.channels = tok;

    tok = msgtok_r(NULL, &toklen, &saveptr);
    if (tok == NULL || toklen == 0) {
        message->message.part.part_message = NULL;
        goto done;
    }

    message->message.part.part_message = tok;

    done:
    message->parse_err = 0;
    return 0;
}


static int parse_message_list(irc_message_s *message, char *saveptr)
{
    size_t toklen;
    char * tok;

    message->type = LIST;

    tok = msgtok_r(NULL, &toklen, &saveptr);

    /*
     * Parameter is optional.
     */
    if (tok == NULL || toklen == 0) {
        return 0;
    }

    message->message.list.channels = tok;
    message->parse_err = 0;

    /*
     * TODO: Add support for <server> parameter.
     */
    return 0;
}


static int parse_message_topic(irc_message_s *message, char *saveptr)
{
    size_t toklen;
    char * tok;

    message->type = TOPIC;

    /*
     * Parse the <channel> parameter.
     */
    tok = msgtok_r(NULL, &toklen, &saveptr);

    if (tok == NULL || toklen == 0) {
        message->parse_err = ERR_NEEDMOREPARAMS;
        return ERR_NEEDMOREPARAMS;
    }

    message->message.topic.channel = tok;

    /*
     * Parse the <topic> parameter.
     */
    tok = msgtok_r(NULL, &toklen, &saveptr);

    /*
     * The empty string ("") is a valid value for the <topic> parameter.
     */
    if (tok == NULL)
        goto done;

    message->message.topic.topic = tok;

    done:
    message->parse_err = 0;
    return 0;
}


static int parse_message_names(irc_message_s *message, char *saveptr)
{
    char *tok;
    size_t toklen;

    message->type = NAMES;

    /*
     * Optional comma-delimited list of channels was specified, so parse it.
     */
    if ((tok = msgtok_r(NULL, &toklen, &saveptr)))
        message->message.names.channels = tok;
    else
        message->message.names.channels = NULL;

    return 0;
}


static int parse_message_away(irc_message_s *message, char *saveptr)
{
    char *tok;
    size_t toklen;

    message->type = AWAY;

    /*
     * Check for optional <text> parameter.
     */
    if ((tok = msgtok_r(NULL, &toklen, &saveptr)))
        message->message.away.text = tok;
    else
        message->message.away.text = NULL;

    /*
     * Parsing the message always succeeds.
     */
    return 0;

}