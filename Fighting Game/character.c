//
//  character.c
//  Fighting Game
//
//  Created by Daniel Keller on 4/20/17.
//  Copyright © 2017 Daniel Keller. All rights reserved.
//

#include "character.h"
#include "engine.h"

void goto_state(character_t *c, int state)
{
    c->next.state = state;
    c->next.fight_state = c->states[state].fight_state;
    c->anim_start = game_time.frame;
    anim_obj_keys(&c->obj, c->states[state].anim);
}

void next_state(character_t *c, int state)
{
    if (game_time.frame - c->anim_start >= c->states[c->prev.state].frames)
        goto_state(c, state);
}

void run_anim(character_t *c)
{
    //next state determines which animation is drawn
    float anim_alpha = ((float)(game_time.frame - c->anim_start) + game_time.alpha)
        / (float)c->states[c->next.state].frames;
    glUniform1f(c->program.pos_alpha, anim_alpha);
}

void step_character(character_t* c, int advance_button, int attack_button)
{
    c->prev = c->next;
    c->next.attack_result = 0;
    c->attack_button |= attack_button;
    c->advance_button = advance_button;
}

//The attack is resolved against the previous state. This
//is so the character which is updated first doesn't have
//an advantage.
void attack(character_t* attacker, attack_t attack)
{
    character_t* victim = attacker->other;
    
    if (game_time.frame - attacker->anim_start != attack.frame)
        return;
    
    if (-victim->prev.ground_pos - attacker->prev.ground_pos > attack.range) {
        attacker->next.attack_result |= WHIFFED;
        return;
    }
    
    strike_point_t* target = &victim->prev.fight_state.hi + attack.target;
    
    int effective_momentum = attack.momentum - target->momentum;
    if (effective_momentum >= attack.momentum / 2) //Attack lands
    {
        int damage = attack.damage - target->defense;
        victim->next.health -= damage;
        if (victim->next.health < 0)
            victim->next.health = 0;
        
        attacker->next.attack_result |= LANDED;
    }
    else
        attacker->next.attack_result |= PARRIED;
    
    int knockback = effective_momentum - victim->prev.fight_state.balance;
    if (knockback > 0)
    {
        victim->next.ground_pos -= .02*knockback;
        attacker->next.attack_result |= KNOCKED;
    }
}

void draw_character(character_t* c)
{
    glUseProgram(c->program.program);
    glBindVertexArray(c->obj.vertexArrayObject);
    
    glUniformMatrix3fv(c->program.camera, 1, GL_FALSE, camera.d);
    
    float ground_pos = c->prev.ground_pos * (1. - game_time.alpha)
    + c->next.ground_pos * game_time.alpha;
    
    Mat3 pos = affine(0., ground_pos * c->direction, 0.);
    pos.d[0] = c->direction;
    glUniformMatrix3fv(c->program.transform, 1, GL_FALSE, pos.d);
    
    run_anim(c);
    
    glDrawArrays(GL_TRIANGLES, 0, c->obj.numVertecies);
}

void make_heath_bar(health_bar_t* hb, direction_t direction)
{
    make_box(&hb->obj);
    load_shader_program(&hb->program, health_bar_vert, color_frag);
    hb->health_unif = glGetUniformLocation(hb->program.program, "health");
    
    glUseProgram(hb->program.program);
    GLint color_unif = glGetUniformLocation(hb->program.program, "main_color");
    glUniform3f(color_unif, 1., 0., 0.);
    GLint direction_unif = glGetUniformLocation(hb->program.program, "direction");
    glUniform1f(direction_unif, (float)direction);
}

void draw_health_bar(character_t *c)
{
    glUseProgram(c->health_bar.program.program);
    glUniform1f(c->health_bar.health_unif, (float)c->next.health);
    glBindVertexArray(box.vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, box.numVertecies);
}

void free_health_bar(health_bar_t* hb)
{
    free_object(&hb->obj);
    free_program(&hb->program);
}
