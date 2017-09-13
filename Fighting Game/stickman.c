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
#include <math.h>
#include <assert.h>

enum stickman_states {
    top, bottom, overhead, forward, block,
    top_step, bottom_step,
    swing, swingup,
    lift, unlift, big_swing_1, big_swing_2,
    lunge, unlunge, lunge_recover,
    poke, drop, poke_recover,
    hi_block, lo_block, hi_dodge, lo_dodge,
    hi_unblock, lo_unblock,
    NUM_STICKMAN_STATES
};

static const float speed = .015;
static const float dodge_dist = .3;

#if FAST_ATTACK
#define UP_FRAMES 7
#define DN_FRAMES 9
#else
#define DN_FRAMES 12
#define UP_FRAMES 13
#endif

//For attacks that give defense/momentum buffs, used symmetrically:
//If the attack is before the midpoint of the animation, advantage goes to the
//first player. After the midpoint, it goes to the second. If it is at the midpoint
//either both attacks land or neither.

//To make dodging make sense, the player must be stationary or slow during attacks

//                frames  next  speed   balance   hi        lo
static const struct state states[NUM_STICKMAN_STATES] = {
    [top]         = {40,    top,        0, {10, {{8,  MIDDLE}, {0,  WEAK}}}},
    [top_step]    = {7,     top,    speed, {10, {{8,  MIDDLE}, {0,  WEAK}}}},
    [bottom]      = {40,    bottom,     0, {8,  {{0,  WEAK},   {8,  MIDDLE}}}},
    [bottom_step] = {7,     bottom, speed, {8,  {{0,  WEAK},   {8,  MIDDLE}}}},
    [swing]       = {DN_FRAMES, bottom, 0, {5,  {{10, HEAVY},  {0,  WEAK}}}},
    [swingup]     = {DN_FRAMES, top,    0, {20, {{0,  WEAK},   {10, HEAVY}}}},
    
#define UNSTEADY                           {4,  {{0, WEAK},    {0, WEAK}}}
    [lift]        = {10,   overhead,    0, UNSTEADY},
    [unlift]      = {6,    top,         0, UNSTEADY},
    [overhead]    = {40,   overhead,    0, UNSTEADY},
    [big_swing_1] = {4,    big_swing_2, 0, UNSTEADY},
    [big_swing_2] = {7,    bottom,      0, {5,  {{10, HEAVY},  {0, WEAK}}}},

//Can't have knockback here, otherwise the drop-punish is too weak
#define REACHING                             {50,  {{0,  WEAK}, {0,  WEAK}}}
    [lunge]         = {4,  lunge_recover, speed*20./4., REACHING},
    [lunge_recover] = {6,  forward,       0,            REACHING},
    [forward]       = {40, forward,       0,            REACHING},
    [poke]          = {4,  poke_recover,  0,            REACHING},
    [poke_recover]  = {6,  forward,       0,            REACHING},
    [unlunge]       = {5,  bottom,        0,            REACHING},
    //Knockback would look weird here
    [drop]          = {30, top,           0, {50,  {{0, WEAK},    {0, WEAK}}}},
    
#define BLOCKED                              {10, {{20, HEAVY},  {20, HEAVY}}}
#define UNBLOCKED                            {10,  {{0, WEAK},    {0, WEAK}}}
    [hi_block]   = {3, block,      0,              BLOCKED},
    [lo_block]   = {5, block,      0,              BLOCKED},
    [hi_dodge]   = {3, block,      -dodge_dist/3., BLOCKED},
    [lo_dodge]   = {5, block,      -dodge_dist/5., BLOCKED},
    [block]      = {6, hi_unblock, 0,              BLOCKED},
    [hi_unblock] = {5, top,        0,              UNBLOCKED},
    [lo_unblock] = {5, bottom,     0,              UNBLOCKED},
};

static const frame_t cancel_frames[NUM_STICKMAN_STATES] = {
    [swing] = 4, [swingup] = 6,
    [lo_block] = 3, [hi_block] = 3,
    [lift] = 5, [big_swing_1] = 4,
};

//Attacks should obviously come after the cancel frame.

static struct attack
//          frame target min- range damage knock force
down_attack     = {6, HI, .2, .6, 14, 15, MIDDLE},
up_attack       = {7, LO, .2, .6, 10, 30, MIDDLE},
overhead_attack = {1, HI, .2, .6, 20, 20, HEAVY},
lunge_attack    = {4, LO, .4, .8, 10, 0,  MIDDLE},
poke_attack     = {2, LO, .7, .8, 8,  0,  LIGHT};

//Note that new actions are considered to start epsilon after the previous frame,
//as opposed to on the start of the next frame. (the previous frame is stored in anim_start)
//So the states and transitions for a 2-frame state look like this:
//       _____#-----=-----=.....@
// anim_start ^  +1 ^  +2 ^
// where '=' are calls to _actions() in the '-' state

int stickman_actions(struct stickman* sm)
{
    struct character* c = sm->character;
    
    assert(cancel_frames[c->prev.state] <= states[c->prev.state].frames
           && "Cancel frame is after the end of the state");
    
    frame_t frame = game_time.frame - c->anim_start;
    int is_last_frame = frame == states[c->prev.state].frames;
    int is_cancel_frame = frame == cancel_frames[c->prev.state];
    
    //Remember not to call shift_button_press until we're really ready to use the input
    
    switch (c->prev.state) {
        case top:
            if (c->buttons.fwd.down)
                goto_state(c, top_step);
        case top_step:
            if (shift_button_press(&c->buttons.dodge))
                goto_state(c, c->buttons.back.down ? hi_dodge : hi_block);
            else if (shift_button_press(&c->buttons.attack))
                goto_state(c, swing);
            else if (shift_button_press(&c->buttons.special))
                goto_state(c, lift);
            break;
        case swing:
            if (is_cancel_frame && shift_button_press(&c->buttons.dodge))
                goto_state(c, c->buttons.back.down ? hi_dodge : hi_block);
            else if (is_cancel_frame && shift_button_cancel(&c->buttons.attack))
                goto_state(c, top);
            attack(c, &down_attack);
            if (c->next.attack_result & PARRIED) push_effect(&effects, make_parry_effect(sm, .6));
            break;
            
        case bottom:
            if (c->buttons.fwd.down)
                goto_state(c, bottom_step);
        case bottom_step:
            if (shift_button_press(&c->buttons.dodge))
                goto_state(c, c->buttons.back.down ? lo_dodge : lo_block);
            else if (shift_button_press(&c->buttons.attack))
                goto_state(c, swingup);
            else if (shift_button_press(&c->buttons.special))
                goto_state(c, lunge);
            break;
        case swingup:
            if (is_cancel_frame && shift_button_press(&c->buttons.dodge))
                goto_state(c, c->buttons.back.down ? lo_dodge : lo_block);
            else if (is_cancel_frame && shift_button_cancel(&c->buttons.attack))
                goto_state(c, bottom);
            attack(c, &up_attack);
            if (c->next.attack_result & PARRIED) push_effect(&effects, make_parry_effect(sm, .7));
            break;

        case lift:
            if (is_cancel_frame && (shift_button_cancel(&c->buttons.special) || c->buttons.dodge.down))
                goto_state(c, top);
            break;
        case overhead:
            if (shift_button_press(&c->buttons.attack))
                goto_state(c, big_swing_1);
            else if (shift_button_press(&c->buttons.special) || c->buttons.dodge.down)
                goto_state(c, unlift);
            break;
        
        case big_swing_1:
            if (is_cancel_frame && shift_button_cancel(&c->buttons.attack))
                goto_state(c, overhead);
            break;
        case big_swing_2:
            attack(c, &overhead_attack);
            if (c->next.attack_result & LANDED)  push_effect(&effects, make_hit_effect(sm));
            if (c->next.attack_result & PARRIED) push_effect(&effects, make_parry_effect(sm, .7));
            break;
            
        case lunge:
            //Lunge isn't cancellable, otherwise it would be a "run" technique
            attack(c, &lunge_attack);
            break;
            
        case forward:
            if (shift_button_press(&c->buttons.special) || c->buttons.dodge.down)
                goto_state(c, unlunge);
            else if (shift_button_press(&c->buttons.attack))
                goto_state(c, poke);
            //fall thru to other state with drop effect
        case lunge_recover:
        case poke_recover:
            if (c->other->prev.attack_result & (KNOCKED | LANDED))
                goto_state(c, drop);
            break;
            
        case poke:
            attack(c, &poke_attack);
            break;
            
        case unlift:
        case unlunge:
            //don't begin dodging if the player dodge-cancelled from a special state
            if (shift_button_cancel(&c->buttons.dodge))
                shift_button_press(&c->buttons.dodge);
            break;
            
        case lo_block:
            if (is_cancel_frame && shift_button_cancel(&c->buttons.dodge))
                goto_state(c, lo_unblock);
            break;
        
        case hi_block:
            if (is_cancel_frame && shift_button_cancel(&c->buttons.dodge))
                goto_state(c, hi_unblock);
            break;
            
        case block:
            if (is_last_frame && shift_button_press(&c->buttons.attack))
                goto_state(c, lo_unblock);
            break;
            
        default:
            break;
    }
    
    next_state(c);
    move_character(c);
    
    return 0;
}
BINDABLE(stickman_actions, struct stickman)

static animation_t animations[NUM_STICKMAN_STATES] = {
    [top] = &stickman_top, [bottom] = &stickman_bottom,
    [top_step] = &stickman_step_top, [bottom_step] = &stickman_step_bottom,
    [swing] = &stickman_swing, [swingup] = &stickman_swing_up,
    [lift] = &stickman_lift, [overhead] = &stickman_overhead, [unlift] = &stickman_unlift,
    [big_swing_1] = &stickman_big_swing, [big_swing_2] = &stickman_big_swing,
    [hi_block] = &stickman_hi_block, [lo_block] = &stickman_lo_block,
    [hi_dodge] = &stickman_hi_dodge, [lo_dodge] = &stickman_lo_dodge,
    [block] = &stickman_block, [hi_unblock] = &stickman_hi_unblock, [lo_unblock] = &stickman_lo_unblock,
    [lunge] = &stickman_lunge, [lunge_recover] = &stickman_lunge, [unlunge] = &stickman_unlunge,
    [forward] = &stickman_forward,
    [poke] = &stickman_poke, [poke_recover] = &stickman_forward, [drop] = &stickman_drop,
};

//'frame' is also the number of the previous frame here.
int draw_stickman(struct stickman* sm)
{
    struct character* c = sm->character;
    int state = c->next.state;
    
    draw_health_bar(c);
    
    const struct animation* anim = animations[state];

    float frame = game_time.frame - c->anim_start + game_time.alpha;
    if (state == big_swing_2)
        frame += states[big_swing_1].frames;
    else if (state == lunge_recover)
        frame += states[lunge].frames;
    else if (anim == &stickman_top || anim == &stickman_bottom
             || anim == &stickman_overhead || anim == &stickman_forward)
        frame /= 10.;
    
    flip_fbo(&fbo);
    glUseProgram(sm->blur_program.program);
    set_character_draw_state(c, &sm->blur_program, stickman, anim, frame);
    if (state == swing || state == swingup || state == big_swing_1 || state == big_swing_2)
        glUniform1i(sm->attacking_unif, 1);
    else
        glUniform1i(sm->attacking_unif, 0);
    glUniform1f(sm->ground_speed_unif, c->next.ground_pos - c->prev.ground_pos);
    
    draw_blur_object(&sm->object);
    
    glUseProgram(sm->program.program);
    set_character_draw_state(c, &sm->program, stickman, anim, frame);
    draw_object(&sm->object);

    if (learning_mode)
        draw_state_indicator(c);
    return 0;
}
BINDABLE(draw_stickman, struct stickman)

int free_stickman(struct stickman* sm)
{
    struct character* c = sm->character;
    
    free_health_bar(&c->health_bar);
    free_state_indicator(&c->state_indicator);
    free_program(&sm->hit_effect);
    free_program(&sm->parry_effect);
    free_object(&sm->object);
    free_program(&sm->program);
    free_program(&sm->blur_program);
    
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
    
    make_anim_obj(&sm->object, stickman);
    load_shader_program(&sm->program, anim_vert, color_frag);
    sm->color_unif = glGetUniformLocation(sm->program.program, "main_color");
    glUniform3f(sm->color_unif, 1., 1., 1.);
    load_shader_program(&sm->blur_program, stickman_blur_vert, stickman_blur_frag);
    sm->attacking_unif = glGetUniformLocation(sm->blur_program.program, "attacking");
    sm->ground_speed_unif = glGetUniformLocation(sm->blur_program.program, "ground_speed");
    
    c->direction = direction;
    c->states = states;
    c->hitbox_width = stickman_hitbox_width;
    c->buttons = no_key_events;
    c->prev = c->next = (struct character_state){
        .ground_pos = -1.f,
        .health = 100,
        .state = top,
    };
    goto_state(c, top);
}
