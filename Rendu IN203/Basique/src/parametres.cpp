#include <cstdlib>
#include <cassert>
#include <ctime>
#include <iostream>
#include "galaxie.hpp"
#include "parametres.hpp"

expansion calcul_expansion(const parametres& c)
{
    double val = std::rand()/(1.*RAND_MAX);
    if (val < 0.01*c.expansion)     // parmi c.expansion, on a 1% de chance d'expansion isotrope...
        return expansion_isotrope;
    if (val < c.expansion)          // ... et 99% de chance d'expansion dans 1 seule direction
        return expansion_unique;
    return pas_d_expansion;
}
//_ ______________________________________________________________________________________________ _
bool calcul_depeuplement(const parametres& c)
{
    double val = std::rand()/(1.*RAND_MAX);
    if (val < c.disparition)
        return true;
    return false;   
}
//_ ______________________________________________________________________________________________ _
bool calcul_inhabitable(const parametres& c)
{
    double val = std::rand()/(1.*RAND_MAX);
    if (val < c.inhabitable)
        return true;
    return false;
}
//_ ______________________________________________________________________________________________ _
bool apparition_technologie(const parametres& p)
{
    double val = std::rand()/(1.*RAND_MAX);
    if (val < p.apparition_civ)
        return true;
    return false;
}
//_ ______________________________________________________________________________________________ _
bool a_un_systeme_proche_colonisable(int i, int j, int width, int height, const char* galaxie)
{
    assert(i >= 0);
    assert(j >= 0);
    assert(i < height);
    assert(j < width);

    if ( (i>0) && (galaxie[(i-1)*width+j] == habitable)) return true;
    if ( (i<height-1) && (galaxie[(i+1)*width+j] == habitable)) return true;
    if ( (j>0) && (galaxie[i*width+j-1] == habitable)) return true;
    if ( (j<width-1) && (galaxie[i*width+j+1] == habitable)) return true;

    return false;
}
//_ ______________________________________________________________________________________________ _
void 
mise_a_jour(const parametres& params, int width, int height, const char* galaxie_previous, char* galaxie_next)
{
    int i, j;
    
    memcpy(galaxie_next, galaxie_previous, width*height*sizeof(char));

    for ( i = 0; i < height; ++i )
      {
        for ( j = 0; j < width; ++j )
        {
            if (galaxie_previous[i*width+j] == habitee)
            {
                if ( a_un_systeme_proche_colonisable(i, j, width, height, galaxie_previous) )
                {
                    expansion e = calcul_expansion(params);
                    if (e == expansion_isotrope)
                    {
                      if ( (i > 0) && (galaxie_previous[(i-1)*width+j] != inhabitable) )
                        {
                            galaxie_next[(i-1)*width+j] = habitee;
                        }
                      if ( (i < height-1) && (galaxie_previous[(i+1)*width+j] != inhabitable) )
                        {
                            galaxie_next[(i+1)*width+j] = habitee;
                        }
                      if ( (j > 0) && (galaxie_previous[i*width+j-1] != inhabitable) )
                        {
                            galaxie_next[i*width+j-1] = habitee;
                        }
                      if ( (j < width-1) && (galaxie_previous[i*width+j+1] != inhabitable) )
                        {
                            galaxie_next[i*width+j+1] = habitee;
                        }
                    }
                    else if (e == expansion_unique)
                    {
                        // Calcul de la direction de l'expansion :
                        int ok = 0;
                        do
                        {
                            int dir = std::rand()%4;
                            if ( (i>0) && (0 == dir) && (galaxie_previous[(i-1)*width+j] != inhabitable) )
                            {
                                galaxie_next[(i-1)*width+j] = habitee;
                                ok = 1;
                            }
                            if ( (i<height-1) && (1 == dir) && (galaxie_previous[(i+1)*width+j] != inhabitable) )
                            {
                                galaxie_next[(i+1)*width+j] = habitee;
                                ok = 1;
                            }
                            if ( (j>0) && (2 == dir) && (galaxie_previous[i*width+j-1] != inhabitable) )
                            {
                                galaxie_next[i*width+j-1] = habitee;
                                ok = 1;
                            }
                            if ( (j<width-1) && (3 == dir) && (galaxie_previous[i*width+j+1] != inhabitable) )
                            {
                                galaxie_next[i*width+j+1] = habitee;
                                ok = 1;
                            }
                        } while (ok == 0);
                    }// End if (e == expansion_unique)
                }// Fin si il y a encore un monde non habite et habitable
                if (calcul_depeuplement(params))
                {
                    galaxie_next[i*width+j] = habitable;
                }
                if (calcul_inhabitable(params))
                {
                    galaxie_next[i*width+j] = inhabitable;
                }
            }  // Fin si habitee
            else if (galaxie_previous[i*width+j] == habitable)
            {
                if (apparition_technologie(params))
                    galaxie_next[i*width+j] = habitee;
            }
            else { // inhabitable
              // nothing to do : le systeme a explose
            }
            // if (galaxie_previous...)
        }// for (j)
      }// for (i)

}
//_ ______________________________________________________________________________________________ _
