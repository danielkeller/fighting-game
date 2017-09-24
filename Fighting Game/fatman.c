//
//  fatman.c
//  Fighting Game
//
//  Created by Daniel Keller on 7/5/17.
//  Copyright © 2017 Daniel Keller. All rights reserved.
//

#include "character_internal.h"
#include "objects/fatman.h"
#include "engine.h"
#include <math.h>
#include <assert.h>

enum fatman_states {
    still, step_fwd, step_fwd_1, step_back, stop,
    backflip, backflip_recover,
    frontflip, frontflip_recover,
    punch, kick,
    lunge_1, lunge_2,
    dive_1, dive_2,
    dodge_1, dodge_held, undodge, back_undodge,
    NUM_FATMAN_STATES
};

static const float speed = .02;
static const float flip_dist = .6;

//         balance   hi           lo
#define REST  {6,  {{0,  WEAK},   {6,  WEAK}}}
#define AIR   {0,  {{5,  WEAK},   {0,  XHEAVY}}}
#define FLOOR {12, {{0,  XHEAVY}, {5,  WEAK}}}

//                     frames  next             speed              fight 
static const struct state states[NUM_FATMAN_STATES] = {
    [still]             = {20, still,              0,             REST},
    [step_fwd_1]        = {6,  stop,               speed,         REST},
    [step_fwd]          = {6,  stop,               speed,         REST},
    [stop]              = {2,  still,              0,             REST},
    [step_back]         = {8,  still,             -speed,         REST},
    [backflip]          = {12, backflip_recover,  -flip_dist/12., AIR},
    //maybe make backflip slower than walking?
    [backflip_recover]  = {8,  still,              0,             REST},
    [frontflip]         = {12, frontflip_recover,  flip_dist/12., AIR},
    [frontflip_recover] = {8,  still,              0,             REST},
    [punch]             = {7,  still,              0,             {6, {{10, HEAVY}, {6, WEAK}}}},
    [kick]              = {9,  still,              0,             {6, {{0, WEAK},   {15, HEAVY}}}},
    [lunge_1]           = {5,  lunge_2,            .7/5.,         AIR},
    [lunge_2]           = {9,  still,              0,             REST},
    [dive_1]            = {10, dive_2,             .04,           FLOOR},
    [dive_2]            = {10, still,              0,             FLOOR},
    [dodge_1]           = {8,  dodge_held,        -.4/8.,         REST},
    [dodge_held]        = {1,  dodge_held,         0,             REST},
    [undodge]           = {5,  still,              .4/5.,         REST},
    [back_undodge]      = {12, still,              0,             REST},
};

static const frame_t cancel_frames[NUM_FATMAN_STATES] = {0};

static struct attack
//          frame target min- range damage knock force
punch_1_attack   = {1, HI, 0, .4, 5,  0,  MIDDLE},
punch_2_attack   = {4, HI, 0, .4, 5,  5,  MIDDLE},
kick_attack      = {5, LO, 0, .5, 6,  10, MIDDLE},
lunge_attack     = {3, HI, 0, .4, 12, 15, HEAVY},
dive_attack      = {9, LO, 0, .9, 10, 20, HEAVY},
flip_attack      = {9, HI, 0, .4, 2,  20, HEAVY};

int fatman_actions(struct fatman* fm)
{
    struct character* c = fm->character;
    
    assert(cancel_frames[c->prev.state] <= states[c->prev.state].frames
           && "Cancel frame is after the end of the state");
    
    int direction = c->buttons.fwd.down ? 1 : c->buttons.back.down ? -1 : 0;
    frame_t frame = game_time.frame - c->anim_start;
    int is_last_frame = frame == states[c->prev.state].frames;
    //int is_cancel_frame = frame == cancel_frames[c->prev.state];
    
    switch (c->prev.state) {
        case still:
        case step_fwd_1:
        case step_fwd:
        case step_back:
            if (shift_button_press(&c->buttons.attack)) {
                if (c->buttons.fwd.down)
                    goto_state(c, punch);
                else if (c->buttons.back.down)
                    goto_state(c, dive_1);
                else
                    goto_state(c, kick);
            }
            else if (shift_button_press(&c->buttons.special)) {
                if (c->buttons.fwd.down)
                    goto_state(c, frontflip);
                else if (c->buttons.back.down && c->prev.ground_pos > -1.f)
                    goto_state(c, backflip);
            }
            break;
        default:
            //Dequeue these actions if the stick direction changed. This prevents
            //a confusing result e.g. if the user double-taps a button then moves
            //the stick.
            if (direction != fm->last_direction) {
                shift_button_press(&c->buttons.attack);
                shift_button_press(&c->buttons.special);
            }
            break;
    }
    
    //Use next state to respect the changes made above. If that didn't happen,
    //c->next.state would be equal to c->prev.state
    switch (c->next.state) {
        case still:
            if (c->buttons.dodge.down && c->prev.ground_pos > -1.f)
                goto_state(c, dodge_1);
            else if (c->buttons.fwd.down)
                goto_state(c, step_fwd_1);
            else if (c->buttons.back.down && c->prev.ground_pos > -1.f)
                goto_state(c, step_back);
            break;
        case step_fwd_1:
        case step_fwd:
            if (is_last_frame && c->buttons.fwd.down)
                goto_state(c, step_fwd);
            break;
        case back_undodge:
            if (is_last_frame && c->buttons.back.down && c->prev.ground_pos > -1.f)
                goto_state(c, step_back);
            break;
        case step_back:
            if (is_last_frame && c->buttons.back.down && c->prev.ground_pos > -1.f)
                goto_state(c, step_back);
            break;
        case dodge_1:
            if (!c->buttons.dodge.down && !c->buttons.back.down) {
                goto_state(c, undodge);
                float progress = (float)frame / states[dodge_1].frames;
                float rev_progress = (1.f - progress) * states[undodge].frames;
                //back up the starting point, since we start in the middle
                c->anim_start -= rev_progress - .5f;
                if (game_time.frame - c->anim_start >= states[undodge].frames)
                    goto_state(c, still);
            }
            break;
        case dodge_held:
            if (shift_button_press(&c->buttons.attack))
                goto_state(c, lunge_1);
            else if (c->buttons.back.down)
                goto_state(c, back_undodge);
            else if (!c->buttons.dodge.down)
                goto_state(c, undodge);
            break;
        case punch:
            attack(c, &punch_1_attack);
            attack(c, &punch_2_attack);
            break;
        case kick:
            attack(c, &kick_attack);
            break;
        case lunge_2:
            attack(c, &lunge_attack);
            break;
        case dive_1:
            attack(c, &dive_attack);
            break;
        case frontflip:
            attack(c, &flip_attack);
            break;
        default:
            break;
    }
    
    fm->last_direction = direction;
    
    next_state(c);
    move_character(c);
    
    return 0;
}
BINDABLE(fatman_actions, struct fatman)

static animation_t animations[NUM_FATMAN_STATES] = {
    [still] = &fatman_still,
    [step_fwd] = &fatman_step_fwd, [step_back] = &fatman_step_back,
    [step_fwd_1] = &fatman_step_fwd1,
    [stop] = &fatman_stop,
    [dodge_1] = &fatman_sway_back, [dodge_held] = &fatman_sway_back,
    [undodge] = &fatman_sway_back, [back_undodge] = &fatman_sway_back,
    [backflip] = &fatman_backflip, [backflip_recover] = &fatman_backflip,
    [frontflip] = &fatman_frontflip, [frontflip_recover] = &fatman_frontflip,
    [punch] = &fatman_punch, [kick] = &fatman_kick,
    [lunge_1] = &fatman_lunge, [lunge_2] = &fatman_lunge,
    [dive_1] = &fatman_dive, [dive_2] = &fatman_dive,
};

int draw_fatman(struct fatman* fm)
{
    struct character* c = fm->character;
    
    int state = c->next.state;
    float frame = game_time.frame - c->anim_start + game_time.alpha;
    const struct animation* anim = animations[state];
    
    if (state == backflip_recover)  frame += states[backflip].frames;
    if (state == frontflip_recover) frame += states[frontflip].frames;
    if (state == back_undodge)      frame += states[dodge_1].frames;
    if (state == dodge_held)        frame = states[dodge_1].frames;
    if (state == lunge_2)           frame += states[lunge_1].frames;
    if (state == dive_2)            frame += states[dive_1].frames;
    if (state == undodge) {
        //play backwards from the middle
        frame *= (float)states[dodge_1].frames / states[undodge].frames;
        frame = fatman_sway_back.length - states[back_undodge].frames - frame;
    }
    
    draw_health_bar(c);
    
    flip_fbo(&fbo);
    glUseProgram(fm->blur_program.program);
    set_character_draw_state(c, &fm->blur_program, fatman, anim, frame);
    glUniform1f(fm->ground_speed_unif, c->next.ground_pos - c->prev.ground_pos);
    draw_blur_object(&fm->object);
    
    glUseProgram(fm->program.program);
    set_character_draw_state(c, &fm->program, fatman, anim, frame);
    draw_object(&fm->object);
    
    if (learning_mode)
        draw_state_indicator(c);
    return 0;
}
BINDABLE(draw_fatman, struct fatman)

int free_fatman(struct fatman* fm)
{
    struct character* c = fm->character;
    
    free_health_bar(&c->health_bar);
    free_state_indicator(&c->state_indicator);
    free_object(&fm->object);
    free_program(&fm->program);
    free_program(&fm->blur_program);
    
    free(fm);
    return 0;
}
BINDABLE(free_fatman, struct fatman)

void make_simulation_fatman(struct fatman* fm, character_t* c, character_t* other)
{
    fm->character = c;
    fm->character->other = other;
    c->actions = ref_bind_fatman_actions(fm);
    
    c->states = states;
    c->hitbox_width = fatman_hitbox_width;
    c->buttons = no_key_events;
    c->prev = c->next = (struct character_state){
        .ground_pos = -1.f,
        .health = 100,
        .state = still,
    };
    goto_state(c, still);
    fm->last_direction = 0;
}

void make_fatman(character_t* c, character_t* other, enum direction direction)
{
    struct fatman* fm = malloc(sizeof(struct fatman));
    
    make_simulation_fatman(fm, c, other);
    
    c->draw = ref_bind_draw_fatman(fm);
    c->free = ref_bind_free_fatman(fm);
    
    c->direction = direction;
    
    make_heath_bar(&c->health_bar, direction);
    make_state_indicator(&c->state_indicator);
    
    make_anim_obj(&fm->object, fatman);
    load_shader_program(&fm->program, anim_vert, color_frag);
    fm->color_unif = glGetUniformLocation(fm->program.program, "main_color");
    glUniform3f(fm->color_unif, 1., 1., 1.);
    load_shader_program(&fm->blur_program, stickman_blur_vert, stickman_blur_frag);
    fm->attacking_unif = glGetUniformLocation(fm->blur_program.program, "attacking");
    fm->ground_speed_unif = glGetUniformLocation(fm->blur_program.program, "ground_speed");
}
