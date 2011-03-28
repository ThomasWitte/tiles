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
		data->dp.push_back((void*)imageloader.load("Fights/Images/Animation/fight1.tga"));
		data->dp.push_back((void*)imageloader.load("Fights/Images/Animation/fight2.tga"));
		data->dp.push_back((void*)imageloader.load("Fights/Images/Animation/fight3.tga"));
		data->dp.push_back((void*)imageloader.load("Fights/Images/Animation/fight4.tga"));
		return 0;
	} else if(step == AnimationData::DESTROY) {
		imageloader.destroy("Fights/Images/Animation/fight1.tga");
		imageloader.destroy("Fights/Images/Animation/fight2.tga");
		imageloader.destroy("Fights/Images/Animation/fight3.tga");
		imageloader.destroy("Fights/Images/Animation/fight4.tga");
		return 0;
	}

	//Animation beenden (Dauer 0.25s)
	if(step >= GAME_TIMER_BPS/4)
		return -1;

	BITMAP *bmp = (BITMAP*)data->dp[(16*step) / GAME_TIMER_BPS];
	masked_blit(bmp, buffer, 0, 0, data->caster.x-bmp->w/2, data->caster.y-bmp->h/2, bmp->w, bmp->h);
	
	return 0;
}
