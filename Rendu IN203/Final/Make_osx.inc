CC = mpic++ -fopenmp
CCX = mpic++ -fopenmp
LIB = -lmpi `sdl2-config --cflags` `sdl2-config --libs` -lSDL2_image