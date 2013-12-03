
#ifndef CRECON_H
#define	CRECON_H
#include <stdio.h>
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

#ifdef	__cplusplus
extern "C" {
#endif

    typedef int recon_status;
    typedef void* recon_wall;
    typedef void* recon_wall_table;
    typedef void* recon_wall_object;

    /**
     * Open an existing Wall file
     * @param File pointer
     * @param Pointer to Wall object
     * @return Status
     */
    recon_status recon_wall_open(FILE*, recon_wall*);
    
    /**
     * Create a new Wall file
     * @param File pointer
     * @param Number of tables
     * @param Number of objects
     * @param Pointer to Wall object
     * @return Status
     */
    recon_status recon_wall_create(FILE*, int, int, recon_wall*);
    
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
     * 
     * @param 
     * @param 
     * @return 
     */
    recon_status recon_wall_table_add_signal(recon_wall_table, const char*);
    recon_status recon_wall_table_add_alias(recon_wall_table, const char*, const char*, char*);
    recon_status recon_wall_get_table(recon_wall, int, recon_wall_table*);
    recon_status recon_wall_find_table(recon_wall, const char*, recon_wall_table*);
    recon_status recon_wall_table_find_signal(recon_wall_table, const char*, int*);
    recon_status recon_wall_table_get_signal(recon_wall_table, int, char**);
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

