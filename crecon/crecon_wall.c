#include "crecon_impl.h"

recon_status recon_wall_open(FILE* fp, recon_wall* out) {
    recon_status status = RECON_OK;
    uint32_t header_size;
    char* header_data;
    msgpack_unpacked header;
    msgpack_object object;
    if (fp == NULL) {
        return RECON_READ_ERROR;
    }
    *out = (recon_wall*) calloc(1, sizeof (wall_file));
    wall_file* wall = (wall_file*) * out;
    wall->fp = fp;
    status = recon_wall_unpack_fixed_header(*wall, &header_size);
    if (RECON_OK != status) {
        return status;
    }
    if (header_size != fread(header_data, 1, header_size, wall->fp)) {
        return RECON_READ_ERROR;
    }
    if (!msgpack_unpack_next(&header, header_data, header_size, NULL)) {
        return RECON_DESERIALIZATION_ERROR;
    }
    object = header.data;
    if (object.type != MSGPACK_OBJECT_MAP) {
        return RECON_DESERIALIZATION_ERROR;
    }
    return status;
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
    msgpack_packer_free(file->packer);
    msgpack_sbuffer_free(file->buffer);
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
                msgpack_pack_nil(file->packer);
                // Signals
                msgpack_pack_raw(file->packer, 4);
                msgpack_pack_raw_body(file->packer, "sigs", 4);
                if (table->nsignals == 0) {
                    msgpack_pack_nil(file->packer);
                } else {
                    msgpack_pack_array(file->packer, table->nsignals);
                    for (j = 0; j < table->nsignals; j++) {
                        msgpack_pack_raw(file->packer, strlen(table->signals[j]));
                        msgpack_pack_raw_body(file->packer, table->signals[j], strlen(table->signals[j]));
                    }
                }
                // Aliases
                msgpack_pack_raw(file->packer, 3);
                msgpack_pack_raw_body(file->packer, "als", 3);
                if (table->naliases == 0) {
                    msgpack_pack_nil(file->packer);
                } else {
                    msgpack_pack_map(file->packer, table->naliases);
                    for (j = 0; j < table->naliases; j++) {
                        msgpack_pack_raw(file->packer, strlen(table->aliases[j]));
                        msgpack_pack_raw_body(file->packer, table->aliases[j], strlen(table->aliases[j]));
                        msgpack_pack_map(file->packer, 2);
                        msgpack_pack_raw(file->packer, 1);
                        msgpack_pack_raw_body(file->packer, "s", 1);
                        msgpack_pack_raw(file->packer, strlen(table->aliased[j]));
                        msgpack_pack_raw_body(file->packer, table->aliased[j], strlen(table->aliased[j]));
                        msgpack_pack_raw(file->packer, 1);
                        msgpack_pack_raw_body(file->packer, "t", 1);
                        msgpack_pack_nil(file->packer);
                    }
                }
                // Variable Meta Data
                msgpack_pack_raw(file->packer, 5);
                msgpack_pack_raw_body(file->packer, "vmeta", 5);
                msgpack_pack_nil(file->packer);
            }
        }
        // Objects


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
    uint32_t header_size;
    char* header = (char*) calloc(18, 1);
    fread(header, 1, 18, file->fp);
    if (18 != fread(header, 1, 18, file->fp)) {
        free(header);
        return RECON_READ_ERROR;
    }
    if (strncmp(header, "recon:wall:v01", 14) != 0) {
        return RECON_WRONG_FILE_TYPE;
    }
    header_size = bytes_to_int(&(header[14]));
    *out = header_size;
    return RECON_OK;
}

recon_status recon_wall_pack_fixed_header(wall_file* file, uint32_t header_size) {
    char* header = (char*) calloc(18, 1);
    memcpy(header, "recon:wall:v01", 14);
    int_to_bytes(header_size, &(header[14]));
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
