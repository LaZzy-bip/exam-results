#include "service_error.h"
#include "../common/str_utils.h"

int service_fail(char *err, const char *message)
{
    if (err != NULL) {
        str_copy(err, ERR_LEN, message);
    }
    return SERVICE_ERROR;
}
