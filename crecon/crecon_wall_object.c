#include "crecon_impl.h"

recon_status recon_wall_add_object(recon_wall wall, const char* name, recon_wall_object* out) {
    recon_wall_object other;
    wall_object* object;
    wall_file* file = (wall_file*) wall;
    if (file->ndefinedobjects >= file->nobjects) {
        // Error
        return RECON_ALREADY_DEFINED;
    }
    if (recon_wall_find_object(wall, name, &other) == RECON_OK) {
        // Error
        return RECON_NAME_NOT_UNIQUE;
    }
    object = &(file->objects[file->ndefinedobjects]);
    object->wall = wall;
    object->name = (char*) malloc(strlen(name) + 1);
    object->ndefinedfields = 0;
    object->sizefields = 10; //Initial size
    object->fields = (wall_field*) malloc(object->sizefields * sizeof (wall_field));
    memcpy(object->name, name, strlen(name) + 1);
    *out = object;
    file->ndefinedobjects++;
    return RECON_OK;
}

recon_status recon_wall_free_object(wall_object* object) {
    int i;
    free(object->name);
    for (i = 0; i < object->ndefinedfields; i++) {
        recon_wall_free_object_field(&(object->fields[i]));
    }
    free(object->fields);
    return RECON_OK;
}

recon_status recon_wall_free_object_field(wall_field* field) {
    free(field->name);
    if (field->fieldischar) {
        free(field->field);
    } else {
        recon_wall_object_free_mobj(field->field);
    }
    return RECON_OK;
}

recon_status recon_wall_n_objects(recon_wall wall, int* n) {
    wall_file* file = (wall_file*) wall;
    *n = file->nobjects;
    return RECON_OK;
}

recon_status recon_wall_get_object(recon_wall wall, int n, recon_wall_object* out) {
    wall_file* file = (wall_file*) wall;
    if (n < 0 || n >= file->ndefinedobjects) {
        return RECON_UNDEFINED;
    }
    *out = (recon_wall_object*) &(file->objects[n]);
    return RECON_OK;
}

recon_status recon_wall_find_object(recon_wall wall, const char* name, recon_wall_object* out) {
    int i;
    wall_file* file = (wall_file*) wall;
    for (i = 0; i < file->ndefinedobjects; i++) {
        if (strcmp(name, file->objects[i].name) == 0) {
            *out = (recon_wall_object*) &(file->objects[i]);
            return RECON_OK;
        }
    }
    return RECON_NOT_FOUND;
}

recon_status recon_wall_start_field_entry(recon_wall_object obj) {
    wall_object* object = (wall_object*) obj;
    wall_file* file = (wall_file*) object->wall;
    if (file->currentfield) {
        return RECON_INCOMPLETE_ROW;
    }
    file->currentfield = object;
    file->positionatfieldstart = file->buffer->size;
    object->nbufferedfields = 0;
    object->sizebufferedfields = 10; //Initial size
    object->bufferedfields = (wall_field*) malloc(object->sizebufferedfields * sizeof (wall_field));
    return RECON_OK;
}

recon_status recon_wall_end_field_entry(recon_wall_object obj) {
    int size, i;
    recon_status status = RECON_OK;
    char* bytes = (char*) malloc(4);
    wall_object* object = (wall_object*) obj;
    wall_file* file = (wall_file*) object->wall;
    msgpack_object *raw_obj_data;
    if (object->nbufferedfields <= 0) {
        return RECON_INCOMPLETE_FIELD;
    }
    msgpack_sbuffer_write(file->buffer, __recon_size_placeholder, 4);
    msgpack_pack_map(file->packer, 1);
    msgpack_pack_raw(file->packer, strlen(object->name));
    msgpack_pack_raw_body(file->packer, object->name, strlen(object->name));
    msgpack_pack_map(file->packer, object->nbufferedfields);
    for (i = 0; i < object->nbufferedfields; i++) {
        msgpack_pack_raw(file->packer, strlen(object->bufferedfields[i].name));
        msgpack_pack_raw_body(file->packer, object->bufferedfields[i].name, strlen(object->bufferedfields[i].name));
        if (object->bufferedfields[i].fieldischar) {
            msgpack_pack_raw(file->packer, strlen(object->bufferedfields[i].field));
            msgpack_pack_raw_body(file->packer, object->bufferedfields[i].field, strlen(object->bufferedfields[i].field));
        } else {
            status = recon_wall_object_get_mobj_data(object->bufferedfields[i].field, &raw_obj_data);
            msgpack_pack_object(file->packer, *(raw_obj_data));
        }
    }

    size = file->buffer->size - (file->positionatfieldstart + 4);
    recon_util_int_to_bytes(size, bytes);
    msgpack_sbuffer_insert(file->buffer, file->positionatfieldstart, bytes, 4);
    file->currentfield = NULL;
    status = recon_wall_object_update_fields(obj);
    object->nbufferedfields = 0;
    object->sizebufferedfields = 0;
    //TODO: Need to free individual fields too
    free(object->bufferedfields);
    object->bufferedfields = NULL;
    free(bytes);
    return status;
}

recon_status recon_wall_object_add_field_string(recon_wall_object obj, const char* name, const char* v) {
    wall_object* object;
    int i = 0;
    object = (wall_object*) obj;
    //Need to make sure field doesn't already exist in bufferedfields
    if (recon_wall_object_find_buffered_field(obj, name, &i) == RECON_OK) {
        return RECON_NAME_NOT_UNIQUE;
    }
    //object->bufferedfields[object->nbufferedfields]->name = (char*)malloc(strlen(v)+1);
    if (object->nbufferedfields >= object->sizebufferedfields - 1) {
        object->bufferedfields = (wall_field*) realloc(object->bufferedfields, (object->sizebufferedfields + 10) * sizeof (wall_field));
    }
    object->bufferedfields[object->nbufferedfields].name = (char*) malloc(strlen(name) + 1);
    memcpy(object->bufferedfields[object->nbufferedfields].name, name, strlen(name) + 1);
    object->bufferedfields[object->nbufferedfields].field = (char*) malloc(strlen(v) + 1);
    memcpy(object->bufferedfields[object->nbufferedfields].field, v, strlen(v) + 1);
    object->bufferedfields[object->nbufferedfields].fieldischar = RECON_TRUE;
    object->nbufferedfields++;
    return RECON_OK;
}

recon_status recon_wall_object_add_field_double(recon_wall_object obj, const char* name, double v) {
    int max_sig_digits = 16;
    recon_status status = RECON_OK;
    char *value = (char*) malloc(max_sig_digits + 2);
    sprintf(value, "%.*g", max_sig_digits, v);
    status = recon_wall_object_add_field_string(obj, name, value);
    free(value);
    return status;
}

recon_status recon_wall_object_add_field_int(recon_wall_object obj, const char* name, int v) {
    char *value = (char*) malloc(recon_util_digits(v) + 1);
    recon_status status = RECON_OK;
    sprintf(value, "%i", v);
    status = recon_wall_object_add_field_string(obj, name, value);
    free(value);
    return status;
}

recon_status recon_wall_object_new_mobj(recon_wall_object_mobj *mobj) {
    wall_object_mobj *m_obj;
    m_obj = (wall_object_mobj*)malloc(sizeof(wall_object_mobj));
    m_obj->buffer = msgpack_sbuffer_new();
    m_obj->packer = msgpack_packer_new(m_obj->buffer, msgpack_sbuffer_write);
    msgpack_sbuffer_init(m_obj->buffer);
    m_obj->msg = NULL;
    m_obj->data = NULL;
    m_obj->finalized = RECON_FALSE;
    *mobj = (recon_wall_object_mobj)m_obj;
    return RECON_OK;
}

recon_status recon_wall_object_new_mobj_from_file(recon_wall_object_mobj *mobj, msgpack_object *obj_in) {
    wall_object_mobj *m_obj;
    m_obj = (wall_object_mobj*)malloc(sizeof(wall_object_mobj));
    m_obj->buffer = NULL;
    m_obj->packer = NULL;
    m_obj->msg = NULL;
    m_obj->data = obj_in;
    m_obj->finalized = RECON_TRUE;
    *mobj = (recon_wall_object_mobj)m_obj;
    return RECON_OK;
}

recon_status recon_wall_object_finish_mobj(recon_wall_object_mobj mobj) {
    wall_object_mobj *m_obj = (wall_object_mobj*)mobj;
    msgpack_sbuffer *buffer = m_obj->buffer;
    m_obj->msg = malloc(sizeof(msgpack_unpacked));
    msgpack_unpacked_init(m_obj->msg);
    msgpack_unpack_next(m_obj->msg, buffer->data, buffer->size, NULL);
    m_obj->data = &(m_obj->msg->data);
    m_obj->finalized = RECON_TRUE;
    return RECON_OK;
}

recon_status recon_wall_object_free_mobj(recon_wall_object_mobj mobj) {
    wall_object_mobj *m_obj = (wall_object_mobj*)mobj;
    if (m_obj->buffer) {
        msgpack_sbuffer_free(m_obj->buffer);
    }
    if (m_obj->packer) {
        msgpack_packer_free(m_obj->packer);        
    }
    if (m_obj->msg) {
        free(m_obj->msg);
    }
    free(m_obj);
	return RECON_OK;
}

recon_status recon_wall_object_get_mobj_packer(recon_wall_object_mobj mobj, msgpack_packer **pack) {
    wall_object_mobj *m_obj = (wall_object_mobj*)mobj;
    if (!m_obj->packer) {
        return RECON_OBJECT_DEFINITION_ERROR;
    }
    *pack = m_obj->packer;
    return RECON_OK;
}

recon_status recon_wall_object_get_mobj_buffer(recon_wall_object_mobj mobj, msgpack_sbuffer **buff) {
    wall_object_mobj *m_obj = (wall_object_mobj*)mobj;
    if (!m_obj->buffer) {
        return RECON_OBJECT_DEFINITION_ERROR;
    }
    *buff = m_obj->buffer;
    return RECON_OK;
}

recon_status recon_wall_object_get_mobj_data(recon_wall_object_mobj mobj, msgpack_object **data) {
    wall_object_mobj *m_obj = (wall_object_mobj*)mobj;
    msgpack_object* msgdata;
    if (!m_obj->buffer || !m_obj->finalized) {
        return RECON_OBJECT_DEFINITION_ERROR;
    }
    *data = m_obj->data;
    return RECON_OK;
}

recon_status recon_wall_object_print_mobj(recon_wall_object_mobj mobj) {
    wall_object_mobj *m_obj = (wall_object_mobj*)mobj;
    if (!m_obj->finalized) {
        return RECON_NOT_FULLY_DEFINED;
    }
    msgpack_object_print(stdout, *(m_obj->data));
    printf("\n\n\n************************************************************\n\n\n\n");
    return RECON_OK;
}


recon_status recon_wall_object_add_field_object(recon_wall_object obj, const char* name, void* v) {
    recon_status status = RECON_OK;
    wall_object* object;
    int i = 0;
    object = (wall_object*) obj;
    //Need to make sure field doesn't already exist in bufferedfields
    if (recon_wall_object_find_buffered_field(obj, name, &i) == RECON_OK) {
        return RECON_NAME_NOT_UNIQUE;
    }
    if (object->nbufferedfields >= object->sizebufferedfields - 1) {
        object->bufferedfields = (wall_field*) realloc(object->bufferedfields, (object->sizebufferedfields + 10) * sizeof (wall_field));
    }
    object->bufferedfields[object->nbufferedfields].name = (char*) malloc(strlen(name) + 1);
    memcpy(object->bufferedfields[object->nbufferedfields].name, name, strlen(name) + 1);
    object->bufferedfields[object->nbufferedfields].field = v;
    object->bufferedfields[object->nbufferedfields].fieldischar = RECON_FALSE;
    object->nbufferedfields++;
    return status;
}

recon_status recon_wall_object_update_fields(recon_wall_object obj) {
    int i, j;
    recon_status status = RECON_OK;
    wall_object* object;
    object = (wall_object*) obj;
    for (i = 0; i < object->nbufferedfields; i++) {
        if (recon_wall_object_find_field(obj, object->bufferedfields[i].name, &j) == RECON_OK) {
            status = recon_wall_object_update_field_value(obj, j, object->bufferedfields[i].field, object->bufferedfields[i].fieldischar);
            if (status != RECON_OK) {
                return status;
            }
        } else {
            status = recon_wall_object_add_field(obj, object->bufferedfields[i].name, object->bufferedfields[i].field, object->bufferedfields[i].fieldischar);
            if (status != RECON_OK) {
                return status;
            }
        }
    }
    return RECON_OK;
}

recon_status recon_wall_object_add_field(recon_wall_object obj, const char* name, void* value, recon_booleantype ischar) {
    wall_object* object;
    object = (wall_object*) obj;
    if (object->ndefinedfields >= object->sizefields - 1) {
        object->fields = (wall_field*) realloc(object->fields, (object->sizefields + 10) * sizeof (wall_field));
        object->sizefields = (object->sizefields + 10);
    }
    object->fields[object->ndefinedfields].name = (char*) malloc(strlen(name) + 1);
    memcpy(object->fields[object->ndefinedfields].name, name, strlen(name) + 1);
    if (ischar) {
        object->fields[object->ndefinedfields].field = (char*) malloc(strlen(value) + 1);
        memcpy(object->fields[object->ndefinedfields].field, value, strlen(value) + 1);
    } else {
        object->fields[object->ndefinedfields].field = (wall_object_mobj*) malloc(sizeof(wall_object_mobj));
        memcpy(object->fields[object->ndefinedfields].field, value, sizeof(wall_object_mobj));
    }
    object->fields[object->ndefinedfields].fieldischar = ischar;
    object->ndefinedfields++;
    return RECON_OK;
}

recon_status recon_wall_object_update_field_value(recon_wall_object obj, int i, void* value, recon_booleantype ischar) {
    wall_object* object;
    object = (wall_object*) obj;
    if (i >= object->ndefinedfields) {
        return RECON_UNDEFINED;
    }
    if (ischar) {
        realloc(object->fields[i].field, strlen(value) + 1);
        memcpy(object->fields[i].field, value, strlen(value) + 1);
    } else {
        memcpy(object->fields[i].field, value, sizeof(wall_object_mobj));
    }
    object->fields[i].fieldischar = ischar;
    return RECON_OK;
}

recon_status recon_wall_object_find_field(recon_wall_object obj, const char* name, int* out) {
    int i;
    wall_object* object;
    object = (wall_object*) obj;
    for (i = 0; i < object->ndefinedfields; i++) {
        if (strcmp(object->fields[i].name, name) == 0) {
            *out = i;
            return RECON_OK;
        }
    }
    return RECON_NOT_FOUND;
}

recon_status recon_wall_object_get_field(recon_wall_object obj, int index, recon_wall_object_field *out) {
    wall_object* object;
    object = (wall_object*) obj;
    if (index < 0 || index > object->ndefinedfields) {
        return RECON_UNDEFINED;
    }
    *out = (recon_wall_object_field)&(object->fields[index]);
}

recon_status recon_wall_object_get_field_value(recon_wall_object_field field, void **value) {
    wall_field* wf;
    wf = (wall_field*) field;
    *value = wf->field;
    return RECON_OK;
}

recon_status recon_wall_object_get_field_ischar(recon_wall_object_field field, recon_booleantype *ischar) {
    wall_field* wf;
    wf = (wall_field*) field;
    *ischar= wf->fieldischar;
    return RECON_OK;
}

recon_status recon_wall_object_find_buffered_field(recon_wall_object obj, const char* name, int* out) {
    int i;
    wall_object* object;
    object = (wall_object*) obj;
    for (i = 0; i < object->nbufferedfields; i++) {
        if (strcmp(object->bufferedfields[i].name, name) == 0) {
            *out = i;
            return RECON_OK;
        }
    }
    return RECON_NOT_FOUND;
}

recon_status recon_wall_object_get_fields(recon_wall_object obj) {
    recon_status status = RECON_OK;
    wall_object* w_object = (wall_object*) obj;
    wall_file* file = (wall_file*) w_object->wall;
    msgpack_object* object;
    msgpack_object_map map;
    int i;
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
                if (memcmp(w_object->name, map.ptr->key.via.raw.ptr, map.ptr->key.via.raw.size) == 0) {
                    //Should be a map - entry name will be key
                    if(map.ptr->val.type == MSGPACK_OBJECT_MAP) {
                        status = recon_wall_object_read_field(obj, &(map.ptr->val.via.map));
                        if (status != RECON_OK) {
                            return status;
                        }
                    }
                }
                break;
        }
    }
    return RECON_OK;
}

recon_status recon_wall_object_read_field(recon_wall_object obj, msgpack_object_map *fieldmap) {
    recon_status status = RECON_OK;
    wall_object* object = (wall_object*) obj;
    recon_wall_object_mobj mobj;
    char *fieldname;
    char *fieldvaluestr;
    uint32_t lenstr;
    recon_booleantype ischar;
    lenstr = (fieldmap->ptr)->key.via.raw.size;
    fieldname = (char*) malloc(lenstr+1);
    memcpy(fieldname, (fieldmap->ptr)->key.via.raw.ptr, lenstr);
    fieldname[lenstr] = '\0';
    ischar = (fieldmap->ptr)->val.type == MSGPACK_OBJECT_RAW;
    if (!ischar) {
        status = recon_wall_object_new_mobj_from_file(&mobj, &((fieldmap->ptr)->val));
    } else {
        lenstr = (fieldmap->ptr)->val.via.raw.size;
        fieldvaluestr = (char*) malloc(lenstr+1);
        memcpy(fieldvaluestr, (fieldmap->ptr)->val.via.raw.ptr, lenstr);
        fieldvaluestr[lenstr] = '\0';
    }
    if (recon_wall_object_find_field(object, fieldname, &index) != RECON_OK) {
        status = recon_wall_object_add_field(object, fieldname, ischar ? fieldvaluestr : mobj, ischar);
    } else {
        status = recon_wall_object_update_field_value(object, fieldname, ischar ? fieldvaluestr : mobj, ischar);
    }
    return status;
}
