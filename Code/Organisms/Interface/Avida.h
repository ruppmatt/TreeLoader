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

#ifndef __Organisms_Interface_Avida_h__
#define __Organisms_Interface_Avida_h__

#include <istream>
#include <vector>
#include <set>
#include <utility>

#include "PhylogeneticTree/Interface/iOrganism.h"

class AvidaOrganism : public PhylogeneticTree::iOrganism
{
protected:
  AvidaOrganism(void);

public:
  AvidaOrganism(const AvidaOrganism &);
  ~AvidaOrganism(void);

  const AvidaOrganism &operator=(const AvidaOrganism &);

  /*** Interface Begin **************************************/
  const int    GetId(void)        const;
  const int    GetParentId(void)  const;
  const double GetBirthTime(void) const;
  const bool   GetIsAlive(void)   const;
  /*** Interface End   **************************************/

  const int    GetParentDistance(void)    const;
  const int    GetCurrentAlive(void)      const;
  const int    GetTotalExisted(void)      const;
  const int    GetGenomeLength(void)      const;
  const double GetMerit(void)             const;
  const double GetGestationTime(void)     const;
  const double GetFitness(void)           const;
  const int    GetUpdateBorn(void)        const;
  const int    GetUpdateDeactivated(void) const;
  const int    GetPhyloDepth(void)        const;
  const char * const GetGenome(void)      const;

  // Builds a new organism from a string constructed from it's data.
  static AvidaOrganism* Parse(const char * const data) throw(int);

  friend const bool operator==(const AvidaOrganism &, const AvidaOrganism &);

private:
  int id;
  int parentId;
  int parentDistance;
  int currentAlive;
  int totalExisted;
  int genomeLength;
  double merit;
  double gestationTime;
  double fitness;
  int updateBorn;
  int updateDeactivated;
  int phyloDepth;
  char *genome;

  void Assign(const AvidaOrganism &);
};

const bool operator==(const AvidaOrganism &, const AvidaOrganism &);
void LoadAvidaOrganisms(std::vector<PhylogeneticTree::iOrganism*> &organisms,
                        std::istream &in,
                        const bool isDetail) throw(std::pair<int,int>);
std::set<int> CheckForDuplicateIds(const std::vector<PhylogeneticTree::iOrganism*> &) throw(int);

#endif // __Organisms_Interface_Avida_h__
