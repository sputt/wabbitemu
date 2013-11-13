#include "stdafx.h"

#include "spasm.h"
#include "hash.h"
#include "storage.h"

/* 
 * Strong hash function for English language (such as line labels)
 * Austin Appleby (murmurhash.googlepages.com)
 */

#ifdef _WIN64
size_t
#else
unsigned int 
#endif
	murmur_hash (const void * key, int len, unsigned int seed) {
#ifdef _WIN64
	typedef unsigned long long uint64_t ;
	const uint64_t m = 0xc6a4a7935bd1e995;
	const int r = 47;

	uint64_t h = seed ^ (len * m);

	const uint64_t * data = (const uint64_t *)key;
	const uint64_t * end = data + (len/8);

	while(data != end)
	{
		uint64_t k = *data++;

		k *= m; 
		k ^= k >> r; 
		k *= m; 
		
		h ^= k;
		h *= m; 
	}

	const unsigned char * data2 = (const unsigned char*)data;

	switch(len & 7)
	{
	case 7: h ^= uint64_t(data2[6]) << 48;
	case 6: h ^= uint64_t(data2[5]) << 40;
	case 5: h ^= uint64_t(data2[4]) << 32;
	case 4: h ^= uint64_t(data2[3]) << 24;
	case 3: h ^= uint64_t(data2[2]) << 16;
	case 2: h ^= uint64_t(data2[1]) << 8;
	case 1: h ^= uint64_t(data2[0]);
	        h *= m;
	};
 
	h ^= h >> r;
	h *= m;
	h ^= h >> r;

	return h;
#else
	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.

	const unsigned int m = 0x5bd1e995;
	const int r = 24;

	// Initialize the hash to a 'random' value

	unsigned int h = seed ^ len;

	// Mix 4 bytes at a time into the hash

	const unsigned char * data = (const unsigned char *)key;

	while(len >= 4)
	{
		unsigned int k = *(unsigned int *)data;

		k *= m; 
		k ^= k >> r; 
		k *= m; 
		
		h *= m; 
		h ^= k;

		data += 4;
		len -= 4;
	}
	
	// Handle the last few bytes of the input array

	switch(len)
	{
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
	        h *= m;
	};

	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
#endif
}


static list_t *list_find(list_t *list, const char *name)
{
	if (list == NULL) {
		return NULL;
	}

	if (strcmp(((store_t *) list->data)->name, name) == 0) {
		return list;
	} else {
		list_find(list->next, name);
	}
}

/*
 * Creates a new hash table with an upper bound size
 */

hash_t *hash_init (size_t size, void remove_callback(void *)) {
	hash_t *hashtable = new hash_t();
	hashtable->list = NULL;

	if (false) { //size <= 16) {
		hashtable->table = NULL;	
	} else	{
		hashtable->table = new hash_t::maptype();
	}
	hashtable->remove_callback = remove_callback;
	return hashtable;
}


/*
 * Inserts a value into a hash table
 */

void *hash_insert (hash_t *ht, void *store) {
	if (ht->table != NULL)
	{
		(*ht->table)[((store_t *) store)->name] = store;
	}
	else
	{
		list_t *item = list_find(ht->list, ((store_t *) store)->name);
	}
	return store;
}


/*
 * Looks up a value from a hash table
 * returns NULL if not found
 */

void *hash_lookup (hash_t *ht, const char *name) {
	auto it = ht->table->find(name);
	if (it == ht->table->end()) {
		return NULL;
	}
	return it->second;
}


/*
 * Removes a hash from a hash table
 */

int hash_remove (hash_t *ht, const char *name) {
	auto it = ht->table->find(name);
	if (it != ht->table->end()) {
		auto store = it->second;
		ht->table->erase(it);
		ht->remove_callback(store);
		return 1;
	}
	return 0;
}

void hash_enum (hash_t *ht, void enum_callback(void *, void *), void *arg) {
	for (auto it = ht->table->cbegin(); it != ht->table->cend(); it++)
	{
		enum_callback(it->second, arg);
	}
}

int hash_count (hash_t *ht) {
	return ht->table->size();
}

/*
 * Free a hash table, removing elements
 */
void hash_free (hash_t *ht) {
	for (auto it = ht->table->cbegin(); it != ht->table->cend(); it++)
	{
		ht->remove_callback(it->second);
	}
	if (ht->table != NULL)
	{
		delete ht->table;
	}
	delete ht;
}
