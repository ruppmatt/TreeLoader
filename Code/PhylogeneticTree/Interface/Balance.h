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

#ifndef __PhylogeneticTree_Interface_Balance_h__
#define __PhylogeneticTree_Interface_Balance_h__

#include <ostream>

class OutputStream;

namespace PhylogeneticTree
{
  class Tree;

  double ComputeBalance(const Tree &,
                        OutputStream &output,
                        const bool generateReport,
                        std::ostream &reportTxt,
                        std::ostream &reportCsv,
                        std::ostream &listTxt,
                        std::ostream &listCsv) throw(int);

} // namespace PhylogeneticTree

#endif // __PhylogeneticTree_Interface_Balance_h__
