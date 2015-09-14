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

#ifndef __Organisms_Interface_SimpleOrganism_h__
#define __Organisms_Interface_SimpleOrganism_h__

#include <istream>
#include <vector>
#include <set>
#include <utility>

#include "PhylogeneticTree/Interface/iOrganism.h"

class SimpleOrganism : public PhylogeneticTree::iOrganism
{
protected:
  SimpleOrganism(void);

public:
  SimpleOrganism(int id, int parentId);
  SimpleOrganism(const SimpleOrganism &);
  ~SimpleOrganism(void);

  const SimpleOrganism &operator=(const SimpleOrganism &);

  /*** Interface Begin **************************************/
  const int    GetId(void)        const;
  const int    GetParentId(void)  const;
  const double GetBirthTime(void) const;
  const bool   GetIsAlive(void)   const;
  /*** Interface End   **************************************/

  friend const bool operator==(const SimpleOrganism &, const SimpleOrganism &);

private:
  int id;
  int parentId;
  int currentAlive;
  int updateBorn;

  void Assign(const SimpleOrganism &data);
};

const bool operator==(const SimpleOrganism &, const SimpleOrganism &);

#endif // __Organisms_Interface_Avida_h__
