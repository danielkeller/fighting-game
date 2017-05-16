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

enum stickman_states {
    top, bottom, overhead, forward, block,
    swing, swingup,
    lift, unlift, big_swing_1, big_swing_2,
    lunge, unlunge, forward_pause,
    poke, drop,
    hi_block, lo_block, hi_unblock, lo_unblock,
};

static const struct anim_sequence torso_sequences[] = {
    [top] = {{{1, &torso_null_Top}}},
    [bottom] = {{{1, &torso_null_Bottom}}},
    [swing] = {{{3, &torso_swing_1}, {2, &torso_swing_2}, {2, &torso_swing_3}}},
    [swingup] = {{{5, &torso_swingup_1}, {4, &torso_swingup_2}}},
    
    [lift] = {{{4, &torso_lift_1}, {6, &torso_lift_2}}},
    [overhead] = {{{1, &torso_null_Overhead}}},
    [unlift] = {{{2, &torso_lift_2}, {4, &torso_lift_1}}, REVERSED},
    [big_swing_1] = {{{3, &torso_big_swing_1}, {1, &torso_big_swing_2}}},
    [big_swing_2] = {{{1, &torso_big_swing_3}, {3, &torso_big_swing_4}}},
    
    [lunge] = {{{4, &torso_lunge}}},
    [forward_pause] = {{{1, &torso_null_Forward}}},
    [forward] = {{{1, &torso_null_Forward}}},
    [unlunge] = {{{5, &torso_lunge}}, REVERSED},
    [poke] = {{{2, &torso_poke_1}, {2, &torso_poke_2}}},
    [drop] = {{{4, &torso_drop_1}, {8, &torso_drop_2}, {6, &torso_drop_3}, {6, &torso_drop_4}, {6, &torso_drop_5}}},
    
    [block] = {{{6, &torso_null_Block}}},
    [hi_block] = {{{3, &torso_hi_block}}},
    [lo_block] = {{{2, &torso_lo_block_1}, {1, &torso_lo_block_2}, {2, &torso_lo_block_3}}},
    [hi_unblock] = {{{5, &torso_hi_block}}, REVERSED},
    [lo_unblock] = {{{2, &torso_lo_block_3}, {1, &torso_lo_block_2}, {2, &torso_lo_block_1}}, REVERSED}
};

//For attacks that give defense/momentum buffs, used symmetrically:
//If the attack is before the midpoint of the animation, advantage goes to the
//first player. After the midpoint, it goes to the second. If it is at the midpoint
//either both attacks land or neither.

//To make dodging make sense, the player must be stationary or slow during attacks

//            frames  next  fwd_speed rev_speed    balance   hi        lo
static const struct state states[] = {
    [top]     = {1, top,    speed, rev_speed, {10, {{8,  MIDDLE}, {0,  WEAK}}}},
    [bottom]  = {1, bottom, speed, rev_speed, {8,  {{0,  WEAK},   {8,  MIDDLE}}}},
    [swing]   = {7, bottom, 0,     0,         {5,  {{10, HEAVY},  {0,  WEAK}}}},
    [swingup] = {9, top,    0,     0,         {20, {{0,  WEAK},   {10, HEAVY}}}},
    
#define UNSTEADY                              {4,  {{0, WEAK},    {0, WEAK}}}
    [lift]        = {10, overhead,    0,         0,             UNSTEADY},
    [unlift]      = {6,  top,         0,         0,             UNSTEADY},
    [overhead]    = {1,  overhead,    speed*.75, rev_speed*.75, UNSTEADY},
    [big_swing_1] = {4,  big_swing_2, 0,         0,             UNSTEADY},
    [big_swing_2] = {4,  bottom,      0,         0, {5,  {{10, HEAVY},  {0,  WEAK}}}},
    
#define REACHING                              {10, {{8,  WEAK}, {8,  WEAK}}}
    [lunge]         = {4,  forward_pause, speed*20./4., 0,             REACHING},
    [forward_pause] = {6,  forward,       0,            0,             REACHING},
    [forward]       = {1,  forward,       speed*.75,    rev_speed*.75, REACHING},
    [poke]          = {4,  forward_pause, 0,            0,             REACHING},
    [unlunge]       = {5,  bottom,        0,            block_dist/5., REACHING},
    //Knockback would look weird here
    [drop]          = {30, top,           0,            0, {50,  {{0, WEAK},    {0, WEAK}}}},
    
#define BLOCKED                               {10, {{20, HEAVY},  {20, HEAVY}}}
#define UNBLOCKED                             {10,  {{0, WEAK},    {0, WEAK}}}
    [hi_block]   = {3, block,      0, block_dist/2., BLOCKED},
    [lo_block]   = {5, block,      0, block_dist/3., BLOCKED},
    [block]      = {6, hi_unblock, 0, 0,             BLOCKED},
    [hi_unblock] = {5, top,        0, 0,             UNBLOCKED},
    [lo_unblock] = {5, bottom,     0, 0,             UNBLOCKED},
};

//Attacks can be in cancellable states, but *only* on the last frame. Otherwise
//the player can cancel the action after the attack comes out.

static struct attack
//          frame target min- range damage knock force
down_attack     = {6, HI, .2, .6, 14, 15, MIDDLE},
up_attack       = {5, LO, .2, .6, 10, 30, MIDDLE},
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
    character_t* c = sm->character;
    
    move_character(c);
    
    int can_cancel = CANCELLING && game_time.frame - c->anim_start <= 3;
    int is_last_frame = game_time.frame - c->anim_start >= states[c->prev.state].frames;
    
    //Remember not to call shift_button_press until we're really ready to use the input
    
    switch (c->prev.state) {
        case top:
            if (shift_button_press(&c->buttons.dodge))
                goto_state(c, hi_block);
            else if (shift_button_press(&c->buttons.attack))
                goto_state(c, swing);
            else if (shift_button_press(&c->buttons.special))
                goto_state(c, lift);
            break;
        case swing:
            if (can_cancel && shift_button_press(&c->buttons.dodge))
                goto_state(c, hi_block);
            else if (can_cancel && !c->buttons.attack.down)
                goto_state(c, top);
            attack(c, &down_attack);
            if (c->next.attack_result & PARRIED) push_effect(&effects, make_parry_effect(sm, .6));
            break;
            
        case bottom:
            if (shift_button_press(&c->buttons.dodge))
                goto_state(c, lo_block);
            else if (shift_button_press(&c->buttons.attack))
                goto_state(c, swingup);
            else if (shift_button_press(&c->buttons.special))
                goto_state(c, lunge);
            break;
        case swingup:
            if (can_cancel && shift_button_press(&c->buttons.dodge))
                goto_state(c, lo_block);
            else if (can_cancel && !c->buttons.attack.down)
                goto_state(c, bottom);
            attack(c, &up_attack);
            if (c->next.attack_result & PARRIED) push_effect(&effects, make_parry_effect(sm, .7));
            break;
            
        case lift:
            if (can_cancel && !c->buttons.special.down)
                goto_state(c, top);
            break;
        case overhead:
            if (shift_button_press(&c->buttons.attack))
                goto_state(c, big_swing_1);
            else if (shift_button_press(&c->buttons.special) || c->buttons.dodge.down)
                goto_state(c, unlift);
            break;
        case unlift:
            //don't dodge if the player let go of the button
            if (!c->buttons.dodge.down)
                shift_button_press(&c->buttons.dodge);
            break;
        case big_swing_1:
            if (can_cancel && !c->buttons.attack.down)
                goto_state(c, top);
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
            //fall thru
        case forward_pause:
            if (c->other->prev.attack_result & (KNOCKED | LANDED))
                goto_state(c, drop);
            break;
        case poke:
            attack(c, &poke_attack);
            break;
        case unlunge:
            //don't dodge if the player let go of the button
            if (!c->buttons.dodge.down)
                shift_button_press(&c->buttons.dodge);
            break;
            
        case lo_block:
            if (can_cancel && !c->buttons.dodge.down)
                goto_state(c, lo_unblock);
            break;
        
        case hi_block:
            if (can_cancel && !c->buttons.dodge.down)
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
    
    set_character_draw_state(sm->character, &sm->program, &sm->torso, &torso_sequences[sm->character->next.state]);
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
    anim_obj_keys(&sm->legs, &legs_walk);
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
