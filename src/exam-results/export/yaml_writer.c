#define _CRT_SECURE_NO_WARNINGS
#include "writers.h"

#include <ctype.h>

static void write_key(FILE *f, const char *name)
{
    const unsigned char *p = (const unsigned char *)name;
    int plain = isalpha(*p) || *p == '_';
    for (; *p && plain; p++) {
        if (!isalnum(*p) && *p != '_') {
            plain = 0;
        }
    }
    if (plain) {
        fputs(name, f);
    } else {
        write_quoted(f, name);
    }
}

static void write_scalar(FILE *f, const Value *v)
{
    switch (v->type) {
    case VALUE_NULL:
        fputs("null", f);
        break;
    case VALUE_INTEGER:
    case VALUE_REAL:
        fputs(v->text, f);
        break;
    default:
        write_quoted(f, v->text);
    }
}

static void write_mapping(FILE *f, const DataTable *table, int row,
                          const ColumnNode *node, int level, int first_inline)
{
    for (int i = 0; i < node->child_count; i++) {
        const ColumnNode *child = &node->children[i];
        if (!(i == 0 && first_inline)) {
            write_indent(f, level);
        }
        write_key(f, child->name);

        if (child->column >= 0) {
            fputs(": ", f);
            write_scalar(f, data_table_value(table, row, child->column));
            fputc('\n', f);
        } else {
            fputs(":\n", f);
            write_mapping(f, table, row, child, level + 1, 0);
        }
    }
}

int write_yaml(const DataTable *table, const char *path)
{
    FILE *f = fopen(path, "wb");
    if (f == NULL) {
        return -1;
    }

    if (table->row_count == 0) {
        fputs("[]\n", f);
    }
    for (int r = 0; r < table->row_count; r++) {
        fputs("- ", f);
        write_mapping(f, table, r, &table->root, 1, 1);
    }

    return fclose(f) == 0 ? 0 : -1;
}
