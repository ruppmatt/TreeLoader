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
#include <queue>
#include <map>
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <cstdlib>

#include "PhylogeneticTree/Interface/Balance.h"

#include "PhylogeneticTree/Interface/Tree.h"
#include "PhylogeneticTree/Interface/TreeIterator.h"
#include "PhylogeneticTree/Interface/iTreeNode.h"
#include "PhylogeneticTree/Interface/iOrganism.h"
#include "Support/Interface/OutputStream.h"

using namespace PhylogeneticTree;
using namespace std;

struct BalanceNode
{
  int tips;
  vector<BalanceNode*> children;
};

const bool ValidateValues(const Tree &tree, const map<int, int> &data);
double ComputeIC(const Tree &tree, const map<int, int> &data) throw(int);
Tree *ConstructTree(int leaves, int internals, map<int, int> &data,
                    vector<iOrganism*> &organisms) throw(int);
void CleanupMin(Tree **tree, vector<iOrganism*> &organisms);

double PhylogeneticTree::ComputeBalance(const Tree &tree,
                                        OutputStream &output,
                                        const bool generateReport,
                                        ostream &reportTxt,
                                        ostream &reportCsv,
                                        ostream &listTxt,
                                        ostream &listCsv) throw(int)
{
  // Figure out how many leaves and internal nodes are present in the data tree
  int internals = 0;
  int leaves = 0;

  TreeIterator i = tree.Begin();
  for(; i != tree.End(); ++i)
  {
    int children = (*i)->HowManyChildren();
    if(children == 0) ++leaves;
    else ++internals;
  }

  if(leaves < 4)
  {
    output << "Error: Balance calculations requires four or more leaves." << endl;
    throw 1;
  }

  // Calculate IC_Max (Polytomy)
  double ic_max = 0;

  for(int x = leaves-internals; x <= leaves-2; ++x)
    ic_max += static_cast<double>(x);

  if(ic_max == 0)
  {
    output << "ERROR: IC_Max calculated as zero." << endl;
    throw 2;
  }

  // Calculate IC_Max (Binary)
  double ic_max_bin = static_cast<double>((leaves-2) * (leaves-1)) / 2.0;

  if(ic_max == 0)
  {
    output << "ERROR: IC_Max calculated as zero." << endl;
    throw 3;
  }

  // Prepare Data Tree
  map<int, int> dataLeafCount;

  vector<TreeIterator> originalLeaves = tree.GetLeaves();
  vector<TreeIterator>::iterator leaf = originalLeaves.begin();
  for(; leaf != originalLeaves.end(); ++leaf)
  {
    TreeIterator ti = *leaf;

    // Proceed up the tree to the root adding one to each internal node's
    //   subtree leaf count
    while(ti != tree.End())
    {
      int id = (*ti)->GetData().GetId();

      map<int, int>::iterator mib = dataLeafCount.find(id);
      if(mib == dataLeafCount.end()) dataLeafCount.insert(make_pair(id, 1));
      else ++mib->second;

      ti.Up();
    }
  }

  // Verify that the values calculated in the last step add up at all depths
  if(ValidateValues(tree, dataLeafCount) == false)
  {
    output << "Error: Subtree leaf counts incorrect." << endl;
    throw 4;
  }

  // Compute IC_Data
  double ic_data = ComputeIC(tree, dataLeafCount);

  // Calculate IC_Min

  // Create temporary tree to simulate minimum tree
  vector<iOrganism*> organisms;
  map<int, int> minData;
  Tree *minTree = 0;
  try
  {
    minTree = ConstructTree(leaves, internals, minData, organisms);
    if(minTree == 0) throw 0;
  }
  catch(int x)
  {
    output << "Error: Failed to create min tree." << endl;
    CleanupMin(0, organisms);
    throw x;
  }

  // Verify that the values calculated in the last step add up at all depths
  if(ValidateValues(*minTree, minData) == false)
  {
    output << "Error: Subtree leaf counts incorrect." << endl;
    throw 5;
  }

  // Compute IC
  double ic_min = ComputeIC(*minTree, minData);

  CleanupMin(&minTree, organisms);

  // Calculate IC_Min (Binary)

  // Create temporary tree to simulate minimum tree
  vector<iOrganism*> organisms_bin;
  map<int, int> minData_bin;
  Tree *minTree_bin = 0;
  try
  {
    minTree_bin = ConstructTree(leaves, leaves-1, minData_bin, organisms_bin);
    if(minTree_bin == 0) throw 0;
  }
  catch(int x)
  {
    output << "Error: Failed to create min tree." << endl;
    CleanupMin(0, organisms_bin);
    throw x;
  }

  // Verify that the values calculated in the last step add up at all depths
  if(ValidateValues(*minTree_bin, minData_bin) == false)
  {
    output << "Error: Subtree leaf counts incorrect." << endl;
    throw 6;
  }

  // Compute IC
  double ic_min_bin = ComputeIC(*minTree_bin, minData_bin);

  CleanupMin(&minTree_bin, organisms_bin);

  // Output information to report file.
  if(generateReport)
  {
    reportTxt << "#Leaves Internal_Nodes UN_IC_Data UN_IC_Min_poly UN_IC_Max_poly ";
    reportTxt << "UN_IC_Min_bin UN_IC_Max_bin N_IC_Data N_IC_Min_poly N_IC_Max_poly ";
    reportTxt << "N_IC_Min_bin NC_IC_Max_bin" << endl;
    reportTxt << leaves     << " ";
    reportTxt << internals  << " ";
    reportTxt << ic_data    << " ";
    reportTxt << ic_min     << " ";
    reportTxt << ic_max     << " ";
    reportTxt << ic_min_bin << " ";
    reportTxt << ic_max_bin << " ";
    reportTxt << ic_data    / ic_max     << " ";
    reportTxt << ic_min     / ic_max     << " ";
    reportTxt << ic_max     / ic_max     << " ";
    reportTxt << ic_min_bin / ic_max_bin << " ";
    reportTxt << ic_max_bin / ic_max_bin;
    reportTxt << endl;

    reportCsv << "Leaves, Internal_Nodes, UN_IC_Data, UN_IC_Min_poly, UN_IC_Max_poly, ";
    reportCsv << "UN_IC_Min_bin, UN_IC_Max_bin, N_IC_Data, N_IC_Min_poly, N_IC_Max_poly, ";
    reportCsv << "N_IC_Min_bin, NC_IC_Max_bin" << endl;
    reportCsv << leaves     << ", ";
    reportCsv << internals  << ", ";
    reportCsv << ic_data    << ", ";
    reportCsv << ic_min     << ", ";
    reportCsv << ic_max     << ", ";
    reportCsv << ic_min_bin << ", ";
    reportCsv << ic_max_bin << ", ";
    reportCsv << ic_data    / ic_max     << ", ";
    reportCsv << ic_min     / ic_max     << ", ";
    reportCsv << ic_max     / ic_max     << ", ";
    reportCsv << ic_min_bin / ic_max_bin << ", ";
    reportCsv << ic_max_bin / ic_max_bin;
    reportCsv << endl;
  }

  output << "(IC, Min, Max) Un-normalized: " << ic_data << " , " << ic_min;
  output << " , " << ic_max << endl;
  output << "(IC, Min, Max)    normalized: " << ic_data/ic_max << " , " << ic_min/ic_max;
  output << " , " << ic_max/ic_max << endl;

  // Output list file
  i = tree.Root(); // TreeIterator i, from beginning of function
  queue<TreeIterator> listQ;
  listQ.push(i);

  while(!listQ.empty())
  {
    i = listQ.front();
    listQ.pop();

    int numChildren = (*i)->HowManyChildren();
    int id          = (*i)->GetData().GetId();
    int updateBorn  = (*i)->GetData().GetBirthTime();

    listTxt << id << " "  << updateBorn << " "  << numChildren;
    listCsv << id << ", " << updateBorn << ", " << numChildren;

    vector<TreeIterator> children = i.GetChildren();
    vector<TreeIterator>::iterator ci = children.begin();
    for(; ci != children.end(); ++ci)
    {
      int childId = (**ci)->GetData().GetId();

      map<int, int>::iterator mib = dataLeafCount.find(childId);
      if(mib == dataLeafCount.end()) throw 7;

      listTxt << " "  << mib->second;
      listCsv << ", " << mib->second;

      // Add each child to the queue
      listQ.push(*ci);
    }

    listTxt << endl;
    listCsv << endl;
  }

  return ic_data;
}

const bool ValidateValues(const Tree &tree, const map<int, int> &data)
{
  queue<TreeIterator> q;
  q.push(tree.Root());

  map<int,int>::const_iterator rit = data.find((*tree.Root())->GetData().GetId());
  if(rit == data.end()) return false;

  int total = rit->second;

  int depth = 1;
  int sum = 0;
  int leavesUsed = 0;
  while(!q.empty())
  {
    TreeIterator ti = q.front();
    q.pop();

    int id = (*ti)->GetData().GetId();
    int currentDepth = ti.GetDepth();

    rit = data.find(id);
    if(rit == data.end()) return false;

    if(currentDepth != depth)
    {
      if(sum != total) return false;

      sum = rit->second + leavesUsed;
      depth = currentDepth;
    }
    else
      sum += rit->second;

    if((*ti)->HowManyChildren() == 0) ++leavesUsed;

    vector<TreeIterator> children = ti.GetChildren();
    vector<TreeIterator>::iterator temp = children.begin();
    for(; temp != children.end(); ++temp) q.push(*temp);
  }

  if(sum != total) return false;

  return true;
}

double ComputeIC(const Tree &tree, const map<int, int> &data) throw(int)
{
  double ic = 0;

  TreeIterator ti = tree.Begin();
  for(; ti != tree.End(); ++ti)
  {
    if((*ti)->HowManyChildren() == 0) continue; // skip children
    if((*ti)->HowManyChildren() == 1) continue; // skip single leaf parents

    // Do all pairs difference in child values
    vector<TreeIterator> children = ti.GetChildren();
    vector<TreeIterator>::iterator child = children.begin();
    double value = 0;
    double counter = 0;
    for(; child != children.end(); ++child)
    {
      int id = (**child)->GetData().GetId();
      map<int, int>::const_iterator mib = data.find(id);
      if(mib == data.end()) throw -1;

      vector<TreeIterator>::iterator temp = child;
      ++temp;
      for(; temp != children.end(); ++temp)
      {
        int id2 = (**temp)->GetData().GetId();
        map<int, int>::const_iterator mib2 = data.find(id2);
        if(mib2 == data.end()) throw -2;

        value += static_cast<double>(abs(mib->second - mib2->second));
        counter += 1;
      }
    }

    // Average values
    value /= counter;
    ic += value;
  }

  return ic;
}

Tree *ConstructTree(int leaves, int internals, map<int, int> &data,
                    vector<iOrganism*> &organisms) throw(int)
{
  // Calculate internal node layout
  map<int, int> layout;

  // root
  layout.insert(make_pair(0, -1));
  data.insert(make_pair(0, leaves));

  queue<int> minQ;
  minQ.push(0);

  int availableId = 1;

  // This will construct a tree with n leaves and n-1 internal nodes
  while(!minQ.empty())
  {
    int id = minQ.front();
    minQ.pop();

    map<int, int>::iterator mii = data.find(id);
    if(mii == data.end()) throw -3;

    int leafCount = mii->second;
    if(leafCount == 1) continue; // leaves have no children

    int v1 = static_cast<int>(pow(2, floor(log2(static_cast<double>(leafCount))))) / 2;
    int v2 = leafCount - v1;

    // Create children
    layout.insert(make_pair(availableId, id));
    layout.insert(make_pair(availableId+1, id));

    data.insert(make_pair(availableId, v1));
    data.insert(make_pair(availableId+1, v2));

    minQ.push(availableId);
    minQ.push(availableId+1);

    availableId += 2;
  }

  // Condense tree to account for polytomies.
  vector<int> rootChildren;
  rootChildren.push_back(1);
  rootChildren.push_back(2);

  int polytomies = leaves - 1 - internals;
  for(int i=0; i<polytomies; ++i)
  {
    vector<int>::iterator rrc    = rootChildren.begin();
    vector<int>::iterator rrcMax = rootChildren.end();
    int max = 1;

    for(; rrc != rootChildren.end(); ++rrc)
    {
      map<int, int>::iterator mii = data.find(*rrc);
      if(mii == data.end()) throw -4;

      if(mii->second > max)
      {
        max = mii->second;
        rrcMax = rrc;
      }
    }

    if(max == 1) throw -5;

    int childId = *rrcMax;
    rootChildren.erase(rrcMax);

    // Remove child from the tree
    map<int,int>::iterator mii = layout.begin();
    for(; mii != layout.end(); ++mii)
      if(mii->first == childId)
        layout.erase(mii);

    // Change all nodes who's parent is the child, and set it to the root
    mii = layout.begin();
    for(; mii != layout.end(); ++mii)
      if(mii->second == childId)
      {
        mii->second = 0;
        rootChildren.push_back(mii->first);
      }
  }

  Tree *tree = 0;
  try
  {
    tree = new Tree(layout, organisms);
    if(tree == 0) throw 0;
  }
  catch(...)
  {
    vector<iOrganism*>::iterator i = organisms.begin();
    for(; i != organisms.end(); ++i)
    {
      if(*i != 0)
      {
        delete *i;
        *i = 0;
      }
    }

    organisms.clear();

    throw -6;
  }

  return tree;
}

void CleanupMin(Tree **tree, vector<iOrganism*> &organisms)
{
  if(tree != 0 && *tree != 0) { delete *tree; *tree = 0; }

  vector<iOrganism*>::iterator i = organisms.begin();
  for(; i != organisms.end(); ++i)
  {
    if(*i != 0)
    {
      delete *i;
      *i = 0;
    }
  }

  organisms.clear();

  return;
}

