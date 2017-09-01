/*
 * Copyright (c) 2017, Martin McDonough. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "qed_greedy.h"

#include "qed_batch.h"
#include "qed_dependency.h"
#include "qed_tinyhash.h"

#include <assert.h>
#include <stdlib.h>

struct qed_greedy_arg{
    unsigned generation;
    struct QED_HashTable *satisfied;
    struct QED_Dependency **dest;
    unsigned max_deps, found_deps;
};

#ifndef NDEBUG
#define VERIFY assert
#else
#define VERIFY(X) X
#endif

static int qed_greedy_iterator(int accum, void *arg, qed_hashkey_t key, qed_hashdata_t data){
    struct qed_greedy_arg *const dep_arg = (struct qed_greedy_arg*)arg;
    if(data == 0){
        struct QED_Dependency *const dep = (struct QED_Dependency *)key;
        const unsigned generation = dep_arg->generation;
        bool satisfied = true;
        unsigned i;
        
        qed_hashdata_t built;
        
        for(i = 0; i < dep->num_dependencies; i++){
            if(QED_HashTableGet(dep_arg->satisfied, dep->dependencies[i], &built) &&
                built == 0 ||
                built >= generation){
                
                assert(built == generation || built == 0);
                /* This dep is unsatisfied, skip it. */
                return accum;
            }
        }
        
        {
            const bool set = QED_HashTableSet(dep_arg->satisfied, dep, generation);
            (void)set;
            assert(set);
        }
        assert(dep_arg->found_deps < dep_arg->max_deps);
        dep_arg->dest[dep_arg->found_deps++] = dep;
        if(dep_arg->found_deps >= dep_arg->max_deps)
            return -(accum+1);
        else
            return accum+1;
    }
    return accum;
}

bool QED_CalculateBatchesGreedy(struct QED_Batch **in_out_batches,
    unsigned *out_num_batches,
    struct QED_HashTable *satisfied,
    struct QED_Dependency **deps,
    unsigned num_deps,
    unsigned max_batch_size){
    
    unsigned num_satisfied = 0, num_batches = 0;
    struct qed_greedy_arg arg;
    arg.satisfied = satisfied;
    arg.max_deps = max_batch_size;
    arg.generation = 0;
    
    while(num_satisfied != num_deps){
        arg.generation++;
        in_out_batches[num_batches] = malloc(sizeof(struct QED_Batch));
        arg.dest = in_out_batches[num_batches]->dependencies = calloc(max_batch_size, sizeof(void*));
        arg.found_deps = 0;
        {
            const int num = QED_HashTableIterate(satisfied, 0, &arg, qed_greedy_iterator);
            
            if(num == 0){
                return false;
            }
            else if(num < 0){
                assert(-num == max_batch_size);
                in_out_batches[num_batches]->num_dependencies = max_batch_size;
            }
            else{
                assert(num <= max_batch_size);
                in_out_batches[num_batches]->num_dependencies = num;
            }
        }
        num_satisfied += in_out_batches[num_batches]->num_dependencies;
        num_batches++;
        assert(num_satisfied <= num_deps);
    }    
    
    out_num_batches[0] = num_batches;
    return true;
}