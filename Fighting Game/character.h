//
//  character.h
//  Fighting Game
//
//  Created by Daniel Keller on 4/20/17.
//  Copyright © 2017 Daniel Keller. All rights reserved.
//

#ifndef character_h
#define character_h

typedef struct character character_t;
struct button;
struct key_events;

enum direction {
    LEFT = -1,
    RIGHT = 1
};

character_t* alloc_character();
int step_character(character_t*, struct key_events* keys);
void character_actions(character_t*);
void draw_character(character_t*);
//Must clear effects after calling this
void free_character(character_t*);

//Rely on init_game_time having been called
void make_stickman(character_t* c, character_t* other, enum direction direction);
void make_fatman(character_t* c, character_t* other, enum direction direction);

#endif /* character_h */
