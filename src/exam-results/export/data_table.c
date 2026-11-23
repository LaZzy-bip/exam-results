#include "data_table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *copy_string(const char *src, size_t len)
{
    char *dst = malloc(len + 1);
    if (dst != NULL) {
        memcpy(dst, src, len);
        dst[len] = '\0';
    }
    return dst;
}

static ColumnNode *find_child(ColumnNode *node, const char *name, size_t len)
{
    for (int i = 0; i < node->child_count; i++) {
        if (strlen(node->children[i].name) == len
            && strncmp(node->children[i].name, name, len) == 0) {
            return &node->children[i];
        }
    }
    return NULL;
}

static ColumnNode *add_child(ColumnNode *node, const char *name, size_t len)
{
    ColumnNode *grown = realloc(node->children,
                                (size_t)(node->child_count + 1) * sizeof *grown);
    if (grown == NULL) {
        return NULL;
    }
    node->children = grown;

    ColumnNode *child = &node->children[node->child_count];
    child->name = copy_string(name, len);
    child->column = -1;
    child->children = NULL;
    child->child_count = 0;
    if (child->name == NULL) {
        return NULL;
    }
    node->child_count++;
    return child;
}

static int add_column_path(ColumnNode *root, const char *path, int column)
{
    ColumnNode *node = root;
    const char *part = path;

    while (1) {
        const char *dot = strchr(part, '.');
        size_t len = dot ? (size_t)(dot - part) : strlen(part);
        if (len == 0) {
            fprintf(stderr, "Пустая часть в имени столбца \"%s\"\n", path);
            return -1;
        }

        ColumnNode *child = find_child(node, part, len);
        if (dot == NULL) {
            /* последняя часть имени — само поле */
            if (child != NULL) {
                fprintf(stderr, "Столбец \"%s\" повторяется\n", path);
                return -1;
            }
            child = add_child(node, part, len);
            if (child == NULL) {
                return -1;
            }
            child->column = column;
            return 0;
        }

        if (child == NULL) {
            child = add_child(node, part, len);
            if (child == NULL) {
                return -1;
            }
        } else if (child->column >= 0) {
            fprintf(stderr, "Столбец \"%s\" конфликтует с полем \"%s\"\n", path, child->name);
            return -1;
        }
        node = child;
        part = dot + 1;
    }
}

static void free_node(ColumnNode *node)
{
    for (int i = 0; i < node->child_count; i++) {
        free_node(&node->children[i]);
    }
    free(node->children);
    free(node->name);
}

static ValueType value_type(int sqlite_type)
{
    switch (sqlite_type) {
    case SQLITE_INTEGER: return VALUE_INTEGER;
    case SQLITE_FLOAT:   return VALUE_REAL;
    case SQLITE_NULL:    return VALUE_NULL;
    default:             return VALUE_TEXT;
    }
}

static int read_row(sqlite3_stmt *stmt, Value *row, int column_count)
{
    memset(row, 0, (size_t)column_count * sizeof *row);
    for (int c = 0; c < column_count; c++) {
        row[c].type = value_type(sqlite3_column_type(stmt, c));
        if (row[c].type == VALUE_NULL) {
            continue;
        }
        const char *text = (const char *)sqlite3_column_text(stmt, c);
        int len = sqlite3_column_bytes(stmt, c);
        row[c].text = copy_string(text ? text : "", (size_t)len);
        if (row[c].text == NULL) {
            return -1;
        }
    }
    return 0;
}

int data_table_load(sqlite3_stmt *stmt, DataTable *table)
{
    memset(table, 0, sizeof *table);

    int columns = sqlite3_column_count(stmt);
    table->column_names = calloc((size_t)columns, sizeof *table->column_names);
    if (columns > 0 && table->column_names == NULL) {
        sqlite3_finalize(stmt);
        return -1;
    }
    table->column_count = columns;

    for (int c = 0; c < columns; c++) {
        const char *name = sqlite3_column_name(stmt, c);
        table->column_names[c] = copy_string(name, strlen(name));
        if (table->column_names[c] == NULL
            || add_column_path(&table->root, name, c) != 0) {
            sqlite3_finalize(stmt);
            data_table_free(table);
            return -1;
        }
    }

    int capacity = 0;
    int rc;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        if (table->row_count == capacity) {
            capacity = capacity ? capacity * 2 : 8;
            Value *grown = realloc(table->values,
                                   (size_t)capacity * (size_t)columns * sizeof *grown);
            if (grown == NULL) {
                break;
            }
            table->values = grown;
        }
        Value *row = table->values + (size_t)table->row_count * (size_t)columns;
        table->row_count++;
        if (read_row(stmt, row, columns) != 0) {
            break;
        }
    }

    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        data_table_free(table);
        return -1;
    }
    return table->row_count;
}

const Value *data_table_value(const DataTable *table, int row, int column)
{
    return &table->values[(size_t)row * (size_t)table->column_count + (size_t)column];
}

void data_table_free(DataTable *table)
{
    for (int r = 0; r < table->row_count; r++) {
        for (int c = 0; c < table->column_count; c++) {
            free(table->values[(size_t)r * (size_t)table->column_count + (size_t)c].text);
        }
    }
    free(table->values);

    for (int c = 0; c < table->column_count; c++) {
        free(table->column_names[c]);
    }
    free(table->column_names);

    free_node(&table->root);
    memset(table, 0, sizeof *table);
}
