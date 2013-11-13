#ifndef HASH_H_
#define HASH_H_

#include "list.h"

typedef struct {
	char *name;
} store_t;

#ifdef _WIN64
size_t
#else
unsigned int 
#endif
	murmur_hash (const void *data, int len, unsigned int seed = 0xdeadbeef);

#if 0
#pragma warning(disable : 4200)

typedef struct _mh_hash_table {
	size_t size, used;
	void (*remove_callback)(void *);
	store_t *table[0];
} hash_t;
#pragma warning(default : 4200)
#else

	struct mystringhash
		: public std::unary_function<const char *, size_t>
	{	// hash functor for basic_string
	size_t operator()(const char* _Keyval) const
		{	// hash _Keyval to size_t value by pseudorandomizing transform
		return (murmur_hash(_Keyval,
			strlen(_Keyval)));
		}
	};

	struct myequalto
		: public std::binary_function<const char *, const char *, bool>
	{	// functor for operator==
	bool operator()(const char * _Left, const char *_Right) const
		{	// apply operator== to operands
		return (strcmp(_Left, _Right) == 0) ? true: false;
		}
	};
typedef struct
{
	typedef std::unordered_map<const char *, void*, mystringhash, myequalto> maptype;
	 maptype *table;
	void (*remove_callback)(void *);
	list_t *list;
}
hash_t;

#endif


typedef void (*HASH_ENUM_CALLBACK)(void *, void *);
typedef void (*HASH_REMOVE_CALLBACK)(void *);

hash_t *hash_init (size_t size, void remove_callback(void *));
void *hash_insert (hash_t *ht, void *store);
void *hash_lookup (hash_t *ht, const char *name);
int hash_remove (hash_t *ht, const char *name);
void hash_enum (hash_t *ht, void enum_callback(void *, void *), void *arg);
int hash_count (hash_t *ht);
void hash_free (hash_t *ht);

#endif /*HASH_H_*/
