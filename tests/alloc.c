#include "util.h"

#define NPAGES0 2
#define NPAGES1 5
#define PAGE1 65539

int32_t EXTISM_EXPORTED_FUNCTION(run_test) {
  ExtismPointer buf[NPAGES0][NPAGES1];
  for (int i = 0; i < NPAGES0; i++) {
    for (int j = 0; j < NPAGES1; j++) {
      buf[i][j] = extism_alloc(PAGE1);
      assert(buf[i][j] > 0);
      for (int x = 0; x < PAGE1; x++) {
        extism_store_u8(buf[i][j] + x, 'a');
      }
      if (i > 0) {
        extism_free(buf[i - 1][j]);
      }
    }
  }

  for (int j = 0; j < NPAGES1; j++) {
    extism_free(buf[NPAGES0 - 1][j]);
  }

  return 0;
}
