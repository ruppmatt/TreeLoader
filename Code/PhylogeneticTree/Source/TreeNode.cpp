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

#include "PhylogeneticTree/Include/TreeNode.h"

#include "PhylogeneticTree/Interface/iOrganism.h"

using namespace std;

PhylogeneticTree::TreeNode::TreeNode(const iOrganism &od)
: data(const_cast<iOrganism&>(od)), parent(0), deleteMe(false)
{
  return;
}

PhylogeneticTree::TreeNode::~TreeNode(void)
{
  parent = 0;
  children.clear();
  deleteMe = false;
  return;
}

void PhylogeneticTree::TreeNode::ChangeChild(const TreeNode &from,
                                             const TreeNode &to)
{
  vector<TreeNode*>::iterator iNode = children.end();

  for(iNode = children.begin(); iNode != children.end(); ++iNode)
  {
    if(*iNode == &from)
    {
      *iNode = const_cast<TreeNode*>(&to);
      break;
    }
  }

  return;
}

const vector<PhylogeneticTree::TreeNode*> &
PhylogeneticTree::TreeNode::GetChildren(void) const
{
  return children;
}

const PhylogeneticTree::iOrganism &
PhylogeneticTree::TreeNode::GetData(void) const
{
  // Assuming data is always valid due to the way it is set.
  return data;
}

const bool PhylogeneticTree::TreeNode::GetDeleteMe(void) const
{
  return deleteMe;
}

PhylogeneticTree::TreeNode *PhylogeneticTree::TreeNode::GetParent(void) const
{
  return parent;
}

const unsigned int PhylogeneticTree::TreeNode::HowManyChildren(void) const
{
  return static_cast<unsigned int>(children.size());
}

void PhylogeneticTree::TreeNode::InsertChild(const TreeNode &node) throw(int)
{
  vector<TreeNode*>::iterator iNode = children.end();

  for(iNode = children.begin(); iNode != children.end(); ++iNode)
  {
    if(&node == *iNode)
    {
      throw 0;
    }
  }
  if(&node == parent)
  {
    throw 1;
  }

  children.push_back(const_cast<TreeNode*>(&node));

  return;
}

void PhylogeneticTree::TreeNode::RemoveChild(const TreeNode &node)
{
  vector<TreeNode*>::iterator iNode = children.end();

  for(iNode = children.begin(); iNode != children.end(); ++iNode)
  {
    if(*iNode == &node)
    {
      children.erase(iNode);
      break;
    }
  }

  return;
}

void PhylogeneticTree::TreeNode::SetAsRoot(void)
{
  parent = 0;
  return;
}

void PhylogeneticTree::TreeNode::SetDeleteMe(const bool b)
{
    deleteMe = b;
    return;
}

void PhylogeneticTree::TreeNode::SetParent(const TreeNode &parentNode)
{
  parent = const_cast<TreeNode*>(&parentNode);
  return;
}
