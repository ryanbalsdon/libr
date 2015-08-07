/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

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
