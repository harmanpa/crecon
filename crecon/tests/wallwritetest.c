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
int digits(int);

void test1() {
    int i, j;
    char* signalname;
    recon_status status;
    recon_wall wall;
    recon_wall wall2;
    recon_wall_table table;
    recon_wall_table table2;
    int nsignals = 100;
    int nrows = 100000;
    int nflush = 100;
    printf("wallwritetest test 1\n");
    FILE* f = fopen("test.wll", "w");
    status = recon_wall_create(f, 1, 0, &wall);
    if (status != RECON_OK) {
        printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed creating file\n");
        return;
    }
    status = recon_wall_add_table(wall, "myTable", nsignals, 0, &table);
    if (status != RECON_OK) {
        printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed adding table\n");
        return;
    }
    for (i = 0; i < nsignals; i++) {
        signalname = (char*) calloc(8 + digits(i), 1);
        sprintf(signalname, "%s%i", "variable", i);
        status = recon_wall_table_add_signal(table, signalname);
        if (status != RECON_OK) {
            printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed adding signal %s, error=%i\n", signalname, status);
            return;
        }
        free(signalname);
    }
    status = recon_wall_flush(wall);
    if (status != RECON_OK) {
        printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed flush\n");
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

    FILE* f2 = fopen("test.wll", "r");

    status = recon_wall_open(f2, &wall2);
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
    /*
    status = recon_wall_find_table(wall2, "myTable", &table2);
    if (status != RECON_OK) {
        printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed finding table\n");
        return;
    }
    
/*
    status = recon_wall_close(wall2);
    if (status != RECON_OK) {
        printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed close\n");
        return;
    }
*/

}

void test2() {
    uint32_t i;
    char* a = (char*) malloc(8);
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
    printf("%%TEST_FINISHED%% time=%d test1 (wallwritetest) \n", (int)(t1 - t0));

    printf("%%SUITE_FINISHED%% time=%d\n", (int)(t1 - t0));

    return (EXIT_SUCCESS);
}

int digits(int n) {
    int count = 1;
    while (n != 0) {
        n /= 10;
        ++count;
    }
    return count;
}