
#ifndef CRECON_IMPL_H
#define	CRECON_IMPL_H

#include "crecon.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#ifdef	__cplusplus
extern "C" {
#endif


	static char* __recon_size_placeholder = "XXXX";

	typedef struct {
		char* data;
		msgpack_unpacked* unpacked;
	} recon_wall_row;

	typedef struct {
		size_t size;
		recon_wall_row* data;
		size_t alloc;
	} recon_wall_row_buffer;

	typedef struct {
		recon_wall wall;
		char* name;
		size_t nsignals;
		size_t ndefinedsignals;
		size_t naliases;
		size_t ndefinedaliases;
		char** signals;
		char** aliases;
		char** aliased;
		char** transforms;
		size_t nrows;
	} wall_table;
        
        typedef struct {
            char *name;
            char **values;
            size_t novalues;
        } wall_field_element;

	typedef struct {
		msgpack_sbuffer* buffer;
		msgpack_packer* packer;
                msgpack_unpacked* msg;
                msgpack_object* data;
                recon_booleantype finalized;
        } wall_object_mobj;

	typedef struct {
		char* name;
		void* field;
		recon_booleantype fieldischar;
                wall_field_element* elements;
                size_t nelements;
	} wall_field;

	typedef struct {
		recon_wall wall;
		char* name;
		wall_field* fields;
		wall_field* bufferedfields;
		size_t sizefields;
		size_t sizebufferedfields;
		size_t ndefinedfields;
		size_t nbufferedfields;
		void *objmeta;
	} wall_object;

	typedef struct {
		FILE* fp;
		recon_booleantype finalized;
		int byteswritten;
		int nfmeta;
		//wall_fmeta *fmeta;
		size_t ntables;
		size_t ndefinedtables;
		wall_table* tables;
		size_t nobjects;
		size_t ndefinedobjects;
		wall_object *objects;
		// Serialization
		msgpack_sbuffer* buffer;
		msgpack_packer* packer;
		// Row writing variables
		wall_table* currentrowtable;
		size_t currentrowtablesize;
		size_t currentrowwritten;
		size_t positionatrowstart;
		//Field writing variables
		wall_object* currentfield;
		size_t positionatfieldstart;
		// Row reading
		size_t nrows;
		recon_wall_row_buffer* rows;
	} wall_file;

	/**
	* 
	* @param 
	* @return 
	*/
	uint32_t recon_util_bytes_to_int(unsigned char*);

	/**
	* 
	* @param 
	* @param 
	*/
	void recon_util_int_to_bytes(uint32_t, char*);
	int recon_util_digits(int);

	recon_status recon_wall_free_object(wall_object*);
	recon_status recon_wall_free_object_field(wall_field*);
	recon_status recon_wall_object_find_buffered_field(recon_wall_object , const char* , int* );
	recon_status recon_wall_object_update_fields(recon_wall_object );
	recon_status recon_wall_pack_fixed_header(wall_file* , uint32_t );
	recon_status recon_wall_free_table(wall_table* );
	recon_status recon_wall_object_find_field(recon_wall_object , const char* , int* );
	recon_status recon_wall_object_update_field_value(recon_wall_object, int, void*, recon_booleantype);
	recon_status recon_wall_add_field(recon_wall_object obj, const char*, void*, recon_booleantype);
	recon_status recon_wall_unpack_fixed_header(wall_file* , uint32_t* );
	recon_status recon_wall_visit_keyvalue(wall_file* , msgpack_object , msgpack_object );
	recon_status recon_wall_visit_fmeta(wall_file*, msgpack_object_map);
	recon_status recon_wall_visit_objs(wall_file* , msgpack_object_map );
	recon_status recon_wall_visit_object(wall_file* , char* , uint32_t , msgpack_object_map );
	recon_status recon_wall_visit_tables(wall_file* , msgpack_object_map );
	recon_status recon_wall_visit_table(wall_file* , char* , uint32_t , msgpack_object_map );
	recon_status recon_wall_table_visit_signals(recon_wall_table , msgpack_object_array , uint32_t );
	recon_status recon_wall_table_visit_aliases(recon_wall_table , msgpack_object_map , uint32_t );
	recon_status recon_wall_table_visit_alias_fields(msgpack_object_map , char** , char** );
	recon_status recon_wall_visit_table_elements(msgpack_object_map , msgpack_object_map* , msgpack_object_array* , msgpack_object_map* , msgpack_object_map*);
	recon_status recon_wall_visit_rows(wall_file* );
	recon_status recon_object_buffer_create(recon_wall_row_buffer** );
	recon_status recon_wall_row_buffer_size(recon_wall_row_buffer* , int* );
	recon_status recon_wall_row_buffer_get(recon_wall_row_buffer* , int , recon_wall_row** );
	recon_status recon_wall_row_buffer_get_object(recon_wall_row_buffer* , int, msgpack_object** );
	recon_status recon_wall_row_buffer_append(recon_wall_row_buffer* , char* , size_t );
	recon_status recon_wall_row_destroy(recon_wall_row* );
	recon_status recon_wall_row_buffer_destroy(recon_wall_row_buffer* );
        recon_status recon_wall_object_add_field(recon_wall_object obj, const char* name, void* value, recon_booleantype ischar);
        recon_status recon_wall_object_new_mobj_from_file(recon_wall_object_mobj *mobj, msgpack_object *obj_in);

#ifdef	__cplusplus
}
#endif

#endif	/* CRECON_IMPL_H */

