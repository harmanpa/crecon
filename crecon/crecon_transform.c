#include "crecon_impl.h"

recon_status recon_transform_create_none(char** t) {
    *t = NULL;
    return RECON_OK;
}

recon_status recon_transform_create_inverse(char** t) {
    *t = "inv";
    return RECON_OK;
}

recon_status recon_transform_create_affine(char** t, double scale, double offset) {
    if(scale==1.0 && offset==0.0) {
        return recon_transform_create_none(t);
    }
    if(scale==-1.0 && offset==0.0) {
        return recon_transform_create_inverse(t);
    }
    sprintf(*t, "aff(%f,%f)", scale, offset);
    return RECON_OK;
}
