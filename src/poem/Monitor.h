//
// Created by frongere on 08/03/25.
//

#ifndef POEM_MONITOR_H
#define POEM_MONITOR_H

#include "cools/pbar/Timer.h"


#ifdef POEM_MON

namespace poem {
  struct Monitor {

    /*
     * Nombre d'acces total
     * temps moyen d'une interpolation
     * temps moyen d'un nearest
     * durée depuis le dernier accès
     * Temps moyen entre 2 acces -> frequence des acces
     * Nombre de tables interrogees (Polar)
     * taille totale objet -> sizeof(*this)
     */



    int nb_access;
    cools::Timer timer;



  };
}

#endif // POEM_MON

#endif //POEM_MONITOR_H
