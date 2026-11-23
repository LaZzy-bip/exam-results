#ifndef DATA_TABLE_H
#define DATA_TABLE_H

#include "../sqlite3.h"

typedef enum {
    VALUE_NULL,
    VALUE_INTEGER,
    VALUE_REAL,
    VALUE_TEXT
} ValueType;

typedef struct {
    ValueType type;
    char *text;
} Value;

typedef struct ColumnNode {
    char *name;
    int column;
    struct ColumnNode *children;
    int child_count;
} ColumnNode;

typedef struct {
    int column_count;
    char **column_names;
    int row_count;
    Value *values;
    ColumnNode root;
} DataTable;

int data_table_load(sqlite3_stmt *stmt, DataTable *table);

const Value *data_table_value(const DataTable *table, int row, int column);

void data_table_free(DataTable *table);

#endif
