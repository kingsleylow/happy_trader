#ifndef _MTQUEUE_PARSEURL_INCLUDED
#define _MTQUEUE_PARSEURL_INCLUDED


/*
 * URL storage
 */
struct parsed_url {
    char *scheme;               /* mandatory */
    char *host;                 /* mandatory */
    char *port;                 /* optional */
    char *path;                 /* optional */
    char *query;                /* optional */
    char *fragment;             /* optional */
    char *username;             /* optional */
    char *password;             /* optional */
};

#ifdef __cplusplus
extern "C" {
#endif

    /*
     * Declaration of function prototypes
     */
    struct parsed_url * parse_url(const char *);

    void parsed_url_free(struct parsed_url *);

#ifdef __cplusplus
}
#endif

#endif /* _MTQUEUE_PARSEURL_INCLUDED */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */

