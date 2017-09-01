/*
 * Copyright (c) 2017, Martin McDonough. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBQED_TINY_HASH_H
#define LIBQED_TINY_HASH_H
#pragma once

#include <stdint.h>
#include <stdbool.h>

#define QED_HASH_TABLE_ENTRIES (64)
#define QED_HASH_TABLE_SIZE (QED_HASH_TABLE_ENTRIES * sizeof(void*))

/* Size is QED_HASH_TABLE_SIZE. Should be zero-initialized. */
struct QED_HashTable;

typedef uintptr_t qed_hashkey_t;
typedef uintptr_t qed_hashdata_t;

/* Return < 0 to quit iteration. Previous return value is the first arg of the
 * next call. */
typedef int QED_HashTableIterCallback(int,
    void*,
    qed_hashkey_t,
    qed_hashdata_t);

typedef void QED_HashTableFreeCallback(qed_hashkey_t, qed_hashdata_t);

/**
 * @brief Tries to get a value from the table.
 *
 * If the key exists, it is placed in out parameter and the function returns
 * true. Otherwise, the parameter is not modified and the function returns
 * false.
 *
 * @return true if the key exists in the table.
 */
bool QED_HashTableGet(struct QED_HashTable *, qed_hashkey_t, qed_hashdata_t *);

/**
 * @brief Inserts a value into the table, retreiving the old value if present.
 *
 * This will add the key if it is not already present.
 *
 * If the key exists, it is placed in out parameter and the function returns
 * true. Otherwise, the parameter is not modified and the function returns
 * false.
 * 
 * @return true if the key exists in the table. The value is inserted anyway.
 */
bool QED_HashTableInsert(struct QED_HashTable *,
    qed_hashkey_t,
    qed_hashdata_t,
    qed_hashdata_t *);

/**
 * @brief Sets a value for an existing key in the table
 *
 * This will not add the key if it is not already present.
 *
 * @return true if the key was already present.
 */
bool QED_HashTableSet(struct QED_HashTable *,
    qed_hashkey_t,
    qed_hashdata_t);

/**
 * @brief Removes a key from the table.
 *
 * If the key exists, it is placed in out parameter and the function returns
 * true. Otherwise, the parameter is not modified and the function returns
 * false.
 *
 * @return true if the key was present.
 */
bool QED_HashTableRemove(struct QED_HashTable *,
    qed_hashkey_t,
    qed_hashdata_t *);

/**
 * @brief Calls a function for every entry in the hash table.
 *
 * Iterates the hash table with the specified function, passing in an
 * accumulator value for every iteration. The accumulated value for the next
 * call is the return value of the previous call. If a value < 0 is returned,
 * iteration is stopped.
 *
 * @return The final value of the accumulator
 */
int QED_HashTableIterate(struct QED_HashTable *,
    int,
    void*,
    QED_HashTableIterCallback);

void QED_FreeHashTable(struct QED_HashTable *, QED_HashTableFreeCallback);

#endif /* LIBQED_TINY_HASH_H */
