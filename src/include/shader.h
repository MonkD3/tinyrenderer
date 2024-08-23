#ifndef _SHADER_H
#define _SHADER_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "geometry.h"
#include "objparser.h"
#include "tgaimage.h"
#include "scene.h"

// v_out is the vertex processed by the shader 
// model is the model containing the (world) vertex data
// fx/vx are the face and vertex index in the model 
// shd is a data buffer used to pass information between vertex and fragment shader
typedef void (*vshader_fn)(Vec3f* v_out, OBJModel_t const* model, int32_t fx, int32_t vx, void * shd);

// c is the output color of the pixel
// model is the current model (useful for textures)
// bc is the barycentric coordinate of the pixel in the triangle
// shd is a data buffer used to pass information between vertex and fragment shader
typedef bool (*fshader_fn)(TGAColor_t* c, OBJModel_t const* model, Vec3f const * bc, void const * shd);

typedef struct {
    vshader_fn vsh;
    fshader_fn fsh;
    size_t datasize; // Size of the shd buffer in bytes
} Shader_t;


// ===================== Gouraud Shader ======================

void gouraud_vertex_sh(Vec3f* v_out, OBJModel_t const* model, int32_t fx, int32_t vx, void * shd);
bool gouraud_fragment_sh(TGAColor_t* c, OBJModel_t const* model, Vec3f const * bc, void const * shd);
typedef struct {
    Vec3f intensity;
} GouraudShaderData_t;
extern Shader_t const GouraudShader;


// ===================== Simple Texture Shader ======================
void texture_vertex_sh(Vec3f* v_out, OBJModel_t const* model, int32_t fx, int32_t vx, void * shd);
bool texture_fragment_sh(TGAColor_t* c, OBJModel_t const* model, Vec3f const * bc, void const * shd);
typedef struct {
    Vec3f intensity;
    Vec3f tpos[3];
} TextureShaderData_t;
extern Shader_t const TextureShader;


// ===================== Texture + Normal maps shader ================
void texture_normal_vertex_sh(Vec3f* v_out, OBJModel_t const* model, int32_t fx, int32_t vx, void * shd);
bool texture_normal_fragment_sh(TGAColor_t* c, OBJModel_t const* model, Vec3f const * bc, void const * shd);
typedef struct {
    Vec3f tpos[3];
    Transform3f uniform_M;
    Transform3f uniform_MIT;
} TextureNormalShaderData_t;
extern Shader_t const TextureNormalShader;

// =================== Texture + Normal maps + Specular map ===========
void texture_normal_spec_vertex_sh(Vec3f* v_out, OBJModel_t const* model, int32_t fx, int32_t vx, void * shd);
bool texture_normal_spec_fragment_sh(TGAColor_t* c, OBJModel_t const* model, Vec3f const * bc, void const * shd);
typedef struct {
    Vec3f tpos[3];
    Transform3f uniform_M;
    Transform3f uniform_MIT;
} TextureNormalSpecShaderData_t;
extern Shader_t const TextureNormalSpecShader;

#endif // _SHADER_H
