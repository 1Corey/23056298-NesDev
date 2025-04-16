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
#include <stdlib.h>

unsigned char frame_count = 0;
unsigned char enemy_direction = 1; // 1 = right, 0 = left

#define YOFFSCREEN 240	// offscreen y position
#define MISSILE_SPEED -4
#define MAX_ENEMY_MISSILES 4

unsigned char missile_x;
unsigned char missile_y;
char missile_active = 0;

#define ENEMY_ROWS 2
#define ENEMY_COLS 5
#define MAX_ENEMIES (ENEMY_ROWS * ENEMY_COLS)

struct Enemy {
    unsigned char x;
    unsigned char y;
    unsigned char direction;
    unsigned char active;
};

struct Enemy enemies[MAX_ENEMIES];


void spawn_enemy_wave(void) {
    unsigned char row, col;
    unsigned char index = 0;

    for (row = 0; row < ENEMY_ROWS; ++row) {
        for (col = 0; col < ENEMY_COLS; ++col) {
            if (index >= MAX_ENEMIES) break;

            enemies[index].x = 40 + col * 32;   // horizontal spacing
            enemies[index].y = 40 + row * 30;   // vertical spacing
            enemies[index].direction = 1;       // start moving right
            enemies[index].active = 1;
            index++;
        }
    }
}


struct EnemyMissile {
    unsigned char x;
    unsigned char y;
    unsigned char active;
};

struct EnemyMissile enemyMissiles[MAX_ENEMY_MISSILES];

unsigned char effect_x = 0;
unsigned char effect_y = 0;
unsigned char effect_timer = 0;

unsigned char player_alive = 1;
unsigned char game_over_displayed = 0;

// Fire missile from the enemy
void fire_enemy_missile(unsigned char enemy_index) {
    unsigned char i;
    for (i = 0; i < MAX_ENEMY_MISSILES; ++i) {
        if (!enemyMissiles[i].active) {
            enemyMissiles[i].x = enemies[enemy_index].x;
            enemyMissiles[i].y = enemies[enemy_index].y + 8; // missile starts just below enemy
            enemyMissiles[i].active = 1;
            return;
        }
    }
}

void reset_game(void) {
	unsigned char i;

    // Reset player
    BoxGuy1.X = 110;
    BoxGuy1.Y = 200;
    missile_y = YOFFSCREEN;
    missile_active = 0;
    player_alive = 1;

    for (i = 0; i < MAX_ENEMIES; ++i) {
        enemies[i].x = 40 + (i * 30);
        enemies[i].y = 40;
        enemies[i].active = 1;
    }

    // Reset enemy missiles
    for (i = 0; i < MAX_ENEMY_MISSILES; ++i) {
        enemyMissiles[i].active = 0;
    }

    // Redraw background
    draw_bg();
	spawn_enemy_wave();
	game_over_displayed = 0;
}

void draw_game_over(void) {
	if (game_over_displayed) return;
    ppu_off();  // turn screen off for safe drawing

    // Set VRAM address to write to (middle of screen: x=11, y=14)
    vram_adr(NTADR_A(12, 14));

    vram_put(0x0A); // G
    vram_put(0x04); // A
    vram_put(0x10); // M
    vram_put(0x08); // E
    vram_put(0x00); // Space
    vram_put(0x12); // O
    vram_put(0x19); // V
    vram_put(0x08); // E
    vram_put(0x15); // R
	vram_put(0x28); // !
	
	vram_adr(NTADR_A(11, 16));

	vram_put(0x13); // P
	vram_put(0x15); // R
	vram_put(0x08); // E
	vram_put(0x16); // S
	vram_put(0x16); // S
	vram_put(0x00); // Space
	vram_put(0x16); // S
	vram_put(0x17); // T
	vram_put(0x04); // A
	vram_put(0x15); // R
	vram_put(0x17); // T

    ppu_on_all();  // turn screen back on
	game_over_displayed = 1;
}



void main (void) {
    rand(); // rand initialized

    ppu_off(); // screen off

    // load the palettes
    pal_bg(palette_bg);
    pal_spr(palette_sp);

    bank_spr(1); // use the second set of tiles for sprites
    set_scroll_y(0xff); // shift the bg down 1 pixel

    BoxGuy1.X = 110;
    BoxGuy1.Y = 200;

    draw_bg();
    spawn_enemy_wave();

    while (1) {
        unsigned char i;
        ppu_wait_nmi(); // wait till beginning of the frame
        frame_count++;

        // Move all enemies and make them fire missiles randomly
        for (i = 0; i < MAX_ENEMIES; ++i) {
            if (enemies[i].active) {
                // Move enemies
                if (enemies[i].direction) {
                    enemies[i].x++;
                    if (enemies[i].x >= 240) enemies[i].direction = 0;
                } else {
                    enemies[i].x--;
                    if (enemies[i].x <= 32) enemies[i].direction = 1;
                }

                // Fire missiles randomly
                if (frame_count % 60 == 0 && rand() % 10 < 2) {
                    fire_enemy_missile(i);
                }
            }
        }

        pad1 = pad_poll(0);

        if (!player_alive) {
            draw_game_over();

            while (1) {
                ppu_wait_nmi();
                pad1 = pad_poll(0);
                pad1_new = get_pad_new(0);

                if (pad1_new & PAD_START) {
                    reset_game();
                    break;
                }
            }

            continue;
        }

        pad1_new = get_pad_new(0);

        if ((pad1 & PAD_A) && missile_y == YOFFSCREEN) {
            missile_x = BoxGuy1.X + 7;
            missile_y = BoxGuy1.Y - 8;
        }

        movement();

        // Move player missile
        if (missile_y != YOFFSCREEN) {
            missile_y += MISSILE_SPEED;
            if (missile_y < 8) missile_y = YOFFSCREEN;
        }

        // Move enemy missiles
        for (i = 0; i < MAX_ENEMY_MISSILES; ++i) {
            if (enemyMissiles[i].active) {
                enemyMissiles[i].y += 2;
                if (enemyMissiles[i].y > 240) {
                    enemyMissiles[i].active = 0;
                }
            }
        }

        // Enemy collision
        for (i = 0; i < MAX_ENEMIES; ++i) {
            if (enemies[i].active &&
                missile_y != YOFFSCREEN &&
                missile_x + 8 > enemies[i].x &&
                missile_x < enemies[i].x + 16 &&
                missile_y + 8 > enemies[i].y &&
                missile_y < enemies[i].y + 16) {

                enemies[i].active = 0;
                missile_y = YOFFSCREEN;
                effect_x = enemies[i].x;
                effect_y = enemies[i].y;
                effect_timer = 30;
            }
        }

        if (effect_timer > 0) {
            --effect_timer;
        }

        if (player_alive) {
            for (i = 0; i < MAX_ENEMY_MISSILES; ++i) {
                if (enemyMissiles[i].active &&
                    BoxGuy1.X + 12 > enemyMissiles[i].x &&
                    BoxGuy1.X < enemyMissiles[i].x + 8 &&
                    BoxGuy1.Y + 12 > enemyMissiles[i].y &&
                    BoxGuy1.Y < enemyMissiles[i].y + 8) {

                    player_alive = 0;
                    enemyMissiles[i].active = 0;
                    effect_x = BoxGuy1.X;
                    effect_y = BoxGuy1.Y;
                    effect_timer = 15;
                }
            }
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

	if (effect_timer > 0) {
    	oam_meta_spr(effect_x, effect_y, ExplosionSprite);
	}

	if (!player_alive && effect_timer > 0) {
    	oam_meta_spr(effect_x, effect_y, ExplosionSprite);  // Draw explosion at player's position
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