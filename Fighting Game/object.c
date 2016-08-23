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

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    obj->numVertecies = sizeof(box_verts)/sizeof(float);
}

void free_object(object_t* obj)
{
    glDeleteVertexArrays(1, &obj->vertexArrayObject);
    glDeleteBuffers(1, &obj->vertexBufferObject);
}