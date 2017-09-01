/*
 * Copyright (c) 2017, Martin McDonough. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBQED_GREEDY_H
#define LIBQED_GREEDY_H
#pragma once

#include <stdbool.h>

struct QED_HashTable;
struct QED_Dependency;
struct QED_Batch;

bool QED_CalculateBatchesGreedy(struct QED_Batch **in_out_batches,
    unsigned *out_num_batches,
    struct QED_HashTable *satisfied,
    struct QED_Dependency **deps,
    unsigned num_deps,
    unsigned max_batch_size);

#endif /* LIBQED_GREEDY_H */
