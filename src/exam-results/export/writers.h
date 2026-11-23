#ifndef WRITERS_H
#define WRITERS_H

#include <stdio.h>
#include "data_table.h"

int write_json(const DataTable *table, const char *path);
int write_csv(const DataTable *table, const char *path);
int write_xml(const DataTable *table, const char *path,
              const char *root_tag, const char *item_tag);
int write_yaml(const DataTable *table, const char *path);

void write_quoted(FILE *f, const char *s);

void write_indent(FILE *f, int level);

#endif
