#ifndef SERVICE_ERROR_H
#define SERVICE_ERROR_H

/*
 * В C нет исключений, поэтому сервисы сообщают об ошибке так:
 * функция возвращает SERVICE_ERROR и записывает текст ошибки в буфер err
 * (аналог raise Exception("...") из методички). Интерфейс выводит этот текст.
 */
#define ERR_LEN 256

#define SERVICE_OK    0
#define SERVICE_ERROR (-1)

/* Записывает сообщение в err и возвращает SERVICE_ERROR. */
int service_fail(char *err, const char *message);

#endif /* SERVICE_ERROR_H */
