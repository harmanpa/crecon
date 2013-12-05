#include "crecon_impl.h"

recon_status recon_wall_open(FILE* fp, recon_wall* out) {
    recon_status status = RECON_OK;
    uint32_t* header_size;
    char* header_data;
    msgpack_unpacked* header;
    msgpack_object object;
    int i;
    msgpack_object_kv kv;
    if (fp == NULL) {
        return RECON_READ_ERROR;
    }
    *out = (recon_wall*) calloc(1, sizeof (wall_file));
    wall_file* wall = (wall_file*) * out;
    wall->fp = fp;
    header_size = (uint32_t*) malloc(sizeof (uint32_t*));
    status = recon_wall_unpack_fixed_header(wall, header_size);
    if (RECON_OK != status) {
        return status;
    }
    header_data = (char*) malloc(*header_size);
    if (*header_size != fread(header_data, 1, *header_size, wall->fp)) {
        return RECON_READ_ERROR;
    }
    header = (msgpack_unpacked*) malloc(sizeof (msgpack_unpacked*));
    msgpack_unpacked_init(header);
    msgpack_unpack_next(header, header_data, *header_size, NULL);
    free(header_size);
    free(header_data);
    object = header->data;
    if (object.type != MSGPACK_OBJECT_MAP) {
        return RECON_DESERIALIZATION_ERROR;
    }
    if (object.via.map.size != 0) {
        msgpack_object_kv* p = object.via.map.ptr;
        msgpack_object_print(stderr, p->key);
        fprintf(stderr, "=>");
        msgpack_object_print(stderr, p->val);
        ++p;
        msgpack_object_kv * const pend = object.via.map.ptr + object.via.map.size;
        for (; p < pend; ++p) {
            fprintf(stderr, ", ");
            msgpack_object_print(stderr, p->key);
            fprintf(stderr, "=>");
            msgpack_object_print(stderr, p->val);
        }
    }



    //for (i = 0; i < object.via.map.size; i++) {

    /*
            switch(object.via.map.ptr[i].key.type) {
                case MSGPACK_OBJECT_RAW:
                    break;
                default:
                    return RECON_DESERIALIZATION_ERROR;
            }
     */
    //if (strcmp("tabs", object.via.map.ptr[i].key.via.raw.ptr)) {
    /*status = recon_wall_visit_tables(wall, object.via.map.ptr[i].val.via.map);
    if (RECON_OK != status) {
        return status;
    }*/
    //}

    //}
    msgpack_unpacked_destroy(header);
    free(header);
    return status;
}

recon_status recon_wall_visit_tables(wall_file* wall, msgpack_object_map map) {
    int i;
    recon_status status = RECON_OK;
    wall->ntables = map.size;
    wall->tables = (wall_table*) calloc(map.size, sizeof (wall_table*));
    for (i = 0; i < map.size; i++) {
        status = recon_wall_visit_table(wall, map.ptr[i].key.via.raw.ptr, map.ptr[i].val.via.map);
        if (RECON_OK != status) {
            return status;
        }
    }
    return status;
}

recon_status recon_wall_visit_table(wall_file* wall, char* name, msgpack_object_map map) {
    return RECON_OK;
}

recon_status recon_wall_create(FILE* fp, int nTables, int nObjects, recon_wall* out) {
    *out = (recon_wall*) calloc(1, sizeof (wall_file));
    wall_file* wall = (wall_file*) * out;
    wall->fp = fp;
    wall->nfmeta = 0;
    wall->ntables = nTables;
    wall->nobjects = nObjects;
    if (nTables > 0) {
        wall->tables = (wall_table*) calloc(nTables, sizeof (wall_table));
    }
    if (nObjects > 0) {
        wall->objects = (wall_object*) calloc(nObjects, sizeof (wall_object));
    }
    wall->ndefinedtables = 0;
    wall->ndefinedobjects = 0;
    wall->buffer = msgpack_sbuffer_new();
    wall->packer = msgpack_packer_new(wall->buffer, msgpack_sbuffer_write);
    return RECON_OK;
}

recon_status recon_wall_close(recon_wall wall) {
    wall_file* file = (wall_file*) wall;
    recon_wall_flush(wall);
    if (0 != fclose(file->fp)) {
        return RECON_WRITE_ERROR;
    }
    if (file->packer != NULL) {
        msgpack_packer_free(file->packer);
        file->packer = NULL;
    }
    if (file->buffer != NULL) {
        msgpack_sbuffer_free(file->buffer);
        file->buffer = NULL;
    }
    free(wall);
    return RECON_OK;
}

recon_status recon_wall_finalize(recon_wall wall) {
    size_t size;
    int i, j;
    wall_file* file = (wall_file*) wall;
    wall_table* table;
    recon_status status = RECON_OK;
    if (file->finalized) {
        return status;
    }
    // Check all data is complete
    if (file->ntables == file->ndefinedtables && file->nobjects == file->ndefinedobjects) {
        msgpack_pack_map(file->packer, 3);
        // Meta
        msgpack_pack_raw(file->packer, 5);
        msgpack_pack_raw_body(file->packer, "fmeta", 5);
        msgpack_pack_nil(file->packer);
        // Tables
        msgpack_pack_raw(file->packer, 4);
        msgpack_pack_raw_body(file->packer, "tabs", 4);
        if (file->ntables == 0) {
            msgpack_pack_nil(file->packer);
        } else {
            msgpack_pack_map(file->packer, file->ntables);
            for (i = 0; i < file->ntables; i++) {
                table = &(file->tables[i]);
                if (table->ndefinedsignals < table->nsignals || table->ndefinedaliases < table->naliases) {
                    // Throw away written data
                    msgpack_sbuffer_clear(file->buffer);
                    return RECON_NOT_FULLY_DEFINED;
                }
                msgpack_pack_raw(file->packer, strlen(table->name));
                msgpack_pack_raw_body(file->packer, table->name, strlen(table->name));
                msgpack_pack_map(file->packer, 4);
                // Table Meta Data
                msgpack_pack_raw(file->packer, 5);
                msgpack_pack_raw_body(file->packer, "tmeta", 5);
                msgpack_pack_map(file->packer, 0);
                // Signals
                msgpack_pack_raw(file->packer, 4);
                msgpack_pack_raw_body(file->packer, "sigs", 4);
                msgpack_pack_array(file->packer, table->nsignals);
                for (j = 0; j < table->nsignals; j++) {
                    msgpack_pack_raw(file->packer, strlen(table->signals[j]));
                    msgpack_pack_raw_body(file->packer, table->signals[j], strlen(table->signals[j]));
                }
                // Aliases
                msgpack_pack_raw(file->packer, 3);
                msgpack_pack_raw_body(file->packer, "als", 3);
                msgpack_pack_map(file->packer, table->naliases);
                for (j = 0; j < table->naliases; j++) {
                    msgpack_pack_raw(file->packer, strlen(table->aliases[j]));
                    msgpack_pack_raw_body(file->packer, table->aliases[j], strlen(table->aliases[j]));
                    msgpack_pack_map(file->packer, table->transforms[j] == NULL ? 1 : 2);
                    msgpack_pack_raw(file->packer, 1);
                    msgpack_pack_raw_body(file->packer, "s", 1);
                    msgpack_pack_raw(file->packer, strlen(table->aliased[j]));
                    msgpack_pack_raw_body(file->packer, table->aliased[j], strlen(table->aliased[j]));
                    if (table->transforms[j] != NULL) {
                        msgpack_pack_raw(file->packer, 1);
                        msgpack_pack_raw_body(file->packer, "t", 1);
                        msgpack_pack_raw(file->packer, strlen(table->transforms[j]));
                        msgpack_pack_raw(file->packer, table->transforms[j], strlen(table->transforms[j]));
                    }
                }
                // Variable Meta Data
                msgpack_pack_raw(file->packer, 5);
                msgpack_pack_raw_body(file->packer, "vmeta", 5);
                msgpack_pack_map(file->packer, 0);
            }
        }
        // Objects
        msgpack_pack_raw(file->packer, 4);
        msgpack_pack_raw_body(file->packer, "objs", 4);
        msgpack_pack_map(file->packer, 0);

        // Write fixed header
        size = file->buffer->size;
        status = recon_wall_pack_fixed_header(file, file->buffer->size);
        if (status != RECON_OK) {
            return status;
        }
        if (size > 0) {
            if (size == fwrite(file->buffer->data, 1, size, file->fp)) {
                msgpack_sbuffer_clear(file->buffer);
            } else {
                return RECON_WRITE_ERROR;
            }
        }
        file->finalized = TRUE;
        return RECON_OK;
    }
    return RECON_NOT_FULLY_DEFINED;
}

recon_status recon_wall_flush(recon_wall wall) {
    size_t size;
    wall_file* file = (wall_file*) wall;
    recon_status status = recon_wall_finalize(wall);
    if (status != RECON_OK) {
        return status;
    }
    size = file->buffer->size;
    if (size > 0) {
        if (size == fwrite(file->buffer->data, 1, size, file->fp)) {
            msgpack_sbuffer_clear(file->buffer);
        } else {
            return RECON_WRITE_ERROR;
        }
    }
    return RECON_OK;
}

recon_status recon_wall_unpack_fixed_header(wall_file* file, uint32_t* out) {
    char* header = (char*) malloc(18);
    if (18 != fread(header, 1, 18, file->fp)) {
        free(header);
        return RECON_READ_ERROR;
    }
    if (strncmp(header, "recon:wall:v01", 14) != 0) {
        return RECON_WRONG_FILE_TYPE;
    }
    *out = bytes_to_int(header + 14);
    free(header);
    return RECON_OK;
}

recon_status recon_wall_pack_fixed_header(wall_file* file, uint32_t header_size) {
    char* header = (char*) calloc(18, 1);
    memcpy(header, "recon:wall:v01", 14);
    int_to_bytes(header_size, header + 14);
    if (0 != fseek(file->fp, 0, SEEK_SET)) {
        return RECON_WRITE_ERROR;
    }
    if (18 == fwrite(header, 1, 18, file->fp)) {
        free(header);
        return RECON_OK;
    }
    free(header);
    return RECON_WRITE_ERROR;
}
