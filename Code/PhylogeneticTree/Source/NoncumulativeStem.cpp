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

#include "PhylogeneticTree/Interface/NoncumulativeStem.h"

#include "PhylogeneticTree/Interface/Tree.h"
#include "PhylogeneticTree/Interface/TreeIterator.h"
#include "PhylogeneticTree/Interface/iTreeNode.h"
#include "PhylogeneticTree/Interface/iOrganism.h"
#include "Support/Interface/OutputStream.h"

using namespace PhylogeneticTree;
using namespace std;

double PhylogeneticTree::NoncumulativeStem(const Tree &tree,
                                           const double timeCutoff,
                                           const char *const outFilename,
                                           OutputStream &output) throw(int)
{
  int total=0, leaves=0, extra=0, singles=0;

  double sum = 0;
  double n = 0;

  // Get the birth time of the root
  TreeIterator iRoot = tree.Root();
  if(iRoot == tree.End()) return 0; // no root
  if(*iRoot == 0)         throw(0); // no data
  double rootTime = (*iRoot)->GetData().GetBirthTime();

  for(TreeIterator iTree = tree.Begin(); iTree != tree.End(); ++iTree)
  {
    if(*iTree == 0) throw(1); // no data

    ++total;
    int children = (*iTree)->HowManyChildren();
    if(children > 2) extra += children - 2;
    if(children == 1) ++singles;
    if(children == 0) ++leaves;

    if(iTree == iRoot) continue; // don't process root node

    const iOrganism &organism = (*iTree)->GetData();
    if((*iTree)->HowManyChildren() == 0) continue; // ignore leaf nodes

    TreeIterator parent = iTree.GetParent();
    if(*parent == 0) throw(2); //  no data

    const double birthTime = organism.GetBirthTime();
    const double parentBirthTime = (*parent)->GetData().GetBirthTime();

    // skewed times
    if(parentBirthTime > birthTime || parentBirthTime < rootTime) throw(3);

    // if the parent is the root, the ratio is 1
    if(parent == iRoot)
    {
      sum += 1;
      n += 1;
      continue;
    }

    sum += (birthTime - parentBirthTime) / (parentBirthTime - rootTime);
    n += 1;
  }

#if 0
  cout << "Total:\t\t" << total << endl;
  cout << "Leaves:\t\t" << leaves << endl;
  cout << "Singles:\t" << singles << endl;
  cout << "inter-n:\t" << n << endl;
  cout << "extra:\t\t" << extra << endl;

  cout << "total - leaves     = " << total - leaves << endl;
  cout << "leaves - 2         = " << leaves - 2 << endl;
  cout << "leaves - 2 - extra = " << leaves - 2 - extra << endl;
  cout << endl;
#endif

  if(n <= 0) return 0;

  return sum / n;
}

