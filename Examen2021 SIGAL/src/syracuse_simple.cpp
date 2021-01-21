#include <iostream>
#include <string>
#include <chrono>

std::pair<unsigned long, unsigned long> syracuse(unsigned long u0)
{
    unsigned long height = u0;
    unsigned long length = 0;

    unsigned long un = u0;
    while (un != 1)
    {
        if (un%2 == 0) un = un / 2;
        else un = 3UL*un+1UL;
        height = (height > un ? height : un);
        length ++;
    }
    return {height, length};
}
// 4n+1 => 12n+4 => 6n+2 => 3n+1 => Ne peut pas faire un cycle autre que 1 -> 2 -> 4 -> 1
// 4n+3 => 12n+10=> 6n+5 => Un candidat possible
int main(int nargs, char* argv[])
{
    unsigned long nb_echantillon = 1000000;
    if ( nargs > 1 ) nb_echantillon = std::stoi(argv[1]);
    double mean_height = 0;
    double mean_length = 0;
    auto start = std::chrono::steady_clock::now();
    for ( unsigned long n = 1; n <= nb_echantillon; ++n )
    {
        unsigned long u0 = 4*n+3;// Les nombres susceptibles d'être candidats n'ont que cette forme
        auto ret = syracuse(u0);
        mean_height += ret.first;
        mean_length += ret.second;
    }
    auto end = std::chrono::steady_clock::now();    
    std::cout << "Hauteur moyenne calculee : " << mean_height/nb_echantillon << std::flush << std::endl;
    std::cout << "Longueur moyenne calculee : " << mean_length/nb_echantillon << std::flush << std::endl;
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "Temps mis par le calcul : " << elapsed_seconds.count() << " secondes." << std::flush << std::endl;
    return EXIT_SUCCESS;
}