/*
 * Copyright (c) 2014 Jerry Lundström <lundstrom.jerry@gmail.com>
 * Copyright (c) 2014 .SE (The Internet Infrastructure Foundation).
 * Copyright (c) 2014 OpenDNSSEC AB (svb)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "CUnit/Basic.h"

#include "../db_configuration.h"
#include "../db_connection.h"
#include "../database_version.h"

#include <string.h>

static db_configuration_list_t* configuration_list = NULL;
static db_configuration_t* configuration = NULL;
static db_connection_t* connection = NULL;

static database_version_t* object = NULL;
static database_version_list_t* object_list = NULL;
static db_value_t id = DB_VALUE_EMPTY;
static db_clause_list_t* clause_list = NULL;

#if defined(ENFORCER_DATABASE_SQLITE3)
int test_database_version_init_suite_sqlite(void) {
    if (configuration_list) {
        return 1;
    }
    if (configuration) {
        return 1;
    }
    if (connection) {
        return 1;
    }

    /*
     * Setup the configuration for the connection
     */
    if (!(configuration_list = db_configuration_list_new())) {
        return 1;
    }
    if (!(configuration = db_configuration_new())
        || db_configuration_set_name(configuration, "backend")
        || db_configuration_set_value(configuration, "sqlite")
        || db_configuration_list_add(configuration_list, configuration))
    {
        db_configuration_free(configuration);
        configuration = NULL;
        db_configuration_list_free(configuration_list);
        configuration_list = NULL;
        return 1;
    }
    configuration = NULL;
    if (!(configuration = db_configuration_new())
        || db_configuration_set_name(configuration, "file")
        || db_configuration_set_value(configuration, "test.db")
        || db_configuration_list_add(configuration_list, configuration))
    {
        db_configuration_free(configuration);
        configuration = NULL;
        db_configuration_list_free(configuration_list);
        configuration_list = NULL;
        return 1;
    }
    configuration = NULL;

    /*
     * Connect to the database
     */
    if (!(connection = db_connection_new())
        || db_connection_set_configuration_list(connection, configuration_list))
    {
        db_connection_free(connection);
        connection = NULL;
        db_configuration_list_free(configuration_list);
        configuration_list = NULL;
        return 1;
    }
    configuration_list = NULL;

    if (db_connection_setup(connection)
        || db_connection_connect(connection))
    {
        db_connection_free(connection);
        connection = NULL;
        return 1;
    }

    return 0;
}
#endif

#if defined(ENFORCER_DATABASE_COUCHDB)
int test_database_version_init_suite_couchdb(void) {
    if (configuration_list) {
        return 1;
    }
    if (configuration) {
        return 1;
    }
    if (connection) {
        return 1;
    }

    /*
     * Setup the configuration for the connection
     */
    if (!(configuration_list = db_configuration_list_new())) {
        return 1;
    }
    if (!(configuration = db_configuration_new())
        || db_configuration_set_name(configuration, "backend")
        || db_configuration_set_value(configuration, "couchdb")
        || db_configuration_list_add(configuration_list, configuration))
    {
        db_configuration_free(configuration);
        configuration = NULL;
        db_configuration_list_free(configuration_list);
        configuration_list = NULL;
        return 1;
    }
    configuration = NULL;
    if (!(configuration = db_configuration_new())
        || db_configuration_set_name(configuration, "url")
        || db_configuration_set_value(configuration, "http://127.0.0.1:5984/opendnssec")
        || db_configuration_list_add(configuration_list, configuration))
    {
        db_configuration_free(configuration);
        configuration = NULL;
        db_configuration_list_free(configuration_list);
        configuration_list = NULL;
        return 1;
    }
    configuration = NULL;

    /*
     * Connect to the database
     */
    if (!(connection = db_connection_new())
        || db_connection_set_configuration_list(connection, configuration_list))
    {
        db_connection_free(connection);
        connection = NULL;
        db_configuration_list_free(configuration_list);
        configuration_list = NULL;
        return 1;
    }
    configuration_list = NULL;

    if (db_connection_setup(connection)
        || db_connection_connect(connection))
    {
        db_connection_free(connection);
        connection = NULL;
        return 1;
    }

    return 0;
}
#endif

static int test_database_version_clean_suite(void) {
    db_connection_free(connection);
    connection = NULL;
    db_configuration_free(configuration);
    configuration = NULL;
    db_configuration_list_free(configuration_list);
    configuration_list = NULL;
    db_value_reset(&id);
    db_clause_list_free(clause_list);
    clause_list = NULL;
    return 0;
}

static void test_database_version_new(void) {
    CU_ASSERT_PTR_NOT_NULL_FATAL((object = database_version_new(connection)));
    CU_ASSERT_PTR_NOT_NULL_FATAL((object_list = database_version_list_new(connection)));
}

static void test_database_version_set(void) {
    CU_ASSERT(!database_version_set_version(object, 1));
}

static void test_database_version_get(void) {
    CU_ASSERT(database_version_version(object) == 1);
}

static void test_database_version_create(void) {
    CU_ASSERT_FATAL(!database_version_create(object));
}

static void test_database_version_clauses(void) {

    CU_ASSERT_PTR_NOT_NULL_FATAL((clause_list = db_clause_list_new()));
    CU_ASSERT_PTR_NOT_NULL(database_version_version_clause(clause_list, database_version_version(object)));
    CU_ASSERT(!database_version_list_get_by_clauses(object_list, clause_list));
    CU_ASSERT_PTR_NOT_NULL(database_version_list_next(object_list));
    db_clause_list_free(clause_list);
    clause_list = NULL;
}

static void test_database_version_list(void) {
    const database_version_t* item;
    database_version_t* item2;

    CU_ASSERT_FATAL(!database_version_list_get(object_list));
    CU_ASSERT_PTR_NOT_NULL_FATAL((item = database_version_list_next(object_list)));
    CU_ASSERT_FATAL(!db_value_copy(&id, database_version_id(item)));

    CU_ASSERT_FATAL(!database_version_list_get(object_list));
    CU_ASSERT_PTR_NOT_NULL_FATAL((item2 = database_version_list_get_next(object_list)));
    database_version_free(item2);
    CU_PASS("database_version_free");
}

static void test_database_version_read(void) {
    CU_ASSERT_FATAL(!database_version_get_by_id(object, &id));
}

static void test_database_version_verify(void) {
    CU_ASSERT(database_version_version(object) == 1);
}

static void test_database_version_change(void) {
    CU_ASSERT(!database_version_set_version(object, 2));
}

static void test_database_version_update(void) {
    CU_ASSERT_FATAL(!database_version_update(object));
}

static void test_database_version_read2(void) {
    CU_ASSERT_FATAL(!database_version_get_by_id(object, &id));
}

static void test_database_version_verify2(void) {
    CU_ASSERT(database_version_version(object) == 2);
}

static void test_database_version_cmp(void) {
    database_version_t* local_object;

    CU_ASSERT_PTR_NOT_NULL_FATAL((local_object = database_version_new(connection)));
    CU_ASSERT(database_version_cmp(object, local_object));
}

static void test_database_version_delete(void) {
    CU_ASSERT_FATAL(!database_version_delete(object));
}

static void test_database_version_list2(void) {
    CU_ASSERT_FATAL(!database_version_list_get(object_list));
    CU_ASSERT_PTR_NULL(database_version_list_next(object_list));
}

static void test_database_version_end(void) {
    if (object) {
        database_version_free(object);
        CU_PASS("database_version_free");
    }
    if (object_list) {
        database_version_list_free(object_list);
        CU_PASS("database_version_list_free");
    }
}

static int test_database_version_add_tests(CU_pSuite pSuite) {
    if (!CU_add_test(pSuite, "new object", test_database_version_new)
        || !CU_add_test(pSuite, "set fields", test_database_version_set)
        || !CU_add_test(pSuite, "get fields", test_database_version_get)
        || !CU_add_test(pSuite, "create object", test_database_version_create)
        || !CU_add_test(pSuite, "object clauses", test_database_version_clauses)
        || !CU_add_test(pSuite, "list objects", test_database_version_list)
        || !CU_add_test(pSuite, "read object by id", test_database_version_read)
        || !CU_add_test(pSuite, "verify fields", test_database_version_verify)
        || !CU_add_test(pSuite, "change object", test_database_version_change)
        || !CU_add_test(pSuite, "update object", test_database_version_update)
        || !CU_add_test(pSuite, "reread object by id", test_database_version_read2)
        || !CU_add_test(pSuite, "verify fields after update", test_database_version_verify2)
        || !CU_add_test(pSuite, "compare objects", test_database_version_cmp)
        || !CU_add_test(pSuite, "delete object", test_database_version_delete)
        || !CU_add_test(pSuite, "list objects to verify delete", test_database_version_list2)
        || !CU_add_test(pSuite, "end test", test_database_version_end))
    {
        return CU_get_error();
    }
    return 0;
}

int test_database_version_add_suite(void) {
    CU_pSuite pSuite = NULL;
    int ret;

#if defined(ENFORCER_DATABASE_SQLITE3)
    pSuite = CU_add_suite("Test of database version (SQLite)", test_database_version_init_suite_sqlite, test_database_version_clean_suite);
    if (!pSuite) {
        return CU_get_error();
    }
    ret = test_database_version_add_tests(pSuite);
    if (ret) {
        return ret;
    }
#endif
#if defined(ENFORCER_DATABASE_COUCHDB)
    pSuite = CU_add_suite("Test of database version (CouchDB)", test_database_version_init_suite_couchdb, test_database_version_clean_suite);
    if (!pSuite) {
        return CU_get_error();
    }
    ret = test_database_version_add_tests(pSuite);
    if (ret) {
        return ret;
    }
#endif
    return 0;
}
