/*
 * Copyright (c) 2017, Martin McDonough. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "qed_batch.h"
#include "qed_dependency.h"
#include "qed_test.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define QED_NUM_TESTS 6

static int QED_TestZeroDependencies(){
    
    struct QED_Batch **batches;
    unsigned num_batches;
    
    QED_EXPECT_TRUE(QED_CalculateBatches(&batches, &num_batches, NULL, 0, 8, QED_eGreedy));
    
    QED_EXPECT_INT_EQ(num_batches, 0);
    
    return 1;
}

static int QED_TestOneDependencies(){
    
    struct QED_Batch **batches;
    unsigned num_batches;
    
    struct QED_Dependency dep, *dep_ptr = &dep;
    dep.dependencies = NULL;
    dep.num_dependencies = 0;
    
    QED_EXPECT_TRUE(QED_CalculateBatches(&batches, &num_batches, &dep_ptr, 1, 8, QED_eGreedy));
    
    QED_EXPECT_INT_EQ(num_batches, 1);
    QED_ASSERT_INT_EQ(batches[0]->num_dependencies, 1);
    QED_EXPECT_TRUE((batches[0]->dependencies[0] == dep_ptr));
    
    return 1;
}

static int QED_TestTwoSeparateDependencies(){
    
    struct QED_Batch **batches;
    unsigned num_batches;
    
    struct QED_Dependency deps[2];
    
    struct QED_Dependency *deps_ptr[2];
    deps_ptr[0] = deps+0;
    deps_ptr[1] = deps+1;
    
    memset(deps, 0, sizeof(struct QED_Dependency)<<1);
    
    QED_EXPECT_TRUE(QED_CalculateBatches(&batches, &num_batches, deps_ptr, 2, 8, QED_eGreedy));
    
    QED_ASSERT_INT_EQ(num_batches, 1);
    QED_ASSERT_INT_EQ(batches[0]->num_dependencies, 2);
    
    {
        struct QED_Dependency **batch_deps = batches[0]->dependencies;
        
        /* The order doesn't matter. We should see two different deps, and we
         * we should see both our inputs appear.
         */
        QED_EXPECT_TRUE((batch_deps[0] != batch_deps[1]));
        QED_EXPECT_TRUE((batch_deps[0] == deps+0 || batch_deps[0] == deps+1));
        QED_EXPECT_TRUE((batch_deps[1] == deps+0 || batch_deps[1] == deps+1));
    }
    return 1;
}

static int QED_TestTwoLinkedDependencies(){
    
    struct QED_Batch **batches;
    unsigned num_batches;
    
    struct QED_Dependency deps[2];
    
    struct QED_Dependency *deps_ptr[2];
    deps_ptr[0] = deps+0;
    deps_ptr[1] = deps+1;
    
    deps[0].num_dependencies = 0;
    deps[0].dependencies = NULL;
    
    deps[1].num_dependencies = 1;
    /* Reuse the deps pointer. */
    deps[1].dependencies = deps_ptr;
    
    QED_EXPECT_TRUE(QED_CalculateBatches(&batches, &num_batches, deps_ptr, 2, 8, QED_eGreedy));
    
    QED_ASSERT_INT_EQ(num_batches, 2);
    QED_ASSERT_INT_EQ(batches[0]->num_dependencies, 1);
    QED_ASSERT_INT_EQ(batches[1]->num_dependencies, 1);
    
    QED_EXPECT_TRUE((batches[0]->dependencies[0] == deps+0));
    QED_EXPECT_TRUE((batches[1]->dependencies[0] == deps+1));
    
    return 1;
}

/* Two deps that both depend on a single other. */
static int QED_TestThreeTreeDependencies(){
    
    struct QED_Batch **batches;
    unsigned num_batches;
    
    struct QED_Dependency deps[3];
    
    struct QED_Dependency *deps_ptr[3];
    deps_ptr[0] = deps+0;
    deps_ptr[1] = deps+1;
    deps_ptr[2] = deps+2;
    
    deps[0].num_dependencies = 0;
    deps[0].dependencies = NULL;
    
    deps[1].num_dependencies = 1;
    /* Reuse the deps pointer. */
    deps[1].dependencies = deps_ptr;
    
    deps[2].num_dependencies = 1;
    /* Reuse the deps pointer. */
    deps[2].dependencies = deps_ptr;
    
    QED_EXPECT_TRUE(QED_CalculateBatches(&batches, &num_batches, deps_ptr, 3, 8, QED_eGreedy));
    
    QED_ASSERT_INT_EQ(num_batches, 2);
    QED_ASSERT_INT_EQ(batches[0]->num_dependencies, 1);
    QED_ASSERT_INT_EQ(batches[1]->num_dependencies, 2);
    
    QED_EXPECT_TRUE((batches[0]->dependencies[0] == deps+0));
    
    /* Expect that both of the other deps appear in the second batch */
    {
        struct QED_Dependency **batch_deps = batches[1]->dependencies;
        
        /* The order doesn't matter. We should see two different deps, and we
         * we should see both our inputs appear.
         */
        QED_EXPECT_TRUE((batch_deps[0] != batch_deps[1]));
        QED_EXPECT_TRUE((batch_deps[0] == deps+1 || batch_deps[0] == deps+2));
        QED_EXPECT_TRUE((batch_deps[1] == deps+1 || batch_deps[1] == deps+2));
    }
    
    return 1;
}

/* One dep that depends on two others. */
static int QED_TestThreeInvertedTreeDependencies(){
    
    struct QED_Batch **batches;
    unsigned num_batches;
    
    struct QED_Dependency deps[3];
    
    struct QED_Dependency *deps_ptr[3];
    deps_ptr[0] = deps+0;
    deps_ptr[1] = deps+1;
    deps_ptr[2] = deps+2;
    
    deps[0].num_dependencies = 2;
    /* Reuse the deps pointer. */
    deps[0].dependencies = deps_ptr + 1;
    
    deps[1].num_dependencies = 0;
    deps[1].dependencies = NULL;
    
    deps[2].num_dependencies = 0;
    deps[2].dependencies = NULL;
    
    QED_EXPECT_TRUE(QED_CalculateBatches(&batches, &num_batches, deps_ptr, 3, 8, QED_eGreedy));
    
    QED_ASSERT_INT_EQ(num_batches, 2);
    QED_ASSERT_INT_EQ(batches[0]->num_dependencies, 2);
    QED_ASSERT_INT_EQ(batches[1]->num_dependencies, 1);
    
    QED_EXPECT_TRUE((batches[1]->dependencies[0] == deps+0));
    
    /* Expect that both of the other deps appear in the second batch */
    {
        struct QED_Dependency **batch_deps = batches[0]->dependencies;
        
        /* The order doesn't matter. We should see two different deps, and we
         * we should see both our inputs appear.
         */
        QED_EXPECT_TRUE((batch_deps[0] != batch_deps[1]));
        QED_EXPECT_TRUE((batch_deps[0] == deps+1 || batch_deps[0] == deps+2));
        QED_EXPECT_TRUE((batch_deps[1] == deps+1 || batch_deps[1] == deps+2));
    }
    
    return 1;
}

const struct QED_Test QED_Tests[QED_NUM_TESTS] = {
    QED_TEST(QED_TestZeroDependencies),
    QED_TEST(QED_TestOneDependencies),
    QED_TEST(QED_TestTwoSeparateDependencies),
    QED_TEST(QED_TestTwoLinkedDependencies),
    QED_TEST(QED_TestThreeTreeDependencies),
    QED_TEST(QED_TestThreeInvertedTreeDependencies)
};

static char *strdup_to_lower(const char *str, char *buffer){
    const unsigned stringlen = strlen(str);
    unsigned i;
    buffer = realloc(buffer, stringlen + 1);
    for(i = 0; i < stringlen; i++){
        const char c = str[i];
        if(c >= 'A' && c <= 'Z')
            buffer[i] = (c - 'A') + 'a';
        else
            buffer[i] = c;
    }
    buffer[stringlen] = '\0';
    return buffer;
}

static unsigned run_test_by_name(const char *name,
    const struct QED_Test *tests,
    unsigned num_tests,
    char **buffer){
    unsigned e;
    for(e = 0; e < num_tests; e++){
        buffer[0] = strdup_to_lower(tests[e].name, *buffer);
        if(strcmp(*buffer, name) == 0){
            unsigned short unused;
            QED_RUN_TEST(tests[e], unused, "Test");
            return 1;
        }
    }
    return 0;
}

QED_TEST_FUNCTION(QED_Test, QED_Tests, "QED")

int main(int argc, char **argv){
    char *buffer = NULL;
    if(argc > 1){
        unsigned i = 1, num_ran = 0;
        char *arg = NULL;
        while(i < (unsigned)argc){
            const char *name = arg = strdup_to_lower(argv[i++], arg);
            if(run_test_by_name(name, QED_Tests, QED_NUM_TESTS, &buffer))
                num_ran++;
            else{
                fputs("[Test]ERROR:\tCould not find test ", stdout); puts(name);
            }
        }
        free(arg);
        free(buffer);
        return ((int)num_ran == argc - 1) ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    {
        int i = 0;
        QED_RUN_TEST_SUITE(QED_Test, i, "QED");
    }
    free(buffer);
    return EXIT_SUCCESS;
}
