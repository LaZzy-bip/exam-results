#define _CRT_SECURE_NO_WARNINGS
#include "writers.h"

#include <string.h>

#define CSV_DELIMITER ';'

static void write_field(FILE *f, const char *s)
{
    if (strchr(s, CSV_DELIMITER) == NULL && strpbrk(s, "\"\r\n") == NULL) {
        fputs(s, f);
        return;
    }
    fputc('"', f);
    for (const char *p = s; *p; p++) {
        if (*p == '"') {
            fputc('"', f);
        }
        fputc(*p, f);
    }
    fputc('"', f);
}

int write_csv(const DataTable *table, const char *path)
{
    FILE *f = fopen(path, "wb");
    if (f == NULL) {
        return -1;
    }

    fputs("\xEF\xBB\xBF", f);

    for (int c = 0; c < table->column_count; c++) {
        if (c > 0) {
            fputc(CSV_DELIMITER, f);
        }
        write_field(f, table->column_names[c]);
    }
    fputs("\r\n", f);

    for (int r = 0; r < table->row_count; r++) {
        for (int c = 0; c < table->column_count; c++) {
            if (c > 0) {
                fputc(CSV_DELIMITER, f);
            }
            const Value *v = data_table_value(table, r, c);
            if (v->type != VALUE_NULL) {
                write_field(f, v->text);
            }
        }
        fputs("\r\n", f);
    }

    return fclose(f) == 0 ? 0 : -1;
}
