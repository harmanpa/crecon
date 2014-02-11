/*
 * The MIT License
 *
 * Copyright 2014 CyDesign Limited
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef CRECON_H
#define	CRECON_H
#include <stdio.h>
#include "msgpack.h"
#define RECON_OK 0
#define RECON_ALREADY_DEFINED -1
#define RECON_NAME_NOT_UNIQUE -2
#define RECON_INVALID_NUMBER_SIGNALS -3
#define RECON_NOT_FOUND -4
#define RECON_WRITE_ERROR -5
#define RECON_READ_ERROR -6
#define RECON_WRONG_FILE_TYPE -7
#define RECON_DESERIALIZATION_ERROR -8
#define RECON_SERIALIZATION_ERROR -9
#define RECON_NOT_FULLY_DEFINED -10
#define RECON_UNDEFINED -11
#define RECON_INCOMPLETE_ROW -12
#define RECON_INCOMPLETE_FIELD -13
#define RECON_OBJECT_DEFINITION_ERROR -14

#ifdef	__cplusplus
extern "C" {
#endif

    typedef int recon_status;
    typedef void* recon_wall;
    typedef void* recon_wall_table;
    typedef void* recon_wall_object;
    typedef void* recon_wall_signal_pointer;
    typedef void* recon_wall_object_mobj;

    /**
     * Open an existing Wall file
     * @param File pointer
     * @param Pointer to Wall object
     * @return Status
     */
    recon_status recon_wall_open(const char*, recon_wall*);
    
    /**
     * Create a new Wall file
     * @param File pointer
     * @param Number of tables
     * @param Number of objects
     * @param Pointer to Wall object
     * @return Status
     */
    recon_status recon_wall_create(char*, int, int, recon_wall*);
    
    /**
     * Close a Wall file
     * @param The Wall object
     * @return Status
     */
    recon_status recon_wall_close(recon_wall);
    
    /**
     * Finalize a Wall file (write header)
     * @param The Wall object
     * @return Status
     */
    recon_status recon_wall_finalize(recon_wall);
    
    /**
     * Flush data to file, finalizing first if necessary
     * @param The Wall object
     * @return Status
     */
    recon_status recon_wall_flush(recon_wall);

    // META DATA

    recon_status recon_wall_add_meta_double(recon_wall, const char*, double);
    recon_status recon_wall_add_meta_int(recon_wall, const char*, int);
    recon_status recon_wall_add_meta_string(recon_wall, const char*, const char*);

    // TABLE FUNCTIONS

    /**
     * Add a new table to the Wall file
     * @param The Wall object
     * @param Name
     * @param Number of signals
     * @param Number of aliases
     * @param Pointer to returned table object
     * @return Status
     */
    recon_status recon_wall_add_table(recon_wall, const char*, int, int, recon_wall_table*);
    
    /**
     * Get the number of tables in the Wall file
     * @param The Wall object
     * @param Pointer to number of tables
     * @return Status
     */
    recon_status recon_wall_n_tables(recon_wall,int*);
    
    /**
     * Fetch a table by index
     * @param The Wall object
     * @param Table index
     * @param Pointer to returned table
     * @return Status
     */
    recon_status recon_wall_get_table(recon_wall, int, recon_wall_table*);
    
    /**
     * Find a table by name
     * @param The Wall object
     * @param Table name
     * @param Pointer to returned table
     * @return Status
     */
    recon_status recon_wall_find_table(recon_wall, const char*, recon_wall_table*);

    /**
     * Add a signal to the table
     * @param 
     * @param 
     * @return 
     */
    recon_status recon_wall_table_add_signal(recon_wall_table, const char*);
    
    /**
     * 
     * @param 
     * @param 
     * @param 
     * @param 
     * @return 
     */
    recon_status recon_wall_table_add_alias(recon_wall_table, const char*, const char*, char*);

    /**
     * 
     * @param 
     * @param 
     * @param 
     * @return 
     */
    recon_status recon_wall_table_find_signal(recon_wall_table, const char*, int*);
    
    /**
     * 
     * @param 
     * @param 
     * @param 
     * @return 
     */
    recon_status recon_wall_table_get_alias(recon_wall_table, int, char**);
    /**
     * 
     * @param 
     * @param 
     * @param 
     * @return 
     */
    recon_status recon_wall_table_get_signal(recon_wall_table, int, char**);
    /**
     * 
     * @param 
     * @param 
     * @param 
     * @return 
     */
    recon_status recon_wall_table_n_signals(recon_wall_table, int*, int*);

    // ROW FUNCTIONS

    /**
     * Start writing a new row
     * @param The table to be written to
     */
    recon_status recon_wall_table_start_row(recon_wall_table);

    /**
     * Append a double to the current row
     * @param The table to be written to
     * @param The value to be written
     */
    recon_status recon_wall_table_row_add_double(recon_wall_table, double);

    /**
     * Append an int to the current row
     * @param The table to be written to
     * @param The value to be written
     */
    recon_status recon_wall_table_row_add_int(recon_wall_table, int);

    /**
     * Append a string to the current row
     * @param The table to be written to
     * @param The value to be written
     */
    recon_status recon_wall_table_row_add_string(recon_wall_table, char*);
    
    /**
     * Append a double array to the current row
     * @param The table to be written to
     * @param The value to be written
     * @param The number of values in the array
     */
    recon_status recon_wall_table_row_add_double_array(recon_wall_table, double*, int);
    
    /**
     * Append an int array to the current row
     * @param The table to be written to
     * @param The value to be written
     * @param The number of values in the array
     */
    recon_status recon_wall_table_row_add_int_array(recon_wall_table, int*, int);
    
    /**
     * Append a string array to the current row
     * @param The table to be written to
     * @param The value to be written
     * @param The number of values in the array
     */
    recon_status recon_wall_table_row_add_string_array(recon_wall_table, char**, int);

    /**
     * Close the current row
     * @param The table to be written to
     */
    recon_status recon_wall_table_end_row(recon_wall_table);
    
    // OBJECT FUNCTIONS

    /**
     * 
     * @param 
     * @param 
     * @param 
     * @return 
     */
    recon_status recon_wall_add_object(recon_wall, const char*, recon_wall_object*);
    /**
     * 
     * @param 
     * @param 
     * @return 
     */
    recon_status recon_wall_n_objects(recon_wall,int*);
    recon_status recon_wall_get_object(recon_wall,int,recon_wall_object*);
    recon_status recon_wall_find_object(recon_wall,const char*,recon_wall_object*);
    
    recon_status recon_wall_start_field_entry(recon_wall_object obj);
    recon_status recon_wall_object_add_field_string(recon_wall_object, const char*, const char*);
    recon_status recon_wall_object_add_field_double(recon_wall_object, const char*, double);
    recon_status recon_wall_object_add_field_int(recon_wall_object, const char*, int);
    recon_status recon_wall_object_add_field_object(recon_wall_object, const char*, void*);
    recon_status recon_wall_end_field_entry(recon_wall_object obj);
    
    /* Helper methods for generation of object field entries in msgpack form */
    recon_status recon_wall_object_new_mobj(recon_wall_object_mobj *mobj);
    recon_status recon_wall_object_finish_mobj(recon_wall_object_mobj mobj);
    recon_status recon_wall_object_free_mobj(recon_wall_object_mobj mobj);
    recon_status recon_wall_object_get_mobj_packer(recon_wall_object_mobj mobj, msgpack_packer **pack);
    recon_status recon_wall_object_get_mobj_buffer(recon_wall_object_mobj mobj, msgpack_sbuffer **buff);
    recon_status recon_wall_object_get_mobj_data(recon_wall_object_mobj mobj, msgpack_object **data);
    recon_status recon_wall_object_print_mobj(recon_wall_object_mobj mobj);

    // TRANSFORMATIONS

    recon_status recon_transform_create_none(char**);
    recon_status recon_transform_create_inverse(char**);
    recon_status recon_transform_create_affine(char**, double, double);

    //recon_status recon_transform_apply_double(recon_transform, double,double);
    //recon_status recon_transform_apply_float(recon_transform, float,float);
    //recon_status recon_transform_apply_int(recon_transform, int,int);


#ifdef	__cplusplus
}
#endif

#endif	/* CRECON_H */

