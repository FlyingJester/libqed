/*
 * Copyright (c) 2017, Martin McDonough. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "qed_batch.h"

#include "qed_greedy.h"
#include "qed_dependency.h"
#include "qed_tinyhash.h"

#include <assert.h>
#include <stdlib.h>

static void qed_add_depencies(struct QED_HashTable *const satisfied,
    struct QED_Dependency **deps,
    unsigned num_deps){
    
    unsigned i;
    for(i = 0; i < num_deps; i++){
        uintptr_t unused;
        if(!QED_HashTableInsert(satisfied, (uintptr_t)deps[i], 0, &unused)){
            qed_add_depencies(satisfied, deps[i]->dependencies, deps[i]->num_dependencies);
        }
        else{
            assert(unused == 0);
        }
    }
}

bool QED_CalculateBatches(struct QED_Batch ***out_batches,
    unsigned *out_num_batches,
    struct QED_Dependency **deps,
    unsigned num_deps,
    unsigned max_batch_size,
    enum QED_BatchAlgorithm algorithm){
    
    unsigned num_satisfied = 0, num_prev_satisfied = 0, num_batches = 0;
    struct QED_Batch **batches = malloc(num_deps * sizeof(void*));
    
    struct QED_HashTable *const satisfied = calloc(1, QED_HASH_TABLE_SIZE);
    
    /* Add all deps with dependencies to the table. */
    qed_add_depencies(satisfied, deps, num_deps);
    
    switch(algorithm){
        case QED_eLookahead: /* FALLTHROUGH until implemented. */
        case QED_eBalanced:  /* FALLTHROUGH until implemented. */
        case QED_eGreedy:
        {
            if(!QED_CalculateBatchesGreedy(batches, &num_batches, satisfied,
                deps, num_deps, max_batch_size))
                goto batch_error;
        }
    }
    
    out_batches[0] = batches;
    QED_FreeHashTable(satisfied, NULL);
    free(satisfied);
    out_num_batches[0] = num_batches;
    return true;

batch_error:

    out_batches[0] = NULL;
    QED_FreeHashTable(satisfied, NULL);
    free(satisfied);
    out_num_batches[0] = 0;
    return false;
}
