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
    object->name = (char*) malloc(strlen(name)+1);
    object->ndefinedfields = 0;
    object->sizefields = 10;    //Initial size
    object->fields = (wall_field*)malloc(object->sizefields*sizeof(wall_field));
    memcpy(object->name, name, strlen(name)+1);
    *out = object;
    file->ndefinedobjects++;
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
    object->sizebufferedfields = 10;    //Initial size
    object->bufferedfields = (wall_field*)malloc(object->sizebufferedfields*sizeof(wall_field));
    return RECON_OK;
}


recon_status recon_wall_end_field_entry(recon_wall_object obj) {
    int size, i;
    recon_status status = RECON_OK;
    char* bytes = (char*)malloc(4);
    wall_object* object = (wall_object*) obj;
    wall_file* file = (wall_file*) object->wall;
    if (object->nbufferedfields <= 0) {
        return RECON_INCOMPLETE_FIELD;
    }
    msgpack_sbuffer_write(file->buffer, __recon_size_placeholder, 4);
    msgpack_pack_map(file->packer, 1);
    msgpack_pack_raw(file->packer, strlen(object->name));
    msgpack_pack_raw_body(file->packer, object->name, strlen(object->name));
    msgpack_pack_map(file->packer, object->nbufferedfields);
    for(i = 0; i<object->nbufferedfields; i++) {
        msgpack_pack_raw(file->packer, strlen(object->bufferedfields[i].name));
        msgpack_pack_raw_body(file->packer, object->bufferedfields[i].name, strlen(object->bufferedfields[i].name));
    }
    
    size = file->buffer->size - (file->positionatfieldstart + 4);
    recon_util_int_to_bytes(size, bytes);
    msgpack_sbuffer_insert(file->buffer, file->positionatfieldstart, bytes, 4);
    file->currentfield = NULL;
    status = recon_wall_object_update_fields(obj);
    object->nbufferedfields = 0;
    object->sizebufferedfields = 0;
    //Need to free individual fields too
    free(object->bufferedfields);
    object->bufferedfields = NULL;
    return status;
}


recon_status recon_wall_object_add_field_string(recon_wall_object obj, const char* name, const char* v) {
    wall_object* object;
    int i = 0;
    object = (wall_object*)obj;
    //Need to make sure field doesn't already exist in bufferedfields
    if (recon_wall_object_find_buffered_field(obj, name, &i) == RECON_OK) {
        return RECON_NAME_NOT_UNIQUE;
    }
    //object->bufferedfields[object->nbufferedfields]->name = (char*)malloc(strlen(v)+1);
    if(object->nbufferedfields>=object->sizebufferedfields-1) {
        object->bufferedfields = (wall_field*)realloc(object->bufferedfields, (object->sizebufferedfields+10)*sizeof(wall_field));
    }
    object->bufferedfields[object->nbufferedfields].name = (char*)malloc(strlen(name)+1);
    memcpy(object->bufferedfields[object->nbufferedfields].name, name, strlen(name)+1);
    object->bufferedfields[object->nbufferedfields].field = (char*)malloc(strlen(v)+1);
    memcpy(object->bufferedfields[object->nbufferedfields].field, v, strlen(v)+1);
    object->nbufferedfields++;
    return RECON_OK;
}

recon_status recon_wall_object_add_field_double(recon_wall_object obj, const char* name, double v) {
    int max_sig_digits = 16;
    recon_status status = RECON_OK;
    char *value = (char*)malloc(max_sig_digits+2);
    sprintf(value, "%.*g", max_sig_digits, v);
    status = recon_wall_object_add_field_string(obj, name, value);
    free(value);
    return status;
}

recon_status recon_wall_object_add_field_int(recon_wall_object obj, const char* name, int v) {
    char *value = (char*)malloc(recon_util_digits(v)+1);
    recon_status status = RECON_OK;
    sprintf(value, "%i", v);
    status = recon_wall_object_add_field_string(obj, name, value);
    free(value);
    return status;
}

recon_status recon_wall_object_update_fields(recon_wall_object obj) {
    int i, j;
    recon_status status = RECON_OK;
    wall_object* object;
    object = (wall_object*)obj;
    for(i=0; i<object->nbufferedfields; i++) {
        if (recon_wall_object_find_field(obj, object->bufferedfields[i].name, &j) == RECON_OK) {
            status = recon_wall_object_update_field_value(obj, j, object->bufferedfields[i].field); if (status != RECON_OK) {return status;}
        } else {
            status = recon_wall_add_field(obj, object->bufferedfields[i].name, object->bufferedfields[i].field); if (status != RECON_OK) {return status;}
        }
    }
    return RECON_OK;
}

recon_status recon_wall_add_field(recon_wall_object obj, const char* name, const char* value) {
    wall_object* object;
    object = (wall_object*)obj;
    if(object->ndefinedfields >= object->sizefields-1) {
        object->fields = (wall_field*)realloc(object->fields, (object->sizefields+10)*sizeof(wall_field));
    }
    object->fields[object->ndefinedfields].name = (char*)malloc(strlen(name)+1);
    memcpy(object->fields[object->ndefinedfields].name, name, strlen(name)+1);
    object->fields[object->ndefinedfields].field = (char*)malloc(strlen(value)+1);
    memcpy(object->fields[object->ndefinedfields].field, value, strlen(value)+1);
    return RECON_OK;
}

recon_status recon_wall_object_update_field_value(recon_wall_object obj, int i, const char* value) {
    wall_object* object;
    object = (wall_object*)obj;
    if(i>=object->ndefinedfields) {
        return RECON_UNDEFINED;
    }
    realloc(object->fields[i].field, strlen(value)+1);
    memcpy(object->fields[i].field, value, strlen(value)+1);
    return RECON_OK;
}

recon_status recon_wall_object_find_field(recon_wall_object obj, const char* name, int* out) {
    int i;
    wall_object* object;
    object = (wall_object*)obj;
    for (i=0; i<object->ndefinedfields; i++) {
        if (strcmp(object->fields[i].name, name)==0) {
            *out = i;
            return RECON_OK;
        }
    }
    return RECON_NOT_FOUND;
}

recon_status recon_wall_object_find_buffered_field(recon_wall_object obj, const char* name, int* out) {
    int i;
    wall_object* object;
    object = (wall_object*)obj;
    for(i=0; i<object->nbufferedfields; i++) {
        if(strcmp(object->bufferedfields[i].name, name)==0) {
            *out = i;
            return RECON_OK;
        }
    }
    return RECON_NOT_FOUND;
}
