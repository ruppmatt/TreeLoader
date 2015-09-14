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

#include "PhylogeneticTree/Interface/TreeIterator.h"

#include "PhylogeneticTree/Include/TreeNode.h"

using namespace std;

PhylogeneticTree::TreeIterator::TreeIterator(TreeNode *n, const int _depth)
: node(n), depth(_depth)
{
  return;
}

PhylogeneticTree::TreeIterator::TreeIterator(const TreeIterator &t)
: node(t.node), depth(t.depth)
{
  return;
}

PhylogeneticTree::TreeIterator::~TreeIterator(void)
{
  node = 0;
  depth = 0;
  return;
}

void PhylogeneticTree::TreeIterator::Next_Depth(void)
{
  if(node == 0) { return; }

  TreeNode *currentNode = node;

  // 1. Go up one node
  node = node->GetParent();
  --depth;

  // 2. If up is 0, we were at the root which is the last node.  It is now
  //      equivalent to the "end", ie. node == 0, depth == 0
  if(node == 0) { return; }

  // 3. If the node we just left was last in the list of children for this
  //      this node, stop.
  if(node->GetChildren().back() == currentNode)
  {
    return;
  }

  // 4. Else goto next child in the list and all the way down to the left
  const vector<TreeNode*> &children = node->GetChildren();
  vector<TreeNode*>::const_iterator i = children.end();
  for(i = children.begin(); i != children.end(); ++i)
  {
    if(*i == currentNode)
    {
      ++i;
      node = *i;
      ++depth;
      break;
    }
  }
  if(i == children.end() || node == 0)
  {
    node = 0;
    depth = 0;
    return;
  }

  // Traverse down the tree as far to the left as possible from node.  I 
  //   assume there are no cycles in the tree.
  while(node->HowManyChildren() > 0)
  {
    node = node->GetChildren().front();
    ++depth;
  }

  return;
}

void PhylogeneticTree::TreeIterator::operator--(void)
{
  Previous_Depth();
  return;
}

void PhylogeneticTree::TreeIterator::operator--(int)
{
  Previous_Depth();
  return;
}

const PhylogeneticTree::iTreeNode *
PhylogeneticTree::TreeIterator::operator*(void) const
{
  return node;
}

const bool PhylogeneticTree::operator< (const TreeIterator &t1,
                                        const TreeIterator &t2)
{
  return t1.node < t2.node;
}

const bool PhylogeneticTree::operator==(const TreeIterator &t1,
                                        const TreeIterator &t2)
{
  return t1.node == t2.node;
}

const bool PhylogeneticTree::operator!=(const TreeIterator &t1,
                                        const TreeIterator &t2)
{
  return t1.node != t2.node;
}

void PhylogeneticTree::TreeIterator::operator++(void)
{
  Next_Depth();
  return;
}

void PhylogeneticTree::TreeIterator::operator++(int)
{
  Next_Depth();
  return;
}

const PhylogeneticTree::TreeIterator &
PhylogeneticTree::TreeIterator::operator=(const TreeIterator &t)
{
  node = t.node;
  depth = t.depth;
  return *this;
}

void PhylogeneticTree::TreeIterator::Previous_Depth(void)
{
  if(node == 0) { return; }

  // 1. If the node is not a leaf, go to last child
  if(node->HowManyChildren() > 0)
  {
    node = node->GetChildren().back();
    ++depth;
    return;
  }

  TreeNode *currentNode = 0;

  do
  {
    currentNode = node;

    // 2. Go up one node
    node = node->GetParent();
    --depth;

    // 3. If up is 0, we were at the root which is the last node.  It is now
    //      equivalent to the "end", ie. node == 0.
    if(node == 0) { return; }

    // 3. If the node we just left was first in the list of children for this
    //      node go to step 1.
  }
  while(node->GetChildren().front() == currentNode);

  // 4. Once we reach a parent where the previous node was not first in the 
  //      list of children.  Find the previous child and go to that node.
  const vector<TreeNode*> &children = node->GetChildren();
  vector<TreeNode*>::const_iterator i = children.end();
  for(i = children.begin(); i != children.end(); ++i)
  {
    if(*i == currentNode)
    {
      --i;
      node = *i;
      ++depth;
      break;
    }
  }
  if(i == children.end() || node == 0)
  {
    node = 0;
    depth = 0;
    return;
  }

  return;
}

void PhylogeneticTree::TreeIterator::Up(void)
{
  if(node == 0) { return; }
  node = node->GetParent();
  --depth;
  return;
}
PhylogeneticTree::TreeIterator
PhylogeneticTree::TreeIterator::GetParent(void) const
{
  if(node == 0) { return TreeIterator(0,0); }
  return TreeIterator(node->GetParent(), depth - 1);
}

vector<PhylogeneticTree::TreeIterator>
PhylogeneticTree::TreeIterator::GetChildren(void) const
{
  vector<TreeIterator> children;

  // return empty list if there are no children
  if(node == 0) return children;

  const vector<TreeNode*> &childNodes = node->GetChildren();
  vector<TreeNode*>::const_iterator i = childNodes.begin();
  for(; i != childNodes.end(); ++i)
    children.push_back(TreeIterator(*i, depth+1));

  return children;
}

