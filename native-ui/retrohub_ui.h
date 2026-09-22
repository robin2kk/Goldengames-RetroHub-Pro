#pragma once
#include <SDL.h>
#include "retrohub_systems.h"
#include "retrohub_runtime.h"
typedef enum RetroHubScreen { RH_SYSTEMS, RH_GAMES } RetroHubScreen;
typedef struct RetroHubState {
 RetroHubScreen screen; int system_index; int game_index; int game_count;
 float carousel; RetroGame games[RETROHUB_MAX_GAMES];
} RetroHubState;
void retrohub_enter_system(RetroHubState *s);
void retrohub_back(RetroHubState *s);
void retrohub_move(RetroHubState *s,int delta);
void retrohub_activate(RetroHubState *s);
void retrohub_render(SDL_Renderer *r,RetroHubState *s);
