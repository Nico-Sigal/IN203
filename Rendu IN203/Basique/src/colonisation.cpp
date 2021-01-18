#include <cstdlib>
#include <string>
#include <iostream>
#include <SDL.h>        
#include <SDL_image.h>
#include <fstream>
#include <ctime>
#include <iomanip>      // std::setw
#include <chrono>

#include "parametres.hpp"
#include "galaxie.hpp"
 
int main(int argc, char ** argv)
{
    char commentaire[4096];
    int width, height;
    SDL_Event event;
    SDL_Window   * window;

    parametres param;


    std::ifstream fich("parametre.txt");
    fich >> width;
    fich.getline(commentaire, 4096);
    fich >> height;
    fich.getline(commentaire, 4096);
    fich >> param.apparition_civ;
    fich.getline(commentaire, 4096);
    fich >> param.disparition;
    fich.getline(commentaire, 4096);
    fich >> param.expansion;
    fich.getline(commentaire, 4096);
    fich >> param.inhabitable;
    fich.getline(commentaire, 4096);
    fich.close();

    std::cout << "Resume des parametres (proba par pas de temps): " << std::endl;
    std::cout << "\t Chance apparition civilisation techno : " << param.apparition_civ << std::endl;
    std::cout << "\t Chance disparition civilisation techno: " << param.disparition << std::endl;
    std::cout << "\t Chance expansion : " << param.expansion << std::endl;
    std::cout << "\t Chance inhabitable : " << param.inhabitable << std::endl;
    std::cout << "Proba minimale prise en compte : " << 1./RAND_MAX << std::endl;
    std::srand(std::time(nullptr));

    SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);

    window = SDL_CreateWindow("Galaxie", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              width, height, SDL_WINDOW_SHOWN);

    galaxie g(width, height, param.apparition_civ);
    galaxie g_next(width, height);
    galaxie_renderer gr(window);

    int deltaT = (20*52840)/width;
    std::cout << "Pas de temps : " << deltaT << " années" << std::endl;

    std::cout << std::endl;

    gr.render(g);
    unsigned long long temps = 0;

    std::chrono::time_point<std::chrono::system_clock> start, end1, end2;
    while (1) {
        start = std::chrono::system_clock::now();
        mise_a_jour(param, width, height, g.data(), g_next.data());
        end1 = std::chrono::system_clock::now();
        g_next.swap(g);
        gr.render(g);
        end2 = std::chrono::system_clock::now();
        
        std::chrono::duration<double> elaps1 = end1 - start;
        std::chrono::duration<double> elaps2 = end2 - end1;
        
        temps += deltaT;
        std::cout << "Temps passe : "
                  << std::setw(10) << temps << " années"
                  << std::fixed << std::setprecision(3)
                  << "  " << "|  CPU(ms) : calcul " << elaps1.count()*1000
                  << "  " << "affichage " << elaps2.count()*1000
                  << "\r" << std::flush;
        //_sleep(1000);
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
          std::cout << std::endl << "The end" << std::endl;
          break;
        }
    }
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
