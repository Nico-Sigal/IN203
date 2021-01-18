#include <cstdlib>
#include <string>
#include <iostream>
#include <SDL.h>        
#include <SDL_image.h>
#include <fstream>
#include <ctime>
#include <iomanip>      // std::setw
#include <chrono>
#include <mpi.h>

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


    MPI_Init(&argc,&argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int nbproc;
    MPI_Comm_size(MPI_COMM_WORLD, &nbproc);
    MPI_Comm globComm;
    MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
    
    std::srand(std::time(nullptr));

    if (height%(nbproc-1) !=0) {std::cerr << "La hauteur n'est pas divisible par nbproc-1 (nombre de processeurs qui calculent)" << std::endl; MPI_Abort(globComm,EXIT_FAILURE);}

    int deltaT = (20*52840)/width;
    unsigned long long temps = 0;  
    std::chrono::time_point<std::chrono::system_clock> start, end1, end2;

    
    if (rank == 0)
    {
	    std::cout << "Resume des parametres (proba par pas de temps): " << std::endl;
	    std::cout << "\t Chance apparition civilisation techno : " << param.apparition_civ << std::endl;
	    std::cout << "\t Chance disparition civilisation techno: " << param.disparition << std::endl;
	    std::cout << "\t Chance expansion : " << param.expansion << std::endl;
	    std::cout << "\t Chance inhabitable : " << param.inhabitable << std::endl;
	    std::cout << "Proba minimale prise en compte : " << 1./RAND_MAX << std::endl;
	    SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);
	    window = SDL_CreateWindow("Galaxie", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	                              width, height, SDL_WINDOW_SHOWN);

	    galaxie g_gathered(width, height, param.apparition_civ);
	    galaxie_renderer gr(window);
	    gr.render(g_gathered);
	    int height_loc = height / (nbproc-1) ;
	    unsigned long cnt = width * height_loc;
	    //int offset_inf;
	    MPI_Status status;

	    galaxie g_buff(width, height_loc);
	    //int offset_sup = (rank+1)*height_loc*width-1;

	    //int tag;
	    std::cout << "Pas de temps : " << deltaT << " années" << std::endl;
	    while (1) {

	        start = std::chrono::system_clock::now();

	        
	        for (int i = 1; i<nbproc; ++i){
	            //char* data = g_gathered.data();
	            //tag=100+i;
	            //offset_inf = i*height_loc*width;
	            //std::cout << "test, " << std::endl;

	            MPI_Recv (g_buff.data(),cnt ,MPI_CHAR ,i,100,globComm, &status );
	            std::copy(g_buff.data(), g_buff.data()+cnt, g_gathered.data() + cnt*(i-1));
	        }
	        end1 = std::chrono::system_clock::now();
	        
	        //g_next.swap(g);
	        gr.render(g_gathered);
	        end2 = std::chrono::system_clock::now();
	        
	        std::chrono::duration<double> elaps1 = end1 - start;
	        std::chrono::duration<double> elaps2 = end2 - end1;
	        
	        temps += deltaT;
	        std::cout << "Temps passé : "
	                  << std::setw(10) << temps << " années"
	                  << std::fixed << std::setprecision(3)
	                  << "  " << "|  CPU(ms) : calcul " << elaps1.count()*1000
	                  << "  " << "affichage " << elaps2.count()*1000
	                  << "\r" << std::flush;
	        //std::sleep(1000);
	        if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
	            std::cout << std::endl << "The end" << std::endl;
	            SDL_DestroyWindow(window);
	            SDL_Quit();
	            MPI_Finalize();
	            break;
	        }
	    }
	}



    if (rank >0)
    {
        int height_loc = height / (nbproc-1) ;
        long cnt = width * height_loc;
        //int offset_inf = rank*height_loc*width;
        //int offset_sup = (rank+1)*height_loc*width-1;
        //int tag = 100+rank;


        //On adapte la taille de la galaxie en fonction du nb de lignes phantomes à prendre en compte
        int height_phantom_loc = height_loc;
        if (rank-1!=0)
            height_phantom_loc++;
        if (rank+1!=nbproc)
            height_phantom_loc++;


        //chaque processeur travaille sur une galaxie réduite
        // + 1 ou +2 lignes correspondant auxl ignes phantomes
        // le passage dans mise a jour prendra donc en compte les lignes phantomes
        // mais ne ces dernieres ne servent que pour une seule itération de boucle
        // Elles sont ensuite remplacées par la nouvelles ligne phantome de la galaxie voisine
        galaxie g_loc(width, height_phantom_loc, param.apparition_civ);
        galaxie g_next_loc(width, height_phantom_loc);
        g_next_loc.swap(g_loc);
        MPI_Status status;
        //g.datasize();
        //g.dataprint();

        

	    while (1) {

	        start = std::chrono::system_clock::now();

	        mise_a_jour(param, width, height_loc, g_loc.data(), g_next_loc.data());
	        //char* data=g_next_loc.data();

	        //std::cout << g_next_loc.datasize() <<" = "<< cnt << std::endl;
	        MPI_Send(g_next_loc.data()+1, cnt, MPI_CHAR, 0, 100, globComm);

	        //envoi-réception de la ligne phantome au processeur précédent (s'il existe)
	        if (rank-1!=0){
	            MPI_Send(g_next_loc.data()+1, width, MPI_CHAR, rank-1, 100, globComm);
	            
	            // cette réception ecrase la ligne phantome de l'itération précédente
	            // qui ne sert plus à rien
	            MPI_Recv(g_next_loc.data(), width, MPI_CHAR, rank-1, 100, globComm, &status);
	        }   

	        //envoi-réception de la ligne phantome au processeur suivant (s'il existe)
	        if (rank+1!=nbproc){
	            MPI_Send(g_next_loc.data()+cnt-width, width, MPI_CHAR, rank+1, 100, globComm);
	            MPI_Recv(g_next_loc.data()+cnt, width, MPI_CHAR, rank+1, 100, globComm, &status);
	        }
	            

	        end1 = std::chrono::system_clock::now();
	        g_next_loc.swap(g_loc);
	        end2 = std::chrono::system_clock::now();
	        
	        std::chrono::duration<double> elaps1 = end1 - start;
	        std::chrono::duration<double> elaps2 = end2 - end1;
	        

	        if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
	          if (rank==0) std::cout << std::endl << "The end" << std::endl;
	          break;
	        }
	    }
    }
    //if (rank == 0)

    //MPI_Abort(globComm,EXIT_FAILURE);

    MPI_Finalize();
    return EXIT_SUCCESS;
}


// EXECUTE AVEC mpirun -np 5 --use-hwthread-cpus ./colonisation.exe 
// NB le makefile a été adapté pour utiliser mpic++ à la compilation