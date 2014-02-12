/*
 * File:   wallwritetest.c
 * Author: pete
 *
 * Created on Dec 2, 2013, 10:54:37 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "crecon.h"
#include "crecon_impl.h"

/*
 * Simple C Test Suite
 */

void test1() {
    int i, j;
    char* signalname, *aliasname, *transform, *signalnamegot, *aliasnamegot;
    char* fieldname;
    recon_status status;
    recon_wall wall;
    recon_wall wall2;
    recon_wall_table table;
    recon_wall_table table2;
    recon_wall_object object;
    recon_wall_object object2;
    int nsignals = 100;
    int naliases = nsignals;
    int nrows = 100;
    int nrows2;
    int nflush = 100;
    int nobjects = 1;
    int nfields = 10;
    double* signal;
    printf("wallwritetest test 1\n");
    status = recon_wall_create("test.wll", 1, nobjects, &wall);
    if (status != RECON_OK) {
        printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed creating file\n");
        return;
    }
    status = recon_wall_add_table(wall, "myTable", nsignals, naliases, &table);
    if (status != RECON_OK) {
        printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed adding table\n");
        return;
    }
    for (i = 0; i < nsignals; i++) {
        signalname = (char*) calloc(9 + recon_util_digits(i), 1);
        aliasname = (char*) calloc(9 + recon_util_digits(i), 1);
        sprintf(signalname, "%s%i", "variable", i);
        sprintf(aliasname, "%i%s", i, "elbairav");
        status = recon_wall_table_add_signal(table, signalname);
        if (status != RECON_OK) {
            printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed adding signal %s, error=%i\n", signalname, status);
            return;
        }
        status = recon_transform_create_affine(&transform, -1.0, (double) i);
        status = recon_wall_table_add_alias(table, aliasname, signalname, transform);
        free(signalname);
        free(aliasname);
        free(transform); //Alloc'd in transform_create
    }
    status = recon_wall_add_object(wall, "object1", &object);
    if (status != RECON_OK) {
        printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed to add object\n");
        return;
    }

    status = recon_wall_flush(wall);
    if (status != RECON_OK) {
        printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed flush\n");
        return;
    }
    status = recon_wall_start_field_entry(object);
    if (status != RECON_OK) {
        printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed to start field entry\n");
        return;
    }
    for (i = 0; i < nfields; i++) {
        fieldname = (char*) malloc(6 + recon_util_digits(i));
        sprintf(fieldname, "field%i", i);
        status = recon_wall_object_add_field_int(object, fieldname, i);
        free(fieldname);
    }
    status = recon_wall_end_field_entry(object);
    if (status != RECON_OK) {
        printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed to end field entry\n");
        return;
    }
    for (i = 0; i < nrows; i++) {
        status = recon_wall_table_start_row(table);
        if (status != RECON_OK) {
            printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed row start\n");
            return;
        }
        for (j = 0; j < nsignals; j++) {
            status = recon_wall_table_row_add_double(table, 1.0 * (j + 1));
            if (status != RECON_OK) {
                printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed row write\n");
                return;
            }
        }
        status = recon_wall_table_end_row(table);
        if (status != RECON_OK) {
            printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed row end\n");
            return;
        }
        if (i % nflush == 0) {
            status = recon_wall_flush(wall);
            if (status != RECON_OK) {
                printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed flush\n");
                return;
            }
        }
    }
    status = recon_wall_close(wall);
    if (status != RECON_OK) {
        printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed close\n");
        return;
    }

    status = recon_wall_open("test.wll", &wall2);
    if (status != RECON_OK) {
        switch (status) {
            case RECON_READ_ERROR:
                printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed read\n");
                return;
            case RECON_DESERIALIZATION_ERROR:
                printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed deser\n");
                return;
        }

    }

    status = recon_wall_find_table(wall2, "myTable", &table2);
    if (status != RECON_OK) {
        printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed finding table\n");
        return;
    }
    for (i = 0; i < nsignals; i++) {
        signalname = (char*) calloc(9 + recon_util_digits(i), 1);
        signalnamegot = (char*) calloc(9 + recon_util_digits(i), 1);
        aliasname = (char*) calloc(9 + recon_util_digits(i), 1);
        aliasnamegot = (char*) calloc(9 + recon_util_digits(i), 1);
        sprintf(signalname, "%s%i", "variable", i);
        sprintf(aliasname, "%i%s", i, "elbairav");
        status = recon_wall_table_get_signal(table2, i, &signalnamegot);
        if (status != RECON_OK || strcmp(signalname, signalnamegot) != 0) {
            printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed finding signal\n");
            return;
        }
        status = recon_wall_table_get_alias(table2, i, &aliasnamegot);
        if (status != RECON_OK || strcmp(aliasname, aliasnamegot) != 0) {
            printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed finding alias\n");
            return;
        }
        free(signalname);
        free(aliasname);
        free(signalnamegot);
        free(aliasnamegot);
    }


    status = recon_wall_table_count_rows(table2, &nrows2);
    if (status != RECON_OK) {
        printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed counting rows\n");
        return;
    }
    fprintf(stderr, "Found %i rows for table", nrows2);
    signal = (double*) malloc(nrows2 * sizeof (double));

    status = recon_wall_table_get_signal_double(table2, "variable1", signal);
    if (status != RECON_OK) {
        printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed reading signal\n");
        return;
    }
    for (i = 0; i < nrows2; i++) {
        fprintf(stderr, "%f,", signal[i]);
    }

    status = recon_wall_find_object(wall2, "object1", &object2);
    if (status != RECON_OK) {
        printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed to find object\n");
        return;
    }
    status = recon_wall_close(wall2);
    if (status != RECON_OK) {
        printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed close\n");
        return;
    }
}

void test2() {
    uint32_t i;
    unsigned char* a = (unsigned char*) malloc(8);
    recon_util_int_to_bytes(22, a + 4);
    i = recon_util_bytes_to_int(a + 4);
    if (i != 22) {
        printf("%%TEST_FAILED%% time=0 testname=test2 (wallwritetest) message=Failed: %i\n", i);
        return;
    }
}

int main(int argc, char** argv) {
    time_t t0, t1;
    printf("%%SUITE_STARTING%% wallwritetest\n");
    printf("%%SUITE_STARTED%%\n");

    printf("%%TEST_STARTED%% test1 (wallwritetest)\n");
    time(&t0);
    test1();
    time(&t1);
    printf("%%TEST_FINISHED%% time=%d test1 (wallwritetest) \n", (int) (t1 - t0));

    printf("%%SUITE_FINISHED%% time=%d\n", (int) (t1 - t0));

    return (EXIT_SUCCESS);
}

