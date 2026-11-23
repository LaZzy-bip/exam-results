#define _CRT_SECURE_NO_WARNINGS
#include "writers.h"

void write_indent(FILE *f, int level)
{
    for (int i = 0; i < level; i++) {
        fputs("  ", f);
    }
}

void write_quoted(FILE *f, const char *s)
{
    fputc('"', f);
    for (const unsigned char *p = (const unsigned char *)s; *p; p++) {
        switch (*p) {
        case '"':  fputs("\\\"", f); break;
        case '\\': fputs("\\\\", f); break;
        case '\n': fputs("\\n", f);  break;
        case '\r': fputs("\\r", f);  break;
        case '\t': fputs("\\t", f);  break;
        case '\b': fputs("\\b", f);  break;
        case '\f': fputs("\\f", f);  break;
        default:
            if (*p < 0x20) {
                fprintf(f, "\\u%04x", *p);
            } else {
                fputc(*p, f);
            }
        }
    }
    fputc('"', f);
}

static void write_value(FILE *f, const Value *v)
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

static void write_object(FILE *f, const DataTable *table, int row,
                         const ColumnNode *node, int level)
{
    fputs("{\n", f);
    for (int i = 0; i < node->child_count; i++) {
        const ColumnNode *child = &node->children[i];
        write_indent(f, level + 1);
        write_quoted(f, child->name);
        fputs(": ", f);
        if (child->column >= 0) {
            write_value(f, data_table_value(table, row, child->column));
        } else {
            write_object(f, table, row, child, level + 1);
        }
        if (i < node->child_count - 1) {
            fputc(',', f);
        }
        fputc('\n', f);
    }
    write_indent(f, level);
    fputc('}', f);
}

int write_json(const DataTable *table, const char *path)
{
    FILE *f = fopen(path, "wb");
    if (f == NULL) {
        return -1;
    }

    if (table->row_count == 0) {
        fputs("[]\n", f);
    } else {
        fputs("[\n", f);
        for (int r = 0; r < table->row_count; r++) {
            write_indent(f, 1);
            write_object(f, table, r, &table->root, 1);
            fputs(r < table->row_count - 1 ? ",\n" : "\n", f);
        }
        fputs("]\n", f);
    }

    return fclose(f) == 0 ? 0 : -1;
}
