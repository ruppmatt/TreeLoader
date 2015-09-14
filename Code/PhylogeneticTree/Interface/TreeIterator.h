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

#ifndef __PhylogeneticTree_Interface_TreeIterator_h__
#define __PhylogeneticTree_Interface_TreeIterator_h__

#include <vector>

namespace PhylogeneticTree
{
  class iTreeNode;
  class TreeNode;

  // The TreeIterator uses a specific ordering for tree traversal.
  //   It first assumes that any given node can have more than two
  //   children and that there are no cycles.  For a simplified
  //   description consider a tree with a root and three children
  //   the order that will be retrieved will be
  //   child1, child2, child3, root

  class TreeIterator
  {
  private:
    TreeNode *node;
    int depth;

    TreeIterator(TreeNode *startNode=0, int depth=0);

  public:
    TreeIterator(const TreeIterator &);
    ~TreeIterator(void);

    const TreeIterator &operator=(const TreeIterator &);

    // Traversal
    void operator++(void); // prefix
    void operator++(int);  // postfix
    void operator--(void); // prefix
    void operator--(int);  // postfix
    void Up(void);

    // Data Access
    const iTreeNode *operator*(void) const; // dereference
    int GetDepth(void) const { return depth; }
    TreeIterator GetParent(void) const;
    std::vector<TreeIterator> GetChildren(void) const;

  private:
    void Next_Depth(void);
    void Previous_Depth(void);

    friend const bool operator< (const TreeIterator &, const TreeIterator &);
    friend const bool operator==(const TreeIterator &, const TreeIterator &);
    friend const bool operator!=(const TreeIterator &, const TreeIterator &);

    friend class Tree;
  };

  const bool operator< (const TreeIterator &, const TreeIterator &);
  const bool operator==(const TreeIterator &, const TreeIterator &);
  const bool operator!=(const TreeIterator &, const TreeIterator &);

} // namespace PhylogeneticTree

#endif // __PhylogeneticTree_Interface_TreeIterator_h__
