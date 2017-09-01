/*
 * Copyright (c) 2017, Martin McDonough. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBQED_DEPENDENCY_H
#define LIBQED_DEPENDENCY_H
#pragma once

#include "qed_callback.h"

struct QED_Dependency {
    struct QED_Callback execute;
    
    struct QED_Dependency **dependencies;
    unsigned num_dependencies;
};

#endif /* LIBQED_DEPENDENCY_H */
