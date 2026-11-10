#ifndef SERVICE_ERROR_H
#define SERVICE_ERROR_H

#define ERR_LEN 256

#define SERVICE_OK    0
#define SERVICE_ERROR (-1)

int service_fail(char *err, const char *message);

#endif
