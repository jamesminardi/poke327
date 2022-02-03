#ifndef TILES_H
#define TILES_H

typedef enum TILES {
			debug,
			empty,
		    grass_1,
		    grass_2,
		    grass_3,
		    clearing_1,
		    clearing_2,
		    clearing_3,
		    path_1,
		    path_2,
		    path_3,
		    boulder_1,
		    boulder_2,
		    boulder_3,
		    tree_1,
		    tree_2,
		    tree_3,
		    water_1,
		    water_2,
		    water_3,
		    pokecenter,
		    pokemart,
} TILES_t;

typedef enum TILE_SYMBOLS {
				s_debug = '0',
				s_empty = ' ',
			   s_grass_1 = ':',
			   s_grass_2 = ':',
			   s_grass_3 = ':',
			   s_clearing_1 = '.',
			   s_clearing_2 = '.',
			   s_clearing_3 = '.',
			   s_path_1 = '#',
			   s_path_2 = '#',
			   s_path_3 = '#',
			   s_boulder_1 = '%',
			   s_boulder_2 = '%',
			   s_boulder_3 = '%',
			   s_tree_1 = '^',
			   s_tree_2 = '^',
			   s_tree_3 = '^',
			   s_water_1 = ' ',
		       s_water_2 = ' ',
			   s_water_3 = ' ',
			   s_pokecenter = 'c',
			   s_pokemart = 'm'
} TILE_SYMBOLS_t;

#endif // TILES_H
