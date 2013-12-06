#include "crecon_impl.h"

recon_status recon_wall_add_table(recon_wall wall, const char* name, int nSignals, int nAliases, recon_wall_table* out) {
    recon_wall_table other;
    wall_table* table;
    wall_file* file = (wall_file*) wall;
    if (file->ndefinedtables >= file->ntables) {
        // Error
        return RECON_ALREADY_DEFINED;
    }
    if (nSignals <= 0) {
        // Error
        return RECON_INVALID_NUMBER_SIGNALS;
    }
    if (recon_wall_find_table(wall, name, &other) == RECON_OK) {
        // Error
        return RECON_NAME_NOT_UNIQUE;
    }
    table = &(file->tables[file->ndefinedtables]);
    table->wall = wall;
    table->name = (char*) calloc(strlen(name), 1);
    memcpy(table->name, name, strlen(name));
    table->nsignals = nSignals;
    table->signals = (char**) calloc(nSignals, sizeof (char*));
    table->naliases = nAliases;
    table->aliases = (char**) calloc(nAliases, sizeof (char*));
    table->aliased = (char**) calloc(nAliases, sizeof (char*));
    table->transforms = (char**) calloc(nAliases, sizeof (char*));
    table->ndefinedsignals = 0;
    file->ndefinedtables++;
    *out = table;
    return RECON_OK;
}

recon_status recon_wall_n_tables(recon_wall wall, int* n) {
    wall_file* file = (wall_file*) wall;
    *n = file->ntables;
    return RECON_OK;
}

recon_status recon_free_table(wall_table* table) {
    
}

recon_status recon_wall_table_add_signal(recon_wall_table tab, const char* name) {
    int other;
    wall_table* table = (wall_table*) tab;
    if (table->ndefinedsignals >= table->nsignals) {
        // Error
        return RECON_ALREADY_DEFINED;
    }
    if (recon_wall_table_find_signal(table, name, &other) == RECON_OK) {
        // Error
        return RECON_NAME_NOT_UNIQUE;
    }
    table->signals[table->ndefinedsignals] = (char*) calloc(strlen(name), 1);
    memcpy(table->signals[table->ndefinedsignals], name, strlen(name));
    table->ndefinedsignals++;
    return RECON_OK;
}

recon_status recon_wall_table_find_signal(recon_wall_table tab, const char* name, int* index) {
    int i;
    wall_table* table = (wall_table*) tab;
    for (i = 0; i < table->ndefinedsignals; i++) {
        if (strcmp(name, table->signals[i]) == 0) {
            *index = i;
            return RECON_OK;
        }
    }
    return RECON_NOT_FOUND;
}

recon_status recon_wall_table_get_signal(recon_wall_table tab, int index, char** name) {
    wall_table* table = (wall_table*) tab;
    if (index < 0 || index >= table->ndefinedsignals) {
        return RECON_UNDEFINED;
    }
    *name = table->signals[index];
    return RECON_OK;
}

recon_status recon_wall_table_add_alias(recon_wall_table tab, const char* alias, const char* signal, char* transform) {
    int other;
    wall_table* table = (wall_table*) tab;
    if (table->ndefinedaliases >= table->naliases) {
        // Error
        return RECON_ALREADY_DEFINED;
    }
    /*
        if (recon_wall_table_find_signal(table, name, &other) == RECON_OK) {
            // Error
            return RECON_NAME_NOT_UNIQUE;
        }
     */
    table->aliases[table->ndefinedaliases] = (char*) calloc(strlen(alias), 1);
    memcpy(table->aliases[table->ndefinedaliases], alias, strlen(alias));
    table->aliased[table->ndefinedaliases] = (char*) calloc(strlen(signal), 1);
    memcpy(table->aliased[table->ndefinedaliases], signal, strlen(signal));
    if (transform) {
        table->transforms[table->ndefinedaliases] = (char*) calloc(strlen(transform), 1);
        memcpy(table->transforms[table->ndefinedaliases], transform, strlen(transform));
    }
    table->ndefinedaliases++;
    return RECON_OK;
}

recon_status recon_wall_get_table(recon_wall wall, int n, recon_wall_table* out) {
    wall_file* file = (wall_file*) wall;
    if (n < 0 || n >= file->ndefinedtables) {
        return RECON_UNDEFINED;
    }
    *out = (recon_wall_table*) &(file->tables[n]);
    return RECON_OK;
}

recon_status recon_wall_find_table(recon_wall wall, const char* name, recon_wall_table* out) {
    int i;
    wall_file* file = (wall_file*) wall;
    for (i = 0; i < file->ndefinedtables; i++) {
        if (strcmp(name, file->tables[i].name) == 0) {
            *out = (recon_wall_table*) &(file->tables[i]);
            return RECON_OK;
        }
    }
    return RECON_NOT_FOUND;
}

