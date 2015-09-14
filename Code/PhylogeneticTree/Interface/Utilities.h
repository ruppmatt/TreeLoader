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

#ifndef __PhylogeneticTree_Interface_Utilities_h__
#define __PhylogeneticTree_Interface_Utilities_h__

#include <ostream>

class RandomNumberGenerator;

namespace PhylogeneticTree
{
  class Tree;
  class TreeIterator;

  // BurnLeaf takes the index of a node to remove.  If the node is a leaf 
  //   it burns all node parents above it until it reaches a bifurcation.
  void BurnLeaf(Tree &, TreeIterator &);

  // Incomplete
  const bool CyclesFound(const Tree &);

  // Prints information about the tree onto the screen
  const bool PrintInformation(const Tree &, std::ostream &);

  // Removes nodes from the tree that are not part of the lineages formed
  //   by the leaf nodes in list of leaf nodes we care about.  Basically,
  //   it strips out all the leaf nodes we do not care about and their
  //   lineages that do not overlap with lineages we are keeping.
  const bool PruneDeadLeaves(Tree &);

  // Removes all nodes from the tree that has only a single child including
  //   the root.
  void RemoveNonfurcatingNodes(Tree &);

  // This function takes a tree and samples it's leaf nodes.  howManyLeaves 
  //   specifies how many leaf nodes, up to the total leaf nodes, that will 
  //   be sampled without replacement.  These new leaf nodes will then be 
  //   used to create a new tree that is returned.
  Tree *Sample(const Tree &,
               const unsigned int howManyLeaves,
               RandomNumberGenerator &) throw(int);

  // Incomplete
  const bool Validate(const Tree &);

} // namespace PhylogeneticTree

#endif // __PhylogeneticTree_Interface_Utilities_h__
