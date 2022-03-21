#ifndef COLORS_H
#define COLORS_H

//Regular text
#define BLK "\e[0;30m" // Black
#define RED "\e[0;31m" // Red
#define GRN "\e[0;32m" // Green
#define YEL "\e[0;33m" // Yellow
#define BLU "\e[0;34m" // Blue
#define MAG "\e[0;35m" // Magenta
#define CYN "\e[0;36m" // Cyan
#define WHT "\e[0;37m" // White
#define BRN "\033[38;5;130m" // Brown
#define BRIGHTGRN "\033[38;5;77m"
#define BOULDER "\033[38;5;102m" // Greyish
#define CHARACTER "\033[38;5;226m" // Bright yellow

#define PRINT_BORDER 	printf("\033[38;5;102m" "%%" CRESET);
#define PRINT_BOULDER 	printf("\033[38;5;102m" "%%" CRESET);
#define PRINT_MOUNTAIN 	printf("\u0394" CRESET);
#define PRINT_TREE 		printf(GRN "^" CRESET);
#define PRINT_FOREST 	printf(GRN "^" CRESET);
#define PRINT_EXIT 		printf(CYN "#" CRESET);
#define PRINT_PATH 		printf(YEL "#" CRESET);
#define PRINT_MART 		printf(CYN "M" CRESET);
#define PRINT_CENTER 	printf(HRED "C" CRESET);
#define PRINT_GRASS 	printf(HGRN ":" CRESET);
#define PRINT_CLEARING 	printf(BRIGHTGRN "." CRESET);
#define PRINT_EMPTY 	printf(" " CRESET);
#define PRINT_DEBUG 	printf(WHT"\u058D" CRESET);
#define PRINT_DEFAULT 	printf(WHT "\u00BF" CRESET);

#define PRINT_PC		printf(HWHT "@" CRESET);
#define PRINT_RIVAL		printf(CHARACTER "r" CRESET);
#define PRINT_HIKER		printf(CHARACTER "h" CRESET);
#define PRINT_STATUE	printf(CHARACTER "s" CRESET);
#define PRINT_PACER 	printf(CHARACTER "p" CRESET);
#define PRINT_WANDERER  printf(CHARACTER "w" CRESET);
#define PRINT_RANDOM	printf(CHARACTER "n" CRESET);
#define PRINT_UNOCCUPIED printf( HBLK "-" CRESET);

//Regular bold text
#define BBLK "\e[1;30m"
#define BRED "\e[1;31m"
#define BGRN "\e[1;32m"
#define BYEL "\e[1;33m"
#define BBLU "\e[1;34m"
#define BMAG "\e[1;35m"
#define BCYN "\e[1;36m"
#define BWHT "\e[1;37m"

//Regular underline text
#define UBLK "\e[4;30m"
#define URED "\e[4;31m"
#define UGRN "\e[4;32m"
#define UYEL "\e[4;33m"
#define UBLU "\e[4;34m"
#define UMAG "\e[4;35m"
#define UCYN "\e[4;36m"
#define UWHT "\e[4;37m"

//Regular background
#define BLKB "\e[40m"
#define REDB "\e[41m"
#define GRNB "\e[42m"
#define YELB "\e[43m"
#define BLUB "\e[44m"
#define MAGB "\e[45m"
#define CYNB "\e[46m"
#define WHTB "\e[47m"

//High intensty background 
#define BLKHB "\e[0;100m"
#define REDHB "\e[0;101m"
#define GRNHB "\e[0;102m"
#define YELHB "\e[0;103m"
#define BLUHB "\e[0;104m"
#define MAGHB "\e[0;105m"
#define CYNHB "\e[0;106m"
#define WHTHB "\e[0;107m"

//High intensty text
#define HBLK "\e[0;90m"
#define HRED "\e[0;91m"
#define HGRN "\e[0;92m"
#define HYEL "\e[0;93m"
#define HBLU "\e[0;94m"
#define HMAG "\e[0;95m"
#define HCYN "\e[0;96m"
#define HWHT "\e[0;97m"

//Bold high intensity text
#define BHBLK "\e[1;90m"
#define BHRED "\e[1;91m"
#define BHGRN "\e[1;92m"
#define BHYEL "\e[1;93m"
#define BHBLU "\e[1;94m"
#define BHMAG "\e[1;95m"
#define BHCYN "\e[1;96m"
#define BHWHT "\e[1;97m"

//Reset
#define CRESET "\e[0m"
#define COLOR_RESET "\e[0m"

#endif // COLORS_H
