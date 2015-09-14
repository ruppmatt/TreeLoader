/**
 * Copyright 2010 Jason Stredwick
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <cstdlib>

#include "PhylogeneticTree/Interface/GammaFunctions.h"

#include "PhylogeneticTree/Interface/Tree.h"
#include "PhylogeneticTree/Interface/TreeIterator.h"
#include "PhylogeneticTree/Interface/iTreeNode.h"
#include "PhylogeneticTree/Interface/iOrganism.h"
#include "Support/Interface/OutputStream.h"

using namespace PhylogeneticTree;
using namespace std;

double PhylogeneticTree::ComputeGamma(const Tree &tree,
                                      const double timeCutoff,
                                      const char *const outFilename,
                                      OutputStream &output) throw(int)
{
  /*** map furcation time to number of new lineages at that time point ******/
  output << "Building map of time to new lineages     ... ";
  // Map furcation time to the quantity of new branches.  The quantity will be
  //   determined as the total number of children minus the number of 
  //   parents branching at that time.  The reason for the subtraction is 
  //   to take into account that one branch from each parent will be replacing
  //   the parents lineage in the total quantity of lineages at a given point
  //   in time.
  //   time  branches
  map<double, double> branches;
  map<double, double>::iterator iBranches = branches.end();

  // These are for storing information related to when a furcation occurs
  //   for a given id. Multiple ids can furcate at the same time.
  multimap<double, pair<unsigned int, int> > idMap;
  multimap<double, pair<unsigned int, int> >::iterator iIdMap = idMap.end();

  // Add each furcation to the branches map, and record id
  TreeIterator iTree = tree.End();
  for(iTree = tree.Begin(); iTree != tree.End(); ++iTree)
  {
    const double furcationTime = (*iTree)->GetData().GetBirthTime();
    if(furcationTime >= timeCutoff) { continue; } // skip these furcations

    const unsigned int children = (*iTree)->HowManyChildren();
    if(children < 2) { continue; } // ignore leaf nodes and non-furcations

    iBranches = branches.find(furcationTime);
    if(iBranches == branches.end()) // this time point has not been added yet
    {
      branches.insert(make_pair(furcationTime, children - 1));
    }
    else // this time point already present, so add to it.
    {
      iBranches->second += children - 1;
    }

    const int id = (*iTree)->GetData().GetId();
    idMap.insert(make_pair(furcationTime, make_pair(children - 1, id)));
  }
  output << "Complete." << endl;;

   // Nothing to do if there are no furcations
  if(branches.empty()) { return 0; }

  output << "Quantity of furcation time points added ";
  output << static_cast<unsigned int>(branches.size());
  output << endl;

  /*** Compute n ************************************************************/
  output << "The total number of lineages found is  ";
  // n is the total number of lineages at the point in time specified by the
  //   cutoff.
  double n = 1;
  for(iBranches = branches.begin(); iBranches != branches.end(); ++iBranches)
  {
    n += iBranches->second;
  }
  output << n << endl;

  // Adding the timeCutoff (with no additional lineages) to simplify the 
  //   following calculations.
  branches.insert(make_pair(timeCutoff, 0));

  /*** Compute ids at each furcation ****************************************/
  output << "Computing ids at each furcation             ... ";
  vector<double> fTime;
  fTime.push_back(0);
  fTime.push_back(0);
  vector<int> id;
  id.push_back(0);
  id.push_back(0);

  for(iIdMap = idMap.begin(); iIdMap != idMap.end(); ++iIdMap)
  {
    const double furcationTime = iIdMap->first;
    const unsigned int children = iIdMap->second.first;
    const int _id = iIdMap->second.second;

    for(unsigned int i=0; i < children; ++i)
    {
      fTime.push_back(furcationTime);
      id.push_back(_id);
    }
  }

  output << "Complete." << endl;

  /*** Compute g_k **********************************************************/
  output << "Computing g_k values                     ... ";

  // Create and initialize the set of g_k values.
  vector<double> g_k;
  vector<double>::iterator iG_k = g_k.end();
  // Make space up front to store all the g_k values.
  g_k.reserve(static_cast<unsigned int>(n)+1);
  g_k.push_back(0); // The time that zero lineages existed is set to zero
  g_k.push_back(0); // The time that one lineage existed is set to zero

  // Consider that we have looked at all the lineages through one.
  double lineagesThusFar = 1;

  // Begin at the first furcation
  iBranches = branches.begin();

  for(double k = 2; k <= n; ++k)
  {
    if(lineagesThusFar + iBranches->second != k)
    {
      g_k.push_back(0); // There is no time that k number of lineages existed
      continue;
    }

    // Get time for the current furcation
    const double currentTime = iBranches->first;

    // Move to next furcation
    ++iBranches;

    // Get time for when the next furcation started
    const double nextTime = iBranches->first;

    // Calculate g_k to be the time from the current furcation to the next.
    //   If we are dealing with g_n the nextTime will be the timeCutoff.
    g_k.push_back(nextTime - currentTime);

    // Set the number of lineages to be k
    lineagesThusFar = k;
  }
  output << "Complete." << endl;

  /*** Compute partial sums, i.e. the inner sums for the numerator **********/
  output << "Computing inner sums for numerator       ... ";
  vector<double> innerSums;
  innerSums.push_back(0); // No calculation for g_0, set to 0
  innerSums.push_back(0); // No calculation for g_1, set to 0

  double total = 0;

  // This loop computes the inner sums in the numerator.  The inner sum is
  //   the sum of all (g_k * k) from k=2..i, where i=2..n-1.  By keeping a 
  //   running total, each inner sum is equal to the previous sum + (g_i * i).
  for(unsigned int i = 2; i <= n-1; ++i)
  {
    total += g_k[i] * static_cast<double>(i);
    innerSums.push_back(total);
  }
  output << "Complete." << endl;

  /*** Compute T ************************************************************/
  output << "Compute T                                ... ";
  double T = total + g_k[static_cast<unsigned int>(n)] * n;
  innerSums.push_back(T); // Adding in the sum k=2..n (g_k * k)
  output << "Complete." << endl;

  /*** Compute outer sum for the numerator **********************************/
  output << "Compute outer sum i=2..n-1 for numerator ... ";
  double outerSum = 0;
  for(unsigned int i = 2; i <= n-1; ++i)
  {
    outerSum += innerSums[i];
  }
  output << "Complete." << endl;

  /*** Compute numerator ****************************************************/
  output << "Compute numerator                        ... ";
  double numerator = (1/(n-2) * outerSum) - (T/2);
  output << "Complete." << endl;

  /*** Compute denominator **************************************************/
  output << "Compute denominator                      ... ";
  double denominator = T * sqrt( 1 / (12*(n-2)) );
  output << "Complete." << endl;

  /*** Compute gamma ********************************************************/
  output << "Compute gamma                            ... ";
  double gamma = numerator / denominator;
  output << "Complete." << endl;

  if(id.size() != g_k.size())
    {
      cout << "Error, size difference: " << id.size() << " " << g_k.size() << endl;
      exit(0);
    }

  /*** Output information to file *******************************************/
  if(outFilename != 0)
  {
    char outFileTxt[256];
    char outFileCsv[256];
    outFileTxt[0] = '\0';
    outFileCsv[0] = '\0';
    strcpy(outFileTxt, outFilename);
    strcpy(outFileCsv, outFilename);
    strcat(outFileTxt, ".txt");
    strcat(outFileCsv, ".csv");

    ofstream outFile1(outFileTxt);
    ofstream outFile2(outFileCsv);
    if(!outFile1) { return 0; }
    if(!outFile2) { return 0; }

    outFile1 << "n = " << n << endl;
    outFile1.precision(10);
    outFile1 << "outer sum   = " << outerSum << endl;
    outFile1.precision(8);
    outFile1 << "T           = " << T << endl;
    outFile1.precision(8);
    outFile1 << "numerator   = " << numerator << endl;
    outFile1.precision(8);
    outFile1 << "denominator = " << denominator << endl;
    outFile1.precision(8);
    outFile1 << "gamma       = " << gamma << endl;
    outFile1 << endl;
    outFile2 << "n = " << n << endl;
    outFile2.precision(10);
    outFile2 << "outer sum   = " << outerSum << endl;
    outFile2.precision(8);
    outFile2 << "T           = " << T << endl;
    outFile2.precision(8);
    outFile2 << "numerator   = " << numerator << endl;
    outFile2.precision(8);
    outFile2 << "denominator = " << denominator << endl;
    outFile2.precision(8);
    outFile2 << "gamma       = " << gamma << endl;
    outFile2 << endl;

    outFile1 << " lineage     g_k  scaled   inner_sum    ftime        id" << endl;
    outFile2 << "lineage,g_k,scaled_g_k,sum(k=2..n | k*g_k), ftime, id" << endl;
    for(unsigned int i=0; i<n; ++i)
    {
      outFile1.width(8);
      outFile1 << i;
      outFile1.width(8);
      outFile1.precision(8);
      outFile1 << g_k.at(i);
      outFile1.width(8);
      outFile1.precision(8);
      outFile1 << g_k.at(i) * i;
      outFile1.width(12);
      outFile1.precision(8);
      outFile1 << innerSums.at(i);
      outFile1.width(9);
      outFile1.precision(8);
      outFile1 << fTime.at(i);
      outFile1.width(10);
      outFile1 << id.at(i);
      outFile1 << endl;

      outFile2 << i << ",";
      outFile2.precision(8);
      outFile2 << g_k.at(i) << ",";
      outFile2.precision(8);
      outFile2 << g_k.at(i) * i << ",";
      outFile2.precision(8);
      outFile2 << innerSums.at(i);
      outFile2.precision(8);
      outFile2 << fTime.at(i);
      outFile2 << id.at(i);
      outFile2 << endl;
    }

    outFile1.close();
    outFile2.close();
  }

  return gamma;
}
