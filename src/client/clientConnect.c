#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "clientConnect.h"
#include "errorCodes.h"
#include "wrappers.h"
#include "user.h"
#include "debug.h"

extern user_t *gUser;   // socket descriptor for the client connection

SSL_CTX *InitCTX(void)
{
    const SSL_METHOD *method;
    SSL_CTX *         ctx;
    OpenSSL_add_all_algorithms(); /* Load cryptos, et.al. */
    SSL_load_error_strings();     /* Bring in and register error messages */
    method = TLS_client_method(); /* Create new client-method instance */
    ctx    = SSL_CTX_new(method); /* Create new context */
    if (ctx == NULL)
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}

void ShowCerts(SSL *ssl)
{
    X509 *cert;
    char *line;
    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if (cert != NULL)
    {
        DBG_INFO("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        DBG_INFO("Subject: %s\n", line);
        free(line); /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        DBG_INFO("Issuer: %s\n", line);
        free(line);      /* free the malloc'ed string */
        X509_free(cert); /* free the malloc'ed certificate copy */
    }
    else
        DBG_INFO("Info: No client certificates configured.\n");
}

proto_err_t clientConnect(char *hostname, unsigned short port_no)
{
    struct sockaddr_in server_address = {0};
    SSL_CTX *          ctx            = InitCTX();

    if (!ctx)
    {
        DBG_ERROR("Failed initing ctx...");
        return ERR_SSL;
    }

    // initialize the server address structure
    memset(&server_address, 0, sizeof(struct sockaddr_in));
    server_address.sin_family = AF_INET;
    server_address.sin_port   = htons(port_no);
    if (inet_pton(AF_INET, hostname, &server_address.sin_addr) <= 0)
    {
        DBG_ERROR("inet_pton error occured: %s\n", strerror(errno));
        return ERR_GENERAL;
    }

    // create a socket to handle data
    gUser = (user_t *)wrappers_malloc(sizeof(*gUser));
    if (!gUser)
    {
        DBG_ERROR("Error creating user!\n");
        return ERR_NO_MEM;
    }

    gUser->client_socket_fd = wrappers_socket(AF_INET, SOCK_STREAM, 0);
    if (gUser->client_socket_fd < 0)
    {
        DBG_ERROR("ERROR opening socket\n");
        wrappers_free(gUser);
        gUser = NULL;
        return ERR_NETWORK_FAILURE;
    }

    // connect to the server address using the socket we created TODO use
    // wrappers
    if (wrappers_connect(gUser->client_socket_fd,
                         (struct sockaddr *)&server_address,
                         sizeof(struct sockaddr_in)) < 0)
    {
        DBG_ERROR("Error connecting: %s\n", strerror(errno));
        wrappers_free(gUser);
        gUser = NULL;
        return ERR_NETWORK_FAILURE;
    }

    gUser->ssl = SSL_new(ctx); /* create new SSL connection state */
    if (gUser->ssl == NULL)
    {
        DBG_INFO("Error creating SSL CTX\n");
        return ERR_SSL;
    }
    SSL_set_fd(gUser->ssl,
               gUser->client_socket_fd); /* attach the socket descriptor */

    /* perform the connection */
    if (SSL_connect(gUser->ssl) == -1)
    {
        ERR_print_errors_fp(stderr);
        wrappers_free(gUser);
        gUser = NULL;
        return ERR_NETWORK_FAILURE;
    }
    return OK;
}