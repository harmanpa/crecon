#include "crecon_impl.h"

/* Pass unallocated strings as parameters */
recon_status recon_transform_create_none(char** t) {
    *t = NULL;
    return RECON_OK;
}


recon_status recon_transform_create_inverse(char** t) {
    *t = (char*)malloc(strlen("inv")+1);
    memcpy(*t, "inv", strlen("inv"));
    (*t)[strlen("inv")]='\0';
    return RECON_OK;
}

recon_status recon_transform_create_affine(char** t, double scale, double offset) {
    int max_sig_digits = 6;
    char transform_string[strlen("aff(-.,-.)") + 2*max_sig_digits+1];
    if(scale==1.0 && offset==0.0) {
        return recon_transform_create_none(t);
    }
    if(scale==-1.0 && offset==0.0) {
        return recon_transform_create_inverse(t);
    }
    sprintf(transform_string, "aff(%.*g,%.*g)", max_sig_digits, scale, max_sig_digits, offset);
    *t = (char*)malloc(strlen(transform_string)+1);
    memcpy(*t, transform_string, strlen(transform_string)+1);
    return RECON_OK;
}
