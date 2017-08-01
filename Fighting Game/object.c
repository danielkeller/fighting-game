//
//  object.c
//  Fighting Game
//
//  Created by Daniel Keller on 8/20/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "engine.h"
#include <assert.h>
#include <string.h>

static struct mesh box_mesh = {
    .size = 6,
    .verts = (float[]){
        0.f, 0.f,
        0.f, 1.f,
        1.f, 1.f,
        0.f, 0.f,
        1.f, 1.f,
        1.f, 0.f
    }
};

void make_object_base(struct object* obj, GLsizei num_verts, GLsizei stride, const void* verts)
{
    glGenVertexArrays(1, &obj->vertexArrayObject);
    glBindVertexArray(obj->vertexArrayObject);
    
    glGenBuffers(1, &obj->vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, num_verts * stride, verts, GL_STATIC_DRAW);
}

void make_object(struct object* obj, mesh_t mesh)
{
    GLsizei stride = sizeof(float)*2;
    make_object_base(obj, mesh->size, stride, mesh->verts);
    obj->numVertecies = mesh->size;
    
    glEnableVertexAttribArray(POSITION_ATTRIB);
    glVertexAttribPointer(POSITION_ATTRIB, 2, GL_FLOAT, GL_FALSE, stride, NULL);
}

void make_box(struct object* obj)
{
    make_object(obj, &box_mesh);
}

#define STRUCT_OFFS(label, member) (void*)&((struct label*)NULL)->member

void make_anim_obj(struct object* obj, anim_mesh_t mesh)
{
    assert(mesh->num_bones <= MAX_BONES && "Mesh has too many bones");
    
    if (!mesh->blur_indices) {
        struct anim_vert* doubled_verts = malloc(sizeof(struct anim_vert) * mesh->num_verts * 2);
        for (size_t i = 0; i < mesh->num_verts; ++i) {
            doubled_verts[i] = doubled_verts[i + mesh->num_verts] = mesh->verts[i];
            doubled_verts[i].blur_alpha = 255;
        }
        mesh->verts = doubled_verts;
        
        //Put all of the regular indices first, then the extruded indices
        mesh->blur_indices = malloc(sizeof(GLushort) * mesh->num_inds * 7);
        memcpy(mesh->blur_indices, mesh->indices, sizeof(GLushort) * mesh->num_inds);
        for (size_t i = 0; i < mesh->num_inds; i += 3) {
            size_t j = i * 6 + mesh->num_inds;
            /*
              ,-------u.
             r``-..   | \
              \    `s-+--v
               \   / /  /
                \ /  | /
                 q---t'
            */
            GLushort
                q = mesh->indices[i],
                r = mesh->indices[i+1],
                s = mesh->indices[i+2],
                t = q + mesh->num_verts,
                u = r + mesh->num_verts,
                v = s + mesh->num_verts;
            
            memcpy(mesh->blur_indices + j,
                ((GLushort[]) {
                    q, t, u, q, u, r,
                    r, u, v, r, v, s,
                    s, v, t, s, t, q}),
                sizeof(GLushort)*18);
        }
    }
    
    const GLsizei stride = sizeof(struct anim_vert);
    make_object_base(obj, mesh->num_verts * 2, stride, mesh->verts);
    obj->numVertecies = mesh->num_inds;
    
    glGenBuffers(1, &obj->indexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->indexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->num_inds * sizeof(GLushort) * 7, mesh->blur_indices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(POSITION_ATTRIB);
    glEnableVertexAttribArray(BONE_ATTRIB);
    glEnableVertexAttribArray(PARENT_ATTRIB);
    glEnableVertexAttribArray(WEIGHT_ATTRIB);
    glEnableVertexAttribArray(BLUR_ALPHA_ATTRIB);
    
    glVertexAttribPointer(POSITION_ATTRIB, 2, GL_FLOAT, GL_FALSE, stride, STRUCT_OFFS(anim_vert, x));
    glVertexAttribIPointer(BONE_ATTRIB, 1, GL_UNSIGNED_BYTE, stride, STRUCT_OFFS(anim_vert, bone));
    glVertexAttribIPointer(PARENT_ATTRIB, 1, GL_UNSIGNED_BYTE, stride, STRUCT_OFFS(anim_vert, parent));
    glVertexAttribPointer(WEIGHT_ATTRIB, 1, GL_UNSIGNED_BYTE, GL_TRUE, stride, STRUCT_OFFS(anim_vert, weight));
    glVertexAttribPointer(BLUR_ALPHA_ATTRIB, 1, GL_UNSIGNED_BYTE, GL_TRUE, stride, STRUCT_OFFS(anim_vert, blur_alpha));
}

void draw_object(struct object* obj)
{
    glBindVertexArray(obj->vertexArrayObject);
    glDrawElements(GL_TRIANGLES, obj->numVertecies, GL_UNSIGNED_SHORT, NULL);
}

void draw_blur_object(struct object* obj)
{
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    glBindVertexArray(obj->vertexArrayObject);
    glDrawElements(GL_TRIANGLES, obj->numVertecies*7, GL_UNSIGNED_SHORT, NULL);
    
    glDisable(GL_DEPTH_TEST);
}

void free_object(struct object* obj)
{
    glDeleteVertexArrays(1, &obj->vertexArrayObject);
    glDeleteBuffers(1, &obj->vertexBufferObject);
}
