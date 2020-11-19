# include <chrono>
# include <random>
# include <cstdlib>
# include <sstream>
# include <string>
# include <fstream>
# include <iostream>
# include <iomanip>
#include <mpi.h>

// Attention , ne marche qu'en C++ 11 ou supérieur :
unsigned long dart_in_circle_counter (unsigned long nbSamples)
{
    typedef std::chrono::high_resolution_clock myclock;
    myclock::time_point beginning = myclock::now();
    myclock::duration d = beginning.time_since_epoch();
    unsigned seed = d.count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution <double> distribution ( -1.0 ,1.0);
    unsigned long nbDarts = 0;
    // Throw nbSamples darts in the unit square [-1 :1] x [-1 :1]
    for ( unsigned sample = 0 ; sample < nbSamples ; ++ sample ) {
        double x = distribution(generator);
        double y = distribution(generator);
        // Test if the dart is in the unit disk
        if ( x*x+y*y<=1 ) nbDarts ++;
    }
    return nbDarts;
}

int main( int nargs, char* argv[] )
{


	MPI_Init( &nargs, &argv );
	MPI_Comm globComm;
	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
	int nbp;
	MPI_Comm_size(globComm, &nbp);

	int rank;
	MPI_Comm_rank(globComm, &rank);

	MPI_Status status;

	int tag=100;
	int nb_dart_per_proc = 1000000;
	unsigned long nb_darts_cercle = 0;
	unsigned long nb_darts_temp = 0;
	unsigned long nb_darts_total = (nbp-1)*nb_dart_per_proc;


	typedef std::chrono::high_resolution_clock aClock; 
    aClock::time_point debut = aClock::now();

	if (rank==0) // on définit la tâche maître
	{
		std::cout << "[0], et j'attends les retours des autres tâches" << std::endl;
		
		for (int i = 1; i<nbp; i++)
		{
			MPI_Recv(&nb_darts_temp, 1, MPI_UNSIGNED_LONG, i, tag, globComm, &status);
			std::cout << "[0], et je reçois " << nb_darts_temp << " de [" << i <<"]" <<std::endl;
			nb_darts_cercle = nb_darts_cercle + nb_darts_temp;
			nb_darts_temp = 0;
		}

		std :: cout<< nb_darts_cercle<<" flechettes dans le cercle, "<< nb_darts_total<< " flechettes envoyees" << std::endl;
		double pi = 4*(double(nb_darts_cercle)/double(nb_darts_total));
		std::cout << "pi a peu près égal à " << pi << std::endl;

		aClock::duration duree = aClock::now() - debut;
        std::cout << "Durée d'exécution du programme : " << (double)duree.count()/1000000000 << std::endl;

	}
	else 
	{
		nb_darts_temp = 0;
		nb_darts_temp = dart_in_circle_counter(nb_dart_per_proc);
		std::cout << "["<<rank<<"], et j'envoie mon résultat " << nb_darts_temp << " a [0]"<< std::endl;
		MPI_Send(&nb_darts_temp, 1, MPI_UNSIGNED_LONG, 0, tag, globComm);

		
	}
	MPI_Finalize();
	return EXIT_SUCCESS;
}
