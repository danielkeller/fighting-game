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
    still, step_fwd, step_fwd_1, step_back, sway_fwd, sway_back,
    backflip, backflip_recover,
    NUM_FATMAN_STATES
};

static const float speed = .02;
static const float backflip_dist = .6;

//            frames  next  fwd_speed rev_speed    balance   hi        lo
static const struct state states[NUM_FATMAN_STATES] = {
#define REST                     {4, {{0,  WEAK}, {6,  WEAK}}}
    [still]      = {20, still,     0,     REST},
    [step_fwd_1] = {6,  sway_fwd,  speed, REST},
    [step_fwd]   = {6,  sway_fwd,  speed, REST},
    [sway_fwd]   = {15, still,     0,     REST},
    [step_back]  = {8,  still,    -speed, REST},
    [sway_back]  = {20, still,     0,     REST},
    [backflip]   = {12, backflip_recover, -backflip_dist/12., REST},
    [backflip_recover] = {8, still, 0,    REST},
};

static const frame_t cancel_frames[NUM_FATMAN_STATES] = {0};

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
            if (c->buttons.fwd.down)
                goto_state(c, step_fwd_1);
            if (c->buttons.back.down)
                goto_state(c, step_back);
            if (shift_button_press(&c->buttons.dodge))
                goto_state(c, sway_back);
            //if (c->prev.advancing && !c->next.advancing)
            //    goto_state(c, sway_fwd);
            break;
        case step_fwd_1:
        case step_fwd:
            if (is_last_frame && shift_button_press(&c->buttons.dodge))
                goto_state(c, backflip);
            else if (is_last_frame && c->buttons.fwd.down)
                goto_state(c, step_fwd);
            break;
        case step_back:
            if (is_last_frame && c->buttons.back.down)
                goto_state(c, step_back);
            break;
        case sway_fwd:
            if (is_last_frame && shift_button_press(&c->buttons.dodge))
                goto_state(c, sway_back);
            if (is_last_frame && c->buttons.back.down)
                goto_state(c, step_back);
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
    [still] = &fatman_still,
    [step_fwd] = &fatman_step_fwd, [step_back] = &fatman_step_back,
    [step_fwd_1] = &fatman_step_fwd1,
    [sway_fwd] = &fatman_sway_fwd, [sway_back] = &fatman_sway_back,
    [backflip] = &fatman_backflip, [backflip_recover] = &fatman_backflip_recover,
};

int draw_fatman(struct fatman* fm)
{
    struct character* c = fm->character;
    
    int state = c->next.state;
    float frame = game_time.frame - c->anim_start + game_time.alpha;
    const struct animation* anim = animations[state];
    
    draw_health_bar(c);
    
    flip_fbo(&fbo);
    glUseProgram(fm->blur_program.program);
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
