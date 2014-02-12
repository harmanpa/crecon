#include "crecon_impl.h"

/* Pass unallocated strings as parameters */
recon_status recon_transform_create_none(char** t) {
    *t = NULL;
    return RECON_OK;
}

recon_status recon_transform_create_inverse(char** t) {
    *t = (char*) malloc(strlen("inv") + 1);
    memcpy(*t, "inv", strlen("inv"));
    (*t)[strlen("inv")] = '\0';
    return RECON_OK;
}

recon_status recon_transform_create_affine(char** t, double scale, double offset) {
    int max_sig_digits = 16;
    char* transform_string;
    transform_string = (char*) malloc(strlen("aff(-.,-.)") + 2 * max_sig_digits + 1);
    if (scale == 1.0 && offset == 0.0) {
        return recon_transform_create_none(t);
    }
    if (scale == -1.0 && offset == 0.0) {
        return recon_transform_create_inverse(t);
    }
    sprintf(transform_string, "aff(%.*g,%.*g)", max_sig_digits, scale, max_sig_digits, offset);
    *t = (char*) malloc(strlen(transform_string) + 1);
    memcpy(*t, transform_string, strlen(transform_string) + 1);
    return RECON_OK;
}

recon_status recon_transform_apply_double(char* transform, double in, double* out) {
	if(transform) {
		if(strcmp(transform, "inv")==0) {
			*out = in * -1.0;
			return RECON_OK;
		}
	}
	*out = in;
	return RECON_OK;
}

recon_status recon_transform_apply_int(char* transform, int in, int* out) {
	if(transform) {
		if(strcmp(transform, "inv")==0) {
			*out = in * -1;
			return RECON_OK;
		}
	}
	*out = in;
	return RECON_OK;
}

recon_status recon_transform_apply_boolean(char* transform, recon_booleantype in, recon_booleantype* out) {
	if(transform) {
		if(strcmp(transform, "inv")==0) {
			if(in) {
				*out = RECON_FALSE;
			} else {
				*out = RECON_TRUE;
			}
			return RECON_OK;
		}
	}
	*out = in;
	return RECON_OK;
}
