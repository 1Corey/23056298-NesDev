/*	example code for cc65, for NES
 *  move some sprites with the controllers
 *	using neslib
 *	Doug Fraker 2018
 */	
 


#include "LIB/neslib.h"
#include "LIB/nesdoug.h"
#include "Sprites.h" // holds our metasprite data
#include "collide.h"
#include "background.h"

unsigned char frame_count = 0;
unsigned char enemy_direction = 1; // 1 = right, 0 = left

#define YOFFSCREEN 240	// offscreen y position
#define MISSILE_SPEED -4

unsigned char missile_x;
unsigned char missile_y;
char missile_active = 0;

#define MAX_ENEMIES 5

typedef struct {
    unsigned char x, y;
    unsigned char active;
} FormationEnemy;

FormationEnemy enemies[MAX_ENEMIES];

void spawn_enemy_wave() {
    unsigned char i;
    for (i = 0; i < MAX_ENEMIES; ++i) {
        enemies[i].x = 40 + (i * 30); // spread out horizontally
        enemies[i].y = 40; // all at the same height
        enemies[i].active = 1;
    }
}


#define MAX_ENEMY_MISSILES 4

struct EnemyMissile {
    unsigned char x;
    unsigned char y;
    unsigned char active;
};

struct EnemyMissile enemyMissiles[MAX_ENEMY_MISSILES];


// Call this occasionally (e.g., every 60 frames or randomly)
void fire_enemy_missile() {
    unsigned char i, e;

    // find an inactive missile slot
    for (i = 0; i < MAX_ENEMY_MISSILES; ++i) {
        if (!enemyMissiles[i].active) {
            // find a random active enemy to shoot from
            for (e = 0; e < MAX_ENEMIES; ++e) {
                if (enemies[e].active) {
                    enemyMissiles[i].x = enemies[e].x;
                    enemyMissiles[i].y = enemies[e].y + 8;
                    enemyMissiles[i].active = 1;
                    return;
                }
            }
        }
    }
}


void main (void) {
	
	ppu_off(); // screen off
	
	// load the palettes
	pal_bg(palette_bg);
	pal_spr(palette_sp);
	
	
	// use the second set of tiles for sprites
	// both bg and sprites are set to 0 by default
	bank_spr(1);
	
	set_scroll_y(0xff); //shift the bg down 1 pixel
	
	BoxGuy1.X = 110;	//player spawns in the middle of the bottom of the screen
	BoxGuy1.Y = 200;

	draw_bg();

	spawn_enemy_wave();

	// turn on screen
	// ppu_on_all(); //already done in draw_bg()
	
	while (1){

		unsigned char i;
		// infinite loop
		ppu_wait_nmi(); // wait till beginning of the frame
		// the sprites are pushed from a buffer to the OAM during nmi
		
		frame_count++;  // you can add this global to track time

	// Fire every 60 frames (~once per second)
	if (frame_count >= 60) {
    	fire_enemy_missile();
    	frame_count = 0;
	}

		pad1 = pad_poll(0); // read the first controller
		pad1_new = get_pad_new(0); // newly pressed button. do pad_poll first
		
		if ((pad1 & PAD_A) && missile_y == YOFFSCREEN) {
    	missile_x = BoxGuy1.X + 7;
   	 	missile_y = BoxGuy1.Y - 8;
	}

		movement();
		
	for (i = 0; i < MAX_ENEMIES; ++i) {
    	if (enemies[i].active) {
        	if (enemy_direction) {
				enemies[i].x += 1;
			}
			else {
				enemies[i].x -= 1;
			}
    	}

		if (enemies[i].x >= 240) {
			enemy_direction = 0;
		}
		else if (enemies[i].x <= 32) {
			enemy_direction = 1;
		}

		// Move enemy missiles
for (i = 0; i < MAX_ENEMY_MISSILES; ++i) {
    if (enemyMissiles[i].active) {
        enemyMissiles[i].y += 2;

        if (enemyMissiles[i].y > 240) {
            enemyMissiles[i].active = 0; // turn off if offscreen
        }
    }
}


	}
		if (missile_y != YOFFSCREEN) {
    	missile_y += MISSILE_SPEED;  // move up
    	if (missile_y < 8) missile_y = YOFFSCREEN;  // if off screen, deactivate
	}
		draw_sprites();
		check_start();
	}
}



void draw_bg(void){
	ppu_off(); // screen off
	
	p_maps = All_Collision_Maps[which_bg];
	// copy the collision map to c_map
	memcpy (c_map, p_maps, 240);
	
	// this sets a start position on the BG, top left of screen
	vram_adr(NAMETABLE_A);
	
	vram_write(background, 1024);
	// draw a row of tiles
	for(temp_y = 0; temp_y < 15; ++temp_y){
		for(temp_x = 0; temp_x < 16; ++temp_x){
			temp1 = (temp_y << 4) + temp_x;

			if(c_map[temp1]){
				vram_put(0x10); // wall
				vram_put(0x10);
			}
			else{
				vram_put(0); // blank
				vram_put(0);
			}
		}
		
		// draw a second row of tiles
		for(temp_x = 0; temp_x < 16; ++temp_x){
			temp1 = (temp_y << 4) + temp_x;

			if(c_map[temp1]){
				vram_put(0x10); // wall
				vram_put(0x10);
			}
			else{
				vram_put(0); // blank
				vram_put(0);
			}
		}
	}
	
	ppu_on_all(); // turn on screen
}



void draw_sprites(void){
    unsigned char i;

    oam_clear(); // clear sprite memory each frame

    // Draw the player
    oam_meta_spr(BoxGuy1.X, BoxGuy1.Y, YellowSpr);

    // Draw the player missile
    if (missile_y != YOFFSCREEN) {
        oam_meta_spr(missile_x, missile_y, Missile);
    }

    // Draw enemies
    for (i = 0; i < MAX_ENEMIES; ++i) {
        if (enemies[i].active) {
            oam_meta_spr(enemies[i].x, enemies[i].y, Enemy);
        }
    }

    // Draw enemy missiles
    for (i = 0; i < MAX_ENEMY_MISSILES; ++i) {
        if (enemyMissiles[i].active) {
            oam_meta_spr(enemyMissiles[i].x, enemyMissiles[i].y, EnemyMissile);
        }
    }
}



	
	
void movement(void){
    // Only handle left and right movement
    if(pad1 & PAD_LEFT){
        BoxGuy1.X -= 1;
    }
    else if (pad1 & PAD_RIGHT){
        BoxGuy1.X += 1;
    }
    
    // Check for collisions
    bg_collision();
    if(collision_R) BoxGuy1.X -= 1;  // If collision on the right, move left
    if(collision_L) BoxGuy1.X += 1;  // If collision on the left, move right
}




void bg_collision(){
	// sprite collision with backgrounds
	
	collision_L = 0;
	collision_R = 0;
	collision_U = 0;
	collision_D = 0;
	
	temp_x = BoxGuy1.X; // left side
	temp_y = BoxGuy1.Y; // top side
	
	if(temp_y >= 0xf0) return;
	// y out of range
	
	coordinates = (temp_x >> 4) + (temp_y & 0xf0); // upper left
	if(c_map[coordinates]){ // find a corner in the collision map
		++collision_L;
		++collision_U;
	}
	
	temp_x = BoxGuy1.X + BoxGuy1.width; // right side
	
	coordinates = (temp_x >> 4) + (temp_y & 0xf0); // upper right
	if(c_map[coordinates]){
		++collision_R;
		++collision_U;
	}
	
	temp_y = BoxGuy1.Y + BoxGuy1.height; // bottom side
	if(temp_y >= 0xf0) return;
	// y out of range
	
	coordinates = (temp_x >> 4) + (temp_y & 0xf0); // bottom right
	if(c_map[coordinates]){
		++collision_R;
		++collision_D;
	}
	
	temp_x = BoxGuy1.X; // left side
	
	coordinates = (temp_x >> 4) + (temp_y & 0xf0); // bottom left
	if(c_map[coordinates]){
		++collision_L;
		++collision_D;
	}
}




void check_start(void){
	// if START is pressed, load another background
	//if(pad1_new & PAD_START){
	//	++which_bg;
	//	if(which_bg >= 4) which_bg = 0;
	//	draw_bg();
	//}	
}