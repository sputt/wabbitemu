#ifndef HASH_H_
#define HASH_H_

typedef struct {
	char *name;
} store_t;

#pragma warning(disable : 4200)
typedef struct _mh_hash_table {
	size_t size, used;
	void (*remove_callback)(void *);
	store_t *table[0];
} hash_t;
#pragma warning(default : 4200)

typedef void (*HASH_ENUM_CALLBACK)(void *, void *);
typedef void (*HASH_REMOVE_CALLBACK)(void *);

unsigned int murmur_hash (const char *data, int len);

hash_t *hash_init (size_t size, void remove_callback(void *));
void *hash_insert (hash_t *ht, void *store);
void *hash_lookup (hash_t *ht, const char *name);
int hash_remove (hash_t *ht, const char *name);
void hash_enum (hash_t *ht, void enum_callback(void *, void *), void *arg);
int hash_count (hash_t *ht);
void hash_free (hash_t *ht);

#endif /*HASH_H_*/
