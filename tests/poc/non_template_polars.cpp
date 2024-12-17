//
// Created by frongere on 17/12/24.
//

#include <iostream>
#include <vector>

/**
 * Vers un refonte sans template de POEM
 *
 * Problème:
 *  Les templates de dimension de polaires sont une pression technique tres forte et empechent certaines
 *  manipulations de reduction comme le squeeze, compliquent les interpolation etc...
 *
 *  Pour faire un poem sans template (sauf de type):
 *
 *  Dimension -> une dimension nommée (on pourra renommer en coordinate) ?
 *  DimensionSet -> un jeu de dimensions qui definit la dimension de la polaire ainsi que l'ordre des dimensions
 *
 *  DimensionPoint est associe a un DimensionSet et est un vecteur de valeurs de coordonnées
 *
 *  DimensionGrid definit la grille en cartesien (vecteurs de valeurs pour chaque dimension). Il permet de generer
 *  un DimensionPointSet par produit cartesien de ces vecteurs.
 *
 *  DimensionPointSet definit de maniere ordonnee mais flat la grille de la polaire. Associe egalement
 *
 *  PolarTable est une table ND, interpolable etc...
 *
 *
 *
 *
 *
 *
 *
 */


class PolarTableBase {


};


template<typename T>
class PolarTable : public PolarTableBase {
 public:
  /*
   * On veut construire une polarTable a partir d'un DimensionPointSet
   * Ce dernier doit pouvoir donner sa taille (nombre d'elements) et son shape
   *
   *
   */

  PolarTable(size_t dim) :
      m_dim(dim) {

  }

 private:
  size_t m_dim;
  std::vector<T> m_data;
};


int main() {

  PolarTable<double> polar_table(3);


  return 0;
}