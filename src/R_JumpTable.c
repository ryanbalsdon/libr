#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "R_JumpTable.h"


void* R_JumpTable_get(R_JumpTable_Entry* table, const R_JumpTable_NamedKey* key) {
  if (table == NULL) return NULL;
  R_JumpTable_Entry* entry = table;
   while (entry->key) {
    if (entry->key == key) return entry->func;
    entry = table++;
  };
  return NULL;
}
