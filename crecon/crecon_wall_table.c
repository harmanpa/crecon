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
    table->name = (char*) malloc(strlen(name) + 1);
    memcpy(table->name, name, strlen(name) + 1);
    table->nsignals = nSignals;
    if (nSignals > 0) {
        table->signals = (char**) malloc(nSignals * sizeof (char*));
    }
    table->naliases = nAliases;
    if (nAliases > 0) {
        table->aliases = (char**) malloc(nAliases * sizeof (char*));
        table->aliased = (char**) malloc(nAliases * sizeof (char*));
        table->transforms = (char**) malloc(nAliases * sizeof (char*));
    }
    table->ndefinedsignals = 0;
    table->ndefinedaliases = 0;
    file->ndefinedtables++;
    *out = table;
    return RECON_OK;
}

recon_status recon_wall_n_tables(recon_wall wall, int* n) {
    wall_file* file = (wall_file*) wall;
    *n = file->ntables;
    return RECON_OK;
}

recon_status recon_wall_table_n_signals(recon_wall_table tab, int* nsignals, int* naliases) {
    wall_table* table = (wall_table*) tab;
    *nsignals = table->nsignals;
    *naliases = table->naliases;
    return RECON_OK;
}

recon_status recon_wall_free_table(wall_table* table) {
    int i;
    for (i = 0; i < table->ndefinedsignals; i++) {
        free(table->signals[i]);
        table->signals[i] = NULL;
    }
    for (i = 0; i < table->ndefinedaliases; i++) {
        free(table->aliases[i]);
        free(table->aliased[i]);
        if (0 != strcmp(table->transforms[i], "")) {
            free(table->transforms[i]);
            table->transforms[i] = NULL;
        }
    }
    if (table->nsignals > 0) {
        free(table->signals);
        table->signals = NULL;
    }
    if (table->naliases > 0) {
        free(table->aliases);
        free(table->aliased);
        free(table->transforms);
        table->aliases = NULL;
        table->aliased = NULL;
        table->transforms = NULL;
    }
    free(table->name);
    return RECON_OK;
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
    table->signals[table->ndefinedsignals] = (char*) malloc(strlen(name) + 1);
    memcpy(table->signals[table->ndefinedsignals], name, strlen(name) + 1);
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

recon_status recon_wall_table_find_alias(recon_wall_table tab, const char* name, char** of, char** transform) {
    int i;
    wall_table* table = (wall_table*) tab;
    for (i = 0; i < table->ndefinedaliases; i++) {
        if (strcmp(name, table->aliases[i]) == 0) {
            *of = table->aliased[i];
            *transform = table->transforms[i];
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

recon_status recon_wall_table_get_alias(recon_wall_table tab, int index, char** name) {
    wall_table* table = (wall_table*) tab;
    if (index < 0 || index >= table->ndefinedaliases) {
        return RECON_UNDEFINED;
    }
    *name = table->aliases[index];
    return RECON_OK;
}

recon_status recon_wall_table_add_alias(recon_wall_table tab, const char* alias, const char* signal, char* transform) {
    int other;
    wall_table* table = (wall_table*) tab;
    if (table->ndefinedaliases >= table->naliases) {
        // Error
        return RECON_ALREADY_DEFINED;
    }
    //Should we test if the base signal exists in the table?
    if (recon_wall_table_find_signal(table, signal, &other) != RECON_OK) {
        // Error - base signal not found
        return RECON_NOT_FOUND;
    }

    table->aliases[table->ndefinedaliases] = (char*) malloc(strlen(alias) + 1);
    memcpy(table->aliases[table->ndefinedaliases], alias, strlen(alias) + 1);
    table->aliased[table->ndefinedaliases] = (char*) malloc(strlen(signal) + 1);
    memcpy(table->aliased[table->ndefinedaliases], signal, strlen(signal) + 1);
    if (transform) {
        table->transforms[table->ndefinedaliases] = (char*) malloc(strlen(transform) + 1);
        memcpy(table->transforms[table->ndefinedaliases], transform, strlen(transform) + 1);
    } else {
        table->transforms[table->ndefinedaliases] = "";
    }
    table->ndefinedaliases++;
    return RECON_OK;
}

recon_status recon_wall_get_table(recon_wall wall, int n, recon_wall_table* out) {
    wall_file* file = (wall_file*) wall;
    if (n < 0 || n >= file->ndefinedtables) {
        return RECON_UNDEFINED;
    }
    *out = (recon_wall_table) &(file->tables[n]);
    return RECON_OK;
}

recon_status recon_wall_find_table(recon_wall wall, const char* name, recon_wall_table* out) {
    int i;
    wall_file* file = (wall_file*) wall;
    for (i = 0; i < file->ndefinedtables; i++) {
        if (strcmp(name, file->tables[i].name) == 0) {
            *out = (recon_wall_table) &(file->tables[i]);
            return RECON_OK;
        }
    }
    return RECON_NOT_FOUND;
}

recon_status recon_wall_find_table_for_signal(recon_wall wall, const char* signalname, recon_wall_table* out) {
    int i;
    int index;
    char* aliased;
    char* transform;
    wall_file* file = (wall_file*) wall;
    for (i = 0; i < file->ndefinedtables; i++) {
        if (RECON_OK == recon_wall_table_find_signal((recon_wall_table)&(file->tables[i]), signalname, &index)) {
            *out = (recon_wall_table) &(file->tables[i]);
            return RECON_OK;
        }
        if (RECON_OK == recon_wall_table_find_alias((recon_wall_table)&(file->tables[i]), signalname, &aliased, &transform)) {
            *out = (recon_wall_table) &(file->tables[i]);
            return RECON_OK;
        }
    }
    return RECON_NOT_FOUND;
}
