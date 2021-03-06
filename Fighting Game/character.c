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
#include <assert.h>

//This overrides next_state because it updates anim_start
void goto_state(struct character *c, int state)
{
    c->next.state = state;
    c->next.fight_state = c->states[state].fight_state;
    c->anim_start = game_time.frame;
}

void next_state(struct character *c)
{
    if (game_time.frame - c->anim_start >= c->states[c->prev.state].frames)
        goto_state(c, c->states[c->prev.state].next_state);
}

void move_character(struct character* c)
{
    c->next.ground_pos += c->states[c->next.state].speed;
    
    //using next may not be fair here, but using prev causes weird oscillations
    float other_pos = -c->other->next.ground_pos;
    float fwd_limit = fmin(1, other_pos) - c->hitbox_width - c->other->hitbox_width;
    c->next.ground_pos = fmin(fmax(c->next.ground_pos, -1), fwd_limit);
}

int step_character(character_t* c, struct key_events* events)
{
    c->prev = c->next;
    c->next.attack_result = 0;
    update_key_events(&c->buttons, events);
    return c->prev.health > 0;
}

//The attack is resolved against the previous state. This
//is so the character which is updated first doesn't have
//an advantage.
void attack(struct character* attacker, struct attack* attack)
{
    character_t* victim = attacker->other;
    
    if (game_time.frame - attacker->anim_start != attack->frame)
        return;
    
    attacker->state_indicator.attack[attack->target] = attack->force;
    attacker->state_indicator.last_attack_time = game_time.current_time;
    
    if (-victim->prev.ground_pos - attacker->prev.ground_pos > attack->range
        || -victim->prev.ground_pos - attacker->prev.ground_pos < attack->min_range) {
        attacker->next.attack_result |= WHIFFED;
        return;
    }
    
    struct strike_point* target = &victim->prev.fight_state.strike_points[attack->target];
    
    int knockback = attack->knock - victim->prev.fight_state.balance;
    
    int damage = attack->damage - target->defense;
    if (attack->force > target->block && damage > 0) { //Attack lands
        victim->next.health -= damage;
        if (victim->next.health < 0)
            victim->next.health = 0;
        
        attacker->next.attack_result |= LANDED;
    }
    else
        attacker->next.attack_result |= PARRIED;
    
    if (attack->force >= target->block && knockback > 0) {
        victim->next.ground_pos -= .02*knockback;
        if (victim->next.ground_pos < -1.)
            victim->next.ground_pos = -1.;
        
        attacker->next.attack_result |= KNOCKED;
    }
}

void set_character_draw_state(struct character* c, struct program* program, anim_mesh_t mesh, animation_t anim, float frame)
{
    assert(anim && "Animation not implemented");
    
    int frame_num = frame;
    //In case we're off the end
    if (frame >= anim->length) {
        frame = anim->length;
        frame_num = anim->length - 1;
    }
    
    glUniform4fv(program->bones_from, mesh->num_bones, (float*)anim->frames[frame_num]);
    glUniform4fv(program->bones_to, mesh->num_bones, (float*)anim->frames[frame_num+1]);
    glUniform1f(program->alpha, frame - frame_num);
    
    glUniform1f(program->time, (float)game_time.current_time / 1000000.f);
    glUniformMatrix3fv(program->camera, 1, GL_FALSE, camera.d);
    
    c->ground_pos = c->prev.ground_pos * (1. - game_time.alpha)
    + c->next.ground_pos * game_time.alpha;
    
    Mat3 pos = affine(0., c->ground_pos * c->direction, 0.);
    pos.d[0] = c->direction;
    glUniformMatrix3fv(program->transform, 1, GL_FALSE, pos.d);
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

character_t* alloc_character()
{
    return malloc(sizeof(character_t));
}

void make_heath_bar(struct health_bar* hb, enum direction direction)
{
    hb->last_health = 100;
    hb->last_health_change_time = 0;
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
        -.1*direction, 1.1, 1}};
    glUniformMatrix3fv(hb->program.transform, 1, GL_FALSE, transform.d);
}

void draw_health_bar(struct character *c)
{
    struct health_bar* hb = &c->health_bar;
    
    if (c->next.health != hb->health) {
        hb->last_health_change_time = game_time.current_time;
        hb->last_health = hb->health;
        hb->health = c->next.health;
    }
    
    glUseProgram(hb->program.program);
    glUniformMatrix3fv(hb->program.camera, 1, GL_FALSE, camera.d);
    glUniform1f(hb->health_unif, hb->health);
    glUniform1f(hb->last_health_unif, hb->last_health);
    glUniform1f(hb->time_since_change_unif,
                (float)(game_time.current_time - hb->last_health_change_time) / 1000000.f);
    glBindVertexArray(box.vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, box.numVertecies);
}

void free_health_bar(struct health_bar* hb)
{
    free_program(&hb->program);
}

void make_state_indicator(struct state_indicator* si)
{
    load_shader_program(&si->program, simple_vert, state_indicator_frag);
    si->top_unif = glGetUniformLocation(si->program.program, "top");
    si->bot_unif = glGetUniformLocation(si->program.program, "bottom");
    si->top_attack_unif = glGetUniformLocation(si->program.program, "top_attack");
    si->bot_attack_unif = glGetUniformLocation(si->program.program, "bottom_attack");
    si->attack[HI] = si->attack[LO] = 0;
}

void draw_state_indicator(struct character* c)
{
    struct state_indicator* si = &c->state_indicator;
    
    if (game_time.current_time - si->last_attack_time > 200000ll)
        si->attack[HI] = si->attack[LO] = 0;
    
    glUseProgram(si->program.program);
    glUniformMatrix3fv(si->program.camera, 1, GL_FALSE, camera.d);
    
    Mat3 transform = {{
        .07*c->direction, 0, 0,
        0, .5, 0,
        (c->ground_pos + .1)*c->direction, .2, 1}};
    glUniformMatrix3fv(si->program.transform, 1, GL_FALSE, transform.d);
    
    glUniform1i(si->top_unif, c->next.fight_state.strike_points[0].block);
    glUniform1i(si->bot_unif, c->next.fight_state.strike_points[1].block);
    glUniform1i(si->top_attack_unif, si->attack[HI]);
    glUniform1i(si->bot_attack_unif, si->attack[LO]);
    
    glBindVertexArray(box.vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, box.numVertecies);
}

void free_state_indicator(struct state_indicator* si)
{
    free_program(&si->program);
}
