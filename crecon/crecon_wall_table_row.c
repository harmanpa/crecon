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

recon_status recon_wall_table_end_row(recon_wall_table tab) {
    wall_table* table = (wall_table*) tab;
    wall_file* file = (wall_file*) table->wall;
    if (file->currentrowwritten < file->currentrowtablesize) {
        return RECON_INCOMPLETE_ROW;
    }
    file->currentrowtable = NULL;
    file->currentrowtablesize = 0;
    file->currentrowwritten = 0;
    return RECON_OK;
}
