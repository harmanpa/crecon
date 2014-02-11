#include "crecon_impl.h"

recon_status recon_wall_table_start_row(recon_wall_table tab) {
    wall_table* table = (wall_table*) tab;
    wall_file* file = (wall_file*) table->wall;
    if (file->currentrowtable) {
        return RECON_INCOMPLETE_ROW;
    }
    file->currentrowtable = table;
    file->currentrowtablesize = table->nsignals;
    file->currentrowwritten = 0;
    file->positionatrowstart = file->buffer->size;
    msgpack_sbuffer_write(file->buffer, __recon_size_placeholder, 4);
    msgpack_pack_map(file->packer, 1);
    msgpack_pack_raw(file->packer, strlen(table->name));
    msgpack_pack_raw_body(file->packer, table->name, strlen(table->name));
    msgpack_pack_array(file->packer, table->nsignals);
    return RECON_OK;
}

recon_status recon_wall_table_row_add_double(recon_wall_table tab, double v) {
    wall_table* table = (wall_table*) tab;
    wall_file* file = (wall_file*) table->wall;
    if (file->currentrowwritten >= file->currentrowtablesize) {
        return RECON_INCOMPLETE_ROW;
    }
    msgpack_pack_double(file->packer, v);
    file->currentrowwritten++;
    return RECON_OK;
}

recon_status recon_wall_table_row_add_int(recon_wall_table tab, int v) {
    wall_table* table = (wall_table*) tab;
    wall_file* file = (wall_file*) table->wall;
    if (file->currentrowwritten >= file->currentrowtablesize) {
        return RECON_INCOMPLETE_ROW;
    }
    msgpack_pack_int(file->packer, v);
    file->currentrowwritten++;
    return RECON_OK;
}

recon_status recon_wall_table_row_add_string(recon_wall_table tab, char* v) {
    wall_table* table = (wall_table*) tab;
    wall_file* file = (wall_file*) table->wall;
    if (file->currentrowwritten >= file->currentrowtablesize) {
        return RECON_INCOMPLETE_ROW;
    }
    msgpack_pack_raw(file->packer, strlen(v));
    msgpack_pack_raw_body(file->packer, v, strlen(v));
    file->currentrowwritten++;
    return RECON_OK;
}

recon_status recon_wall_table_row_add_double_array(recon_wall_table tab, double *v, int nv) {
    int i;
    wall_table* table = (wall_table*) tab;
    wall_file* file = (wall_file*) table->wall;
    if (file->currentrowwritten >= file->currentrowtablesize) {
        return RECON_INCOMPLETE_ROW;
    }
    msgpack_pack_array(file->packer, nv);
    for (i = 0; i < nv; i++) {
        msgpack_pack_double(file->packer, v[i]);
    }
    file->currentrowwritten++; //Assume array is single element in row
    return RECON_OK;
}

recon_status recon_wall_table_row_add_int_array(recon_wall_table tab, int *v, int nv) {
    int i;
    wall_table* table = (wall_table*) tab;
    wall_file* file = (wall_file*) table->wall;
    if (file->currentrowwritten >= file->currentrowtablesize) {
        return RECON_INCOMPLETE_ROW;
    }
    msgpack_pack_array(file->packer, nv);
    for (i = 0; i < nv; i++) {
        msgpack_pack_int(file->packer, v[i]);
    }
    file->currentrowwritten++; //Assume array is single element in row
    return RECON_OK;
}

recon_status recon_wall_table_row_add_string_array(recon_wall_table tab, char **v, int nv) {
    int i;
    wall_table* table = (wall_table*) tab;
    wall_file* file = (wall_file*) table->wall;
    if (file->currentrowwritten >= file->currentrowtablesize) {
        return RECON_INCOMPLETE_ROW;
    }
    msgpack_pack_array(file->packer, nv);
    for (i = 0; i < nv; i++) {
        msgpack_pack_raw(file->packer, strlen(v[i]));
        msgpack_pack_raw_body(file->packer, v[i], strlen(v[i]));
    }
    file->currentrowwritten++; //Assume array is single element in row
    return RECON_OK;
}

recon_status recon_wall_table_end_row(recon_wall_table tab) {
    size_t size;
    char* bytes = (char*) malloc(4);
    wall_table* table = (wall_table*) tab;
    wall_file* file = (wall_file*) table->wall;
    if (file->currentrowwritten < file->currentrowtablesize) {
        return RECON_INCOMPLETE_ROW;
    }
    size = file->buffer->size - (file->positionatrowstart + 4);
    recon_util_int_to_bytes(size, bytes);
    msgpack_sbuffer_insert(file->buffer, file->positionatrowstart, bytes, 4);
    file->currentrowtable = NULL;
    file->currentrowtablesize = 0;
    file->currentrowwritten = 0;
    return RECON_OK;
}

recon_status recon_wall_table_count_rows(recon_wall_table tab, int* nrows) {
    recon_status status = RECON_OK;
    wall_table* table = (wall_table*) tab;
    wall_file* file = (wall_file*) table->wall;
    msgpack_object* object;
    msgpack_object_map map;
    int i;
    int n = 0;
    if (file->nrows < 0) {
        status = recon_wall_visit_rows(file);
        if (status != RECON_OK) {
            return status;
        }
    }
    for (i = 0; i < file->nrows; i++) {
        status = recon_wall_row_buffer_get_object(file->rows, i, &object);
        if (status != RECON_OK) {
            return status;
        }
        switch (object->type) {
            case MSGPACK_OBJECT_MAP:
                map = object->via.map;
                if (memcmp(table->name, map.ptr->key.via.raw.ptr, map.ptr->key.via.raw.size) == 0) {
                    n++;
                }
                break;
        }
    }
    *nrows = n;
    return RECON_OK;
}


#define RECON_WALL_ROW_BUFFER_SIZE 100

recon_status recon_object_buffer_create(recon_wall_row_buffer** buffer) {
    *buffer = (recon_wall_row_buffer*) malloc(sizeof (recon_wall_row_buffer*));
    (*buffer)->data = (recon_wall_row*) malloc(RECON_WALL_ROW_BUFFER_SIZE * sizeof (recon_wall_row));
    (*buffer)->alloc = RECON_WALL_ROW_BUFFER_SIZE;
    (*buffer)->size = 0;
    return RECON_OK;
}

recon_status recon_wall_row_buffer_size(recon_wall_row_buffer* buffer, int* size) {
    *size = buffer->size;
    return RECON_OK;
}

recon_status recon_wall_row_buffer_get(recon_wall_row_buffer* buffer, int i, recon_wall_row** obj) {
    if (i < 0 || i >= buffer->size) {
        return RECON_NOT_FOUND;
    }
    *obj = &(buffer->data[i]);
    return RECON_OK;
}

recon_status recon_wall_row_buffer_get_object(recon_wall_row_buffer* buffer, int i, msgpack_object** obj) {
    if (i < 0 || i >= buffer->size) {
        return RECON_NOT_FOUND;
    }
    *obj = &(buffer->data[i].unpacked->data);
    return RECON_OK;
}

recon_status recon_wall_row_buffer_append(recon_wall_row_buffer* buffer, char* data, size_t size) {
    recon_wall_row* row;
    if (buffer->alloc - buffer->size < 1) {
        size_t nsize = (buffer->alloc) ? buffer->alloc * 2 : RECON_WALL_ROW_BUFFER_SIZE;
        void* tmp = realloc(buffer->data, nsize);
        if (!tmp) {
            return RECON_BUFFER_RESIZE_ERROR;
        }
        buffer->data = (recon_wall_row*) tmp;
        buffer->alloc = nsize;
    }
    row = buffer->data + buffer->size;
    row->data = (char*) malloc(size);
    memcpy(row->data, data, size);
    row->unpacked = (msgpack_unpacked*) malloc(sizeof (msgpack_unpacked));
    msgpack_unpacked_init(row->unpacked);
    if (msgpack_unpack_next(row->unpacked, row->data, size, NULL)) {
        buffer->size++;
        return RECON_OK;
    }
    return RECON_SERIALIZATION_ERROR;
}

recon_status recon_wall_row_destroy(recon_wall_row* row) {
    msgpack_unpacked_destroy(row->unpacked);
    free(row->unpacked);
    free(row->data);
    return RECON_OK;
}

recon_status recon_wall_row_buffer_destroy(recon_wall_row_buffer* buffer) {
    int i;
    recon_status status = RECON_OK;
    for (i = 0; i < buffer->size; i++) {
        status = recon_wall_row_destroy(buffer->data + i);
        if (status != RECON_OK) {
            return status;
        }
    }
    free(buffer->data);
    return RECON_OK;
}

recon_status recon_wall_table_get_signal_double(recon_wall_table tab, char* signal, double* s) {
    recon_status status = RECON_OK;
    wall_table* table = (wall_table*) tab;
    wall_file* file = (wall_file*) table->wall;
    msgpack_object* object;
    msgpack_object_map map;
    msgpack_object_array array;
    int i;
    int n = 0;
    int column;
    status = recon_wall_table_find_signal(tab, signal, &column);
    if (status != RECON_OK) {
        return status;
    }
    if (file->nrows < 0) {
        status = recon_wall_visit_rows(file);
        if (status != RECON_OK) {
            return status;
        }
    }
    for (i = 0; i < file->nrows; i++) {
        status = recon_wall_row_buffer_get_object(file->rows, i, &object);
        if (status != RECON_OK) {
            return status;
        }
        switch (object->type) {
            case MSGPACK_OBJECT_MAP:
                map = object->via.map;
                if (memcmp(table->name, map.ptr->key.via.raw.ptr, map.ptr->key.via.raw.size) == 0) {
                    switch (map.ptr->val.type) {
                        case MSGPACK_OBJECT_ARRAY:
                            array = map.ptr->val.via.array;
                            if (column >= array.size) {
                                return RECON_INCOMPLETE_ROW;
                            }
                            switch (array.ptr[column].type) {
                                case MSGPACK_OBJECT_DOUBLE:
                                    s[n] = array.ptr[column].via.dec;
                                    break;
                                case MSGPACK_OBJECT_POSITIVE_INTEGER:
                                    s[n] = 1.0 * array.ptr[column].via.u64;
                                    break;
                                case MSGPACK_OBJECT_NEGATIVE_INTEGER:
                                    s[n] = 1.0 * array.ptr[column].via.i64;
                                    break;
                                case MSGPACK_OBJECT_BOOLEAN:
                                    s[n] = array.ptr[column].via.boolean ? 1.0 : 0.0;
                                    break;
                                default:
                                    s[n] = 0.0;
                            }
                            n++;
                            break;
                    }
                }
                break;
        }
    }
    return RECON_OK;
}


recon_status recon_wall_table_get_signal_integer(recon_wall_table tab, char* signal, int* s) {
    recon_status status = RECON_OK;
    wall_table* table = (wall_table*) tab;
    wall_file* file = (wall_file*) table->wall;
    msgpack_object* object;
    msgpack_object_map map;
    msgpack_object_array array;
    int i;
    int n = 0;
    int column;
    status = recon_wall_table_find_signal(tab, signal, &column);
    if (status != RECON_OK) {
        return status;
    }
    if (file->nrows < 0) {
        status = recon_wall_visit_rows(file);
        if (status != RECON_OK) {
            return status;
        }
    }
    for (i = 0; i < file->nrows; i++) {
        status = recon_wall_row_buffer_get_object(file->rows, i, &object);
        if (status != RECON_OK) {
            return status;
        }
        switch (object->type) {
            case MSGPACK_OBJECT_MAP:
                map = object->via.map;
                if (memcmp(table->name, map.ptr->key.via.raw.ptr, map.ptr->key.via.raw.size) == 0) {
                    switch (map.ptr->val.type) {
                        case MSGPACK_OBJECT_ARRAY:
                            array = map.ptr->val.via.array;
                            if (column >= array.size) {
                                return RECON_INCOMPLETE_ROW;
                            }
                            switch (array.ptr[column].type) {
                                case MSGPACK_OBJECT_POSITIVE_INTEGER:
                                    s[n] = (int)array.ptr[column].via.u64;
                                    break;
                                case MSGPACK_OBJECT_NEGATIVE_INTEGER:
                                    s[n] = (int)array.ptr[column].via.i64;
                                    break;
                                case MSGPACK_OBJECT_BOOLEAN:
                                    s[n] = array.ptr[column].via.boolean ? 1 : 0;
                                    break;
                                default:
                                    s[n] = 0;
                            }
                            n++;
                            break;
                    }
                }
                break;
        }
    }
    return RECON_OK;
}


recon_status recon_wall_table_get_signal_boolean(recon_wall_table tab, char* signal, recon_booleantype* s) {
    recon_status status = RECON_OK;
    wall_table* table = (wall_table*) tab;
    wall_file* file = (wall_file*) table->wall;
    msgpack_object* object;
    msgpack_object_map map;
    msgpack_object_array array;
    int i;
    int n = 0;
    int column;
    status = recon_wall_table_find_signal(tab, signal, &column);
    if (status != RECON_OK) {
        return status;
    }
    if (file->nrows < 0) {
        status = recon_wall_visit_rows(file);
        if (status != RECON_OK) {
            return status;
        }
    }
    for (i = 0; i < file->nrows; i++) {
        status = recon_wall_row_buffer_get_object(file->rows, i, &object);
        if (status != RECON_OK) {
            return status;
        }
        switch (object->type) {
            case MSGPACK_OBJECT_MAP:
                map = object->via.map;
                if (memcmp(table->name, map.ptr->key.via.raw.ptr, map.ptr->key.via.raw.size) == 0) {
                    switch (map.ptr->val.type) {
                        case MSGPACK_OBJECT_ARRAY:
                            array = map.ptr->val.via.array;
                            if (column >= array.size) {
                                return RECON_INCOMPLETE_ROW;
                            }
                            switch (array.ptr[column].type) {
                                case MSGPACK_OBJECT_POSITIVE_INTEGER:
                                    if(array.ptr[column].via.u64>0) {
                                        s[n] = RECON_TRUE;
                                    } else {
                                        s[n] = RECON_FALSE;
                                    }
                                    break;
                                case MSGPACK_OBJECT_NEGATIVE_INTEGER:
                                    s[n] = RECON_FALSE;
                                    break;
                                case MSGPACK_OBJECT_BOOLEAN:
                                    if(array.ptr[column].via.boolean) {
                                        s[n] = RECON_TRUE;
                                    } else {
                                        s[n] = RECON_FALSE;
                                    }
                                    break;
                                default:
                                    s[n] = RECON_FALSE;
                            }
                            n++;
                            break;
                    }
                }
                break;
        }
    }
    return RECON_OK;
}


recon_status recon_wall_table_get_signal_object(recon_wall_table tab, char* signal, msgpack_object* s) {
    recon_status status = RECON_OK;
    wall_table* table = (wall_table*) tab;
    wall_file* file = (wall_file*) table->wall;
    msgpack_object* object;
    msgpack_object_map map;
    msgpack_object_array array;
    int i;
    int n = 0;
    int column;
    status = recon_wall_table_find_signal(tab, signal, &column);
    if (status != RECON_OK) {
        return status;
    }
    if (file->nrows < 0) {
        status = recon_wall_visit_rows(file);
        if (status != RECON_OK) {
            return status;
        }
    }
    for (i = 0; i < file->nrows; i++) {
        status = recon_wall_row_buffer_get_object(file->rows, i, &object);
        if (status != RECON_OK) {
            return status;
        }
        switch (object->type) {
            case MSGPACK_OBJECT_MAP:
                map = object->via.map;
                if (memcmp(table->name, map.ptr->key.via.raw.ptr, map.ptr->key.via.raw.size) == 0) {
                    switch (map.ptr->val.type) {
                        case MSGPACK_OBJECT_ARRAY:
                            array = map.ptr->val.via.array;
                            if (column >= array.size) {
                                return RECON_INCOMPLETE_ROW;
                            }
                            s[n] = array.ptr[column];
                            n++;
                            break;
                    }
                }
                break;
        }
    }
    return RECON_OK;
}
