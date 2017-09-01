/*
 * Copyright (c) 2017, Martin McDonough. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBQED_BATCH_H
#define LIBQED_BATCH_H
#pragma once

#include <stdbool.h>

struct QED_Dependency;

struct QED_Batch{
    unsigned num_dependencies;
    struct QED_Dependency **dependencies;
};

enum QED_BatchAlgorithm {
    /** The earliest set of endlings (deps with no remaining unfulfilled
     * dependencies) are chosen for each batch. Mostly for testing.
     */
    QED_eGreedy,
    QED_eLookahead, /**< The entire most efficient tree is calculated. */
    QED_eBalanced /**< A heuristic is applied to limit computation. */
};

bool QED_CalculateBatches(struct QED_Batch ***out_batches,
    unsigned *out_num_batches,
    struct QED_Dependency **deps,
    unsigned num_deps,
    unsigned max_batch_size,
    enum QED_BatchAlgorithm algorithm);

#endif /* LIBQED_BATCH_H */
