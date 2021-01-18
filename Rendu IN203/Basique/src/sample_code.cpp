
bool calcul_depeuplement(const parametres& c, std::uniform_real_distribution<double>& rd,
                           std::minstd_rand0& generator)
{
    double val = rd(generator);//std::rand()/(1.*RAND_MAX);
    if (val < c.disparition)
        return true;
    return false;   
}

      ...
      unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count() + 173501*omp_get_thread_num();
      std::minstd_rand0 generator (seed1);       
      std::uniform_real_distribution<double> distribution(0.0,1.0);
      std::uniform_int_distribution<int> choix(0,3);
      ...
      