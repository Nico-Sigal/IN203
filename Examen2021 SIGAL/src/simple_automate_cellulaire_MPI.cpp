#include <iostream>
#include <cstdint>
#include <vector>
#include <chrono>
#include "lodepng_old/lodepng.h"

int main(int nargs, char* argv[])
{
    // On initialise le contexte MPI qui va s'occuper :
    //    1. Créer un communicateur global, COMM_WORLD qui permet de gérer
    //       et assurer la cohésion de l'ensemble des processus créés par MPI;
    //    2. d'attribuer à chaque processus un identifiant ( entier ) unique pour
    //       le communicateur COMM_WORLD
    //    3. etc...
    MPI_Init( &nargs, &argv );
    // Pour des raisons de portabilité qui débordent largement du cadre
    // de ce cours, on préfère toujours cloner le communicateur global
    // MPI_COMM_WORLD qui gère l'ensemble des processus lancés par MPI.
    MPI_Comm globComm;
    MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
    // On interroge le communicateur global pour connaître le nombre de processus
    // qui ont été lancés par l'utilisateur :
    int nbp;
    MPI_Comm_size(globComm, &nbp);
    // On interroge le communicateur global pour connaître l'identifiant qui
    // m'a été attribué ( en tant que processus ). Cet identifiant est compris
    // entre 0 et nbp-1 ( nbp étant le nombre de processus qui ont été lancés par
    // l'utilisateur )
    int rank;
    MPI_Comm_rank(globComm, &rank);
    // Création d'un fichier pour ma propre sortie en écriture :
    std::stringstream fileName;
    fileName << "Output" << std::setfill('0') << std::setw(5) << rank << ".txt";
    std::ofstream output( fileName.str().c_str() );

    MPI_Init(&nargs, &argv);
    int numero_du_processus, nombre_de_processus, temps;

    MPI_Comm_rank(MPI_COMM_WORLD,
                  &numero_du_processus);
    MPI_Comm_size(MPI_COMM_WORLD, 
                  &nombre_de_processus);

    double MPI_start = MPI_Wtime();    

    int range; // Degré voisinage (par défaut 1)
    int nb_cases; // 2^(2*range+1)
    std::int64_t dim; // Nombre de cellules prises pour la simultation
    std::int64_t nb_iter;

    range = 1;
    if ( nargs > 1 ) range = std::stoi(argv[1]);

    nb_cases = (1<<(2*range+1));
    dim = 1000;
    if ( nargs > 2 ) dim = std::stoll(argv[2]);
    nb_iter = dim;
    if ( nargs > 3 ) nb_iter = std::stoll(argv[3]);

    std::cout << "Resume des parametres : " << std::flush << std::endl;
    std::cout << "\tNombre de cellules  : " << dim << std::flush << std::endl;
    std::cout << "\tNombre d'iterations : " << nb_iter << std::flush << std::endl;
    std::cout << "\tDegre de voisinage  : " << range << std::flush << std::endl;
    std::cout << "\tNombre de cas       : " << (1ULL<<nb_cases) << std::flush << std::endl << std::endl;

    double chrono_calcul = 0.;
    double chrono_png    = 0.;
    for ( std::int64_t num_config = 0; num_config < (1LL<<nb_cases); ++num_config)
    {
        auto start = std::chrono::steady_clock::now();
        std::uint64_t nb_cells = dim+2*range;// Les cellules avec les conditions limites (toujours à zéro en séquentiel)
        std::vector<std::uint8_t> cells(nb_iter*nb_cells, 0);
        cells[nb_cells/2] = 1;
        for ( std::int64_t iter = 1; iter < nb_iter; ++iter )
        {
            for ( std::int64_t i = range; i < range + dim; ++i )
            {
                int val = 0;
                for ( std::int64_t j = i-range; j <= i+range; j++ )
                {
                    val = 2*val + cells[(iter-1)*nb_cells + j];
                }
                val = (1<<val);
                if ((val&num_config) == 0) 
                    cells[iter*nb_cells + i] = 0;
                else 
                {
                    cells[iter*nb_cells + i] = 1;
                }
            }
        }
        auto end = std::chrono::steady_clock::now(); 
        double MPI_end = MPI_Wtime();    
        std::chrono::duration<double> elapsed_seconds = MPI_end-MPI_start;
        chrono_calcul += elapsed_seconds.count();
        // Sauvegarde de l'image :
        start = std::chrono::steady_clock::now();
        std::vector<std::uint8_t> image(4*nb_iter*dim, 0xFF);
        for ( std::int64_t iter = 0; iter < nb_iter; iter ++ )
        {
            for ( std::int64_t i = range; i < range+dim; i ++ )
            {
                if (cells[iter*nb_cells + i] == 1) 
                {
                    image[4*iter*dim + 4*(i-range) + 0 ] = 0;
                    image[4*iter*dim + 4*(i-range) + 1 ] = 0;
                    image[4*iter*dim + 4*(i-range) + 2 ] = 0;
                    image[4*iter*dim + 4*(i-range) + 3 ] = 0xFF;
                }
            }
        }
        end = std::chrono::steady_clock::now();  
        double MPI_end2 = MPI_Wtime();    
        char filename[256];
        sprintf(filename, "config%03lld.png",num_config);
        lodepng::encode(filename, image.data(), dim, nb_iter);  // ligne a commenter pour supprimer la sauvegarde des fichiers
        elapsed_seconds = MPI_end2-MPI_start;
        chrono_png += elapsed_seconds.count();
    }
    std::cout << "Temps mis par le calcul : " << chrono_calcul << " secondes." << std::flush << std::endl;
    std::cout << "Temps mis pour constituer les images : " << chrono_png << " secondes." << std::flush << std::endl; //Ici on souhaite que chaque processus MPI renvoie son temps de calcul, on prendra alors le maximum

    MPI_Finalize();

    return EXIT_SUCCESS;
}
