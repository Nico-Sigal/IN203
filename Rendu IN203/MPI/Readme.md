# Simulation d'une colonisation d'une galaxie

En 1950, lors d'un repas avec ses collègues, Enrico Fermi, discutant de la vie extraterrestre,
s'exclame : "Mais où sont-ils donc passés ?" en parlant des extraterrestres. Sachant que le Soleil est
une étoile assez jeune dans la galaxie et qu'il existe un grand nombre d'étoiles bien plus anciennes,
des civilisations extraterrestres beaucoup plus anciennes auraient dû apparaître dans la galaxie
et on devrait en trouver des traces, or ce n'est pas le cas.

Nommé "paradoxe de Fermi", plusieurs auteurs ont débattu sur ce paradoxe et ont montré plusieurs faiblesses
sur les hypothèses de départ (non exhaustives ici) :

  - Le paradoxe part du principe qu'on pourra comprendre une intelligence sûrement très différente de la nôtre
  - Il part également du principe que le voyage dans l'espace est facile et que les civilisations technologiques
  sont faites pour durer
  - Enfin, et c'est ce qui nous intéresse, on peut montrer par une approche par percolation, que les civilisations
  extraterrestres qui colonisent une galaxie peuvent selon certains paramètres "oublier" de grandes zones
  où la Terre peut très bien se trouver.

On se propose de paralléliser un programme qui simule la colonisation d'une galaxie  par diverses civilisations technologiques à l'aide d'un modèle très
simplifié et permet de voir la dynamique de colonisation de
la galaxie et pourquoi le paradoxe de Fermi peut ne pas être vraiment un paradoxe...

## Installation des packages nécessaires

Pour faire fonctionner le programme, il faut installer la librairie SDL 2 qui permet sous divers OS
d'afficher une fenêtre graphique. Les commandes pour installer les librairies nécessaires sont données
par OS.

### Sous Ubuntu

    sudo apt install libsdl2-dev
    sudo apt install libsdl2-image-dev

### Sous MSys 2

    pacman -S mingw64/mingw-w64-x86_64-SDL2
    pacman -S mingw64/mingw-w64-x86_64-SDL2_image

### Sous Mac OS-X

    brew install sdl2
    brew install sdl2_image

## Description de la simulation de colonisation de la galaxie

En entrée, le programme lit un fichier contenant des paramètres, à savoir :

- Le nombre de pixel **width** de la fenêtre en longueur. Ce nombre de pixel est corrélé ici au pas de temps
  que choisira la simulation. En effet, la Voie lactée fait 52840 années lumière, et donc un pixel
  (en longueur) fera 52840/**width** années lumière. En supposant que la vitesse moyenne maximale permise
  pour aller d'une étoile à une autre soit de 5% de la vitesse de la lumière, il vient qu'il faudra
  vingt ans pour faire une année lumière. En considérant une planète habitable par pixel, on aura donc
  un temps de voyage entre les planètes habitables de 20x52840/**width** années qu'on prendre pour pas de temps.
  Par exemple, pour  une largeur d'écran de 800 pixels, on aura donc 20x52840/800 = 1321 années.
- Le nombre de pixel **height** de la fenêtre en hauteur. Pas d'autres incidences que l'affichage !
- La probabilité qu'une civilisation ayant une technologie suffisante pour coloniser l'espace apparaisse : cette probabilité est une probabilité qui sera testée à chaque pas de temps pour chaque planète habitable ne possédant pas encore de civilisation technologique.
- La probabilité qu'une civilisation technologique disparaisse : cela peut représenter la probabilité qu'une civilisation régresse (à cause d'une guerre, des maladies, etc.) ou bien que la race civilisée s'éteigne mais une autre forme d'intelligence et de civilisation peut réapparaître plus tard
- La probabilité qu'une civilisation technologique décide d'aller coloniser des planètes d'un autres système solaire
(la civilisation peut ne pas vouloir à cause du coût, des risques ou tout simplement parce qu'elle n'en voit pas
l'intérêt !)
- La probabilité qu'une civilisation technologique rende ou que sa planète devienne inhabitable (guerre nucléaire, destruction de la planète par super nova de l'étoile, etc.).

À l'initialisation de la simulation, on tire pour chaque pixel la probabilité qu'une civilisation technologique existe.

Puis à chaque tour, on regarde :
- pour chaque planète possédant une civilisation technologique si celle-ci décide de coloniser un ou les systèmes planétaires habitables autour d'elle
- puis si cette civilisation disparaît ou non (même si elle a lancé des colonies dans l'espace) 
- ou si la planète est détruite ou devenue inhabitable. 
- Pour les planètes habitables ne possédant pas encore de civilisation technologique, on regarde si une telle civilisation apparaît.

Enfin, on affiche graphiquement le résultat de ce pas de temps :
un pixel vert si la planète est habitée,
un pixel rouge si la planète est inhabitable,
on ne trace pas les planètes habitables mais inhabitées.



*Remarque* : Quand on parle ici "d'autour une planète", on parle des pixels à gauche, à droite, au dessus ou ou en dessous,
pas des pixels se trouvant en diagonale.

# Parallélisation du code

Lors de la parallélisation du code, il ne faudra pas hésiter à modifier les classes et leurs méthodes.

Le code devra être parallélisé sur deux niveaux (mémoire partagée et mémoire distribuée), ainsi que sur plusieurs stratégies :

## Parallélisation de boucle en mémoire partagée

On parallélisera le code dans un premier temps en mémoire partagée, en mettant en œuvre un parallélisme de la boucle de calcul. 
On s'assurera que si il y a condition
de data race, bien expliquer quelles sont ces data races dans le rapport du projet, et bien s'assurer que
le cas échéant, elles ne porteront pas préjudice au déroulement du code et aux résultats.


## Recouvrement calcul / affichage en mémoire partagée
Les *Timers* montrent que la boucle de calcul et la boucle d'affichage prennent un temps similaire (l'équilibre dépend toutefois grandement de la machine). 
On mettra donc en place la stratégie suivante : 
- le premier *thread* affiche la galaxie initiale
- pendant ce temps d'affichage, un deuxième *thread* (ou tous les autres *threads* si vous cumulez avec la question précédente) calcule le pas de temps suivant
- lorsque les deux threads ont terminé, le premier *thread* affiche le nouvel état pendant que les autres *threads* continuent le calcul.

**Note** : cette technique est un grand classique de programmation, qui s'appelle le *recouvrement des entrées/sorties par du calcul* ; 
elle est souvent employée autour des sauvegardes sur disque dur qui est un goulot d'étranglement du code difficilement accélérable autrement.


## Parallélisation en mémoire distribuée

Une fois le code parallélisé en mémoire partagée, on effectuera un deuxième niveau de parallélisme en le parallélisant
pour de la mémoire distribuée.

Dans un premier temps, on se contentera de partager d'un côté le calcul des planètes habitées/habitables/non habitables par percolation, de l'autre côté le rendu graphique :

  - Le processus de rang zéro se contentera uniquement d'afficher graphiquement le résultat à chaque pas de temps,
  - Le processus de rang un calculera le pas de temps suivant en utilisant l'algorithme de percolation.

Autrement dit on réalisera la version MPI de la technique de recouvrement calcul / affichage de la question précédente.


Ensuite, 
pour paralléliser en mémoire distribuée sur **nbp** processus, on utilisera toujours le processus zéro comme celui qui affichera le résultat à l'écran et pour les autres processus, on découpera la grille de pixel en plusieurs morceaux, soit par tranche dans une direction (horizontale par exemple) soit en découpant dans la direction horizontale mais aussi dans la direction verticale.

On obtiendra alors des sous-grilles auxquelles on rajoutera sur chaque bord des cellules fantômes, c'est à dire
des cellules dont les valeurs qu'on remplira avec la percolation seront ensuite transmises aux cellules de la sous-grille
voisine, cellules qui correspondent aux cellules fantômes. 

Ainsi, dans cet exemple en une dimension avec deux sous-grilles, on aura :

                cellule fantôme (où on calcule si planète habitée, habitable ou inhabitable)
                       |
                       v
    |--|--|--|--|--|--|..|          <----- Première grille
                    ^   |
                    |   v   Les flèches indiquent le sens de transmission de l'information
                   |..|--|--|--|--|--|--|--|   <-------- Seconde grille
                     ^
                     |
                cellule fantôme

Ainsi, une cellule "fantôme" de la première grille enverra donc son nouvel état à la seconde grille
qui mettra sa cellule correspondante à jour (voir la *remarque* ci-dessous pour la mise à jour).
Chaque grille aura donc **width**+2 cellules en largeur et éventuellement **height**+2 cellules en
hauteur (si on découpe dans les deux directions). 

Ne pas oublier de regrouper les données des cellules fantômes qu'on enverra.

Remarques
---------
- *Faire attention cependant, car une planète qui devient à la fois habitée et habitable (non habitée)
sera toujours habitée. Par contre, une planète qui devient à la fois habitée et inhabitable sera
toujours inhabitable.*

- Cette technique de découpage du domaine de calcul par processeur est un grand classique du calcul haute performance, nommée *décomposition de domaines*. La stratégie des cellules fantômes est une des solutions possibles pour gérer le raccordement entre les domaines tout en minimisant le nombre d'échanges MPI.


# Travail à rendre

  - Le code parallélisé en mémoire partagée ET en mémoire distribuée (le même code).
  - Un rapport où on parlera des éventuels conflits mémoires et pourquoi ils ne posent pas de problème,
    ainsi que le calcul des accélérations (speedup) obtenus en mémoire partagée uniquement, en mémoire
    distribuée uniquement et en mélangeant les deux parallélisations.
