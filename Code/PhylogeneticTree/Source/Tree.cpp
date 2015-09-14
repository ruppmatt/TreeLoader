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

#include "PhylogeneticTree/Interface/Tree.h"

#include "Organisms/Interface/SimpleOrganism.h"
#include "PhylogeneticTree/Interface/iOrganism.h"
#include "PhylogeneticTree/Include/TreeNode.h"

using namespace std;

PhylogeneticTree::TreeNode *toDelete = 0;

PhylogeneticTree::Tree::Tree(const vector<iOrganism*> &organisms) throw(int)
: root(0)
{
  ProcessOrganisms(organisms);
  return;
}

PhylogeneticTree::Tree::Tree(const map<int,int> &layout,
                             vector<iOrganism*> &output) throw(int)
: root(0)
{
  ConstructLayout(layout, output);
  return;
}

PhylogeneticTree::Tree::Tree(const Tree &otherTree) throw(int) : root(0)
{
  Copy(otherTree);
  return;
}

PhylogeneticTree::Tree::~Tree(void)
{
  CleanUp();
  return;
}

const PhylogeneticTree::Tree &
PhylogeneticTree::Tree::operator=(const Tree &otherTree) throw(int)
{
  if(&otherTree != this) { Copy(otherTree); }
  return *this;
}

PhylogeneticTree::TreeIterator PhylogeneticTree::Tree::Begin(void) const throw(int)
{
  if(root == 0)
  {
    return TreeIterator();
  }

  // The first node is the left-most leaf.
  int depth = 1;
  TreeNode *node = root;
  while(node->HowManyChildren() > 0)
  {
    if(node == 0) { throw 0; }
    node = node->GetChildren().front();
    ++depth;
  }

  return TreeIterator(node, depth);
}

void PhylogeneticTree::Tree::CleanUp(void)
{
  root = 0;

  unsigned int x = 0;
  vector<TreeNode*>::iterator i = treeNodes.end();
  for(i = treeNodes.begin(); i != treeNodes.end(); ++i, ++x)
  {
    if(*i != 0)
    {
      delete *i;
      *i = 0;
    }
  }

  return;
}

void PhylogeneticTree::Tree::ClearDeleted(void)
{
  // Go through each TreeNode in the treeNodes vector.  If that node is
  //   invalid(0) or is set to be deleted, then
  //     1. Swap it with the last element in the list
  //     2. Pop off the back of the vector
  //     3. Delete node if possible
  // This algorithm only moves ahead when it does NOT delete a node.  This
  //   is because when deleted it is swapped with the last node, and if 
  //   that node is supposed to be deleted we don't want to skip over it
  //   by moving ahead.  If the last node was deleted, the index does not
  //   need to be incremented because the loop will automatically fail
  //   because the size of the vector has changed.
  for(unsigned int i = 0; i < treeNodes.size(); )
  {
    if(treeNodes[i] == 0 || treeNodes[i]->GetDeleteMe() == true)
    {
      TreeNode *node = treeNodes[i];
      treeNodes[i] = treeNodes[treeNodes.size()-1];
      treeNodes[treeNodes.size()-1] = 0;
      treeNodes.pop_back();

      if(node != 0) { delete node; }
      continue;
    }

    ++i;
  }

  return;
}

void PhylogeneticTree::Tree::ConstructLayout(const map<int,int> &layout,
                                             vector<iOrganism*> &output) throw(int)
{
  // Temporary map of id to index used during the linking phase
  map<int, TreeNode*> idToNode;

  map<int,int>::const_iterator iLayout = layout.begin();
  for(; iLayout != layout.end(); ++iLayout)
  {
    iOrganism *so = 0;
    TreeNode *tn = 0;
    try
    {
      so = new SimpleOrganism(iLayout->first, iLayout->second);
      tn = new TreeNode(*so);
      if(so == 0 || tn == 0) throw 0;
    }
    catch(...)
    {
      if(so != 0) delete so;
      if(tn != 0) delete tn;
      throw 1;
    }
    output.push_back(so);
    idToNode.insert(make_pair(so->GetId(), tn));
    treeNodes.push_back(tn);
    // check for error -- throw 2
  }

  // Traverse all the TreeNodes and attach them to their parent by using
  //   the InsertChild method.
  vector<TreeNode*>::iterator i = treeNodes.end();
  for(i = treeNodes.begin(); i != treeNodes.end(); ++i)
  {
    // Assuming all treeNodes are valid, otherwise the above code would have 
    //   thrown 1.
    int parentId = (*i)->GetData().GetParentId();
    if(parentId == -1)
    {
      // The root has no parent so its parent will be set to itself
      (*i)->SetAsRoot();
      root = *i;
      continue;
    }

    // Look up the parent by id
    map<int, TreeNode*>::iterator temp = idToNode.find(parentId);
    if(temp == idToNode.end()) // Could not find parent
    {
      throw 3;
    }

    // Assuming temp->second will not be 0, otherwise the above code would 
    //   have thrown 1.
    // Set parent for current node
    (*i)->SetParent(*(temp->second));

    // Add this node as child for parent node
    temp->second->InsertChild(**i);
  }

  return;
}

void PhylogeneticTree::Tree::Copy(const Tree &otherTree) throw(int)
{
  CleanUp();

  map<TreeNode*, TreeNode*> oldToNewMap;

  // Make copies of all the TreeNodes, but do not hook them up yet.
  vector<TreeNode*>::const_iterator i = otherTree.treeNodes.end();
  for(i = otherTree.treeNodes.begin(); i != otherTree.treeNodes.end(); ++i)
  {
    TreeNode *node = *i;
    if(node == 0) { throw -1; }

    TreeNode *tn = new TreeNode(node->GetData());
    if(tn == 0) { throw -2; }

    treeNodes.push_back(tn);
    oldToNewMap.insert(make_pair(node, tn));
    // Check for error, throw 1
  }

  // Hook up new TreeNodes
  map<TreeNode*, TreeNode*>::iterator mi = oldToNewMap.end();
  for(mi = oldToNewMap.begin(); mi != oldToNewMap.end(); ++mi)
  {
    // Assuming first and second are non-zero or the above code would have 
    //   thrown 1.
    TreeNode *oldNode = mi->first;
    TreeNode *newNode = mi->second;

    TreeNode *parent = oldNode->GetParent();

    map<TreeNode*, TreeNode*>::iterator temp = oldToNewMap.end();
    if(parent != 0)
    {
      temp = oldToNewMap.find(parent);
      // Check for error, throw 2
      newNode->SetParent(*(temp->second));
    }
    else
    {
      newNode->SetAsRoot();
      root = newNode;
    }

    vector<TreeNode*>::const_iterator vi = oldNode->GetChildren().end();
    for(vi = oldNode->GetChildren().begin(); vi != oldNode->GetChildren().end(); ++vi)
    {
      temp = oldToNewMap.find(*vi);
      // Check for error, throw 3
      newNode->InsertChild(*(temp->second));
    }
  }

  return;
}

void PhylogeneticTree::Tree::DeleteLeaf(TreeIterator &i)
{
  if(i.node == 0 || i.node->HowManyChildren() > 0)
  {
    return;
  }

  TreeNode *node   = i.node;
  TreeNode *parent = node->GetParent();
  i.node = 0;
  i.depth = 0;

  if(node == root) root = 0;
  else             parent->RemoveChild(*node);

  node->SetDeleteMe(true);

  return;
}

void PhylogeneticTree::Tree::DeleteSingle(TreeIterator &i)
{
  if(i.node == 0 || i.node->HowManyChildren() != 1)
  {
    return;
  }

  TreeNode *node = i.node;
  TreeNode *child = i.node->GetChildren().front();
  i.node = 0;
  i.depth = 0;

  if(root == node)
  {
    root = child;
    child->SetAsRoot();
  }
  else
  {
    TreeNode *parent = node->GetParent();
    parent->ChangeChild(*node, *child);
    child->SetParent(*parent);
  }

  node->SetDeleteMe(true);

  return;
}

PhylogeneticTree::TreeIterator PhylogeneticTree::Tree::End(void) const
{
  // End is what specifies that you have gone beyond the end/beginning of
  //   the tree.  It is represented as an TreeIterator with it's node of 0 
  //   and depth of 0.
  return TreeIterator();
}

PhylogeneticTree::TreeIterator PhylogeneticTree::Tree::Find(const int id) const throw(int)
{
  TreeIterator end = End();
  for(TreeIterator i = Begin(); i != end; ++i)
  {
    if(*i == 0) throw 0;

    if((*i)->GetData().GetId() == id) return i;
  }

  // return End if the id was not found
  return TreeIterator();
}

vector<PhylogeneticTree::TreeIterator>
PhylogeneticTree::Tree::GetLeaves(void) const throw(int)
{
  vector<TreeIterator> leaves;

  TreeIterator end = End();
  for(TreeIterator i = Begin(); i != end; ++i)
  {
    if(*i == 0) throw 0;

    if((*i)->HowManyChildren() == 0) leaves.push_back(i);
  }

  return leaves;
}

PhylogeneticTree::TreeIterator PhylogeneticTree::Tree::Last(void) const
{
  // The last node is the root.
  return TreeIterator(root, 1);
}

void PhylogeneticTree::Tree::ProcessOrganisms(const vector<iOrganism*> &organisms) throw(int)
{
  // Temporary map of id to index used during the linking phase
  map<int, TreeNode*> idToNode;

  // Create a TreeNode for each organism that was loaded and then
  //   add it to a map that maps id to TreeNode
  vector<iOrganism*>::const_iterator iod = organisms.end();
  for(iod = organisms.begin(); iod != organisms.end(); ++iod)
  {
    if(*iod == 0) { throw 0; }

    TreeNode *tn = new TreeNode(**iod);
    if(tn == 0) { throw 1; }

    idToNode.insert(make_pair((*iod)->GetId(), tn));

    treeNodes.push_back(tn);
    // check for error -- throw 3
  }

  // Traverse all the TreeNodes and attach them to their parent by using
  //   the InsertChild method.
  vector<TreeNode*>::iterator i = treeNodes.end();
  for(i = treeNodes.begin(); i != treeNodes.end(); ++i)
  {
    // Assuming all treeNodes are valid, otherwise the above code would have 
    //   thrown 1.
    int parentId = (*i)->GetData().GetParentId();
    if(parentId == -1)
    {
      // The root has no parent so its parent will be set to itself
      (*i)->SetAsRoot();
      root = *i;
      continue;
    }

    // Look up the parent by id
    map<int, TreeNode*>::iterator temp = idToNode.find(parentId);
    if(temp == idToNode.end()) // Could not find parent
    {
      throw 4;
    }

    // Assuming temp->second will not be 0, otherwise the above code would 
    //   have thrown 1.
    // Set parent for current node
    (*i)->SetParent(*(temp->second));

    // Add this node as child for parent node
    temp->second->InsertChild(**i);
  }

  return;
}

PhylogeneticTree::TreeIterator PhylogeneticTree::Tree::Root(void) const
{
  return TreeIterator(root, 1);
}

const unsigned int PhylogeneticTree::Tree::Size(void) const
{
  return static_cast<unsigned int>(treeNodes.size());
}
