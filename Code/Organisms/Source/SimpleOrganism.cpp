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

#include <cstring>
#include <sstream>

#include "Organisms/Interface/SimpleOrganism.h"

using namespace std;
using namespace PhylogeneticTree;

SimpleOrganism::SimpleOrganism(void)
: id(-1),
  parentId(-1),
  currentAlive(0),
  updateBorn(0)
{
  return;
}

SimpleOrganism::SimpleOrganism(int m_id, int m_parentId)
: id(m_id),
  parentId(m_parentId),
  currentAlive(0),
  updateBorn(0)
{
  return;
}

SimpleOrganism::SimpleOrganism(const SimpleOrganism &data)
: id(-1),
  parentId(-1),
  currentAlive(0),
  updateBorn(0)
{
  Assign(data);
  return;
}

SimpleOrganism::~SimpleOrganism(void)
{
  return;
}

const SimpleOrganism &SimpleOrganism::operator=(const SimpleOrganism &data)
{
  if(&data != this) { Assign(data); }
  return *this;
}

const int SimpleOrganism::GetId(void) const
{
  return id;
}

const int SimpleOrganism::GetParentId(void) const
{
  return parentId;
}

const double SimpleOrganism::GetBirthTime(void) const
{
  return updateBorn;
}

const bool SimpleOrganism::GetIsAlive(void) const
{
  return (currentAlive > 0) ? true : false;
}


const bool operator==(const SimpleOrganism &o1, const SimpleOrganism &o2)
{
  if(o1.id                == o2.id                &&
     o1.parentId          == o2.parentId          &&
     o1.currentAlive      == o2.currentAlive      &&
     o1.updateBorn        == o2.updateBorn)
  {
    return true;
  }
  return false;
}

void SimpleOrganism::Assign(const SimpleOrganism &data)
{
  id           = data.id;
  parentId     = data.parentId;
  currentAlive = data.currentAlive;
  updateBorn   = data.updateBorn;

  return;
}

