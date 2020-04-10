/* simple.c
** libstrophe XMPP client library -- basic usage example
**
** Copyright (C) 2005-2009 Collecta, Inc.
**
**  This software is provided AS-IS with no warranty, either express
**  or implied.
**
**  This software is distributed under license and may not be copied,
**  modified or distributed except as expressly authorized under the
**  terms of the license contained in the file LICENSE.txt in this
**  distribution.
*/

/* simple message example
**
** This example was provided by Elmo Todurov <tod...@gmail.com>
**
** This progam just sends a simple message to the given Jabber ID.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <strophe.h>


/** Send a single text message.
 *  This function sends a simple text message to a given recipient.
 *
 *  @param conn a Strophe connection object
 *  @param to the JID of the recipient
 *  @param message the text of the message to send
 *
 *  @ingroup Connections
 */

char check = 'i';
struct msgdata {
    char* to;
    char* msg;
};


void xmpp_send_message(xmpp_conn_t *conn,
      const char * const to,
      const char * const message)
{
   xmpp_stanza_t *msg, *body, *text;
   xmpp_ctx_t *ctx = xmpp_conn_get_context(conn);

   msg = xmpp_stanza_new(ctx);
   xmpp_stanza_set_name(msg, "message");
   xmpp_stanza_set_type(msg, "chat");
   xmpp_stanza_set_attribute(msg, "to", to);

   body = xmpp_stanza_new(ctx);
   xmpp_stanza_set_name(body, "body");

   text = xmpp_stanza_new(ctx);
   xmpp_stanza_set_text(text, message);
   xmpp_stanza_add_child(body, text);
   xmpp_stanza_add_child(msg, body);

   xmpp_send(conn, msg);
   xmpp_stanza_release(msg);
}

void xmpp_send_iq(xmpp_conn_t *conn,
      const char * const to,
      const char * const message)
{
   xmpp_stanza_t *msg, *body, *text, *query;
   xmpp_ctx_t *ctx = xmpp_conn_get_context(conn);

   msg = xmpp_stanza_new(ctx);
   xmpp_stanza_set_name(msg, "iq");
   xmpp_stanza_set_type(msg, "get");
   xmpp_stanza_set_attribute(msg, "to", "sasi@localhost");
   xmpp_stanza_set_attribute(msg, "from", "shashi@localhost");
   xmpp_stanza_set_attribute(msg, "id", "version_1");
   query = xmpp_stanza_new(ctx);
   xmpp_stanza_set_name(query, "query");
   xmpp_stanza_set_ns(query, "jabber:iq:version");
   xmpp_stanza_add_child(msg, query);
   xmpp_send(conn, msg);
   xmpp_stanza_release(msg);
}


/* define a handler for connection events */
void conn_handler(xmpp_conn_t * const conn, const xmpp_conn_event_t status,
          const int error, xmpp_stream_error_t * const stream_error,
          void * const userdata)
{
    xmpp_ctx_t *ctx = xmpp_conn_get_context(conn);
    struct msgdata* msg = (struct msgdata*)userdata;

    if (status == XMPP_CONN_CONNECT) {
    xmpp_stanza_t* pres;
    fprintf(stderr, "DEBUG: connected\n");

    /* Send initial <presence/> so that we appear online to contacts */
    pres = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(pres, "presence");
    xmpp_send(conn, pres);
    xmpp_stanza_release(pres);

    /* Send a message */
    //xmpp_send_simple_message(conn, msg->to, msg->msg);
    if(check == 'i')
    {
        xmpp_send_iq(conn, msg->to, msg->msg);
    }
    else
    {
        xmpp_send_message(conn, msg->to, msg->msg);
    }
    free(msg);

    /* Causes the conn_handler to be executed, where the event loop gets shut down */
    xmpp_disconnect(conn);
    }
    else {
    fprintf(stderr, "DEBUG: disconnected\n");
    xmpp_stop(ctx);
    }
}

int main(int argc, char **argv)
{
    xmpp_ctx_t *ctx;
    xmpp_conn_t *conn;
    xmpp_log_t *log;
    char *jid, *pass;
    long flags = 0;

    /* take a jid and password on the command line */
    if (argc != 5) {
    fprintf(stderr, "Usage: ./simple <jid> <pass> <to> <message>\n\n");
    return 1;
    }
    check = *argv[1];
    jid = argv[1];
    pass = argv[2];
    flags |= XMPP_CONN_FLAG_DISABLE_TLS;

    /* init library */
    xmpp_initialize();

    /* create a context */
    log = xmpp_get_default_logger(XMPP_LEVEL_DEBUG); /* pass XMPP_LEVEL_ERROR instead to silence output */
    ctx = xmpp_ctx_new(NULL, log);

    /* create a connection */
    conn = xmpp_conn_new(ctx);

    /* configure connection properties (optional) */
    xmpp_conn_set_flags(conn, flags);

    /* setup authentication information */
    xmpp_conn_set_jid(conn, jid);
    xmpp_conn_set_pass(conn, pass);

    /* Prepare the message for sending */
    struct msgdata* msg = (struct msgdata*)malloc(sizeof(struct msgdata));
    msg->to = argv[3];
    msg->msg = argv[4];

    /* initiate connection */
    xmpp_connect_client(conn, NULL, 0, conn_handler, msg);

    /* enter the event loop -
       our connect handler will trigger an exit */
    xmpp_run(ctx);

    /* release our connection and context */
    xmpp_conn_release(conn);
    xmpp_ctx_free(ctx);

    /* final shutdown of the library */
    xmpp_shutdown();

    return 0;
} 