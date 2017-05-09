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
    .size = 12*sizeof(float),
    .stride = 0,
    .verts = (float[]){
        0.f, 0.f,
        0.f, 1.f,
        1.f, 1.f,
        0.f, 0.f,
        1.f, 1.f,
        1.f, 0.f
    }
};

void make_object(struct object* obj, mesh_t mesh)
{
    GLsizei stride = mesh->stride;
    if (stride == 0) stride = sizeof(float)*2;
    
    glGenVertexArrays(1, &obj->vertexArrayObject);
    glBindVertexArray(obj->vertexArrayObject);
    
    glGenBuffers(1, &obj->vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, mesh->size, mesh->verts, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(POSITION_ATTRIB);
    glVertexAttribPointer(POSITION_ATTRIB, 2, GL_FLOAT, GL_FALSE, stride, NULL);
    
    obj->numVertecies = mesh->size / stride;
    obj->stride = stride;
}

void make_box(struct object* obj)
{
    make_object(obj, &box_mesh);
}

void make_anim_obj(struct object* obj, mesh_t mesh)
{
    make_object(obj, mesh);
    
    glEnableVertexAttribArray(POS_FROM_ATTRIB);
    glEnableVertexAttribArray(POS_TO_ATTRIB);
    glEnableVertexAttribArray(DERIV_FROM_ATTRIB);
    glEnableVertexAttribArray(DERIV_TO_ATTRIB);
}

void anim_obj_keys(struct object* obj, const struct anim_step* step)
{
    assert(obj->stride > step->d_from && obj->stride > step->d_to
           && obj->stride > step->p_from && obj->stride > step->p_to
           && "Not enough attributes");
    
    glBindVertexArray(obj->vertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vertexBufferObject);
    glVertexAttribPointer(POS_FROM_ATTRIB, 2, GL_FLOAT, GL_FALSE,
                          obj->stride, (char*)NULL + step->p_from);
    glVertexAttribPointer(POS_TO_ATTRIB, 2, GL_FLOAT, GL_FALSE,
                          obj->stride, (char*)NULL + step->p_to);
    glVertexAttribPointer(DERIV_FROM_ATTRIB, 2, GL_FLOAT, GL_FALSE,
                          obj->stride, (char*)NULL + step->d_from);
    glVertexAttribPointer(DERIV_TO_ATTRIB, 2, GL_FLOAT, GL_FALSE,
                          obj->stride, (char*)NULL + step->d_to);
}

void free_object(struct object* obj)
{
    glDeleteVertexArrays(1, &obj->vertexArrayObject);
    glDeleteBuffers(1, &obj->vertexBufferObject);
}
