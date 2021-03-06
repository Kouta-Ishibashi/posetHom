/////////////////////////////////////////////////////////////////////////////
///
/// @file simpleHom.cpp
///
/// This file is the main program of simpleHom.
/// Using the maximal pimplex data in a text, compute homology
/// and whitney class if defined.
///
/// @auther Kota Ishibashi
///
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2015-2016 by Kota Ishibashi
//
// This file is part of the posetHom program.  This is free software;
// you can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation;
// either version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this software; see the file "license.txt".  If not, write to the
// Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
// MA 02111-1307, USA.

// Started in 2015. Last revision: January 29, 2016.

const char *title = "\
posetHom, ver. 1.0.1, 16/01/29. Copyright (C) 2015-2016 by Kota Isibashi.\n\
This is free software. No warranty. Consult 'license.txt' for details.";



#include "include/Z2integer.h"
#include "include/chain.h"
#include "include/Z2matrix.h"
int main(int argc,char *argv[])
{
  std::cout << title << std::endl;
  std::cout <<  std::endl;
  if (argc == 1){
    std::cout << "argument is missing. The number of argument should be only one: text file name." << std::endl;
    return 0;
  }
  if (argc > 2)
    {
      std::cout << "Too many argument.The number of argument should be only one: text file name." << std::endl;
      return 0;
    }

  std::string filename = argv[1];



  // TODO: isDataValid for maximal simplex version.
  bool valid = isValidSimplex(filename);
  if (!valid){
    std::cerr << filename << ": Invalid Data, please check its content." << std::endl;
    return 0;
  }

  // TODO: isEuler for simplex version.

  // maximalChains(maximalSimplex) is obteined directly from the data.

  std::vector<std::vector<int>> maximalSimplexList = getMaximalSimplex(filename);
  std::vector<std::vector<int>> simpleces;

  simpleces = getChainComplexGenerators(maximalSimplexList);

  // make vertexList and arrowsList from maximalSimplex this means 0-dim simplexList and 1-dim simplexList

  // int numArrow = numberOfArrow(filename);

  std::vector<int> vertexList;
  std::vector<std::array<int,2>> arrowList;
  makeVertexAndArrowList(simpleces,vertexList,arrowList);
  int numberOfArrows = arrowList.size();
  // vertexSet(arrowList,numArrow,vertexList);
  bool euler = isEulerSimpComp(simpleces);
  if (euler)
    {
      std::cout << "This is Euler. " << std::endl;
    } else {
    std::cout << "This is NOT Euler. " << std::endl;
  }


  Z2matrix boundary = Z2matrix();
  boundary = makeBoundaryMatrix(simpleces);

  Z2matrix boundaryOrigin = Z2matrix();
  boundaryOrigin = boundary;

  Z2matrix v = Z2matrix();
  v.identity(boundary.getncols());
  boundary.reduceBoundary(boundary,v);

  /* --- showing matrix for debug --- */
  // std::cout << "boundaryOrigin" << std::endl;
  // showAsMatrix(boundaryOrigin);

  // std::cout << "boundary" << std::endl;
  // showAsMatrix(boundary);

  // std::cout << "basechange" << std::endl;
  // showAsMatrix(v);

  showHomology(boundary,simpleces);
  std::vector<int> generatorIndexList = {};
  showHomologyGenerator(boundary,v,simpleces,generatorIndexList);

  Z2matrix inv = v;
  inv.invert();

  Z2matrix image = Z2matrix();
  image.multiply(inv,boundary);
  // std::cout << "image" << std::endl;
  // showAsMatrix(image);

  if(euler){
    chain whitneyChianBefore = chain();
    for (int i = 0; i < dimention(simpleces); ++i)
      {
        whitneyChianBefore.add(makeWhitneyChain(i,simpleces,vertexList,arrowList,numberOfArrows));
      }

    Z2matrix c = Z2matrix();
    c = action(inv,whitneyChianBefore);
    chain whitneyChainAfter = c.getcol(0);

    detectWhitneyClass(whitneyChainAfter, image,generatorIndexList);
    std::vector<chain> whitneyGeneratorList = {};
    showWhitneyClass(whitneyChainAfter,simpleces,generatorIndexList);
    std::cout << std::endl;
  }
  return 0;
}
