#include "crecon_impl.h"

recon_status recon_wall_open(char *filename, recon_wall* out) {
    recon_status status = RECON_OK;
    uint32_t* header_size;
    char* header_data;
    msgpack_unpacked* header;
    msgpack_object object;
    FILE *fp = fopen(filename, "a+");
    if (fp == NULL) {
        return RECON_READ_ERROR;
    }
    *out = (recon_wall*) malloc(sizeof (wall_file));
    wall_file* wall = (wall_file*) * out;
    wall->fp = fp;
    rewind(wall->fp);
    wall->ndefinedtables = 0;
    wall->ndefinedobjects = 0;
    wall->finalized = RECON_FALSE;
    wall->nrows = -1;
    wall->buffer = msgpack_sbuffer_new();
    wall->packer = msgpack_packer_new(wall->buffer, msgpack_sbuffer_write);
    wall->packer = msgpack_packer_new(wall->buffer, msgpack_sbuffer_write);

    header_size = (uint32_t*) malloc(sizeof (uint32_t));
    status = recon_wall_unpack_fixed_header(wall, header_size);
    if (RECON_OK != status) {
        return status;
    }
    header_data = (char*) malloc(*header_size);
    if (*header_size != fread(header_data, 1, *header_size, wall->fp)) {
        return RECON_READ_ERROR;
    }
    header = (msgpack_unpacked*) malloc(sizeof (msgpack_unpacked));
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
        status = recon_wall_visit_keyvalue(wall, p->key, p->val);
        if (RECON_OK != status) {
            return status;
        }
        ++p;
        msgpack_object_kv * const pend = object.via.map.ptr + object.via.map.size;
        for (; p < pend; ++p) {
            status = recon_wall_visit_keyvalue(wall, p->key, p->val);
            if (RECON_OK != status) {
                return status;
            }
        }
    }
    msgpack_unpacked_destroy(header);
    free(header);
    wall->finalized = RECON_TRUE;
    return status;
}

recon_status recon_wall_visit_keyvalue(wall_file* wall, msgpack_object key, msgpack_object value) {
    if (memcmp(key.via.raw.ptr, "fmeta", key.via.raw.size) == 0) {
        return recon_wall_visit_fmeta(wall, value.via.map);
    }
    if (memcmp(key.via.raw.ptr, "tabs", key.via.raw.size) == 0) {
        return recon_wall_visit_tables(wall, value.via.map);
    }
    if (memcmp(key.via.raw.ptr, "objs", key.via.raw.size) == 0) {
        return recon_wall_visit_objs(wall, value.via.map);
    }
}

recon_status recon_wall_visit_fmeta(wall_file* wall, msgpack_object_map map) {
    return RECON_OK;
}

recon_status recon_wall_visit_objs(wall_file* wall, msgpack_object_map map) {
    int i;
    recon_status status = RECON_OK;
    wall->nobjects = map.size;
    wall->objects = (wall_object*) malloc(map.size * sizeof (wall_object));
    for (i = 0; i < map.size; i++) {
        status = recon_wall_visit_object(wall, map.ptr[i].key.via.raw.ptr, map.ptr[i].key.via.raw.size, map.ptr[i].val.via.map);
        if (RECON_OK != status) {
            return status;
        }
    }
    return status;
}

recon_status recon_wall_visit_object(wall_file* wall, char* name, uint32_t namesize, msgpack_object_map map) {
    int i;
    recon_status status = RECON_OK;
    recon_wall_object object;
    char* objectname = (char*) malloc(namesize + 1);
    memcpy(objectname, name, namesize);
    objectname[namesize] = '\0';
    status = recon_wall_add_object(wall, objectname, &object);
    //TODO: Process object meta data.
    return status;
}

recon_status recon_wall_visit_tables(wall_file* wall, msgpack_object_map map) {
    int i;
    recon_status status = RECON_OK;
    wall->ntables = map.size;
    wall->tables = (wall_table*) malloc(map.size * sizeof (wall_table));
    for (i = 0; i < map.size; i++) {
        status = recon_wall_visit_table(wall, map.ptr[i].key.via.raw.ptr, map.ptr[i].key.via.raw.size, map.ptr[i].val.via.map);
        if (RECON_OK != status) {
            return status;
        }
    }
    return status;
}

recon_status recon_wall_visit_table(wall_file* wall, char* name, uint32_t namesize, msgpack_object_map map) {
    msgpack_object_array signalsmap;
    msgpack_object_map aliasesmap, tmetamap, vmetamap;
    recon_status status = RECON_OK;
    recon_wall_table table;
    char* tablename = (char*) malloc(namesize + 1);
    memcpy(tablename, name, namesize);
    tablename[namesize] = '\0';

    status = recon_wall_visit_table_elements(map, &tmetamap, &signalsmap, &aliasesmap, &vmetamap);
    if (status != RECON_OK) {
        return status;
    }
    status = recon_wall_add_table((recon_wall) wall, tablename, signalsmap.size, aliasesmap.size, &table);
    if (status != RECON_OK) {
        return status;
    }
    status = recon_wall_table_visit_signals(table, signalsmap, signalsmap.size);
    if (status != RECON_OK) {
        return status;
    }
    status = recon_wall_table_visit_aliases(table, aliasesmap, aliasesmap.size);
    if (status != RECON_OK) {
        return status;
    }

    free(tablename);

    return status;
}

recon_status recon_wall_table_visit_signals(recon_wall_table table, msgpack_object_array array, int num_signals) {
    int i = 0;
    uint32_t string_size = 2;
    recon_status status = RECON_OK;
    char* variable_name = (char*) malloc(string_size); //Yuk - nominal value
    for (i = 0; i < num_signals; i++) {
        if (array.ptr[i].via.raw.size + 1 > string_size) {
            variable_name = (char*) realloc(variable_name, array.ptr[i].via.raw.size + 1);
            string_size = array.ptr[i].via.raw.size + 1;
        }
        memcpy(variable_name, array.ptr[i].via.raw.ptr, array.ptr[i].via.raw.size);
        variable_name[array.ptr[i].via.raw.size] = '\0';
        status = recon_wall_table_add_signal(table, variable_name);
        if (status != RECON_OK) {
            return status;
        }
    }
    free(variable_name);
    return status;
}

recon_status recon_wall_table_visit_aliases(recon_wall_table table, msgpack_object_map map, int num_aliases) {
    int i = 0;
    uint32_t string_size = 2;
    char *aliasname, *aliasof, *transform;
    recon_status status = RECON_OK;
    aliasname = (char*) malloc(string_size);
    aliasof = (char*) malloc(string_size);
    transform = (char*) malloc(string_size);
    for (i = 0; i < num_aliases; i++) {
        if (map.ptr[i].val.type != MSGPACK_OBJECT_MAP) {
            return RECON_DESERIALIZATION_ERROR;
        }
        if (map.ptr[i].key.via.raw.size + 1 > string_size) {
            aliasname = realloc(aliasname, map.ptr[i].key.via.raw.size + 1);
        }
        memcpy(aliasname, map.ptr[i].key.via.raw.ptr, map.ptr[i].key.via.raw.size);
        aliasname[map.ptr[i].key.via.raw.size] = '\0';
        status = recon_wall_table_visit_alias_fields(map.ptr[i].val.via.map, &aliasof, &transform);
        if (status != RECON_OK) {
            return status;
        }
        status = recon_wall_table_add_alias(table, aliasname, aliasof, map.ptr[i].val.via.map.size == 2 ? transform : NULL);
        if (status != RECON_OK) {
            return status;
        }
    }
    free(aliasname);
    free(aliasof);
    free(transform);
    return status;
}

recon_status recon_wall_table_visit_alias_fields(msgpack_object_map map, char** base, char** transform) {
    if (map.size > 0 && map.size <= 2) {
        if (memcmp(map.ptr[0].key.via.raw.ptr, "s", map.ptr[0].key.via.raw.size) == 0) {
            *base = realloc(*base, map.ptr[0].val.via.raw.size + 1);
            memcpy(*base, map.ptr[0].val.via.raw.ptr, map.ptr[0].val.via.raw.size);
            (*base)[map.ptr[0].val.via.raw.size] = '\0';
        } else {
            //Field key should be "s"
            return RECON_DESERIALIZATION_ERROR;
        }
        if (map.size == 2) {
            if (memcmp(map.ptr[1].key.via.raw.ptr, "t", map.ptr[1].key.via.raw.size) == 0) {
                *transform = realloc(*transform, map.ptr[1].val.via.raw.size + 1);
                memcpy(*transform, map.ptr[1].val.via.raw.ptr, map.ptr[1].val.via.raw.size);
                (*transform)[map.ptr[1].val.via.raw.size] = '\0';
            } else {
                //Field key should be "t"
                return RECON_DESERIALIZATION_ERROR;
            }
        }
    } else {
        return RECON_DESERIALIZATION_ERROR;
    }
    return RECON_OK;
}

recon_status recon_wall_visit_table_elements(msgpack_object_map map, msgpack_object_map* tmeta, msgpack_object_array* signals, msgpack_object_map* aliases, msgpack_object_map* vmeta) {
    int i;
    for (i = 0; i < map.size; i++) {
        if (memcmp(map.ptr[i].key.via.raw.ptr, "tmeta", map.ptr[i].key.via.raw.size) == 0) {
            //Any restrictions on structure of vals?
            *tmeta = map.ptr[i].val.via.map;
        } else if (memcmp(map.ptr[i].key.via.raw.ptr, "sigs", map.ptr[i].key.via.raw.size) == 0) {
            //Check the value is an array
            if (map.ptr[i].val.type != MSGPACK_OBJECT_ARRAY) {
                return RECON_DESERIALIZATION_ERROR;
            }
            *signals = map.ptr[i].val.via.array;
        } else if (memcmp(map.ptr[i].key.via.raw.ptr, "als", map.ptr[i].key.via.raw.size) == 0) {
            //Should be a map
            if (map.ptr[i].val.type != MSGPACK_OBJECT_MAP) {
                return RECON_DESERIALIZATION_ERROR;
            }
            *aliases = map.ptr[i].val.via.map;
        } else if (memcmp(map.ptr[i].key.via.raw.ptr, "vmeta", map.ptr[i].key.via.raw.size) == 0) {
            //Should be a map
            if (map.ptr[i].val.type != MSGPACK_OBJECT_MAP) {
                return RECON_DESERIALIZATION_ERROR;
            }
            *vmeta = map.ptr[i].val.via.map;
        } else {
            //Unknown element in header
            return RECON_DESERIALIZATION_ERROR;
        }
    }
    return RECON_OK;
}

recon_status recon_wall_create(char* filename, int nTables, int nObjects, recon_wall* out) {
    *out = (recon_wall*) malloc(sizeof (wall_file));
    FILE* fp = fopen(filename, "w");
    wall_file* wall = (wall_file*) * out;
    wall->fp = fp;
    wall->nfmeta = 0;
    wall->ntables = nTables;
    wall->nobjects = nObjects;
    wall->nrows = -1;
    if (nTables > 0) {
        wall->tables = (wall_table*) malloc(nTables * sizeof (wall_table));
    }
    if (nObjects > 0) {
        wall->objects = (wall_object*) malloc(nObjects * sizeof (wall_object));
    }
    wall->ndefinedtables = 0;
    wall->ndefinedobjects = 0;
    wall->buffer = msgpack_sbuffer_new();
    wall->packer = msgpack_packer_new(wall->buffer, msgpack_sbuffer_write);
    wall->finalized = RECON_FALSE;
    wall->currentrowtable = NULL;
    wall->currentfield = NULL;
    return RECON_OK;
}

recon_status recon_wall_close(recon_wall wall) {
    int i;
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
    for (i = 0; i < file->ndefinedtables; i++) {
        recon_wall_free_table(file->tables[i]);
    }
    free(file->tables);
    /*
        for (i = 0; i < file->ndefinedobjects; i++) {
            recon_wall_free_object(file->objects[i]);
        }
        free(file->objects);
     */
    free(wall);
    return RECON_OK;
}

recon_status recon_wall_finalize(recon_wall wall) {
    size_t size;
    int i, j;
    wall_file* file = (wall_file*) wall;
    wall_table* table;
    wall_object* object;
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
        msgpack_pack_map(file->packer, 0);
        // Tables
        msgpack_pack_raw(file->packer, 4);
        msgpack_pack_raw_body(file->packer, "tabs", 4);
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
                    msgpack_pack_raw_body(file->packer, table->transforms[j], strlen(table->transforms[j]));
                }
            }
            // Variable Meta Data
            msgpack_pack_raw(file->packer, 5);
            msgpack_pack_raw_body(file->packer, "vmeta", 5);
            msgpack_pack_map(file->packer, 0);
        }
        // Objects
        msgpack_pack_raw(file->packer, 4);
        msgpack_pack_raw_body(file->packer, "objs", 4);
        msgpack_pack_map(file->packer, file->nobjects);
        for (j = 0; j < file->nobjects; j++) {
            if (file->nobjects != file->ndefinedobjects) {
                msgpack_sbuffer_clear(file->buffer);
                return RECON_NOT_FULLY_DEFINED;
            }
            object = &(file->objects[j]);
            msgpack_pack_raw(file->packer, strlen(object->name));
            msgpack_pack_raw_body(file->packer, object->name, strlen(object->name));
            msgpack_pack_map(file->packer, 0);
        }

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
        file->finalized = RECON_TRUE;
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
    *out = recon_util_bytes_to_int(header + 14);
    free(header);
    return RECON_OK;
}

recon_status recon_wall_pack_fixed_header(wall_file* file, uint32_t header_size) {
    char* header = (char*) malloc(18);
    memcpy(header, "recon:wall:v01", 14);
    recon_util_int_to_bytes(header_size, header + 14);
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

recon_status recon_wall_visit_rows(wall_file* file) {
    recon_status status = RECON_OK;
    uint32_t n;
    char* sizeindicator = (char*) malloc(4);
    char* row_data = (char*) malloc(1);
    msgpack_unpacked* row_unpacked = (msgpack_unpacked*) malloc(sizeof (msgpack_unpacked));
    status = recon_object_buffer_create(&(file->rows));
    if (status != RECON_OK) {
        free(row_unpacked);
        free(sizeindicator);
        free(row_data);
        return status;
    }
    file->nrows = 0;
    while (4 == fread(sizeindicator, 1, 4, file->fp)) {
        n = recon_util_bytes_to_int(sizeindicator);
        row_data = (char*) realloc(row_data, n);
        if (n != fread(row_data, 1, n, file->fp)) {
            free(row_unpacked);
            free(sizeindicator);
            free(row_data);
            return RECON_READ_ERROR;
        }
        msgpack_unpacked_init(row_unpacked);
        msgpack_unpack_next(row_unpacked, row_data, n, NULL);
        status = recon_object_buffer_append(file->rows, row_unpacked->data);
        if (status != RECON_OK) {
            msgpack_unpacked_destroy(row_unpacked);
            free(row_unpacked);
            free(sizeindicator);
            free(row_data);
            return status;
        }
        file->nrows++;
        msgpack_unpacked_destroy(row_unpacked);
    }
    free(row_unpacked);
    free(row_data);
    free(sizeindicator);
    return RECON_OK;
}
