/*
 * Copyright (c) 2017, Martin McDonough. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBQED_CALLBACK_H
#define LIBQED_CALLBACK_H
#pragma once

typedef int QED_CallbackFunction(void *action_data, void *user_data);

struct QED_Callback {
    QED_CallbackFunction *func;
    void *user_data;
};

#endif /* LIBQED_CALLBACK_H */
