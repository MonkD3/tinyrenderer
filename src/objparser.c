#include "include/objparser.h"
#include "include/geometry.h"
#include "include/tgaimage.h"
#include <stdio.h>
#include <string.h>

OBJModel_t* OBJModel_init(void){
    OBJModel_t* out = calloc(1, sizeof(OBJModel_t));
    out->ft = FACETYPE_UNKNOWN;

    out->fx = NULL;
    out->fvx = NULL;
    out->ftx = NULL;
    out->fnx = NULL;

    out->v = NULL;
    out->t = NULL;
    out->n = NULL;
    out->texture = NULL;

    return out;
}

void OBJModel_destroy(OBJModel_t *obj){
    if (obj) {
        if (obj->fx) free(obj->fx);
        if (obj->fvx) free(obj->fvx);
        if (obj->ftx) free(obj->ftx);
        if (obj->fnx) free(obj->fnx);
        if (obj->v) free(obj->v);
        if (obj->t) free(obj->t);
        if (obj->n) free(obj->n);
        if (obj->texture){
            TGAImage_destroy(obj->texture);
            free(obj->texture);
        }
        memset(obj, 0, sizeof(*obj));
    }
}

bool OBJModel_read_file(OBJModel_t *obj, const char *filename){
    if (!obj){
        fprintf(stderr, "Tried to read a .obj file into a NULL pointer\n");
        return false;
    }

    FILE* in = fopen(filename, "r");
    if (!in) {
        fprintf(stderr, "Cannot open file '%s'\n", filename);
        return false;
    }

    uint8_t  dv  = 0;
    uint8_t  dt  = 0;
    uint64_t nv = 0;
    uint64_t nf = 0;
    uint64_t nt = 0;
    uint64_t nn = 0;
    uint64_t nfv = 0; // Final size of f (= xf[nf])
    FaceType_t ft = FACETYPE_UNKNOWN;

    // First pass for vertex and face count
    char* line = NULL;
    size_t linesize = 0;
    size_t lineread = 0;
    // TODO: change strncmpt for a switch case. The match of the switch 
    // should be a unique number computed on the 2 first character of the line
    while (getline(&line, &linesize, in) >= 0){
        lineread++;

        if (!strncmp("#", line, 1)) continue; 
        if (!strncmp("\n", line, 1)) continue; 
                                               
        if (!strncmp("v ", line, 2)){
            nv++;
            // Only check the dimension on the first vertex.
            // This is not robust : if a subsequent vertex ...
            //   1) has more dimensions : the error goes unnoticed as we only read the first d dimension
            //   2) has less dimensions : this will cause an error during parsing because we try to read more tokens
            if (!dv){
                strtok(line, " ");
                while (strtok(NULL, " ")) dv++;
            }

        } 
        else if (!strncmp("f ", line, 2)){
            nf++;
            char* tok = strtok(line, " ");
            while ((tok = strtok(NULL, " "))) {
                nfv++;
                if (ft == FACETYPE_UNKNOWN){
                    int a,b,c; // just trash variable
                    if (sscanf(tok, "%d/%d/%d", &a, &b, &c) == 3) ft = FACETYPE_VTN;
                    else if (sscanf(tok, "%d/%d", &a, &b) == 2) ft = FACETYPE_VT;
                    else if (sscanf(tok, "%d//%d", &a, &b) == 2) ft = FACETYPE_VN;
                    else if (sscanf(tok, "%d", &a) == 1) ft = FACETYPE_V;
                    else {
                        fprintf(stderr, 
                                "Wrong face data : each vertex data should be of the form \n"
                                "\t v1 ==> only vertex data\n"
                                "\t v1/t1 ==> vertex + texture data\n"
                                "\t v1/t1/n1 ==> vertex + texture + normal data\n"
                                "\t v1//n1 ==> vertex + norma data\n"
                                "Obtained instead : %s\n",
                                tok);
                        return false;
                    }
                }
            }
        } 
        else if (!strncmp("vt", line, 2)){
            nt++;
            if (!dt){
                strtok(line, " ");
                while (strtok(NULL, " ")) dt++;
            }
        }
        else if (!strncmp("vn", line, 2)){
            nn++;
        }
        else {
            char buf[3] = {line[0], line[1], '\0'};
            fprintf(stderr, "Ignoring unsupported .obj element '%s' on line %zu\n", buf, lineread);
        }
    }

    // Reset file cursor
    rewind(in);
    uint64_t* fx = malloc(sizeof(uint64_t) * (nf+1));
    int64_t*  fvx  = malloc(sizeof(int64_t) * nfv);
    int64_t*  ftx  = NULL;
    int64_t*  fnx  = NULL;

    float*   v  = malloc(sizeof(float) * nv * dv);
    float*   t = NULL;
    float*   n = NULL;
    if (ft == FACETYPE_VTN || ft == FACETYPE_VT) {
        t = malloc(sizeof(*t) * nt * dt);
        ftx = malloc(sizeof(*ftx) * nfv);
    }
    if (ft == FACETYPE_VTN || ft == FACETYPE_VN) {
        n = malloc(sizeof(*n) * nn * 3);
        fnx = malloc(sizeof(*fnx) * nfv);
    }

    nv = 0;
    nf = 0;
    nt = 0;
    nn = 0;
    nfv = 0; 

    fx[0] = 0;

    while (getline(&line, &linesize, in) >= 0){
        if (!strncmp("#", line, 1)) continue; 
        if (!strncmp("\n", line, 1)) continue; 
                                               
        if (!strncmp("v ", line, 2)){
            // Only check the dimension on the first vertex.
            // This is not robust : if a subsequent vertex ...
            //   1) has more dimensions : the error goes unnoticed as we only read the first d dimension
            //   2) has less dimensions : this will cause an error during parsing because we try to read more tokens
            char* tok = strtok(line, " ");
            for (int32_t i = 0; i < dv; i++){
                tok = strtok(NULL, " ");
                v[nv*dv+i] = strtof(tok, NULL);
            }
            nv++;
        } 
        else if (!strncmp("vt", line, 2)){
            char* tok = strtok(line, " ");
            for (int32_t i = 0; i < dt; i++){
                tok = strtok(NULL, " ");
                t[nt*dt+i] = strtof(tok, NULL);
            }
            nt++;
        }
        else if (!strncmp("vn", line, 2)){
            char* tok = strtok(line, " ");
            for (int32_t i = 0; i < 3; i++){
                tok = strtok(NULL, " ");
                n[nn*3+i] = strtof(tok, NULL);
            }
            nn++;
        }
        else if (!strncmp("f ", line, 2)){
            char* tok = strtok(line, " ");
            while ((tok = strtok(NULL, " "))) {
                switch (ft){
                    case FACETYPE_V:
                        if (sscanf(tok, "%ld", &fvx[nfv]) != 1) goto cleanup_on_parse_error;
                        fvx[nfv]--;
                        break;
                    case FACETYPE_VT:
                        if (sscanf(tok, "%ld/%ld", &fvx[nfv], &ftx[nfv]) != 2) goto cleanup_on_parse_error;
                        fvx[nfv]--; ftx[nfv]--;
                        break;
                    case FACETYPE_VN:
                        if (sscanf(tok, "%ld//%ld", &fvx[nfv], &fnx[nfv]) != 2) goto cleanup_on_parse_error;
                        fvx[nfv]--; fnx[nfv]--;
                        break;
                    case FACETYPE_VTN:
                        if (sscanf(tok, "%ld/%ld/%ld", &fvx[nfv], &ftx[nfv], &fnx[nfv]) != 3) goto cleanup_on_parse_error;
                        fvx[nfv]--; ftx[nfv]--; fnx[nfv]--;
                        break;
                    default:
                        break;
                }
                nfv++;
            }
            nf++;
            fx[nf] = nfv;
        } 
    }
    free(line);
    fprintf(stderr, "Successfully read from '%s': \n"
                    "\t%zu vertices of dimension %d\n"
                    "\t%zu faces of type %d\n"
                    "\t%zu texture of dimension %d\n"
                    "\t%zu normals\n",
                    filename, nv, dv, nf, ft, nt, dt, nn);

    for (int32_t i = 0; i < nn; i++){
        float norm = Vec3f_norm((Vec3f*) (n + 3*i));
        n[i*3] *= norm;
        n[i*3+1] *= norm;
        n[i*3+2] *= norm;
    }

    // Overwrite current object
    OBJModel_destroy(obj); 
    obj->dv  = dv;
    obj->dt  = dt;
    obj->ft = ft;

    obj->nv = nv;
    obj->nf = nf;

    obj->fx = fx;
    obj->fvx  = fvx;
    obj->ftx  = ftx;
    obj->fnx  = fnx;
    obj->v  = v;
    obj->t  = t;
    obj->n  = n;

    return true;

    cleanup_on_parse_error:
        free(fx);
        free(fvx);
        free(ftx);
        free(fnx);
        free(v);
        free(t);
        free(n);
        fprintf(stderr, "Unexpected token encountered while parsing faces, aborting function\n");
        return false;
}

bool OBJModel_read_texture(OBJModel_t *obj, const char *texturefile){
    if (!obj) return false;
    if (obj->texture) {
        TGAImage_destroy(obj->texture);
        free(obj->texture);
    }
    obj->texture = calloc(1, sizeof(*obj->texture));
    bool ret = TGAImage_read_tga_file(obj->texture, texturefile);
    TGAImage_flip_vertically(obj->texture);
    return ret;
}
