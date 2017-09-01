/*
 * Copyright (c) 2017, Martin McDonough. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "qed_tinyhash.h"

#include <stdlib.h>

struct QED_HashEntry{
    qed_hashkey_t key;
    qed_hashdata_t data;
    struct QED_HashEntry *next;
};

typedef struct QED_HashEntry *QED_HashEntryPtrStruct;

#ifdef __GNUC__
__attribute__((const))
#endif
uintptr_t QED_Hash(qed_hashkey_t);

uintptr_t QED_Hash(qed_hashkey_t p){
    return (p >> 4) & (QED_HASH_TABLE_ENTRIES-1);
}

struct QED_HashTable{
    QED_HashEntryPtrStruct table[QED_HASH_TABLE_ENTRIES];
};

static struct QED_HashEntry *qed_hash_table_get(struct QED_HashTable *table,
    qed_hashkey_t key){
    
    struct QED_HashEntry *entry;
    {
        const uintptr_t hash = QED_Hash(key);
        entry = table->table[hash];
    }
    
    while(entry != NULL){
        if(entry->key == key)
            return entry;
        entry = entry->next;
    }
    return NULL;
}

bool QED_HashTableGet(struct QED_HashTable *table, qed_hashkey_t key, qed_hashdata_t *out){
    
    struct QED_HashEntry *const entry = qed_hash_table_get(table, key);
    if(entry){
        out[0] = entry->data;
        return true;
    }
    else{
        return false;
    }
}

bool QED_HashTableInsert(struct QED_HashTable *table,
    qed_hashkey_t key,
    qed_hashdata_t data,
    qed_hashdata_t *out){
    
    struct QED_HashEntry *const entry = qed_hash_table_get(table, key);
    if(entry){
        out[0] = entry->data;
        entry->data = data;
        return true;
    }
    else{
        const uintptr_t hash = QED_Hash(key);
        struct QED_HashEntry **dest = table->table + hash,
            *const first_entry = *dest,
            *const new_entry = malloc(sizeof(struct QED_HashEntry));
        
        new_entry->key = key;
        new_entry->data = data;
        new_entry->next = first_entry;
        
        dest[0] = new_entry;
        return false;
    }
}

bool QED_HashTableSet(struct QED_HashTable *table,
    qed_hashkey_t key,
    qed_hashdata_t data){
    struct QED_HashEntry *const entry = qed_hash_table_get(table, key);
    if(entry){
        entry->data = data;
        return true;
    }
    else{
        return false;
    }
}

bool QED_HashTableRemove(struct QED_HashTable *table,
    qed_hashkey_t key,
    qed_hashdata_t *out){
    
    struct QED_HashEntry *entry, *prev;
    {
        const uintptr_t hash = QED_Hash(key);
        entry = table->table[hash];
        /* Empty bucket */
        if(entry == NULL)
            return false;
        
        if(entry->key == key){
            /* This is the first key, clear the bucket. */
            table->table[hash] = NULL;
            free(entry);
            return true;
        }
    }
    
    /* First iteration, prev will at least be filled now. */
    prev = entry;
    entry = entry->next;
    
    while(entry){
        if(entry->key == key){
            
            /* Stitch the list */
            prev->next = entry->next;
            
            /* Set the output */
            out[0] = entry->data;
            
            free(entry);
            return true;
        }
        prev = entry;
        entry = entry->next;
    }
    
    return false;
}

int QED_HashTableIterate(struct QED_HashTable *table,
    int accum,
    void *arg,
    QED_HashTableIterCallback cb){
    
    unsigned i = 0;
    do{
        struct QED_HashEntry *entry = table->table[i];
        while(entry && accum >= 0){
            accum = cb(accum, arg, entry->key, entry->data);
            entry = entry->next;
        }
    }while(++i < QED_HASH_TABLE_ENTRIES);
    
    return accum;
}

void QED_FreeHashTable(struct QED_HashTable *table, QED_HashTableFreeCallback cb){
    unsigned i;
    for(i = 0; i < QED_HASH_TABLE_ENTRIES; i++){
        struct QED_HashEntry *entry = table->table[i];
        while(entry){
            struct QED_HashEntry *const next = entry->next;
            if(cb != NULL)
                cb(entry->key, entry->data);
            free(entry);
            entry = next;
        }
    }
}
