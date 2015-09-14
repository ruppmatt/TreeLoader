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
#include <sstream>
#include <cmath>
#include <cstring>
#include <cstdlib>

#include "PhylogeneticTree/Interface/NewickOutput.h"

#include "PhylogeneticTree/Interface/Tree.h"
#include "PhylogeneticTree/Interface/TreeIterator.h"
#include "PhylogeneticTree/Interface/iTreeNode.h"
#include "PhylogeneticTree/Interface/iOrganism.h"
#include "Support/Interface/OutputStream.h"

using namespace PhylogeneticTree;
using namespace std;

void PhylogeneticTree::NewickOutput(const Tree &tree,
                                    const double timeCutoff,
                                    const char *const outFilename,
                                    OutputStream &output) throw(int)
{
  output << "Building newick format of tree           ... ";

  // Tree is empty -- special case 1
  if(tree.Size() == 0) { output << "Complete." << endl; return; }

  // Open output file
  if(outFilename == 0) throw 0;

  char outFileTxt[256];
  outFileTxt[0] = '\0';
  strcpy(outFileTxt, outFilename);
  strcat(outFileTxt, ".newick");

  ofstream outFile(outFileTxt);
  if(!outFile) throw 1;

  // If there is only one node -- special case 2
  if(tree.Size() == 1)
  {
    TreeIterator sc2 = tree.Begin();
    if(*sc2 == 0) { outFile.close(); throw 2; }
    outFile << "()F" << (*sc2)->GetData().GetId() << endl;
    outFile.close();
    output << "Complete." << endl;
    return;
  }

  // Process and output data
  TreeIterator end = tree.End();

  // current node is considered the root for this algorithm
  bool first = true;
  int depth = 1;
  for(TreeIterator i = tree.Begin(); i != end; ++i)
  {
    // For the sake of the algorithm, this will set the parent to i if 
    //   the current node is the root.  Otherwise it is the parent.
    const TreeIterator parent = (i.GetParent() != end) ? i.GetParent() : i;
    if(*i == 0 || *parent == 0) { outFile.close(); throw 3; }

    const iOrganism & org = (*i)->GetData();
    const iOrganism & porg = (*parent)->GetData();

    const int diff = i.GetDepth() - depth;
    if(diff == 0)
    {
      // Add comma before this entry.
      outFile << ",F" << org.GetId() << ":";
      outFile << org.GetBirthTime() - porg.GetBirthTime();
    }
    else if(diff == -1)
    {
      // Moving to parent (up 1)

      // close last group
      outFile << ")F" << org.GetId();

      // Don't output the branch length for the root.
      if(i.GetParent() != end)
        outFile << ":" << org.GetBirthTime() - porg.GetBirthTime();

      depth = i.GetDepth();
    }
    else if(diff > 0)
    {
      // Starting at base of new sub tree

      // Add comma with previous group
      if(first == true) first = false;
      else outFile << ",";

      // Open up new groups based on how many levels have changed
      for(int x = 0; x < diff; ++x) outFile << "(";

      // Output data
      outFile << "F" << org.GetId() << ":";
      outFile << org.GetBirthTime() - porg.GetBirthTime();

      // Update
      depth = i.GetDepth();
    }
    else
    {
      outFile.close();
      throw 4;
    }
  }

  // Add semicolon to the end of the file.
  outFile << ";";

  // Create empty line at end of file.
  outFile << endl;

  // Close file and wrap this up
  outFile.close();

  output << "Complete." << endl;

  return;
}
