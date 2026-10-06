// Katherine Agent
//
// Implementation of a hash table.

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

// The hash table expands by this factor on rehash
#define HASH_TABLE_GROWTH_FACTOR            4

#define HASH_TABLE_MAX_LOAD_FACTOR          2

// Factor used to determine when rehashing should be done.
//
// Load factor is defined as n/m, where n is the number of items stored in the hash table and m is the number of buckets.
// Per wikipedia, in the limit of large n/m, the each bucket has a poisson distribution: https://en.wikipedia.org/wiki/Hash_table#Load_factor
// In the limit as n,m goes to infinity, n/m = 1. Therefore, ideally the load factor should approximate 1.
// When the load factor is 2, it means that n=2m. If we grow m by 4, we have m' = 4m -> m'/4 = m, thus
// n = 2m = 2(m'/4) = m'/2 -> n/m' = 1/2. In other words, growing the buckets by 4 will cause the load factor
// to drop to 1/2. In regular program operation, the load factor therefore stays in the range 1/2 to 2,
// which hopefully causes each bucket to approximate a poisson distribution, making it fast and good.
#define HASH_TABLE_REHASH_FACTOR            HASH_TABLE_MAX_LOAD_FACTOR

// The initial number of items in the table. This is arbitrary.
#define HASH_TABLE_INITIAL_NUM_OF_BUCKETS   1024

#include <stddef.h>

typedef struct pair_t {
    void* key;
    void* value;
} pair_t;

typedef struct pair_node_t {
    void* key;
    void* value;
    struct pair_node_t* next;
} pair_node_t;

typedef pair_node_t* bucket_t;

typedef struct ht_t {

    bucket_t* buckets;

    size_t bucket_num; 

    size_t node_num;

    // These functions are provided by the caller, and used to determine hash table type
    // when the hash table is created, similar to how a hash table type is declared in 
    // java, e.g. new Hashtable<KeyType, ValueType>.
    int  (*cmp_keys)(const void* a, const void* b);
    int  (*cmp_vals)(const void* a, const void* b);
    void (*free_key)(void* key);
    void (*free_val)(void* value);
    size_t size_of_key;
    size_t size_of_val;
} ht_t;

// Add key if it does not exist.
// Returns 0 if key did not exist and was inserted successfully.
// Returns 1 if key already exists or could otherwise not be inserted.
int ht_insert(ht_t* ht, void* key, void* value);

// Apply unary `function` to each value of hashtable.
// `function` is of the form: `int function(const void* a) -> int`
// void* hashtbl_apply(Hash_Table* hashTable, int (*function)(const void*));

// Remove key and its associated value.
// Returns 0 if key existed and was removed successfully.
// Returns 1 if key did not exist.
int ht_remove(ht_t* ht, void* key);

// Get value associated with key.
// Returns a pointer to the value associated with the given key.
// If there is no such value, returns NULL.
void* ht_get(ht_t* ht, void* key);

// Returns the number of key-value pairs stored in the hash table.
size_t ht_length(ht_t* ht);

// Returns a pointer to an array of all key-value pairs in hash table.
// Allocates memory for the array on the heap -- the caller is expected
// to free the memory associated with the array when finished with it.
const pair_t** const ht_to_array(ht_t* ht);

/* Allocate the memory and return a pointer for an empty hashtable */
ht_t* ht_create(
    int (*cmp_keys)(const void* a, const void* b),
    int (*cmp_vals)(const void* a, const void* b),
    void (*free_key)(void* key),
    void (*free_val)(void* value),
    size_t size_of_key,
    size_t size_of_val
);

// Free all memory associated with the hash table.
void ht_free(ht_t* ht);

#endif


