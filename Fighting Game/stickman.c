//
//  stickman.c
//  Fighting Game
//
//  Created by Daniel Keller on 9/1/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "character_internal.h"
#include "objects/stickman.h"
#include "engine.h"

static const float speed = .015;
static const float rev_speed = RETREAT * speed;
static const float block_dist = .3;

//For attacks that give defense/momentum buffs, used symmetrically:
//If the attack is before the midpoint of the animation, advantage goes to the
//first player. After the midpoint, it goes to the second. If it is at the midpoint
//either both attacks land or neither.

//To make dodging make sense, the player must be stationary or slow during attacks

//            frames  next  fwd_speed rev_speed    balance   hi        lo
static const struct state states[] = {
    [top]       = {1, top,       speed, rev_speed, {10, {{8,  MIDDLE}, {0,  WEAK}}}},
    [bottom]    = {1, bottom,    speed, rev_speed, {8,  {{0,  WEAK},   {8,  MIDDLE}}}},
    [swing_1]   = {4, swing_2,   0,     0,         {5,  {{10, HEAVY},  {0,  WEAK}}}},
    [swing_2]   = {3, bottom,    0,     0,         {5,  {{10, HEAVY},  {0,  WEAK}}}},
    [swingup_1] = {5, swingup_2, 0,     0,         {20, {{0,  WEAK},   {10, HEAVY}}}},
    [swingup_2] = {4, top,       0,     0,         {20, {{0,  WEAK},   {10, HEAVY}}}},
    
#define UNSTEADY                                   {4,  {{0, WEAK},    {0, WEAK}}}
    [lift_1]        = {4, lift_2,      0,         0,             UNSTEADY},
    [lift_2]        = {6, overhead,    0,         0,             UNSTEADY},
    [unlift_1]      = {5, unlift_2,    0,         0,             UNSTEADY},
    [unlift_2]      = {4, top,         0,         0,             UNSTEADY},
    [overhead]      = {1, overhead,    speed*.75, rev_speed*.75, UNSTEADY},
    [big_swing_1]   = {3, big_swing_2, 0,         0,             UNSTEADY},
    [big_swing_2]   = {1, big_swing_3, 0,         0,             UNSTEADY},
    [big_swing_3]   = {1, big_swing_4, 0,         0, {5,  {{10, HEAVY},  {0,  WEAK}}}},
    [big_swing_4]   = {2, bottom,      0,         0, {5,  {{10, HEAVY},  {0,  WEAK}}}},
    
    [lunge]   = {4, forward, speed*20./4., 0,             {10, {{8,  WEAK}, {8,  WEAK}}}},
    [forward] = {1, forward, speed*.75,    rev_speed*.75, {10, {{8,  WEAK}, {8,  WEAK}}}},
    [unlunge] = {4, bottom,  0,            block_dist/4,  {10, {{8,  WEAK}, {8,  WEAK}}}},
    
#define BLOCKED                                    {10, {{20, HEAVY},  {20, HEAVY}}}
#define UNBLOCKED                                  {10,  {{0, WEAK},    {0, WEAK}}}
    [hi_block]   =   {2, block,        0, block_dist/2., BLOCKED},
    [lo_block_1] =   {1, lo_block_2,   0, block_dist/3., BLOCKED},
    [lo_block_2] =   {1, lo_block_3,   0, block_dist/3., BLOCKED},
    [lo_block_3] =   {1, block,        0, block_dist/3., BLOCKED},
    [block]      =   {6, hi_unblock,   0, 0,             BLOCKED},
    [hi_unblock] =   {4, top,          0, 0,             UNBLOCKED},
    [lo_unblock_1] = {1, lo_unblock_2, 0, 0,             UNBLOCKED},
    [lo_unblock_2] = {1, lo_unblock_3, 0, 0,             UNBLOCKED},
    [lo_unblock_3] = {1, bottom,       0, 0,             UNBLOCKED},
};

//Attacks can be in cancellable states, but *only* on the last frame. Otherwise
//the player can cancel the action after the attack comes out.

static struct attack
//          frame target range damage knock force
down_attack     = {2, HI, .6, 14, 15, MIDDLE},
up_attack       = {5, LO, .6, 10, 30, MIDDLE},
overhead_attack = {1, HI, .6, 20, 20, HEAVY},
lunge_attack    = {4, LO, .8, 10, 0,  MIDDLE};

//Note that new actions are considered to start epsilon after the previous frame,
//as opposed to on the start of the next frame. (the previous frame is stored in anim_start)
//So the states and transitions for a 2-frame state look like this:
//       _____#-----=-----=.....@
// anim_start ^  +1 ^  +2 ^
// where '=' are calls to _actions() in the '-' state
int stickman_actions(struct stickman* sm)
{
    character_t* c = sm->character;
    
    move_character(c);
    
    int can_dodge = DODGE && (c->prev.ground_pos > -1. + block_dist || c->prev.advancing);
    int can_cancel = CANCELLING && game_time.frame - c->anim_start <= 3;
    int is_last_frame = game_time.frame - c->anim_start >= states[c->prev.state].frames;
    
    //Remember not to call shift_button_press until we're really ready to use the input
    
    switch (c->prev.state) {
        case top:
            if (can_dodge && shift_button_press(&c->buttons.dodge))
                goto_state(c, hi_block);
            else if (shift_button_press(&c->buttons.attack))
                goto_state(c, swing_1);
            else if (shift_button_press(&c->buttons.special))
                goto_state(c, lift_1);
            break;
        case swing_1:
            if (can_cancel && can_dodge && shift_button_press(&c->buttons.dodge))
                goto_state(c, hi_block);
            else if (can_cancel && !c->buttons.attack.down)
                goto_state(c, top);
            break;
        case swing_2:
            attack(c, &down_attack);
            if (c->next.attack_result & PARRIED) push_effect(&effects, make_parry_effect(sm, .6));
            break;
            
        case bottom:
            if (can_dodge && shift_button_press(&c->buttons.dodge))
                goto_state(c, lo_block_1);
            else if (shift_button_press(&c->buttons.attack))
                goto_state(c, swingup_1);
            else if (shift_button_press(&c->buttons.special))
                goto_state(c, lunge);
            break;
        case swingup_1:
            if (can_cancel && can_dodge && shift_button_press(&c->buttons.dodge))
                goto_state(c, lo_block_1);
            else if (can_cancel && !c->buttons.attack.down)
                goto_state(c, bottom);
            attack(c, &up_attack);
            if (c->next.attack_result & PARRIED) push_effect(&effects, make_parry_effect(sm, .7));
            break;
            
        case lift_1:
            if (can_cancel && !c->buttons.special.down)
                goto_state(c, top);
            break;
        case overhead:
            if (shift_button_press(&c->buttons.attack))
                goto_state(c, big_swing_1);
            else if (shift_button_press(&c->buttons.special) || c->buttons.dodge.down)
                goto_state(c, unlift_1);
            break;
        case big_swing_1:
            if (can_cancel && !c->buttons.attack.down)
                goto_state(c, top);
            break;
        case big_swing_3:
            attack(c, &overhead_attack);
            if (c->next.attack_result & LANDED)  push_effect(&effects, make_hit_effect(sm));
            if (c->next.attack_result & PARRIED) push_effect(&effects, make_parry_effect(sm, .7));
            break;
            
        case lunge:
            attack(c, &lunge_attack);
            break;
        
        case forward:
            if (shift_button_press(&c->buttons.special) || c->buttons.dodge.down)
                goto_state(c, unlunge);
            break;
            
        case lo_block_3:
            if (can_cancel && !c->buttons.dodge.down)
                goto_state(c, lo_unblock_1);
            break;
        
        case hi_block:
            if (can_cancel && !c->buttons.dodge.down)
                goto_state(c, hi_unblock);
            break;
            
        case block:
            if (is_last_frame && shift_button_press(&c->buttons.attack))
                goto_state(c, lo_unblock_1);
            break;
            
        default:
            break;
    }
    
    next_state(c);
    anim_obj_keys(&sm->torso, &stickman_torso_anims[c->next.state]);
    
    glUseProgram(sm->program.program);
    if (DODGE && !can_dodge)
        glUniform3f(sm->color_unif, 1., .8, .8);
    else
        glUniform3f(sm->color_unif, 1., 1., 1.);
        
    //This effect is annoying
    //if (c->other->prev.attack_result & KNOCKED)
    //    goto_state(c, bottom);
    
    return 0;
}
BINDABLE(stickman_actions, struct stickman)

//'frame' is also the number of the previous frame here.
int draw_stickman(struct stickman* sm)
{
    glUseProgram(sm->program.program);
    
    set_character_draw_state(sm->character, &sm->program);
    glBindVertexArray(sm->torso.vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, sm->torso.numVertecies);
    
    set_character_legs_draw_state(sm->character, &sm->program, .32);
    glBindVertexArray(sm->legs.vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, sm->legs.numVertecies);
    
    draw_health_bar(sm->character);
    if (learning_mode)
        draw_state_indicator(sm->character);
    return 0;
}
BINDABLE(draw_stickman, struct stickman)

int free_stickman(struct stickman* sm)
{
    character_t* c = sm->character;
    
    free_health_bar(&c->health_bar);
    free_state_indicator(&c->state_indicator);
    free_program(&sm->hit_effect);
    free_program(&sm->parry_effect);
    free_object(&sm->torso);
    free_object(&sm->legs);
    free_program(&sm->program);
    
    free(sm);
    return 0;
}
BINDABLE(free_stickman, struct stickman)

void make_stickman(character_t* c, character_t* other, enum direction direction)
{
    struct stickman* sm = malloc(sizeof(struct stickman));
    
    sm->character = c;
    sm->character->other = other;
    
    c->actions = ref_bind_stickman_actions(sm);
    c->draw = ref_bind_draw_stickman(sm);
    c->free = ref_bind_free_stickman(sm);
    
    make_heath_bar(&c->health_bar, direction);
    make_state_indicator(&c->state_indicator);
    load_shader_program(&sm->hit_effect, screenspace_vert, blast_frag);
    load_shader_program(&sm->parry_effect, particles_vert, color_frag);
    
    make_anim_obj(&sm->torso, stickman_torso_mesh);
    make_anim_obj(&sm->legs, stickman_legs_mesh);
    anim_obj_keys(&sm->legs, &stickman_legs_anims[0]);
    load_shader_program(&sm->program, anim_vert, color_frag);
    sm->color_unif = glGetUniformLocation(sm->program.program, "main_color");
    glUniform3f(sm->color_unif, 1., 1., 1.);
    
    c->direction = direction;
    c->states = states;
    c->hitbox_width = stickman_hitbox_width;
    c->next = (struct character_state){
        .ground_pos = -1.f,
        .health = 100,
        .advancing = 0,
    };
    c->buttons = no_key_events;
    goto_state(c, top);
    
    //Do nothing for one frame so the state is consistent
    step_character(c, &no_key_events);
    stickman_actions(sm);
}
