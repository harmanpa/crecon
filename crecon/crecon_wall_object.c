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
    object->name = (char*) calloc(strlen(name), 1);
    memcpy(object->name, name, strlen(name));
    *out = object;
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

recon_status recon_wall_object_add_field_string(recon_wall_object obj, const char* v) {

}

recon_status recon_wall_object_add_field_double(recon_wall_object obj, double v) {

}

recon_status recon_wall_object_add_field_int(recon_wall_object obj, int v) {

}
