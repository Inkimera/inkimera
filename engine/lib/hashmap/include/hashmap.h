#ifndef HASHMAP_SIZE
#define HASHMAP_SIZE 128
#endif

#ifndef MAX_KEY_SIZE
#define MAX_KEY_SIZE 32
#endif

#define STR_NOT_NULL(src) \
  (src[0] != '\0')

/*
 * DECLARE_HASHMAP
 */

#define DECLARE_HASHMAP(type) \
  typedef unsigned long hashmap_ ## type ## _key_t; \
  typedef struct { \
    char key[MAX_KEY_SIZE]; \
    type value; \
  } hashmap_ ## type ## _entry_t; \
  typedef struct { \
    hashmap_ ## type ## _entry_t entries[HASHMAP_SIZE]; \
  } hashmap_ ## type ## _t; \
  void \
  hashmap_ ## type ## _init(hashmap_ ## type ## _t * map); \
  hashmap_ ## type ## _key_t \
  hashmap_ ## type ## _hash(const char *key); \
  void \
  hashmap_ ## type ## _insert( \
  hashmap_ ## type ## _t * map, const char *key, type value); \
  type* \
  hashmap_ ## type ## _lookup(hashmap_ ## type ## _t * map, const char *key); \
  void \
  hashmap_ ## type ## _iterate( \
  hashmap_ ## type ## _t * map, void (*callback)( \
  hashmap_ ## type ## _entry_t*, \
  void* \
  ), void *data); \
  void \
  hashmap_ ## type ## _remove(hashmap_ ## type ## _t * map, const char *key); \
  void \
  hashmap_ ## type ## _clear(hashmap_ ## type ## _t * map);

/*
 * DEFINE_HASHMAP
 */

#define DEFINE_HASHMAP(type) \
  char* \
  _ ## type ## _strnull(char *dest) { \
  int i = 0; \
  while (dest[i] != '\0') { \
      dest[i] = '\0'; \
      i++; \
  } \
    return dest; \
  } \
  char* \
  _ ## type ## _strcpy(char *dest, const char *src) { \
  int i = 0; \
  while (src[i] != '\0') { \
      dest[i] = src[i]; \
      i++; \
  } \
    return dest; \
  } \
  int \
  _ ## type ## _strcmp(const char *s1, const char *s2) { \
  int i = 0; \
  while (s1[i] != '\0' && s2[i] != '\0') { \
      if (s1[i] != s2[i]) { \
        return s1[i] - s2[i]; \
      } \
      i++; \
  } \
    return 0; \
  } \
  void \
  hashmap_ ## type ## _init(hashmap_ ## type ## _t * map) { \
  for (int i = 0; i < HASHMAP_SIZE; i++) { \
      _ ## type ## _strnull(map->entries[i].key); \
  } \
  } \
  hashmap_ ## type ## _key_t \
  hashmap_ ## type ## _hash(const char *key) { \
  unsigned long hash = 5381; \
  int c; \
  while ((c = *key++)) \
    hash = ((hash << 5) + hash) +c; \
    return hash % HASHMAP_SIZE; \
  } \
  void \
  hashmap_ ## type ## _insert( \
  hashmap_ ## type ## _t * map, const char *key, type value) { \
  hashmap_ ## type ## _key_t index = hashmap_ ## type ## _hash(key); \
  while (STR_NOT_NULL(map->entries[index].key)) { \
      index = (index + 1) % HASHMAP_SIZE; \
  } \
  _ ## type ## _strcpy(map->entries[index].key, key); \
  map->entries[index].value = value; \
  } \
  type* \
  hashmap_ ## type ## _lookup( \
  hashmap_ ## type ## _t * map, const char *key) { \
  hashmap_ ## type ## _key_t index = hashmap_ ## type ## _hash(key); \
  while (STR_NOT_NULL(map->entries[index].key)) { \
      if (_ ## type ## _strcmp(map->entries[index].key, key) == 0) { \
        return &map->entries[index].value; \
      } \
      index = (index + 1) % HASHMAP_SIZE; \
  } \
    return NULL; \
  } \
  void \
  hashmap_ ## type ## _iterate( \
  hashmap_ ## type ## _t * map, void (*callback)( \
  hashmap_ ## type ## _entry_t*, \
  void* \
  ), void *data) { \
  for (int i = 0; i < HASHMAP_SIZE; i++) { \
      if (STR_NOT_NULL(map->entries[i].key)) { \
        callback(&map->entries[i], data); \
      } \
  } \
  } \
  void \
  hashmap_ ## type ## _remove(hashmap_ ## type ## _t * map, const char *key) { \
  hashmap_ ## type ## _key_t index = hashmap_ ## type ## _hash(key); \
  while (STR_NOT_NULL(map->entries[index].key)) { \
      if (_ ## type ## _strcmp(map->entries[index].key, key) == 0) { \
        _ ## type ## _strnull(map->entries[index].key); \
        return; \
      } \
      index = (index + 1) % HASHMAP_SIZE; \
  } \
  } \
  void \
  hashmap_ ## type ## _clear(hashmap_ ## type ## _t * map) { \
  for (int i = 0; i < HASHMAP_SIZE; i++) { \
      _ ## type ## _strnull(map->entries[i].key); \
  } \
  }
