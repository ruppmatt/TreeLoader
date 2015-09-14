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

#ifndef __PhylogeneticTree_Interface_Tree_h__
#define __PhylogeneticTree_Interface_Tree_h__

#include <vector>
#include <map>

#include "PhylogeneticTree/Interface/TreeIterator.h"

namespace PhylogeneticTree
{
  class TreeNode;
  class iOrganism;

  class Tree
  {
  private:
    TreeNode *root;
    std::vector<TreeNode*> treeNodes;

  public:
    Tree(const std::vector<iOrganism*> &) throw(int);
    Tree(const std::map<int,int> &, std::vector<iOrganism*> &output) throw(int);
    Tree(const Tree &) throw(int);
    ~Tree(void);

    const Tree &operator=(const Tree &) throw(int);

    TreeIterator              Begin(void)        const throw(int);
    void                      ClearDeleted(void);
    // For both DeleteLeaf and DeleteSingle
    // Removes it from tree, but does not delete it only marks it for delete
    //   To actually delete the node in a proper fashion causes some
    //   algorithms to run in O(n^2) time.  By utilizing a delete flag method
    //   and its proper use causes it to run in O(n) time.  The time
    //   difference is significant.
    void                      DeleteLeaf(TreeIterator &);
    void                      DeleteSingle(TreeIterator &);
    TreeIterator              End(void)          const;
    TreeIterator              Find(const int id) const throw(int);
    std::vector<TreeIterator> GetLeaves(void)    const throw(int);
    TreeIterator              Last(void)         const;
    TreeIterator              Root(void)         const;
    const unsigned int        Size(void)         const;

  private:
    void CleanUp(void);
    void Copy(const Tree &) throw(int);
    void ProcessOrganisms(const std::vector<iOrganism*> &) throw(int);
    void ConstructLayout(const std::map<int,int> &,
                         std::vector<iOrganism*> &output) throw(int);
  };

} // namespace PhylogeneticTree

#endif // __PhylogeneticTree_Interface_Tree_h__
