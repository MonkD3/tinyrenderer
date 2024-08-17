#ifndef __OBJPARSER_H_
#define __OBJPARSER_H_

#include "tgaimage.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum {
    FACETYPE_UNKNOWN = 0,  // Unknown format or empty object
    FACETYPE_V       = 1,  // Line of the form : f v1 v2 v3 ...
    FACETYPE_VT      = 2,  // Line of the form : f v1/t1 v2/t2 v3/t3 ...
    FACETYPE_VN      = 3,  // Line of the form : f v1//n1 v2//n2 v3//n3 ...
    FACETYPE_VTN     = 4   // Line of the form : f v1/t1/n1 v2/t2/n2 v3/t3/n3 ...
} FaceType_t;

/* Represent an object stored in .obj format.
 * Note that we loose 5 padding bytes for the alignment of the structure.
 * This is fine as we will likely not have a great number of such structure.
 *
 * xf, v and f form the CSR representation of the face-vertex adjacency matrix.
 */
typedef struct {
    uint8_t dv;     // Dimension of vertices
    uint8_t dt;     // Dimension of texture
    FaceType_t ft;  // Type of the face
    // Size of lists
    uint64_t nv;    // Lenght of vertices
    uint64_t nf;    // Number of faces
    uint64_t nt;    // Number of texture
    uint64_t nn;    // Number of normals
    // Index in lists
    uint64_t* fx;   // Face index. Starting index of face i in fvx/ftx/fnx
    int64_t* fvx;   // faces vertex index. Size = xf[nf]
    int64_t* ftx;   // face textures index. Size = xf[nf]
    int64_t* fnx;   // face normals index. Size = xf[nf]
    // Lists
    float* v;      // Vertices. Size = nv*dv
    float* t;      // texture. Size = nt*dt
    float* n;      // Normals. Size = nn*3
    TGAImage_t* texture;
} OBJModel_t;
OBJModel_t* OBJModel_init(void);
void OBJModel_destroy(OBJModel_t* obj);

/* Read a .obj file and put it in obj.
 * Warning: the pointers in @obj have to be either NULL or malloc'd.
 *          i.e. @obj has to be allocated using C11 "Object obj = {0}" notation
 *               or using the OBJModel_init() function.
 *          if they are NULL they will be malloc'd 
 *          if they are malloc'd they will be freed and malloc'd
 */
bool OBJModel_read_file(OBJModel_t* obj, char const* filename);
bool OBJModel_read_texture(OBJModel_t* obj, char const* texturefile);

#endif // __OBJPARSER_H_
