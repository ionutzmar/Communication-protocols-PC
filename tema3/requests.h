#ifndef _REQUESTS_
#define _REQUESTS_

char *compute_get_request(char *host, char *url, char *url_params);
char *compute_request(char *protocol, char *host, char *url, char *url_params, char *header, char *form_data);

#endif
