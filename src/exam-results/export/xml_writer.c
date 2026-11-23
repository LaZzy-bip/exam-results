#define _CRT_SECURE_NO_WARNINGS
#include "writers.h"

#include <ctype.h>

static void write_tag_name(FILE *f, const char *name)
{
    const unsigned char *p = (const unsigned char *)name;
    if (isdigit(*p) || *p == '-' || *p == '.') {
        fputc('_', f);
    }
    for (; *p; p++) {
        if (*p < 0x80 && !isalnum(*p) && *p != '_' && *p != '-' && *p != '.') {
            fputc('_', f);
        } else {
            fputc(*p, f);
        }
    }
}

static void write_text(FILE *f, const char *s)
{
    for (const unsigned char *p = (const unsigned char *)s; *p; p++) {
        switch (*p) {
        case '&': fputs("&amp;", f); break;
        case '<': fputs("&lt;", f);  break;
        case '>': fputs("&gt;", f);  break;
        default:
            if (*p >= 0x20 || *p == '\t' || *p == '\n' || *p == '\r') {
                fputc(*p, f);
            }
        }
    }
}

static void open_tag(FILE *f, const char *name)
{
    fputc('<', f);
    write_tag_name(f, name);
    fputc('>', f);
}

static void close_tag(FILE *f, const char *name)
{
    fputs("</", f);
    write_tag_name(f, name);
    fputs(">\n", f);
}

static void write_fields(FILE *f, const DataTable *table, int row,
                         const ColumnNode *node, int level)
{
    for (int i = 0; i < node->child_count; i++) {
        const ColumnNode *child = &node->children[i];
        write_indent(f, level);

        if (child->column < 0) {
            /* вложенный объект — тег с дочерними тегами */
            open_tag(f, child->name);
            fputc('\n', f);
            write_fields(f, table, row, child, level + 1);
            write_indent(f, level);
            close_tag(f, child->name);
            continue;
        }

        const Value *v = data_table_value(table, row, child->column);
        if (v->type == VALUE_NULL) {
            fputc('<', f);
            write_tag_name(f, child->name);
            fputs("/>\n", f);
        } else {
            open_tag(f, child->name);
            write_text(f, v->text);
            close_tag(f, child->name);
        }
    }
}

int write_xml(const DataTable *table, const char *path,
              const char *root_tag, const char *item_tag)
{
    FILE *f = fopen(path, "wb");
    if (f == NULL) {
        return -1;
    }

    fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n", f);
    open_tag(f, root_tag);
    fputc('\n', f);
    for (int r = 0; r < table->row_count; r++) {
        write_indent(f, 1);
        open_tag(f, item_tag);
        fputc('\n', f);
        write_fields(f, table, r, &table->root, 2);
        write_indent(f, 1);
        close_tag(f, item_tag);
    }
    close_tag(f, root_tag);

    return fclose(f) == 0 ? 0 : -1;
}
