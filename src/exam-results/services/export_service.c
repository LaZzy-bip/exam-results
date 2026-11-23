#include "export_service.h"
#include "service_error.h"
#include "../export/data_table.h"
#include "../export/writers.h"
#include "../repositories/export_repository.h"

#include <direct.h>
#include <errno.h>

static int ensure_dir(const char *path)
{
    if (_mkdir(path) == 0 || errno == EEXIST) {
        return 0;
    }
    return -1;
}

static int save_all_formats(const DataTable *table, const char *root_tag, const char *item_tag)
{
    return write_json(table, EXPORT_DIR "/data.json") == 0
        && write_csv(table, EXPORT_DIR "/data.csv") == 0
        && write_xml(table, EXPORT_DIR "/data.xml", root_tag, item_tag) == 0
        && write_yaml(table, EXPORT_DIR "/data.yaml") == 0;
}

int export_students(int *count, char *err)
{
    if (ensure_dir(EXPORT_DIR) != 0) {
        return service_fail(err, "Не удалось создать папку " EXPORT_DIR);
    }

    DataTable table;
    int n = export_repository_get_students(&table);
    if (n < 0) {
        return service_fail(err, "Не удалось прочитать данные из базы");
    }

    int saved = save_all_formats(&table, "students", "student");
    data_table_free(&table);
    if (!saved) {
        return service_fail(err, "Не удалось записать файлы в папку " EXPORT_DIR);
    }

    *count = n;
    return SERVICE_OK;
}
