#include "crecon_impl.h"

recon_status recon_transform_create_none(recon_transform* t) {
    *t = NULL;
    return RECON_OK;
}

recon_status recon_transform_create_inverse(recon_transform* t) {
    *t = "inv";
    return RECON_OK;
}

recon_status recon_transform_create_affine(recon_transform* t, double scale, double offset) {
    if(scale==1.0 && offset==0.0) {
        return recon_transform_create_none(t);
    }
    if(scale==-1.0 && offset==0.0) {
        return recon_transform_create_inverse(t);
    }
    sprintf(*t, "aff(%f,%f)", scale, offset);
    return RECON_OK;
}
