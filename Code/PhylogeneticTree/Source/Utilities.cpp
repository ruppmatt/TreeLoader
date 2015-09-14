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

#include <vector>
#include <set>

#include "PhylogeneticTree/Interface/Utilities.h"

#include "Support/Interface/random.h"
#include "PhylogeneticTree/Interface/Tree.h"
#include "PhylogeneticTree/Interface/iTreeNode.h"
#include "PhylogeneticTree/Interface/iOrganism.h"

using namespace std;

void PhylogeneticTree::BurnLeaf(Tree &tree, TreeIterator &it)
{
  while(it != tree.End() && (*it)->HowManyChildren() == 0)
  {
    TreeIterator temp = it;
    it.Up(); // Move to parent
    tree.DeleteLeaf(temp);
  }

  tree.ClearDeleted();

  return;
}

const bool PhylogeneticTree::CyclesFound(const Tree &)
{
  return false;
}

const bool PhylogeneticTree::PrintInformation(const Tree &tree,
                                              std::ostream &o)
{
  int leaves=0, singles=0, doubles=0, others=0;

  TreeIterator it = tree.End();
  for(it = tree.Begin(); it != tree.End(); ++it)
  {
    if(*it == 0) { return false; }

    if((*it)->HowManyChildren() == 0)      { ++leaves;  }
    else if((*it)->HowManyChildren() == 1) { ++singles; }
    else if((*it)->HowManyChildren() == 2) { ++doubles; }
    else                                   { ++others;  }
  }

  o << "Leaf         node count: " << leaves << endl;
  o << "1 branch     node count: " << singles << endl;
  o << "2 Branch     node count: " << doubles << endl;
  o << "2+ branching node count: " << doubles+others << endl;
  o << "3+ branching node count: " << others << endl;
  o << "Total        node count: " << tree.Size() << endl;
  o << endl;

  return true;
}

const bool PhylogeneticTree::PruneDeadLeaves(Tree &tree)
{
  try
  {
    vector<TreeIterator> leaves = tree.GetLeaves();

    vector<TreeIterator>::iterator i = leaves.end();
    for(i = leaves.begin(); i != leaves.end(); ++i)
    {
      if((**i)->GetData().GetIsAlive() == false)
      {
        BurnLeaf(tree, *i);
      }
    }
  }
  catch(int) { return false; }

  return true;
}

void PhylogeneticTree::RemoveNonfurcatingNodes(Tree &tree)
{
  TreeIterator it = tree.Last();

  while(it != tree.End()) // Also checks that internal node is non-zero
  {
    TreeIterator temp = it;

    --it; // Move to previous node

    if((*temp)->HowManyChildren() == 1)
    {
      tree.DeleteSingle(temp);
    }
  }

  tree.ClearDeleted();

  return;
}

PhylogeneticTree::Tree *PhylogeneticTree::Sample(const Tree &tree,
                                                 const unsigned int howManyLeaves,
                                                 RandomNumberGenerator &rng)
  throw(int)
{
  /*** Copy orignal tree ****************************************************/
  Tree *newTree = new Tree(tree);
  if(newTree == 0) { return 0; }

  /*** Find all the leaves **************************************************/
  vector<TreeIterator> leaves = newTree->GetLeaves();

  /*** Get an inverse sample of the leaves **********************************/
  unsigned int inverseSize = static_cast<unsigned int>(leaves.size());
  if(howManyLeaves <= inverseSize)
  {
    inverseSize -= howManyLeaves;
  }

  vector<TreeIterator> inverseSample;
  for(unsigned int i=0; !leaves.empty() && i<inverseSize; ++i)
  {
    unsigned int whichOne =
      rng.GetUInt(static_cast<unsigned int>(leaves.size()));
    inverseSample.push_back(leaves.at(whichOne));
    leaves.at(whichOne) = leaves.back();
    leaves.pop_back();
  }

  /*** Burn away all the other leaves ***************************************/
  vector<TreeIterator>::iterator iSample = inverseSample.end();
  for(iSample = inverseSample.begin(); iSample != inverseSample.end(); ++iSample)
  {
    BurnLeaf(*newTree, *iSample);
  }

  return newTree;
}

const bool PhylogeneticTree::Validate(const Tree &)
{
  return true;
}
