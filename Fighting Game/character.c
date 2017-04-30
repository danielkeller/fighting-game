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

//This overrides next_state because it updates anim_start
void goto_state(character_t *c, int state)
{
    c->next.state = state;
    c->next.fight_state = c->states[state].fight_state;
    c->anim_start = game_time.frame;
}

void next_state(character_t *c)
{
    if (game_time.frame - c->anim_start >= c->states[c->prev.state].frames)
        goto_state(c, c->states[c->prev.state].next_state);
}

void move_character(character_t* c)
{
    float move_amt = 0;
#if SCOOCH
    move_amt = c->prev.advancing
        ? c->states[c->prev.state].fwd_speed
        : c->states[c->prev.state].rev_speed;
#else
    move_amt = c->move_button == c->prev.advancing ? c->speed : c->dodge;
#endif
    move_amt *= c->move_button ? 1. : -1.;
    c->next.ground_pos += move_amt;
    
    //using next may not be fair here, but using prev causes weird oscillations
    float other_pos = -c->other->next.ground_pos;
    float fwd_limit = fmin(1, other_pos) - c->hitbox_width - c->other->hitbox_width;
    c->next.ground_pos = fmin(fmax(c->next.ground_pos, -1), fwd_limit);
}

int step_character(character_t* c, int move_button, int dodge_button, int attack_button)
{
    c->prev = c->next;
    c->next.attack_result = 0;
    c->next.advancing = c->move_button = move_button;
    c->attack_button |= attack_button;
    c->dodge_button |= dodge_button;
    return c->prev.health > 0;
}

//The attack is resolved against the previous state. This
//is so the character which is updated first doesn't have
//an advantage.
void attack(character_t* attacker, attack_t* attack)
{
    character_t* victim = attacker->other;
    
    if (game_time.frame - attacker->anim_start != attack->frame)
        return;
    
    //I suppose this kind of defeats the purpose of the T thing
    if (attack->target == T(hi))
        attacker->state_indicator.top_attack = attack->force;
    else
        attacker->state_indicator.bot_attack = attack->force;
    attacker->state_indicator.last_attack_time = game_time.current_time;
    
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

void set_character_draw_state(character_t* c, program_t* program)
{
    glUniform1f(program->time, (float)game_time.current_time / 1000000.f);
    glUniformMatrix3fv(program->camera, 1, GL_FALSE, camera.d);
    
    c->ground_pos = c->prev.ground_pos * (1. - game_time.alpha)
    + c->next.ground_pos * game_time.alpha;
    
    Mat3 pos = affine(0., c->ground_pos * c->direction, 0.);
    pos.d[0] = c->direction;
    glUniformMatrix3fv(program->transform, 1, GL_FALSE, pos.d);
    
    //next state determines which animation is drawn
    float anim_alpha = ((float)(game_time.frame - c->anim_start) + game_time.alpha)
    / (float)c->states[c->next.state].frames;
    glUniform1f(program->pos_alpha, anim_alpha);
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

void draw_health_bar(character_t *c)
{
    health_bar_t* hb = &c->health_bar;
    
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

void free_health_bar(health_bar_t* hb)
{
    free_program(&hb->program);
}

void make_state_indicator(state_indicator_t* si)
{
    load_shader_program(&si->program, simple_vert, state_indicator_frag);
    si->top_unif = glGetUniformLocation(si->program.program, "top");
    si->bot_unif = glGetUniformLocation(si->program.program, "bottom");
    si->top_attack_unif = glGetUniformLocation(si->program.program, "top_attack");
    si->bot_attack_unif = glGetUniformLocation(si->program.program, "bottom_attack");
    si->top_attack = si->bot_attack = 0;
}

void draw_state_indicator(character_t* c)
{
    state_indicator_t* si = &c->state_indicator;
    
    if (game_time.current_time - si->last_attack_time > 200000ll)
        c->state_indicator.top_attack = c->state_indicator.bot_attack = 0;
    
    glUseProgram(si->program.program);
    glUniformMatrix3fv(si->program.camera, 1, GL_FALSE, camera.d);
    
    Mat3 transform = {{
        .07*c->direction, 0, 0,
        0, .5, 0,
        (c->ground_pos + .1)*c->direction, .2, 1}};
    glUniformMatrix3fv(si->program.transform, 1, GL_FALSE, transform.d);
    
    glUniform1i(si->top_unif, c->next.fight_state.hi.block);
    glUniform1i(si->bot_unif, c->next.fight_state.lo.block);
    glUniform1i(si->top_attack_unif, si->top_attack);
    glUniform1i(si->bot_attack_unif, si->bot_attack);
    
    glBindVertexArray(box.vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, box.numVertecies);
}

void free_state_indicator(state_indicator_t* si)
{
    free_program(&si->program);
}
