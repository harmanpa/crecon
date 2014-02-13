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

typedef struct {
    char *str_a;
    char *str_b;
    char **str_array;
} comp_object;


#define BUFFER_STRING_LENGTH 30

recon_status test1_write_mobj_field(recon_wall_object object) {
    recon_status status = RECON_OK;
    int no_array_elements = 3, i = 0;
    char buffer_string[BUFFER_STRING_LENGTH];
    char field_name[BUFFER_STRING_LENGTH];
    char *tofree;
    comp_object myobj;
    recon_wall_object_mobj msgobj;
    msgpack_packer *packer;
    msgpack_sbuffer *buffer;

    // Assemble the user object
    myobj.str_a = (char*) malloc(strlen("string_a") + 1);
    myobj.str_a = "string_a";
    myobj.str_b = (char*) malloc(strlen("string_b") + 1);
    myobj.str_b = "string_b";
    myobj.str_array = (char**) malloc(no_array_elements * sizeof (char*));
    for (i = 0; i < no_array_elements; i++) {
        sprintf(buffer_string, "string_array_elem%i", i);
        myobj.str_array[i] = (char*) malloc(strlen(buffer_string) + 1);
        memcpy(myobj.str_array[i], buffer_string, strlen(buffer_string) + 1);
    }
    // Prepare recon_wall_object_mobj object to help construction
    status = recon_wall_object_new_mobj(&msgobj);
    if (status != RECON_OK) {
        return status;
    }
    status = recon_wall_object_get_mobj_packer(msgobj, &packer);
    if (status != RECON_OK) {
        return status;
    }
    status = recon_wall_object_get_mobj_buffer(msgobj, &buffer);
    if (status != RECON_OK) {
        return status;
    }
    // Manually construct msgpack version of object
    msgpack_pack_map(packer, 3); //myobj : {str_a, str_b, str_array}
    msgpack_pack_raw(packer, strlen("str_a"));
    msgpack_pack_raw_body(packer, "str_a", strlen("str_a"));
    msgpack_pack_raw(packer, strlen(myobj.str_a));
    msgpack_pack_raw_body(packer, myobj.str_a, strlen(myobj.str_a));
    msgpack_pack_raw(packer, strlen("str_b"));
    msgpack_pack_raw_body(packer, "str_b", strlen("str_b"));
    msgpack_pack_raw(packer, strlen(myobj.str_b));
    msgpack_pack_raw_body(packer, myobj.str_b, strlen(myobj.str_b));
    msgpack_pack_raw(packer, strlen("str_array"));
    msgpack_pack_raw_body(packer, "str_array", strlen("str_array"));
    msgpack_pack_array(packer, no_array_elements);
    for (i = 0; i < no_array_elements; i++) {
        msgpack_pack_raw(packer, strlen(myobj.str_array[i]));
        msgpack_pack_raw_body(packer, myobj.str_array[i], strlen(myobj.str_array[i]));
    }
    // Finished manual construction
    status = recon_wall_object_finish_mobj(msgobj);
    if (status != RECON_OK) {
        return status;
    }
    // Check it looks right
    printf("Object to be written to wall:\n\n");
    status = recon_wall_object_print_mobj(msgobj);
    if (status != RECON_OK) {
        return status;
    }
    // Serialize it to wall file
    status = recon_wall_start_field_entry(object);
    if (status != RECON_OK) {
        return status;
    }
    status = recon_wall_object_add_field_object(object, "myobj", msgobj);
    if (status != RECON_OK) {
        return status;
    }
    status = recon_wall_end_field_entry(object);
    /*free(myobj.str_a);
    free(myobj.str_b);
    for (i = 0; i < no_array_elements; i++) {
        free(myobj.str_array[i]);
    }
    free(myobj.str_array);*/
    return status;
}

recon_status test1_read_myobj(recon_wall_object object) {
    int fieldindex = -1, noelements = 0, i = 0, j = 0, len_array = 0;
    comp_object myobj;
    recon_booleantype isstring;
    recon_wall_object_mobj msgobj;
    recon_wall_object_field field;
    recon_wall_object_field_element elems;
    char *fieldname;
    char **fieldarray;
    recon_status status = RECON_OK;
    status = recon_wall_object_find_field(object, "myobj", &fieldindex);
    if (status != RECON_OK) {
        return status;
    }
    status = recon_wall_object_get_field(object, fieldindex, &field);
    if (status != RECON_OK) {
        return status;
    }
    status = recon_wall_object_get_field_ischar(field, &isstring);
    if (status != RECON_OK) {
        return status;
    }
    status = recon_wall_object_get_field_value(field, (void**) &msgobj);
    if (status != RECON_OK) {
        return status;
    }
    printf("Object fetched from wall (msgpack form):\n\n");
    status = recon_wall_object_print_mobj(msgobj);
    printf("After field elements have been tokenized\n\n");
    if (!isstring) {
        status = recon_wall_object_parse_field_value_elements(field, &elems, &noelements);
        if (status != RECON_OK) {
            return status;
        }
        for (i = 0; i < noelements; i++) {
            status = recon_wall_object_get_field_value_element(elems, i, &fieldname, &fieldarray, &len_array);
            if (status != RECON_OK) {
                return status;
            }
            printf("name: %s\n", fieldname);
            printf("value: %s", fieldarray[0]);
            for (j = 1; j < len_array; j++) {
                printf(", %s", fieldarray[j]);
            }
            printf("\n");
        }
    }
    return status;
}

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
    int fieldindex = -1;
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
        //status = recon_transform_create_inverse(&transform);
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
    for (i = 0; i < nfields; i++) {
        status = recon_wall_start_field_entry(object);
        if (status != RECON_OK) {
            printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed to start field entry\n");
            return;
        }
        fieldname = (char*) malloc(6 + recon_util_digits(i));
        sprintf(fieldname, "field%i", i);
        status = recon_wall_object_add_field_int(object, fieldname, i);
        if (status != RECON_OK) {
            printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed to add field entry\n");
            return;
        }
        free(fieldname);
        status = recon_wall_end_field_entry(object);
        if (status != RECON_OK) {
            printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed to end field entry\n");
            return;
        }
    }
    status = test1_write_mobj_field(object);
    if (status != RECON_OK) {
        printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed to write complex object field\n");
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

    status = recon_wall_table_count_rows(table2, &nrows2);
    if (status != RECON_OK) {
        printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed counting rows\n");
        return;
    }
    fprintf(stderr, "Found %i rows for table", nrows2);
    signal = (double*) malloc(nrows2 * sizeof (double));

    status = recon_wall_table_get_signal_double(table2, "variable1", signal);
    if (status != RECON_OK) {
        printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed reading signal (%i)\n", status);
        return;
    }
    for (i = 0; i < nrows2; i++) {
        fprintf(stderr, "%f,", signal[i]);
    }

    status = recon_wall_table_get_signal_double(table2, "1elbairav", signal);
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
    status = recon_wall_object_get_fields(object2);
    if (status != RECON_OK) {
        printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed to find object in file\n");
        return;
    }
    status = recon_wall_object_find_field(object2, "field0", &fieldindex);
    if (status != RECON_OK) {
        printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed to find field\n");
        return;
    }
    status = test1_read_myobj(object2);
    if (status != RECON_OK) {
        printf("%%TEST_FAILED%% time=0 testname=test1 (wallwritetest) message=Failed to read complex object field\n");
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

