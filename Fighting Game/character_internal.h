//
//  character_internal.h
//  Fighting Game
//
//  Created by Daniel Keller on 4/27/17.
//  Copyright © 2017 Daniel Keller. All rights reserved.
//

#ifndef character_internal_h
#define character_internal_h
#include "character.h"
#include "types.h"
#include "function.h"
#include "options.h"

enum force {
    WEAK, LIGHT, MIDDLE, HEAVY, XHEAVY
};

enum attack_result {
    WHIFFED = 1<<0,
    LANDED  = 1<<1,
    PARRIED = 1<<2,
    KNOCKED = 1<<3,
};

enum strike_position {
    HI, LO,
    NUM_STRIKE_POINTS
};

struct strike_point {
    int defense;
    enum force block;
};

struct fight_state {
    int balance;
    struct strike_point strike_points[NUM_STRIKE_POINTS];
};

struct state
{
    int frames;
    int next_state;
    float fwd_speed, rev_speed;
    struct fight_state fight_state;
};

struct health_bar {
    struct program program;
    GLint health_unif, last_health_unif, time_since_change_unif;
    int health, last_health;
    usec_t last_health_change_time;
};

struct state_indicator {
    struct program program;
    GLint top_unif, bot_unif, top_attack_unif, bot_attack_unif;
    enum force attack[NUM_STRIKE_POINTS];
    usec_t last_attack_time;
};

struct character_state {
    int state;
    float ground_pos;
    int advancing;
    struct fight_state fight_state;
    enum attack_result attack_result;
    int health;
};

typedef struct character {
    bound_t actions, draw, free;
    
    const struct state* states;
    float hitbox_width;
    
    enum direction direction;
    struct character* other;
    struct health_bar health_bar;
    struct state_indicator state_indicator;
    
    struct key_events buttons;
    long long anim_start;
    struct character_state prev, next;
    float ground_pos;
} character_t;

struct attack {
    //The frame of the attack can never be 0. This is intentional: at frame 0 of a
    //state, only next.state equals that state, and attacks are resolved against prev.
    //To put an attack on a state transition, put it at the end of the previous state.
    int frame;
    enum strike_position target;
    float min_range, range;
    int damage, knock;
    enum force force;
};

struct anim_sequence_step {
    int num_frames;
    const struct anim_step* anim_step;
};

#define MAX_ANIMS_PER_STATE 5
#define REVERSED 1

struct anim_sequence {
    //The extra one is needed to detect the end
    struct anim_sequence_step steps[MAX_ANIMS_PER_STATE+1];
    int reversed;
};

int shift_button_press(struct button*);

void goto_state(character_t *c, int state);
void next_state(character_t *c);

void move_character(character_t* c);
void attack(character_t* attacker, struct attack* attack);

void set_character_draw_state(character_t* c, struct program*, struct object*, const struct anim_sequence* current_sequence);
void set_character_legs_draw_state(character_t* c, struct program* program, float step_length);

void make_heath_bar(struct health_bar* hb, enum direction direction);
void draw_health_bar(character_t *c);
void free_health_bar(struct health_bar* hb);

void make_state_indicator(struct state_indicator*);
void draw_state_indicator(character_t*);
void free_state_indicator(struct state_indicator*);

//*** Stickman
struct stickman {
    character_t* character;
    struct program program;
    struct object torso, legs;
    GLint color_unif;
    struct program hit_effect, parry_effect;
};

static const float stickman_hitbox_width = .15;

typedef struct bound bound_t;
bound_t make_hit_effect(struct stickman* sm);
bound_t make_parry_effect(struct stickman* sm, float y);

#endif /* character_internal_h */
