//
//  character.c
//  Fighting Game
//
//  Created by Daniel Keller on 4/20/17.
//  Copyright © 2017 Daniel Keller. All rights reserved.
//

#include "character_internal.h"
#include "engine.h"
#include <math.h>

void goto_state(character_t *c, int state)
{
    c->next.state = state;
    c->next.fight_state = c->states[state].fight_state;
    c->anim_start = game_time.frame;
    anim_obj_keys(&c->obj, &c->anims[state]);
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

void move_character(character_t* c)
{
    c->next.ground_pos += c->speed + c->states[c->prev.state].scooch;
    
    //using next may not be fair here, but using prev causes weird oscillations
    float other_pos = -c->other->next.ground_pos;
    float fwd_limit = fmin(1, other_pos) - c->hitbox_width - c->other->hitbox_width;
    c->next.ground_pos = fmin(fmax(c->next.ground_pos, -1), fwd_limit);
}

int step_character(character_t* c, int dodge_button, int attack_button)
{
    c->prev = c->next;
    c->next.attack_result = 0;
    c->attack_button |= attack_button;
    c->dodge_button |= dodge_button;
    return c->prev.health > 1;
}

//The attack is resolved against the previous state. This
//is so the character which is updated first doesn't have
//an advantage.
void attack(character_t* attacker, attack_t* attack)
{
    character_t* victim = attacker->other;
    
    if (game_time.frame - attacker->anim_start != attack->frame)
        return;
    
    if (-victim->prev.ground_pos - attacker->prev.ground_pos > attack->range) {
        attacker->next.attack_result |= WHIFFED;
        return;
    }
    
    strike_point_t* target = &victim->prev.fight_state.hi + attack->target;
    
    int knockback = attack->knock - victim->prev.fight_state.balance;
    
    if (attack->force > target->block) //Attack lands
    {
        int damage = attack->damage - target->defense;
        
        victim->next.health -= damage;
        if (victim->next.health < 0)
            victim->next.health = 0;
        
        attacker->next.attack_result |= LANDED;
    }
    else
        attacker->next.attack_result |= PARRIED;
    
    if (attack->force >= target->block && knockback > 0)
    {
        victim->next.ground_pos -= .02*knockback;
        if (victim->next.ground_pos < -1.)
            victim->next.ground_pos = -1.;
        
        attacker->next.attack_result |= KNOCKED;
    }
}

void draw_character_internal(character_t* c)
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

void character_actions(character_t* c)
{
    CALL_BOUND(c->actions);
}
void draw_character(character_t* c)
{
    CALL_BOUND(c->draw);
}
void free_character(character_t* c)
{
    CALL_BOUND(c->free);
}

void make_heath_bar(health_bar_t* hb, direction_t direction)
{
    hb->last_health = 100;
    hb->last_health_change_time = 0;
    make_box(&hb->obj);
    load_shader_program(&hb->program, simple_vert, health_bar_frag);
    hb->health_unif = glGetUniformLocation(hb->program.program, "health");
    hb->last_health_unif = glGetUniformLocation(hb->program.program, "last_health");
    hb->time_since_change_unif = glGetUniformLocation(hb->program.program, "time_since_last_change");
    
    glUseProgram(hb->program.program);
    GLint color_unif = glGetUniformLocation(hb->program.program, "main_color");
    glUniform3f(color_unif, 1., 0., 0.);
    Mat3 transform = {{
        -.8*direction, 0, 0,
        0, .1, 0,
        -.1*direction, .8, 1}};
    glUniformMatrix3fv(hb->program.transform, 1, GL_FALSE, transform.d);
    glUniformMatrix3fv(hb->program.camera, 1, GL_FALSE, eye3.d);
}

void draw_health_bar(character_t *c)
{
    health_bar_t* bar = &c->health_bar;
    
    if (c->next.health != bar->health) {
        bar->last_health_change_time = game_time.current_time;
        bar->last_health = bar->health;
        bar->health = c->next.health;
    }
    
    glUseProgram(bar->program.program);
    glUniform1f(bar->health_unif, bar->health);
    glUniform1f(bar->last_health_unif, bar->last_health);
    glUniform1f(bar->time_since_change_unif,
                (float)(game_time.current_time - bar->last_health_change_time) / 1000000.f);
    glBindVertexArray(box.vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, box.numVertecies);
}

void free_health_bar(health_bar_t* hb)
{
    free_object(&hb->obj);
    free_program(&hb->program);
}
