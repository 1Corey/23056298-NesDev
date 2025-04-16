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
	- 8,- 8,0x04,0,
	  0,- 8,0x05,0,
	- 8,  0,0x06,0,
	  0,  0,0x07,0,
	0x80
};


const unsigned char ExplosionSprite[]={
	- 8,- 8,0x0c,0,
	  0,- 8,0x0d,0,
	- 8,  0,0x1c,0,
	  0,  0,0x1d,0,
	0x80
};


