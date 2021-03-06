/////////////////////////////////////////////////////////////////////////////
///
/// @file poset_convert.h
///
/// This file contains functions related to converters or tools to convert.
///
/// @author Kota Ishibashi
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

#ifndef POSET_CONVERT_H
#define POSET_CONVERT_H


#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <regex>
#include <vector>
#include <array>
#include <set>
#include <map>

//^\n
bool isComment(std::string str){
    std::regex comment(";.+");
    return std::regex_match(str,comment);
      }

bool isArrow(std::string str){
  std::regex arrow("\\{[0-9]+,[0-9]+.",std::regex_constants::extended);
  std::regex arrowEd(".+}",std::regex_constants::extended);
  return std::regex_match(str,arrow) && std::regex_match(str,arrowEd);
    }

bool isSimplex(std::string str){
  std::regex smpl("\\{([0-9]+,)*[0-9]+\\}");
  return std::regex_match(str,smpl);
}


bool isDataValid(std::string filename,int& lineNumber){
  std::ifstream ifs(filename);
  std::string str;
  if (ifs.fail())
    {
      std::cout << "File open is failed." << std::endl;
      return false;
    }
  while(getline(ifs,str))
    {
      if (isComment(str)){
        /* ignore comment*/
      }else if (isArrow(str))
        {
          lineNumber += 1;
        } else {
        std::cerr << "something is wrong with line: " << lineNumber << std::endl;
        return false;
      }
    }
  return true;
}

bool isDataValid(std::string filename){
  std::ifstream ifs(filename);
  std::string str;
  if (ifs.fail())
    {
      return false;
    }
  while(getline(ifs,str))
    {
      if (!isComment(str) && !isArrow(str))
        {
        return false;
      }
    }
  return true;
}

bool isValidSimplex(std::string filename){
  std::ifstream ifs(filename);
  std::string str;
  if (ifs.fail())
    {
      return false;
    }
  while(getline(ifs,str))
    {
      if (!isComment(str) && !isSimplex(str))
        {
        return false;
      }
    }
  return true;
}



std::string trim(std::string str){
  std::string trimed = str;
  if (str.find_first_of("{")==0){
    trimed = str.erase(0,1);
  }
  if(str.find_last_of("}")==str.size()-1){
    trimed = trimed.erase(trimed.size()-1);
  }
  return trimed;
}

std::string fileNameTrim(std::string str){
  std::string trimed = str;
  trimed = trimed.erase(str.find_first_of("."));
  return trimed;
}


int str_to_int(std::string str){
  int ret = -1;
  sscanf(str.c_str(), "%d",&ret);
  return ret;
}

/* x \in X or not */
bool isIn(std::vector<int> list,int n){
  for (auto x:list)
    {
      if (n == x){
        return true;
      }
    }
  return false;
}


/* return the true when B is subset of A and dimB=dimA-1  B < A*/
bool isFace(std::vector<int> listA,std::vector<int> listB){
  if(listB.size() != listA.size() - 1) return false;
  for (auto x:listB)
    {
      if (!isIn(listA,x)){
        return false;
      }
    }
  return true;
}

/* return the true when B is subset of A ; B<A */
bool isSubset(std::vector<int> listA,std::vector<int> listB){
  for (auto x:listB)
    {
      if (!isIn(listA,x)){
        return false;
      }
    }
  return true;
}



std::vector<int> maximals(std::vector<std::array<int,2>> arrowList/* int arrowList[][2] */,int numberOfArrows){
  std::vector<int> maximalList;
  int frag = 1;
  for (int i = 0; i < numberOfArrows; ++i)
    {
      int cand = arrowList[i][1];
      frag = 1;
      for (int j = 0; j < numberOfArrows; ++j)
        {
          if (i != j){
            if (cand == arrowList[j][0]){
              frag = frag * 0;
            }
          }
        }
      if (frag == 1)
        {
          if (!isIn(maximalList,cand))
            {
              maximalList.push_back(cand);
            }
        }
    }
  return maximalList;
}

std::vector<int> minimals(std::vector<std::array<int,2>> arrowList/* int arrowList[][2] */,int numberOfArrows){
  std::vector<int> minimalList;
  int frag = 1;
  for (int i = 0; i < numberOfArrows; ++i)
    {
      int cand = arrowList[i][0];
      frag = 1;
      for (int j = 0; j < numberOfArrows; ++j)
        {
          if (i != j){
            if (cand == arrowList[j][1]){
              frag = frag * 0;
            }
          }
        }
      if (frag == 1)
        {
          if (!isIn(minimalList,cand))
            {
              minimalList.push_back(cand);
            }
        }
    }
  return minimalList;
}

void vertexSet(std::vector<std::array<int,2>> arrowList/* int arrowList[][2] */,int numberOfArrows,std::vector<int>& vertexList){
  for (int i = 0; i < numberOfArrows; ++i)
    {
      vertexList.push_back(arrowList[i][0]);
      vertexList.push_back(arrowList[i][1]);
    }
  std::sort(vertexList.begin(), vertexList.end());
  vertexList.erase(std::unique(vertexList.begin(), vertexList.end()), vertexList.end());
}

void vertexPairSet(std::vector<std::array<int,2>> arrowList/* ,int arrowList[][2] */,int numberOfArrows,std::vector<std::vector<int>>& pairList){
  std::vector<int> vertexList;
  vertexSet(arrowList,numberOfArrows,vertexList);
  int k = 0;
  for (int i = 0 ; i < vertexList.size()-1; ++i)
    {
      for (int j = i + 1; j < vertexList.size(); ++j)
        {
          pairList[k][0] = vertexList[i];
          pairList[k][1] = vertexList[j];
          ++k;
        }
    }
}


std::vector<int> nexts(std::vector<std::array<int,2>> arrowList/* ,int arrowList[][2] */,int numberOfArrows,int n){
  std::vector<int> nextList;
  for (int i = 0; i < numberOfArrows; ++i)
    {
      if (arrowList[i][0] == n)
        {
          nextList.push_back(arrowList[i][1]);
        }
    }
  return nextList;
}

std::vector<int> previous(std::vector<std::array<int,2>> arrowList/* int arrowList[][2] */,int numberOfArrows,int n){
  std::vector<int> previousList;
  for (int i = 0; i < numberOfArrows; ++i)
    {
      if (arrowList[i][1] == n)
        {
          previousList.push_back(arrowList[i][0]);
        }
    }
  return previousList;
}


void biggerMake(std::vector<std::array<int,2>> arrowList/* int arrowList[][2] */,int numberOfArrows,std::vector<int> vertexSet,std::vector<int> &bigger,int v){
  std::vector<int> maximalList = maximals(arrowList,numberOfArrows);
  if (isIn(maximalList,v))
    {
    } else {
    for(auto n: nexts(arrowList,numberOfArrows,v))
      {
        bigger.push_back(n);
        biggerMake(arrowList,numberOfArrows,vertexSet,bigger,n);
      }
  }
  std::sort(bigger.begin(), bigger.end());
  bigger.erase(std::unique(bigger.begin(), bigger.end()), bigger.end());
}

std::vector<int> biggerVertex(std::vector<std::array<int,2>> arrowList/* int arrowList[][2] */,int numberOfArrows,std::vector<int> vertexSet,int v){
  std::vector<int> bigger;
  biggerMake(arrowList,numberOfArrows,vertexSet,bigger,v);
  return bigger;
}

void smallerMake(std::vector<std::array<int,2>> arrowList/* int arrowList[][2] */,int numberOfArrows,std::vector<int> vertexSet,std::vector<int> &smaller,int v){
  std::vector<int> minimalList = minimals(arrowList,numberOfArrows);
  if (isIn(minimalList,v))
    {
    } else {
    for(auto n: previous(arrowList,numberOfArrows,v))
      {
        smaller.push_back(n);
        smallerMake(arrowList,numberOfArrows,vertexSet,smaller,n);
      }
  }
  std::sort(smaller.begin(), smaller.end());
  smaller.erase(std::unique(smaller.begin(), smaller.end()), smaller.end());
}

std::vector<int> smallerVertex(std::vector<std::array<int,2>> arrowList/* int arrowList[][2] */,int numberOfArrows,std::vector<int> vertexSet,int v){
  std::vector<int> smaller;
  smallerMake(arrowList,numberOfArrows,vertexSet,smaller,v);
  return smaller;
}

std::vector<int> intersectionSet(std::vector<int> x,std::vector<int> y){
  std::vector<int> intersec;
  for (auto v:x)
    {
      if (isIn(y,v))
        {
          intersec.push_back(v);
        }
    }
  return intersec;
}

std::vector<int> unionSet(std::vector<int> x,std::vector<int> y){
  std::vector<int> uni;
  for (auto v:x)
    {
      uni.push_back(v);
    }

    for (auto w:y)
    {
      uni.push_back(w);
    }
    std::sort(uni.begin(), uni.end());
    uni.erase(std::unique(uni.begin(), uni.end()), uni.end());
  return uni;
}


std::vector<int> betweenVertex(std::vector<std::array<int,2>> arrowList/* int arrowList[][2]*/,int numberOfArrows,std::vector<int> vertexSet,int x,int y){
  std::vector<int> biggerX = biggerVertex(arrowList,numberOfArrows,vertexSet,x);
  std::vector<int> biggerY = biggerVertex(arrowList,numberOfArrows,vertexSet,y);
  std::vector<int> smallerX = smallerVertex(arrowList,numberOfArrows,vertexSet,x);
  std::vector<int> smallerY = smallerVertex(arrowList,numberOfArrows,vertexSet,y);
  return unionSet(intersectionSet(biggerX,smallerY),intersectionSet(smallerX,biggerY));
}

int extends(std::vector<std::array<int,2>> arrowList/* int arrowList[][2] */, int numberOfArrows,int currentPoint, std::vector<int> &currentSeed,std::vector<std::vector<int>> &resultList){
  if (isIn(maximals(arrowList,numberOfArrows),currentPoint))
    {
      resultList.push_back(currentSeed);
    } else {
    if (nexts(arrowList,numberOfArrows,currentPoint).empty())
      {
      } else {
      for (auto n:nexts(arrowList,numberOfArrows,currentPoint))
        {
          std::vector<int> tmp_currentSeed = currentSeed;
          tmp_currentSeed.push_back(n);
          extends(arrowList,numberOfArrows,n,tmp_currentSeed,resultList);
        }
    }
  }
}


void maximalChains(std::vector<std::array<int,2>> arrowList/* int arrowList[][2] */,int numberOfArrows,std::vector<std::vector<int>> &resultList){
  std::vector<int> minlist = minimals(arrowList,numberOfArrows);
  int size = minlist.size();
  for (int i = 0; i < size; ++i)
    {
       std::vector<int> currentSeed = {minlist[i]};
       extends(arrowList,numberOfArrows,minlist[i],currentSeed,resultList);
    }
}


int numberOfArrow(std::string filename) {
  int num = 0;
  if(isDataValid(filename,num))
    {
      return num;
    }
    return 0;
}

void makeArrowList(std::string filename,std::vector<std::array<int,2>> &arrowList/* int arrowList[][2] */){
  int numberOfArrows = 0;
  if(isDataValid(filename, numberOfArrows)){

    std::ifstream ifs(filename);
    std::string str;
    int arrowIndex = 0;
    while(getline(ifs,str))
      {
        if(isArrow(str))
          {
            std::string tmp;
            std::istringstream strm(str);
            int vertexIndex = 0;
            std::array<int,2> test;
            while(getline(strm,tmp,','))
              {
                /* arrowList[arrowIndex][vertexIndex] = str_to_int(trim(tmp));*/
                /* arrowList[vertexIndex][arrowIndex] = str_to_int(trim(tmp)); */
                test[vertexIndex] = str_to_int(trim(tmp));
                vertexIndex += 1;
              }
            arrowList.push_back(test);
          }
      }
    return;
  }
  return;
}

/* if a < b  in P ,then return true. */
bool greater(int a,int b,std::vector<int> vertexSet,std::vector<std::array<int,2>> arrowList /* int arrowList[][2] */,int numberOfArrows){
  if (!(isIn(vertexSet,a) & isIn(vertexSet,b)))
    {
      std::cerr << "Nor first and second integer is included vertexSet";
    }
  std::vector<int> bigger;
  bigger = biggerVertex(arrowList,numberOfArrows,vertexSet,a);
  return isIn(bigger,b);
}


void remove(std::vector<int>& vector, int index)
{
    vector.erase(vector.begin() + index);
}


std::vector<std::vector<int>> boundary(std::vector<int> chain){
  std::vector<std::vector<int>> boundaryList;
  int dim = chain.size();
  for (int i = 0; i < dim; ++i)
    {
      std::vector<int> tmpChain = chain;
      remove(tmpChain,i);
      boundaryList.push_back(tmpChain);
    }
  return boundaryList;
}

std::string vecToVer(std::vector<int> vec){
  std::ostringstream stream;
  stream << "{";
  for (int i = 0; i < vec.size()-1; ++i)
    {
      stream << vec[i];
      stream << ",";
    }
  stream << vec[vec.size()-1];
  stream << "}";
  return stream.str();
}

int vectorIndex(std::vector<std::vector<int>> list, std::vector<int> element){
  std::vector<std::vector<int>>::iterator it = std::find(list.begin(),list.end(),element);
  if (it != list.end()){
    return std::distance(list.begin(),it);
  } else {
    return -1;
  }
}

std::string vectorsToString(std::vector<int> former,std::vector<int> latter,std::vector<std::vector<int>> newVertexList){
  std::ostringstream stream;
  std::string retString;
  stream << "{";
  stream << vectorIndex(newVertexList,former);
  stream << ",";
  stream << vectorIndex(newVertexList,latter);
  stream << "}";

  return stream.str();
}


void getAllBoundary(std::vector<std::vector<int>> &newVertexList, std::vector<int> maximalChain){
  newVertexList.push_back(maximalChain);
  if (maximalChain.size() != 1)
    {
      for (auto v : boundary(maximalChain))
        {
          newVertexList.push_back(v);
          getAllBoundary(newVertexList,v);
        }
    }
    std::sort(newVertexList.begin(), newVertexList.end(),[](const std::vector<int>& a,const std::vector<int>& b){
      return a[0] < b[0];
    });

  std::sort(newVertexList.begin(), newVertexList.end(),[](const std::vector<int>& a,const std::vector<int>& b){
      if(a.size() == b.size()) {
        for (int i = 0; i < a.size(); ++i)
          {
            if (a[i]!=b[i])
              {
                return a[i] < b[i];
              }
          }
      } else {
        return a.size() < b.size();
      }});
  newVertexList.erase(std::unique(newVertexList.begin(), newVertexList.end()), newVertexList.end());
}

int dimention(std::vector<std::vector<int>> resultList){
  int max = 0;
  for(auto x:resultList){
    if (x.size() > max){
      max = x.size();
    }
  }
  return max;
}

void showVector(std::vector<int> vec){
  std::cout <<"{"<< std::flush;
  for (int i = 0; i < vec.size()-1; ++i)
    {
      std::cout << vec[0] <<"," << std::flush;
    }
  std::cout << vec[vec.size()] << "}"<< std::endl;
  std::cout << std::endl;
}

void showChainComplexGenerator(int dim,std::vector<std::vector<int>> newresultList){
  if (dim > dimention(newresultList))
    {
      std::cout << "dimention is too large." << std::endl;
    } else {
    std::vector<std::vector<int>> possibleGen;
    for (auto  v:newresultList)
      {
        getAllBoundary(possibleGen,v);
      }

    for (auto g:possibleGen){
      if(g.size()==dim){
        showVector(g);
      }
    }
  }
}

std::vector<std::vector<int>> getChainComplexGenerators(std::vector<std::vector<int>> newresultList){
  std::vector<std::vector<int>> possibleGen;
  for (auto  v:newresultList)
    {
      getAllBoundary(possibleGen,v);
    }
  return possibleGen;
}


/* getMaximalSimplex */

std::vector<std::vector<int>> getMaximalSimplex(std::string file){
  std::string filename = file;
  std::vector<std::vector<int>> resultList;
  int numberOfArrows = 0;

  if(isValidSimplex(file)){
    std::ifstream ifs(filename);
    std::string str;

    while(getline(ifs,str))
      {
        std::string tmp;
        std::istringstream strm(str);
        int vertexIndex = 0;
        std::vector<int> simplex;
        while(getline(strm,tmp,','))
          {
            simplex.push_back(str_to_int(trim(tmp)));
            vertexIndex += 1;
          }
        resultList.push_back(simplex);
      }
    return resultList;
  } else {
    std::cerr << "Data is not valid.";
  }
}







/* getMaximalChains */

std::vector<std::vector<int>> getMaximalChains(std::string simplexFile){
  std::string filename = simplexFile;
  int numberOfArrows = 0;
  if(isDataValid(filename, numberOfArrows)){
    /* int arrowList[numberOfArrows][2]; */
    std::vector<std::array<int,2>> arrowList;

    std::ifstream ifs(filename);
    std::string str;
    int arrowIndex = 0;
    while(getline(ifs,str))
      {
        if(isArrow(str))
          {
            std::string tmp;
            std::istringstream strm(str);
            int vertexIndex = 0;
            std::array<int,2> test;
            while(getline(strm,tmp,','))
              {
                /* arrowList[arrowIndex][vertexIndex] = str_to_int(trim(tmp));*/
                /* arrowList[vertexIndex][arrowIndex] = str_to_int(trim(tmp)); */
                test[vertexIndex] = str_to_int(trim(tmp));
                vertexIndex += 1;
              }
            arrowList.push_back(test);
          }
      }
    std::vector<std::vector<int>> resultList;
    maximalChains(arrowList,numberOfArrows,resultList);
    return resultList;
  } else {
    std::cerr << "Data is not valid.";
  }
}

void makeVertexAndArrowList(std::vector<std::vector<int>> simpleces,std::vector<int> &vertexList,std::vector<std::array<int,2>> &arrowList){
  for (int i = 0 ; i < simpleces.size() ; ++i)
    {
      if(simpleces[i].size() == 1){
        vertexList.push_back(simpleces[i][0]);
      } else if (simpleces[i].size()==2){
        std::array<int,2> tmp;
        tmp[0] = simpleces[i][0];
        tmp[1] = simpleces[i][1];
        arrowList.push_back(tmp);
      }
    }
}


/* first index which represent p-dim simplex */
int beginNum(int p,std::vector<std::vector<int>> simpleces){
  for (int i = 0; i < simpleces.size(); ++i)
    {
      if (simpleces[i].size() == p)
        {
          return i;
        }
    }
}

/* last index which represent p-dim simplex*/
int endNum(int p,std::vector<std::vector<int>> simpleces){
  for (int i = 0; i < simpleces.size(); ++i)
    {
      if (simpleces[i].size() == p + 1)
        {
          return i-1;
        }
    }
}

/* totally oreder subsets with vertrces bigger than v.*/
std::vector<std::vector<int>> biggerSimplex(std::vector<std::array<int,2>> arrowList/* int arrowList[][2] */,int numberOfArrows,std::vector<int> vertex,std::vector<std::vector<int>> simpleces, int v){
  std::vector<std::vector<int>> resultList;
  for (auto ch:simpleces)
    {
      if (isIn(biggerVertex(arrowList,numberOfArrows,vertex,v),ch[0])) resultList.push_back(ch);
    }
  return resultList;
}

/* totally oreder subsets with vertrces smaller than v.*/
std::vector<std::vector<int>> smallerSimplex(std::vector<std::array<int,2>> arrowList/* int arrowList[][2] */,int numberOfArrows,std::vector<int> vertex,std::vector<std::vector<int>> simpleces, int v){
  std::vector<std::vector<int>> resultList;
  for (auto ch:simpleces)
    {
      if (isIn(smallerVertex(arrowList,numberOfArrows,vertex,v),ch[ch.size()-1])) resultList.push_back(ch);
    }
  return resultList;
}

/* totally oreder subsets with vertrices between v & w.*/
std::vector<std::vector<int>> betweenSimplex(std::vector<std::array<int,2>> arrowList/* int arrowList[][2] */,int numberOfArrows,std::vector<int> vertex,std::vector<std::vector<int>> simpleces, int v , int w){
  std::vector<std::vector<int>> resultList;
  for (auto ch:simpleces)
    {
      if (isIn(smallerVertex(arrowList,numberOfArrows,vertex,v),ch[ch.size()-1])
          && isIn(smallerVertex(arrowList,numberOfArrows,vertex,w),ch[ch.size()-1])
          && isIn(biggerVertex(arrowList,numberOfArrows,vertex,v),ch[0])
          && isIn(biggerVertex(arrowList,numberOfArrows,vertex,w),ch[0])
          ) resultList.push_back(ch);
    }
  return resultList;
}


/*
  euler poset or not
*/
bool isEuler(std::vector<std::array<int,2>> arrowList/* int arrowList[][2] */,int numberOfArrows,std::vector<std::vector<int>> simpleces){
  int sum = 0;
  std::vector<int> vertex;
  vertexSet(arrowList,numberOfArrows,vertex);
  int n = vertex.size();
  int pairsNum = (n*(n-1))/2;
  std::vector< std::vector<int> > pair (pairsNum, std::vector<int>(2));
  vertexPairSet(arrowList,numberOfArrows,pair);
  for(auto v:vertex)
    {
      sum = 0;
      sum = biggerSimplex(arrowList,numberOfArrows,vertex,simpleces,v).size();
      sum = 0;

      if (sum % 2 == 1){
        return false;
      }
      sum = smallerSimplex(arrowList,numberOfArrows,vertex,simpleces,v).size();

      if (sum % 2 == 1){
        return false;
      }
    }
  for (int i = 0; i < pair.size();++i)
    {
      sum = 0;
      int x = pair[i][0];
      int y = pair[i][1];
      sum = betweenSimplex(arrowList, numberOfArrows, vertex, simpleces, x , y).size();
      if (sum % 2 == 1){
        return false;
      }
    }
  return true;
}

bool isEuler(std::string filename){
  int numberOfArrows = 0;
  if(isDataValid(filename, numberOfArrows)){
    /* int arrowList[numberOfArrows][2]; */
    std::vector<std::array<int,2>> arrowList;
    std::ifstream ifs(filename);
    std::string str;
    int arrowIndex = 0;
    while(getline(ifs,str))
      {
        if(isArrow(str))
          {
            std::string tmp;
            std::istringstream strm(str);
            int vertexIndex = 0;
            std::array<int,2> test;
            while(getline(strm,tmp,','))
              {
                /* arrowList[arrowIndex][vertexIndex] = str_to_int(trim(tmp));*/
                /* arrowList[vertexIndex][arrowIndex] = str_to_int(trim(tmp)); */
                test[vertexIndex] = str_to_int(trim(tmp));
                vertexIndex += 1;
              }
            arrowList.push_back(test);
          }
      }
    std::vector<std::vector<int>> maximalChains = getMaximalChains(filename);
    std::vector<std::vector<int>> simpleces = getChainComplexGenerators(maximalChains);
    return isEuler(arrowList,numberOfArrows,simpleces);
  }
}

bool isEulerSimpComp(std::vector<std::vector<int>> simpleces){
  int sum = 0;
  std::vector<int> vertexList;
  std::vector<std::array<int,2>> arrowList;

  makeVertexAndArrowList(simpleces,vertexList,arrowList);

  int n = vertexList.size();
  int numberOfArrows = arrowList.size();
  int pairsNum = (n*(n-1))/2;
  std::vector< std::vector<int> > pair (pairsNum, std::vector<int>(2));
  vertexPairSet(arrowList,numberOfArrows,pair);

  for(auto v:vertexList)
    {
      sum = 0;
      sum = biggerSimplex(arrowList,numberOfArrows,vertexList,simpleces,v).size();
      if (sum % 2 == 1){
        return false;
      }
      sum = 0;
      sum = smallerSimplex(arrowList,numberOfArrows,vertexList,simpleces,v).size();
      if (sum % 2 == 1){
        return false;
      }
    }
  for (int i = 0; i < pair.size();++i)
    {
      sum = 0;
      int x = pair[i][0];
      int y = pair[i][1];
      sum = betweenSimplex(arrowList, numberOfArrows, vertexList, simpleces, x , y).size();
      if (sum % 2 == 1){
        return false;
      }
    }
  return true;
}


/*
 euler judegement:another edition
*/

/* bool isEuler(int arrowList[][2],int numberOfArrows){ */
/*   int sum = 0; */
/*   std::vector<int> vertex; */
/*   vertexSet(arrowList,numberOfArrows,vertex); */
/*   int n = vertex.size(); */
/*   int pairsNum = (n*(n-1))/2; */
/*   std::vector< std::vector<int> > pair (pairsNum, std::vector<int>(2)); */
/*   vertexPairSet(arrowList,numberOfArrows,pair); */

/*   for(auto v:vertex) */
/*     { */
/*       sum = 0; */
/*       sum = biggerVertex(arrowList,numberOfArrows,vertex,v).size(); */
/*       if (sum % 2 == 1){ */
/*         return false; */
/*       } */
/*       sum = 0; */
/*       sum = smallerVertex(arrowList,numberOfArrows,vertex,v).size(); */
/*       if (sum % 2 == 1){ */
/*         return false; */
/*       } */
/*     } */
/*   for (int i = 0; i < pair.size();++i) */
/*     { */
/*       sum = 0; */
/*       int x = pair[i][0]; */
/*       int y = pair[i][1]; */
/*       sum = betweenVertex(arrowList,numberOfArrows,vertex,x,y).size(); */
/*       if (sum % 2 == 1){ */
/*         return false; */
/*       } */
/*     } */
/*   return true; */
/* } */

/* bool isEuler(std::string filename){ */
/*   int numberOfArrows = 0; */
/*   if(isDataValid(filename, numberOfArrows)){ */
/*     int arrowList[numberOfArrows][2]; */
/*     std::ifstream ifs(filename); */
/*     std::string str; */
/*     int arrowIndex = 0; */
/*     while(getline(ifs,str)) */
/*       { */
/*         if(isArrow(str)) */
/*           { */
/*             std::string tmp; */
/*             std::istringstream strm(str); */
/*             int vertexIndex = 0; */
/*             while(getline(strm,tmp,',')) */
/*               { */
/*                 arrowList[arrowIndex][vertexIndex] = str_to_int(trim(tmp)); */
/*                 vertexIndex += 1; */
/*               } */
/*             arrowIndex += 1; */
/*           } */
/*       } */
/*     return isEuler(arrowList,numberOfArrows); */
/*   } */
/* } */


/*
   converter from poset to top simpleces
   this is made for a ChomP's program "homsimpl"
   result is preserved as ***poset.txt
 */
/* void convert(std::string file){ */
/*   std::string filename = file; */
/*   int numberOfArrows = 0; */
/*   if(isDataValid(filename, numberOfArrows)){ */
/*     int arrowList[numberOfArrows][2]; */
/*     std::ifstream ifs(filename); */
/*     std::string str; */
/*     int arrowIndex = 0; */
/*     while(getline(ifs,str)) */
/*       { */
/*         if(isArrow(str)) */
/*           { */
/*             std::string tmp; */
/*             std::istringstream strm(str); */
/*             int vertexIndex = 0; */
/*             while(getline(strm,tmp,',')) */
/*               { */
/*                 arrowList[arrowIndex][vertexIndex] = str_to_int(trim(tmp)); */
/*                 vertexIndex += 1; */
/*               } */
/*             arrowIndex += 1; */
/*           } */
/*       } */
/*     std::vector<std::vector<int>> resultList; */
/*     maximalChains(arrowList,numberOfArrows,resultList); */
/*     std::string posetfile = fileNameTrim(filename) + "poset.txt"; */
/*     std::fstream poset(posetfile,std::ios::out); */
/*     if(poset.fail()) */
/*       { */
/*         std::cout << "output file "<< posetfile <<" coundn't open. " << std::endl; */
/*         return; */
/*       } */
/*     for (int i = 0; i < resultList.size(); ++i) */
/*       { */
/*         poset << "{" << std::flush; */
/*         for (int j = 0; j < resultList[i].size()-1; ++j) */
/*           { */
/*             poset << resultList[i][j]<< "," << std::flush; */
/*           } */
/*         poset << resultList[i][resultList[i].size()-1]<< "}" << std::endl; */
/*       } */
/*   /\* maximal chains are known in resultList *\/ */
/*     if(isEuler(arrowList,numberOfArrows)){ */
/*         std::cout << "This is Euler!"<< std::endl; */
/*         std::cout << "We make further convertion to the subdivided form before computation, especially to compute whitney classes." << std::endl; */

/*         /\* make the subdivision *\/ */
/*         std::vector<std::vector<int>> newVertexList; */
/*         for (auto x:resultList) */
/*           { */
/*             getAllBoundary(newVertexList,x); */
/*           } */
/*         std::vector<std::string> newArrowList; */
/*         int k = 0; */
/*         std::vector<std::vector<int>> seedList; */
/*         for (auto v:newVertexList) */
/*           { */
/*             if (v.size()>1) */
/*               { */
/*                 seedList.push_back(v); */
/*               } */
/*           } */
/*         /\* we got (inf subdivided sense) vertexList *\/ */
/*         /\* we make the new arrow list and save it tmp file *\/ */
/*         for (auto m:seedList) */
/*           { */
/*             for(auto b:boundary(m)) */
/*               { */
/*                 newArrowList.push_back(vectorsToString(b,m,newVertexList)); */
/*               } */
/*           } */
/*         std::sort(newArrowList.begin(), newArrowList.end()); */
/*         newArrowList.erase(std::unique(newArrowList.begin(), newArrowList.end()), newArrowList.end()); */
/*         std::cout << "We renamed vertrices as follow:" << std::endl; */

/*         /\*output of the vertrices rename*\/ */
/*         for (auto x:newVertexList) */
/*           { */
/*             std::cout << vecToVer(x) << " --> " << vectorIndex(newVertexList,x) << std::endl; */
/*           } */

/*         std::string sdfile = fileNameTrim(filename) + "SD.txt"; */
/*         std::fstream sd(sdfile,std::ios::out); */
/*         if(sd.fail()){ */
/*           std::cout << "outputfile couldn't open." << std::endl; */
/*           return; */
/*         } */
/*         for (auto st:newArrowList) */
/*           { */
/*             sd << st << std::endl; */
/*           } */
/*         /\* subdivided text file is steady as sdfile *\/ */
/*         int numberOfArrows = 0; */
/*         if(isDataValid(sdfile, numberOfArrows)){ */
/*           int newarrowList[numberOfArrows][2]; */
/*           std::ifstream newifs(sdfile); */
/*           std::string newstr; */
/*           int arrowIndex = 0; */
/*           /\* making the arrowList*\/ */
/*           while(getline(newifs,newstr)) */
/*             { */
/*               if(isArrow(newstr)) */
/*                 { */
/*                   std::string newtmp; */
/*                   std::istringstream newstrm(newstr); */
/*                   int vertexIndex = 0; */
/*                   while(getline(newstrm,newtmp,',')) */
/*                     { */
/*                       newarrowList[arrowIndex][vertexIndex] = str_to_int(trim(newtmp)); */
/*                       vertexIndex += 1; */
/*                     } */
/*                   arrowIndex += 1; */
/*                 } */
/*             } */
/*           /\*write maximal list in ***poset.txt*\/ */
/*           std::vector<std::vector<int>> newresultList; */
/*           maximalChains(newarrowList,numberOfArrows,newresultList); */
/*           std::string posetfile = fileNameTrim(filename) + "poset.txt"; */
/*           std::fstream poset(posetfile,std::ios::out); */
/*           if(poset.fail()) */
/*             { */
/*               std::cout << "output file,\"poset.txt\" coundn't open. " << std::endl; */
/*               return; */
/*             } */

/*           for (int i = 0; i < newresultList.size(); ++i) */
/*             { */
/*               poset << "{" << std::flush; */
/*               for (int j = 0; j < newresultList[i].size()-1; ++j) */
/*                 { */
/*                   poset << newresultList[i][j]<< "," << std::flush; */
/*                 } */
/*               poset << newresultList[i][newresultList[i].size()-1]<< "}" << std::endl; */
/*             } */
/*           std::cout << "--------------------------converted--------------------------" << std::endl; */
/*           /\*show chain complex's generator of each dimention*\/ */
/*           for (int i = 0; i < dimention(newresultList);++i) */
/*             { */
/*               std::cout << "dimension " << i <<" generators:" << std::endl; */
/*               showChainComplexGenerator(i,newresultList); */
/*             } */
/*           std::cout << "dimension " << dimention(newresultList) <<" generators:" << std::endl; */
/*           for (auto c:newresultList) */
/*             { */
/*               showVector(c); */
/*             } */
/*         }else */
/*           { */
/*             std::cerr << "Something is wrong."; */
/*             return; */
/*           } */
/*         //--- case of euler --- */
/*     }else{ */
/*       std::cout << "This is NOT Euler"<< std::endl; */
/*       std::cout << "We compute the homology and its generator."<< std::endl; */
/*       std::vector<std::vector<int>> resultList; */
/*       maximalChains(arrowList,numberOfArrows,resultList); */
/*       std::string posetfile = fileNameTrim(filename) + "poset.txt"; */
/*       std::fstream poset(posetfile,std::ios::out); */
/*       if(poset.fail()) */
/*         { */
/*           std::cout << "output file,\"poset.txt\" coundn't open. " << std::endl; */
/*           return; */
/*         } */
/*       for (int i = 0; i < resultList.size(); ++i) */
/*         { */
/*           poset << "{" << std::flush; */
/*           for (int j = 0; j < resultList[i].size()-1; ++j) */
/*             { */
/*               poset << resultList[i][j]<< "," << std::flush; */
/*             } */
/*           poset << resultList[i][resultList[i].size()-1]<< "}" << std::endl; */
/*         } */
/*     } */
/*   } else { */
/*     std::cerr << "Something is wrong."; */
/*     return; */
/*   } */
/*   return; */
/* } */


/*
  converter from poset to top simpleces.
  this is made for a ChomP's program "homsimpl".
  if euler ,this make subdivision and rename vertrices.
  rename correspondant is recorded in conversionList
  result is preserved as ***SD.txt
 */

/* void sdConvert(std::string file,std::vector<std::string> &conversionList){ */
/*   std::string filename = file; */
/*   int numberOfArrows = 0; */
/*   if(isDataValid(filename, numberOfArrows)){ */
/*     int arrowList[numberOfArrows][2]; */
/*     std::ifstream ifs(filename); */
/*     std::string str; */
/*     int arrowIndex = 0; */
/*     while(getline(ifs,str)) */
/*       { */
/*         if(isArrow(str)) */
/*           { */
/*             std::string tmp; */
/*             std::istringstream strm(str); */
/*             int vertexIndex = 0; */
/*             while(getline(strm,tmp,',')) */
/*               { */
/*                 arrowList[arrowIndex][vertexIndex] = str_to_int(trim(tmp)); */
/*                 vertexIndex += 1; */
/*               } */
/*             arrowIndex += 1; */
/*           } */
/*       } */
/*     std::vector<std::vector<int>> resultList; */
/*     maximalChains(arrowList,numberOfArrows,resultList); */
/*     /\* maximal chains are known in resultList *\/ */
/*     if(isEuler(arrowList,numberOfArrows)){ */
/*       /\* make the subdivision *\/ */
/*       std::vector<std::vector<int>> newVertexList; */
/*       for (auto x:resultList) */
/*         { */
/*           getAllBoundary(newVertexList,x); */
/*         } */
/*       std::vector<std::string> newArrowList; */
/*       // int k = 0; */
/*       std::vector<std::vector<int>> seedList; */
/*       for (auto v:newVertexList) */
/*         { */
/*           if (v.size()>1) */
/*             { */
/*               seedList.push_back(v); */
/*             } */
/*         } */
/*       for (auto m:seedList) */
/*         { */
/*           for(auto b:boundary(m)) */
/*             { */
/*               newArrowList.push_back(vectorsToString(b,m,newVertexList)); */
/*             } */
/*         } */
/*       std::sort(newArrowList.begin(), newArrowList.end()); */
/*       newArrowList.erase(std::unique(newArrowList.begin(), newArrowList.end()), newArrowList.end()); */

/*       /\* std::cout << "We renamed vertrices as follow:" << std::endl; *\/ */

/*       /\* the vertrices rename are formally renamed that correspondance is written in conversionList *\/ */
/*       for (auto x:newVertexList) */
/*         { */
/*           /\* std::cout << vecToVer(x) << " --> " << vectorIndex(newVertexList,x) << std::endl; *\/ */
/*           conversionList.push_back(vecToVer(x)); */
/*         } */

/*       std::string sdfile = fileNameTrim(filename) + "SD.txt"; */
/*       std::fstream sd(sdfile,std::ios::out); */
/*       if(sd.fail()){ */
/*         std::cout << "outputfile couldn't open." << std::endl; */
/*         return; */
/*       } */
/*       for (auto st:newArrowList) */
/*         { */
/*           sd << st << std::endl; */
/*         } */
/*     }else{ */
/*       std::cout << "This is NOT Euler"<< std::endl; */
/*     } */
/*   } else { */
/*     std::cerr << "Data form is not valid."; */
/*     return; */
/*   } */
/*   return; */
/* } */


#endif
