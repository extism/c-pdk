#include "../../extism-pdk.h"
#include "sqlite3.h"

#include <stdio.h>
#include <string.h>

int handleCallback(void *data, int n, char **cols, char **names) {
  size_t len = strlen(cols[0]);
  extism_log(cols[0], len, ExtismLogInfo);
  return SQLITE_OK;
}

int32_t sqlite_file() {
  sqlite3 *db = NULL;

  int rc = sqlite3_open_v2("db", &db,
                           SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
  if (rc != SQLITE_OK)
    return 1;

  char *errmsg = NULL;

  // Create `testing` table
  sqlite3_exec(db,
               "CREATE TABLE IF NOT EXISTS testing(ID INTEGER PRIMARY KEY, "
               "name TEXT NOT NULL);",
               NULL, NULL, NULL);

  // List all tables
  sqlite3_exec(db, "SELECT name FROM sqlite_master WHERE type='table';",
               handleCallback, NULL, &errmsg);
  if (errmsg != NULL) {
    extism_log(errmsg, strlen(errmsg), ExtismLogError);
    sqlite3_free(errmsg);
  }

  // uint64_t offs_ = extism_alloc(n);
  // extism_store(offs_, (const uint8_t *)out, n);
  // extism_output_set(offs_, n);

  sqlite3_close_v2(db);
  return 0;
}
