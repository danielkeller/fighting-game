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
    punch, kick, kick_recover,
    dodge_1, dodge_held, undodge, back_undodge,
    NUM_FATMAN_STATES
};

static const float speed = .02;
static const float flip_dist = .6;

//         balance   hi        lo
#define REST {4, {{0,  WEAK}, {6,  WEAK}}}

//                     frames  next             speed              fight 
static const struct state states[NUM_FATMAN_STATES] = {
    [still]             = {20, still,              0,             REST},
    [step_fwd_1]        = {6,  stop,               speed,         REST},
    [step_fwd]          = {6,  stop,               speed,         REST},
    [stop]              = {2,  still,              0,             REST},
    [step_back]         = {8,  still,             -speed,         REST},
    [backflip]          = {12, backflip_recover,  -flip_dist/12., REST},
    //maybe make backflip slower than walking?
    [backflip_recover]  = {8,  still,              0,             REST},
    [frontflip]         = {12, frontflip_recover,  flip_dist/12., REST},
    [frontflip_recover] = {8,  still,              0,             REST},
    [punch]             = {7,  still,              0,             REST},
    [kick]              = {14, kick_recover,       0,             REST},
    [kick_recover]      = {14, still,              0,             REST},
    [dodge_1]           = {13, dodge_held,        -.4/13.,        REST},
    [dodge_held]        = {1,  dodge_held,         0,             REST},
    [undodge]           = {6,  still,              .4/6.,         REST},
    [back_undodge]      = {12, still,              0,             REST},
};

static const frame_t cancel_frames[NUM_FATMAN_STATES] = {0};

static struct attack
//          frame target min- range damage knock force
punch_1_attack   = {1, HI, 0, .4, 5, 0, MIDDLE},
punch_2_attack   = {4, HI, 0, .4, 5, 5, MIDDLE},
kick_attack      = {9, LO, 0, .5, 10, 10, MIDDLE};

int fatman_actions(struct fatman* fm)
{
    character_t* c = fm->character;
    
    assert(cancel_frames[c->prev.state] <= states[c->prev.state].frames
           && "Cancel frame is after the end of the state");
    
    frame_t frame = game_time.frame - c->anim_start;
    int is_last_frame = frame == states[c->prev.state].frames;
    //int is_cancel_frame = frame == cancel_frames[c->prev.state];
    
    switch (c->prev.state) {
        case still:
            if (shift_button_press(&c->buttons.attack))
                goto_state(c, punch);
            else if (c->buttons.dodge.down)
                goto_state(c, dodge_1);
            else if (c->buttons.fwd.down)
                goto_state(c, step_fwd_1);
            else if (c->buttons.back.down && c->prev.ground_pos > -1.f)
                goto_state(c, step_back);
            break;
        case step_fwd_1:
        case step_fwd:
            if (!c->buttons.fwd.down)
                /*don't do anything, so the player can change the stick position*/;
            else if (shift_button_press(&c->buttons.attack))
                goto_state(c, punch);
            else if (shift_button_press(&c->buttons.special))
                goto_state(c, frontflip);
            else if (is_last_frame)
                goto_state(c, step_fwd);
            break;
        case back_undodge:
            if (is_last_frame && c->buttons.back.down && c->prev.ground_pos > -1.f)
                goto_state(c, step_back);
            break;
        case step_back:
            if (!c->buttons.back.down)
                /*don't do anything, so the player can change the stick position*/;
            else if (shift_button_press(&c->buttons.attack))
                goto_state(c, kick);
            else if (shift_button_press(&c->buttons.special))
                goto_state(c, backflip);
            else if (is_last_frame && c->prev.ground_pos > -1.f)
                goto_state(c, step_back);
            break;
        case dodge_1:
            if (c->buttons.attack.down || (!c->buttons.dodge.down && !c->buttons.back.down)) {
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
            if (c->buttons.attack.down || (!c->buttons.dodge.down && !c->buttons.back.down))
                goto_state(c, undodge);
            else if (c->buttons.back.down)
                goto_state(c, back_undodge);
            break;
        case punch:
            attack(c, &punch_1_attack);
            attack(c, &punch_2_attack);
            break;
        case kick:
            attack(c, &kick_attack);
            break;
        default:
            break;
    }
    
    next_state(c);
    move_character(c);
    
    return 0;
}
BINDABLE(fatman_actions, struct fatman)

static animation_t animations[NUM_FATMAN_STATES] = {
    [still] = &fatman_rest,
    [step_fwd] = &fatman_step_fwd, [step_back] = &fatman_step_back,
    [step_fwd_1] = &fatman_step_fwd1,
    [stop] = &fatman_stop,
    [dodge_1] = &fatman_sway_back, [dodge_held] = &fatman_sway_back,
    [undodge] = &fatman_sway_back, [back_undodge] = &fatman_sway_back,
    [backflip] = &fatman_backflip, [backflip_recover] = &fatman_backflip,
    [frontflip] = &fatman_frontflip, [frontflip_recover] = &fatman_frontflip,
    [punch] = &fatman_punch, [kick] = &fatman_kick, [kick_recover] = &fatman_kick,
};

int draw_fatman(struct fatman* fm)
{
    struct character* c = fm->character;
    
    int state = c->next.state;
    float frame = game_time.frame - c->anim_start + game_time.alpha;
    const struct animation* anim = animations[state];
    
    if (state == kick_recover)      frame += states[kick].frames;
    if (state == backflip_recover)  frame += states[backflip].frames;
    if (state == frontflip_recover) frame += states[frontflip].frames;
    if (state == back_undodge)      frame += states[dodge_1].frames;
    if (state == dodge_held)        frame = states[dodge_1].frames;
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
    character_t* c = fm->character;
    
    free_health_bar(&c->health_bar);
    free_state_indicator(&c->state_indicator);
    free_object(&fm->object);
    free_program(&fm->program);
    free_program(&fm->blur_program);
    
    free(fm);
    return 0;
}
BINDABLE(free_fatman, struct fatman)

void make_fatman(character_t* c, character_t* other, enum direction direction)
{
    struct fatman* fm = malloc(sizeof(struct fatman));
    
    fm->character = c;
    fm->character->other = other;
    
    c->actions = ref_bind_fatman_actions(fm);
    c->draw = ref_bind_draw_fatman(fm);
    c->free = ref_bind_free_fatman(fm);
    
    make_heath_bar(&c->health_bar, direction);
    make_state_indicator(&c->state_indicator);
    
    make_anim_obj(&fm->object, fatman);
    load_shader_program(&fm->program, anim_vert, color_frag);
    fm->color_unif = glGetUniformLocation(fm->program.program, "main_color");
    glUniform3f(fm->color_unif, 1., 1., 1.);
    load_shader_program(&fm->blur_program, stickman_blur_vert, stickman_blur_frag);
    fm->attacking_unif = glGetUniformLocation(fm->blur_program.program, "attacking");
    fm->ground_speed_unif = glGetUniformLocation(fm->blur_program.program, "ground_speed");
    
    c->direction = direction;
    c->states = states;
    c->hitbox_width = fatman_hitbox_width;
    c->buttons = no_key_events;
    c->prev = c->next = (struct character_state){
        .ground_pos = -1.f,
        .health = 100,
        .state = still,
    };
    goto_state(c, still);
}
