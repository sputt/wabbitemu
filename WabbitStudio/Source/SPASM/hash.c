#include "spasm.h"
#include "hash.h"
#include "storage.h"

#include <string.h>
#include <stdlib.h>

/* 
 * Strong hash function for english language (such as line labels)
 * Austin Appleby (murmurhash.googlepages.com)
 */

unsigned int murmur_hash (const char *data, int len) {
    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.

    const unsigned int m = 0x7fd652ad;
    const int r = 16;

    // Initialize the hash to a 'random' value

    unsigned int h = 0xdeadbeef;
  
    // Mix 4 bytes at a time into the hash

    while (len >= 4) {
        h += *((unsigned int *)data);
        h *= m;
        h ^= h >> r;

        data += 4;
        len -= 4;
    }

    // Handle the last few bytes of the input array

	switch (len) {
	    case 3:
	    	h += data[2] << 16;
	    case 2:
	    	h += data[1] << 8;
	    case 1:
			h += data[0];
			h *= m;
			h ^= h >> r;
	}
  
    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.

	h *= m;
	h ^= h >> 10;
	h *= m;
	h ^= h >> 17;

	return h;
}


/*
 * Creates a new hash table with an upper bound size
 */

hash_t *hash_init (size_t size, void remove_callback(void *)) {
	hash_t *ht = (hash_t *) malloc(sizeof (hash_t) + sizeof (store_t *) * (size + (size < 2)));
	
	ht->size = size + (size < 2);
	ht->remove_callback = remove_callback;
	memset (ht->table, 0, sizeof(store_t *) * ht->size);

	ht->used = 0;
	return ht;
}


/*
 * Inserts a value into a hash table
 */

void *hash_insert (hash_t *ht, void *store) {
	const char *name = ((store_t *)store)->name;
	unsigned int hash = murmur_hash(name, strlen (name));
	
	ht->used++;
	hash %= ht->size;

	//if (mht->table[hash] != NULL)
		//printf("collision (%s:%8x %s:%8x)!\n", ((store_t *)store)->name, hash, mht->table[hash]->name, hash);
	
	//int clump_size = 0;
	while (ht->table[hash] != NULL) {
		hash = (hash + 1) % ht->size;
		//clump_size++;
	}
	//if (clump_size != 0) printf("collision: clump size: %d\n", clump_size);
	
	ht->table[hash] = (store_t *)store;
	return store;
}


/*
 * Looks up a value from a hash table
 * returns NULL if not found
 */

void *hash_lookup (hash_t *ht, const char *name) {
	unsigned int hash = murmur_hash (name, strlen (name));
	
	hash %= ht->size;

	// Search the entire clump
	while (ht->table[hash] != NULL && strcmp (name, ht->table[hash]->name))
		hash = (hash + 1) % ht->size;
	
	// Return the store -- could be NULL (which indicates no entry)
	return ht->table[hash];
}


/*
 * Removes a hash from a hash table
 */

int hash_remove (hash_t *ht, const char *name) {
	unsigned int hash, clump;
	// Array for remembering all stores that need to be rehashed
	store_t **rehash;
	int rehash_count,i;

	ht->used--;

	// Find the start of the collision clump
	// This includes all hashes that are the same
	// as the one we are looking up
	for (	hash = murmur_hash (name, strlen (name)),
			clump = (hash - 1) % ht->size;
			
			ht->table[clump] != NULL &&
			murmur_hash (ht->table[clump]->name, 
					strlen(ht->table[clump]->name)) == hash;
			
			clump = (clump - 1) % ht->size) ;

	// Stores that need to be rehashed are those that fall in the
	// same collision clump as the hash we're removing
	for (	clump = (clump + 1) % ht->size,
			rehash = NULL,
			rehash_count = 0; 
			
			ht->table[clump] != NULL;
			
			clump = (clump + 1) % ht->size,
			rehash_count++) {
		
		rehash = (store_t **) realloc (rehash, (rehash_count + 1) * sizeof (store_t *));
		rehash[rehash_count] = ht->table[clump];
		
		ht->table[clump] = NULL;
	}
	
	// If there was nothing nothing at all to rehash, exit
	if (rehash == NULL)
		return 0;
	
	// Rehash everything except for the one being removed
	for (i = 0; i < rehash_count; i++) {
		if (strcmp (rehash[i]->name, name)) {
			hash_insert (ht, rehash[i]);
		} else {
			ht->remove_callback (rehash[i]);
		}
	}

	free (rehash);
	return 1;
}

void hash_enum (hash_t *ht, void enum_callback(void *, void *), void *arg) {
	int i;
	
	for (i = 0; i < ht->size; i++) {
		if (ht->table[i] != NULL) {
			enum_callback(ht->table[i], arg);
		}
	}
}

int hash_count (hash_t *ht) {
	if (ht == NULL) 
		return -1;

	int count = 0;
	for (int i = 0; i < ht->size; i++)
		if (ht->table[i] != NULL) count++;
	
	return count;
}

/*
 * Free a hash table, removing elements
 */
void hash_free (hash_t *ht) {
	if (ht == NULL || ht->remove_callback == NULL)
		return;

	for (int i = 0; i < ht->size; i++) {
		if (ht->table[i] != NULL) {
			ht->remove_callback(ht->table[i]);
		}
	}
	
	free(ht);
}