#ifndef R_JumpTable_h
#define R_JumpTable_h

typedef char* R_JumpTable_NamedKey;

typedef struct {
  R_JumpTable_NamedKey const* key;
  void* func;
} R_JumpTable_Entry;
typedef R_JumpTable_Entry R_JumpTable;

#define R_JumpTable_DeclareKey(name) extern const R_JumpTable_NamedKey R_JumpTable_Key__##name
#define R_JumpTable_DeclareFunction(name, return_type, ...) typedef return_type (*R_JumpTable_Function__##name)(__VA_ARGS__)

#define R_JumpTable_DefineKey(name) R_JumpTable_NamedKey const R_JumpTable_Key__##name = #name

#define R_JumpTable_Key(name) &R_JumpTable_Key__##name
#define R_JumpTable_Function(name) R_JumpTable_Function__##name
#define R_JumpTable_Entry_Make(name, method) {R_JumpTable_Key(name), method}
#define R_JumpTable_Entry_NULL {NULL, NULL}

void* R_JumpTable_get(R_JumpTable_Entry* table, const R_JumpTable_NamedKey* key);
#define R_JumpTable_call(table, key, ...) ((R_JumpTable_Function(key))R_JumpTable_get(table,R_JumpTable_Key(key)))(__VA_ARGS__)
#define R_JumpTable_hasEntry(table, key) R_JumpTable_get(table,R_JumpTable_Key(key))

#endif /* R_JumpTable_h */
