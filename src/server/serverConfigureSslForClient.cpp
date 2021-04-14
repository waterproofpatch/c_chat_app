#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#include <openssl/evp.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/x509.h>

#include "serverConfigureSslForClient.hpp"
#include "server.hpp"
#include "wrappers.hpp"
#include "debug.hpp"

/* cert files */
static char gCertFilename[] = "cert.pem";
static char gKeyFilename[]  = "key.pem";

/**
 * @brief Initialize the openSSL library.
 *
 */
static void init_openssl()
{
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

/**
 * @brief Cleanup the openSSL library.
 *
 */
static void cleanup_openssl()
{
    EVP_cleanup();
}

/**
 * @brief Create openSSL context.
 *
 * @return SSL_CTX* new context.
 */
SSL_CTX *create_context()
{
    const SSL_METHOD *method = NULL;
    SSL_CTX *         ctx    = NULL;

    method = SSLv23_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx)
    {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

/**
 * @brief Configure an SSL context.
 *
 * @param ctx to configure.
 * @param certFilename path to the certificate PEM file.
 * @param keyFilename path to the key PEM file.
 */
static void configure_context(SSL_CTX *ctx,
                              char *   certFilename,
                              char *   keyFilename)
{
    SSL_CTX_set_ecdh_auto(ctx, 1);

    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(ctx, certFilename, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, keyFilename, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

proto_err_t serverConfigureSslForClient(user_t *user)
{
    SSL_CTX *ctx = NULL;
    SSL *    ssl = NULL;

    init_openssl();
    ctx = create_context();
    if (!ctx)
    {
        DBG_ERROR("Failed creating SSL context\n");
        return ERR_SSL;
    }

    configure_context(ctx, gCertFilename, gKeyFilename);

    ssl = SSL_new(ctx);
    if (!ssl)
    {
        DBG_ERROR("Failed creating SSL object\n");
        return ERR_SSL;
    }

    SSL_set_fd(ssl, user->client_socket_fd);

    DBG_INFO("Waiting for client SSL accept...\n");
    if (SSL_accept(ssl) <= 0)
    {
        ERR_print_errors_fp(stderr);
        cleanup_openssl();
        return ERR_SSL;
    }

    user->ssl = ssl;
    return OK;
}