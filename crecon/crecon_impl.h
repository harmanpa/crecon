
#ifndef CRECON_IMPL_H
#define	CRECON_IMPL_H
#include "crecon.h"
#include "msgpack.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/time.h>

#ifdef	__cplusplus
extern "C" {
#endif
    typedef unsigned char booleantype;
#define FALSE 0;
#define TRUE 1;

    typedef struct {
        recon_wall wall;
        char* name;
        int nsignals;
        int ndefinedsignals;
        int naliases;
        int ndefinedaliases;
        char** signals;
        char** aliases;
        char** aliased;
        char** transforms;
        int nrows;
    } wall_table;

    typedef struct {
        recon_wall wall;
        char* name;
    } wall_object;

    typedef struct {
        FILE* fp;
        booleantype finalized;
        int byteswritten;
        int nfmeta;
        //wall_fmeta *fmeta;
        int ntables;
        int ndefinedtables;
        wall_table* tables;
        int nobjects;
        int ndefinedobjects;
        wall_object *objects;
        // Serialization
        msgpack_sbuffer* buffer;
        msgpack_packer* packer;
        // Row writing variables
        wall_table* currentrowtable;
        int currentrowtablesize;
        int currentrowwritten;
        size_t positionatrowstart;
    } wall_file;
    
    /**
     * 
     * @param 
     * @return 
     */
    uint32_t recon_util_bytes_to_int(char*);
    
    /**
     * 
     * @param 
     * @param 
     */
    void recon_util_int_to_bytes(uint32_t, char*);
#ifdef	__cplusplus
}
#endif

#endif	/* CRECON_IMPL_H */
