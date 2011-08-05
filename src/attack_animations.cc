/*  Copyright 2009-2011 Thomas Witte

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "attack_animations.h"

#include <iostream>
int fight_ani(int step, AnimationData *data, BITMAP *buffer) {
	if(step == AnimationData::INITIALIZE) {
		data->dp.push_back((void*)IMGLOADER.load("Fights/Images/Animation/fight1.tga"));
		data->dp.push_back((void*)IMGLOADER.load("Fights/Images/Animation/fight2.tga"));
		data->dp.push_back((void*)IMGLOADER.load("Fights/Images/Animation/fight3.tga"));
		data->dp.push_back((void*)IMGLOADER.load("Fights/Images/Animation/fight4.tga"));
		return 0;
	} else if(step == AnimationData::DESTROY) {
		IMGLOADER.destroy("Fights/Images/Animation/fight1.tga");
		IMGLOADER.destroy("Fights/Images/Animation/fight2.tga");
		IMGLOADER.destroy("Fights/Images/Animation/fight3.tga");
		IMGLOADER.destroy("Fights/Images/Animation/fight4.tga");
		return 0;
	}

	//Animation beenden (Dauer 0.25s)
	if(step >= GAME_TIMER_BPS/4)
		return -1;

	BITMAP *bmp = (BITMAP*)data->dp[(16*step) / GAME_TIMER_BPS];
#if ALLEGRO_SUB_VERSION >= 4
	draw_sprite_ex(buffer, bmp, data->caster.x-bmp->w/2, data->caster.y-bmp->h/2, DRAW_SPRITE_NORMAL,
		(data->caster.x > data->targets[0].x ? DRAW_SPRITE_NO_FLIP : DRAW_SPRITE_H_FLIP));
#else
	if(data->caster.x > data->targets[0].x)
		draw_sprite(buffer, bmp, data->caster.x-bmp->w/2, data->caster.y-bmp->h/2);
	else
		draw_sprite_h_flip(buffer, bmp, data->caster.x-bmp->w/2, data->caster.y-bmp->h/2);
#endif

	return 0;
}

int heal_ani(int step, AnimationData *data, BITMAP *buffer) {
	if(step == AnimationData::INITIALIZE) {
		for(int i = 1; i <= 25; i++)
			data->dp.push_back((void*)IMGLOADER.load("Fights/Images/Animation/cure" + to_string(i) + ".tga"));
		return 0;
	} else if(step == AnimationData::DESTROY) {
		for(int i = 1; i <= 25; i++)
			IMGLOADER.destroy("Fights/Images/Animation/cure" + to_string(i) + ".tga");
		return 0;
	}

	//Animation beenden (Dauer 1s)
	if(step >= 5*GAME_TIMER_BPS/3)
		return -1;

	BITMAP *bmp = (BITMAP*)data->dp[(15*step) / GAME_TIMER_BPS];
	for(unsigned int i = 0; i < data->targets.size(); i++)
		draw_sprite(buffer, bmp, data->targets[i].x-bmp->w/2, data->targets[i].y-bmp->h/2);
	
	return 0;
}
