//
//  object.c
//  Fighting Game
//
//  Created by Daniel Keller on 8/20/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "engine.h"
#include <assert.h>

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

void make_object_base(struct object* obj, GLsizei n_verts, GLsizei stride, const void* verts)
{
    obj->numVertecies = n_verts;
    
    glGenVertexArrays(1, &obj->vertexArrayObject);
    glBindVertexArray(obj->vertexArrayObject);
    
    glGenBuffers(1, &obj->vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, n_verts * stride, verts, GL_STATIC_DRAW);
}

void make_object(struct object* obj, mesh_t mesh)
{
    GLsizei stride = sizeof(float)*2;
    make_object_base(obj, mesh->size, stride, mesh->verts);
    
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
    
    const GLsizei stride = sizeof(struct anim_vert);
    make_object_base(obj, mesh->size, stride, mesh->verts);
    
    glEnableVertexAttribArray(POSITION_ATTRIB);
    glEnableVertexAttribArray(BONE_ATTRIB);
    glEnableVertexAttribArray(PARENT_ATTRIB);
    glEnableVertexAttribArray(WEIGHT_ATTRIB);
    
    glVertexAttribPointer(POSITION_ATTRIB, 2, GL_FLOAT, GL_FALSE, stride, STRUCT_OFFS(anim_vert, x));
    glVertexAttribIPointer(BONE_ATTRIB, 1, GL_UNSIGNED_BYTE, stride, STRUCT_OFFS(anim_vert, bone));
    glVertexAttribIPointer(PARENT_ATTRIB, 1, GL_UNSIGNED_BYTE, stride, STRUCT_OFFS(anim_vert, parent));
    glVertexAttribPointer(WEIGHT_ATTRIB, 1, GL_UNSIGNED_BYTE, GL_TRUE, stride, STRUCT_OFFS(anim_vert, weight));
}

void free_object(struct object* obj)
{
    glDeleteVertexArrays(1, &obj->vertexArrayObject);
    glDeleteBuffers(1, &obj->vertexBufferObject);
}
