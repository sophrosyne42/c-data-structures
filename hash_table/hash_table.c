// Katherine Agent
//
// See hash_table.h for info.

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hash_table.h"
#include "crc64.h"

// Decides the hash algorithm.
// Takes a function of with the prototype:
// hash(char* bytes, size_t size)
// where 
// `bytes` is an array of bytes, and
// `size` is the size of the array.
#define hash(key, size_of_key) crc64((char*) (key), (size_of_key))

// Don't rehash/rebuild the hash table. Exists testing purposes.
// #define NO_REHASH

static bucket_t* get_bucket_from_key(bucket_t* buckets, size_t bucket_num, void* key, size_t size_of_key) {
    if (buckets == NULL) {
        fprintf(stderr, "ERR: get_bucket_from_key received a hashtable with an unallocated bucket");
        exit(1);
    }
    // Since char* is treated as an array with each value being 8 bytes,
    // casting any type to a character array is effectively the same as
    // asking the compiler to treat the value as an array of bytes.
    unsigned long long hash = hash(key, size_of_key);
    size_t index = hash % bucket_num;

    assert( (index < bucket_num && index >= 0) && "The index must be between 0 and bucket_num-1" );

    return (bucket_t*) buckets+index;
};


static int transfer_nodes(ht_t* ht, bucket_t* new_buckets, size_t new_bucket_num) {
    for ( size_t bucket_index = 0; bucket_index < ht->bucket_num; ) {
        pair_node_t* cur_head = ht->buckets[bucket_index];
        bucket_t* cur_head_p = ht->buckets+bucket_index;
        if (cur_head != NULL) {
            bucket_t* cur_new_bucket = get_bucket_from_key(new_buckets, new_bucket_num, cur_head->key, ht->size_of_key);
            pair_node_t* new_next = *cur_new_bucket;

            *cur_new_bucket = cur_head;
            pair_node_t* old_next = cur_head->next;
            cur_head->next = new_next;
            *cur_head_p = old_next;

            continue;
        } else {
            bucket_index++;
        }
    }
    return 0;
}

static ht_t* ht_rehash(ht_t* ht) {
    // -- How to rehash a hash table, by Katherine Agent --
    // (0) Find how many more buckets to make
    size_t new_bucket_num = ht->bucket_num * HASH_TABLE_GROWTH_FACTOR;
    bucket_t* new_bucket = calloc(new_bucket_num, sizeof(bucket_t));

    // (1) Transfer all nodes from the old buckets to the new buckets.
    transfer_nodes(ht, new_bucket, new_bucket_num);

    // (2) Reassign the bucket pointer in the hash table from the old buckets to the new buckets
    bucket_t* old_buckets = ht->buckets;
    ht->buckets = new_bucket;

    // (3) free all the stuff in the old buckets.
    free(old_buckets);

    // (4) Update the bucket num to reflect the new num of buckets
    ht->bucket_num = new_bucket_num;

    // (5) be happy or soemthin I dunno
    return ht;
}

ht_t* ht_create(
    int (*cmp_keys)(const void* a, const void* b),
    int (*cmp_vals)(const void* a, const void* b),
    void (*free_key)(void* key),
    void (*free_val)(void* value),
    size_t size_of_key,
    size_t size_of_val
) {
    ht_t* ht  = malloc(sizeof(ht_t));
    ht->buckets     = calloc(HASH_TABLE_INITIAL_NUM_OF_BUCKETS, sizeof(bucket_t));
    ht->bucket_num  = HASH_TABLE_INITIAL_NUM_OF_BUCKETS;
    ht->node_num    = 0;
    ht->cmp_keys    = cmp_keys;
    ht->cmp_vals    = cmp_vals;
    ht->free_key    = free_key;
    ht->free_val    = free_val;
    ht->size_of_key = size_of_key;
    ht->size_of_val = size_of_val;

    return ht;
}

// This function gets a bucket from get_bucket_from_key, and then
// iterates through the linked list to get to the node that holds the
// key given. If no node in the given bucket holds that key, then 
// it returns null.
static void* get_node_from_bucket(ht_t* ht, bucket_t* bucket, void* key) {

    for( pair_node_t* cur_node = *bucket; cur_node != NULL; cur_node = cur_node->next ) {
        // Since cmp_keys has the same specification as cmp function qsort expects,
        // it returns 0 when the keys match. Therefore, !cmp_keys will return true
        // when the keys match.
        if ( !(ht->cmp_keys(cur_node->key,key)) ) return cur_node;
    }
    return NULL;
}

// Assumes key and value are already malloc'd
int ht_insert(ht_t* ht, void* key, void* value) { 
    bucket_t* bucket = get_bucket_from_key(ht->buckets, ht->bucket_num, key, ht->size_of_key);
    assert(bucket != NULL);

    // If key already exists, do nothing and indicate thus.
    if (get_node_from_bucket(ht, bucket, key) != NULL) {
        return 1;
    }

    pair_node_t* new_next = (*bucket);
    pair_node_t* new_head = malloc(sizeof(pair_node_t));

    new_head->key = key;
    new_head->value = value;
    new_head->next = new_next;

    // Change the pointer stored in the hash table's array to point to the new head node.
    *bucket = new_head;

    (ht->node_num)++;

    #ifndef NO_REHASH
    double load_factor = (double) ht->node_num / (double) ht->bucket_num;

    if (  load_factor >= HASH_TABLE_MAX_LOAD_FACTOR ) {
        ht_t* result = ht_rehash(ht);
        assert(result && "_ht_rehash failed to rehash (returned null pointer)");
    }
    #endif

    return 0;
}

static int ht_remove_helper(ht_t* ht, bucket_t* bucket, void* key) {
    // Could not find node associated with key.
    pair_node_t* prev_node = NULL;
    for (pair_node_t* cur_node = *bucket; cur_node != NULL; cur_node = cur_node->next) {
        if ( !(ht->cmp_keys( cur_node->key, key )) ) {
            pair_node_t* next_node = cur_node->next;

            ht->free_key(cur_node->key);
            ht->free_val(cur_node->value);
            free(cur_node);

            (ht->node_num)--;

            prev_node == NULL ? 
                *bucket = next_node : 
                ((prev_node->next) = next_node);


            return 0;
        }
        prev_node = cur_node;
    }
    return 1;
}

// Remove the node associated with `key`
int ht_remove(ht_t* ht, void* key) {
    bucket_t* bucket = get_bucket_from_key(ht->buckets, ht->bucket_num, key, ht->size_of_key);
    return ht_remove_helper(ht, bucket, key);
}

// Return a pointer to the value associated with `key`
void* ht_get(ht_t* ht, void* key) { 
    bucket_t* bucket = get_bucket_from_key(ht->buckets, ht->bucket_num, key, ht->size_of_key);
    pair_node_t* node = get_node_from_bucket(ht, bucket, key);
    return (node == NULL) ? NULL : node->value;
}

const pair_t** const ht_to_array(ht_t* ht) {

    assert( ht && "hashTable must be nonnull." );

    pair_t** pair_buffer = calloc( sizeof(pair_t*),(ht->node_num) );

    size_t node_index = 0;
    size_t bucket_index = 0;
    pair_node_t* cur_node = *ht->buckets;

    while (node_index != ht->node_num) {

        assert( node_index <= ht->node_num && bucket_index <= ht->bucket_num  && 
               "ht_to_array must only access memory for indices within the "
               "bounds of the memory allocated to the hash table.");

        if (cur_node == NULL) {
            bucket_index++;
            cur_node = ht->buckets[bucket_index];
            continue;
        }
        memcpy(pair_buffer+node_index, &cur_node, sizeof(void*));
        cur_node = cur_node->next;
        node_index++;
    }

    return (const pair_t** const) pair_buffer;

}

size_t ht_length(ht_t* ht) {
    return ht->node_num;
}

// Free all memory associated with `hashTable`
void ht_free(ht_t* ht) {

    for (size_t bucket_index = 0; bucket_index < ht->bucket_num;) {
        pair_node_t* cur_head = ht->buckets[bucket_index];
        bucket_t* cur_head_p = ht->buckets+bucket_index;
        if (bucket_index >= ht->bucket_num) break;
        if (cur_head != NULL) {
            // This function resets the head to be the next key.
            ht_remove_helper(ht, cur_head_p, cur_head->key);
            continue;
        } else {
            bucket_index++;
        }
    }

    bucket_t* buckets = ht->buckets;
    free(buckets);
    free(ht);
}


