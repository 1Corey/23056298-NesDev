// a 16x16 pixel metasprite

const unsigned char YellowSpr[]={
	0, 0,0x02,0,
	8, 0,0x02,0|OAM_FLIP_H,
  	0,  8,0x03,0,
	8,  8,0x03,0|OAM_FLIP_H,
  0x80
};


const unsigned char BlueSpr[]={
	  0,  0,0x00,1,
	  8,  0,0x00,1|OAM_FLIP_H,
	  0,  8,0x10,1,
	  8,  8,0x10,1|OAM_FLIP_H,
	128
};

const unsigned char Missile[]={
	- 4,- 8,0x0b,0,
	0x80
};

const unsigned char EnemyMissile[]={
	- 4,- 8,0x0b,0,
	0x80
};

const unsigned char Enemy[]={
	- 8,- 8,0x01,0,
	  0,- 8,0x01,0|OAM_FLIP_H,
	- 8,  0,0x01,0|OAM_FLIP_V,
	  0,  0,0x01,0|OAM_FLIP_H|OAM_FLIP_V,
	0x80
};

const unsigned char ExplosionSprite[] = {
  0, 0, 0x2A, 0,
  128
};

