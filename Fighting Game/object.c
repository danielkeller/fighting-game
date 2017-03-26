//
//  object.c
//  Fighting Game
//
//  Created by Daniel Keller on 8/20/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "object.h"
#include "shader.h"

#include "gl_core_3_3.h"
#include <assert.h>

static const GLfloat box_verts[] = {
    0.f, 0.f,
    0.f, 1.f,
    1.f, 1.f,
    0.f, 0.f,
    1.f, 1.f,
    1.f, 0.f
};

void make_box(object_t* obj)
{
    glGenVertexArrays(1, &obj->vertexArrayObject);
    glBindVertexArray(obj->vertexArrayObject);
    
    glGenBuffers(1, &obj->vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(box_verts), box_verts, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(POSITION_ATTRIB);
    glVertexAttribPointer(POSITION_ATTRIB, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    
    obj->numVertecies = sizeof(box_verts)/sizeof(float);
    obj->stride = 0;
}

void make_anim_obj(object_t* obj, float* verts, GLsizei verts_sz, GLsizei stride)
{
    glGenVertexArrays(1, &obj->vertexArrayObject);
    glBindVertexArray(obj->vertexArrayObject);
    
    glGenBuffers(1, &obj->vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, verts_sz, verts, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(POS_FROM_ATTRIB);
    glEnableVertexAttribArray(POS_TO_ATTRIB);
    glEnableVertexAttribArray(DERIV_FROM_ATTRIB);
    glEnableVertexAttribArray(DERIV_TO_ATTRIB);
    
    obj->numVertecies = verts_sz / stride;
    obj->stride = stride;
}

void anim_obj_keys(object_t* obj, const anim_step_t* step)
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

void free_object(object_t* obj)
{
    glDeleteVertexArrays(1, &obj->vertexArrayObject);
    glDeleteBuffers(1, &obj->vertexBufferObject);
}
