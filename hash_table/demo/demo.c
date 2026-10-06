// Katherine Agent

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../hash_table.h"

int cmp_keys(const void* s1, const void* s2) {
    return strcmp( (char*) s1, (char*) s2);
}

int cmp_values(const void* i1_p, const void* i2_p) {
    int i1 = *(int*) i1_p;
    int i2 = *(int*) i2_p;
    if (i1 < i2) return -1;
    if (i1 > i2) return 1;
    else return 0;
}

// First sort by count (reversed), then sort lexicographically.
int cmp_pairs(const void* pair1, const void* pair2) {
    pair_t* p1 = *(pair_t**) pair1;
    pair_t* p2 = *(pair_t**) pair2;

    // // Negative so that qsort provides the list in descending order.
    // if (p1 == NULL || p2 == NULL) { 
    //     return -1; 
    // }

    int cmp_ret = -(cmp_values(p1->value, p2->value));
    if ( cmp_ret != 0 ) return cmp_ret;
    else return cmp_keys(p1->key, p2->key);
}


int main(void) {
    ht_t* ht = ht_create(cmp_keys, cmp_values, free, free, sizeof(char)*100, sizeof(int));

    // char* testKey = malloc(sizeof(char)*10);
    // sprintf(testKey,"TestKey!");
    // int* testValue = malloc(sizeof(int));
    // *testValue = 5;

    // char* testKey;
    // for (int i = 0; i < HASH_TABLE_INITIAL_NUM_OF_BUCKETS*4*4*4*4*4; i++) {
    for (int i = 0; i < 1024*4*4*4*4*4; i++) {
        char* test_key = malloc(sizeof(char)*100);
        // testKey = malloc(sizeof(long));
        snprintf(test_key,ht->size_of_key,"TestKey%i!",i);
        // *testKey = 10210293123*i;
        int* test_value = malloc(sizeof(int));
        assert(test_value);
        *test_value = i;
        int ret = ht_insert(ht, test_key, test_value);
        if (ret) {
            free(test_key);
            free(test_value);
        }
    }

    // *testKey = 10210293123*10; 
    // sprintf(testKey,"TestKey5!");
    int* gotten_value;
    for (int i = 0; i < ht->node_num; i++) {
        char key[100];
        sprintf(key, "TestKey%i!", i);
        gotten_value = ht_get(ht, "TestKey%i!");
        gotten_value == NULL ? 0 : (*gotten_value)++;
        // gottenValue == NULL ?
        //     printf("The value was not found!\n") : printf("This is the value I got from my key: %i\n", *gottenValue);
        // printf("I am dereferencing the pointer then incrementing!\n");
        // (gottenValue == NULL) ? printf("Hold up, that is a null pointer!\n") : (*gottenValue)++;
    }
    gotten_value = ht_get(ht, "TestKey5!" );
    gotten_value == NULL ?
        printf("The value was not found!\n") : printf("This is the value I got from my key: %i\n", *gotten_value);

    size_t* numOfEntries = &ht->node_num;

    printf("Here is the number of entries before removing one: %lu\n", *numOfEntries);
    printf("I will remove '%s'!\n", "TestKey5");
    // hashTableRemove(hashTable, "TestKey5!");
    printf("I have removed '%s'!\n", "TestKey5");
    gotten_value = ht_get(ht, "TestKey5!");
    gotten_value == NULL ? printf("value is null!") : printf("value is not null!");
    printf("\n");

    printf("Here is the number of entries after removing one: %lu\n", *numOfEntries);


    // printf("This is the headKey and value!\n");
    // void* headKey = (*hashTable->buckets)->key;
    // void* headValue = (*hashTable->buckets)->value;
    // printf("key: %s\n", (char*) headKey);
    // printf("value: %i\n", *(int*) headValue);
    // printf("Removing head key and value...\n");
    // hashTableRemove(hashTable, headKey);
    // printf("Removed head key and value!!!\n");

    // printf("Here is the new head key and value\n");
    // headKey = (*hashTable->buckets)->key;
    // headValue = (*hashTable->buckets)->value;
    // printf("key: %s\n", (char*) headKey);
    // printf("value: %i\n", *(int*) headValue);

    printf("hashTable node num: %lu\n", ht->node_num);
    const pair_t** const pair_arr = ht_to_array(ht);
    qsort(pair_arr, ht->node_num, sizeof(pair_t*), cmp_pairs);

    printf("There are %lu elements in the hashtable. Here are all of them!\n", ht->node_num);
    for(size_t i = 0; i < ht->node_num; i++) {
        if (i == 296) {
            printf("whyyy");
        }
        if (pair_arr[i] == NULL) {
            printf("ERR: node is NULL! %lu", i); 
            continue;
        }
        printf("keyValueArr[%lu] --- k: %s; v: %i\n", i, (char*)(pair_arr[i]->key), *(int*)pair_arr[i]->value);
    }

    free(pair_arr);
    ht_free(ht);
        
} 
