/*
 * Copyright (c) 2017, Martin McDonough. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef QED_TEST_H
#define QED_TEST_H
#pragma once

#include <stdio.h>

typedef int(*QED_TestFunction)(void);

struct QED_Test{
    QED_TestFunction function;
    const char * const name;
    unsigned enabled;
};

#define QED_EXPECT_TRUE(WHAT)\
    do{\
        if(!(WHAT)) {\
            printf("%s:%i in function %s: Expected %s to be true\n",\
                __FILE__, __LINE__, __FUNCTION__, #WHAT);\
        }\
    }while(0)

#define QED_EXPECT_FALSE(WHAT)\
    do{\
        if((WHAT)) {\
            printf("%s:%i in function %s: Expected %s to be false\n",\
                __FILE__, __LINE__, __FUNCTION__, #WHAT);\
        }\
    }while(0)

#define QED_EXPECT_INT_EQ(WHAT, EXPECTED)\
    do{\
        const int what_ = (int)WHAT, expected_ = (int)EXPECTED;\
        if(what_ != expected_) {\
            printf("%s:%i in function %s: Expected %s (%i) to be equal to %s (%i)\n",\
                __FILE__, __LINE__, __FUNCTION__, #WHAT, what_, #EXPECTED, expected_);\
        }\
    } while(0)

#define QED_ASSERT_INT_EQ(WHAT, EXPECTED)\
    do{ \
        const int what_ = (int)(WHAT), expected_ = (int)(EXPECTED);\
        if(what_ != expected_) {\
            printf("%s:%i in function %s: Failed assert %s (%i) to be equal to %s (%i)\n",\
                __FILE__, __LINE__, __FUNCTION__, #WHAT, what_, #EXPECTED, expected_);\
            return 0;\
        }\
    } while(0)

#define QED_ASSERT_QED_STR_EQ_LITERAL(STR, LITERAL)\
    do{\
        const unsigned STR_LEN = sizeof(LITERAL) - 1;\
        QED_EXPECT_INT_EQ((STR)->length, STR_LEN);\
        QED_EXPECT_INT_EQ(memcmp((STR)->text, LITERAL, STR_LEN), 0);\
    }while(0)

#define QED_TEST(FUNC)\
{FUNC, #FUNC, 1}

#define QED_DISABLED_TEST(FUNC)\
{FUNC, #FUNC, 0}

#define QED_RUN_TEST(T, SUCESSES, NAME)\
if(!T.enabled){\
    puts("[" NAME "]Disabled test:");\
    putchar('\t');\
    puts(T.name);\
}\
else if(!T.function()){\
    puts("[Test]Failed test:");\
    putchar('\t');\
    puts(T.name);\
}\
else do{\
    fputs("[Test]Passed:\t", stdout);\
    puts(T.name);\
    SUCESSES++;\
}while(0)

#define QED_TEST_FUNCTION(FUNC_NAME, TEST_ARRAY, NAME)\
int FUNC_NAME(){\
    unsigned i;\
    union {\
        unsigned u;\
        unsigned short s[2];\
    }results;\
    results.s[0] = 0;\
    results.s[1] = sizeof(TEST_ARRAY) / sizeof(TEST_ARRAY[0]);\
    for(i = 0; i<sizeof(TEST_ARRAY) / sizeof(TEST_ARRAY[0]); i++)\
        QED_RUN_TEST(TEST_ARRAY[i], results.s[0], NAME);\
    return results.u;\
}

#define QED_RUN_TEST_SUITE(R, I, NAME)\
{\
    union {\
        unsigned u;\
        unsigned short s[2];\
    }results;\
    putchar('\n');\
    puts("========== " #R " ==========\n[" NAME "]Beginnning " #R);\
    results.u = R();\
    printf("[" NAME "]" #R " Results: %i/%i\n========== " #R\
        " ==========\n", results.s[0], results.s[1]);\
    I |= (results.s[0] != results.s[1]);\
    putchar('\n');\
}

/*

An example of how to use these tests:

*//* Write a few tests. Returning 1 is a success, 0 is a failure. *//*
int QED_TestWorks(){
    return 1;
}

int QED_TestFails(){
    return 0;
}

*//* Create an array of the tests. *//*

static struct QED_Test athena_tests[] = {
    QED_TEST(QED_TestWorks),
    QED_TEST(QED_TestFails)
};

*//* This creates the test function for this module. *//*
QED_TEST_FUNCTION(QED_Test, athena_tests)

*//* Call that function somewhere (The first argument to 
    QED_TEST_FUNCTION is the name of the function). That runs the tests.*//*

#define ENABLE_TESTS
int main(){

#ifdef ENABLE_TESTS
*//* This should show one working test and one successful test. *//*
    QED_Test();
#endif

}

*/

#endif QED_TEST_H
