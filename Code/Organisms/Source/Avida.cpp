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

#include "Organisms/Interface/Avida.h"

using namespace std;
using namespace PhylogeneticTree;

void LoadSingleAvidaOrganism(vector<iOrganism*> &iorganisms,
                             istream &in,
                             const bool isDetail) throw(int);

AvidaOrganism::AvidaOrganism(void)
: id(-1),
  parentId(-1),
  parentDistance(0),
  currentAlive(0),
  totalExisted(0),
  genomeLength(0),
  merit(0),
  gestationTime(0),
  fitness(0),
  updateBorn(0),
  updateDeactivated(0),
  phyloDepth(-1),
  genome(0)
{
  return;
}

AvidaOrganism::AvidaOrganism(const AvidaOrganism &data)
: id(-1),
  parentId(-1),
  parentDistance(0),
  currentAlive(0),
  totalExisted(0),
  genomeLength(0),
  merit(0),
  gestationTime(0),
  fitness(0),
  updateBorn(0),
  updateDeactivated(0),
  phyloDepth(-1),
  genome(0)
{
  Assign(data);
  return;
}

AvidaOrganism::~AvidaOrganism(void)
{
  if(genome != 0)
  {
    delete [] genome;
  }
  genome = 0;
  genomeLength = 0;

  return;
}

const AvidaOrganism &AvidaOrganism::operator=(const AvidaOrganism &data)
{
  if(&data != this) { Assign(data); }
  return *this;
}

const int AvidaOrganism::GetId(void) const
{
  return id;
}

const int AvidaOrganism::GetParentId(void) const
{
  return parentId;
}

const double AvidaOrganism::GetBirthTime(void) const
{
  return updateBorn;
}

const bool AvidaOrganism::GetIsAlive(void) const
{
  return (currentAlive > 0) ? true : false;
}

const int AvidaOrganism::GetParentDistance(void) const
{
  return parentDistance;
}

const int AvidaOrganism::GetCurrentAlive(void) const
{
  return currentAlive;
}

const int AvidaOrganism::GetTotalExisted(void) const
{
  return totalExisted;
}

const int AvidaOrganism::GetGenomeLength(void) const
{
  return genomeLength;
}

const double AvidaOrganism::GetMerit(void) const
{
  return merit;
}

const double AvidaOrganism::GetGestationTime(void) const
{
  return gestationTime;
}

const double AvidaOrganism::GetFitness(void) const
{
  return fitness;
}

const int AvidaOrganism::GetUpdateBorn(void) const
{
  return updateBorn;
}

const int AvidaOrganism::GetUpdateDeactivated(void) const
{
  return updateDeactivated;
}

const int AvidaOrganism::GetPhyloDepth(void) const
{
  return phyloDepth;
}

const char * const AvidaOrganism::GetGenome(void) const
{
  return genome;
}

void AvidaOrganism::Assign(const AvidaOrganism &data)
{
  if(genome != 0)
  {
    delete [] genome;
  }
  genome = 0;
  genomeLength = 0;

  id                = data.id;
  parentId          = data.parentId;
  parentDistance    = data.parentDistance;
  currentAlive      = data.currentAlive;
  totalExisted      = data.totalExisted;
  genomeLength      = data.genomeLength;
  merit             = data.merit;
  gestationTime     = data.gestationTime;
  fitness           = data.fitness;
  updateBorn        = data.updateBorn;
  updateDeactivated = data.updateDeactivated;
  phyloDepth        = data.phyloDepth;

  if(data.genome == 0 || data.genomeLength == 0 ||
     static_cast<int>(strlen(data.genome)) != data.genomeLength)
  {
    genome = 0;
    genomeLength = 0;
  }
  else
  {
    genome = new char[genomeLength+1];
    if(genome == 0) { genomeLength = 0; }
    else { strcpy(genome, data.genome); }
  }

  return;
}

AvidaOrganism* AvidaOrganism::Parse(const char * const data) throw(int)
{
  stringstream ss;
  ss << data;

  ss >> ws;

  char c = static_cast<char>(ss.peek());
  if(ss.eof() || c == '#')
  {
    // do nothing if the line is blank or it is a comment
    return 0;
  }

  AvidaOrganism od;

  ss >> od.id;
  // Make sure there were no errors while reading in the data thus far
  if(ss.fail() || ss.bad() || ss.eof())
  {
    throw 1;
  }

  ss >> od.parentId;
  // Make sure there were no errors while reading in the data thus far
  if(ss.fail() || ss.bad() || ss.eof())
  {
    throw 2;
  }

  ss >> od.parentDistance;
  // Make sure there were no errors while reading in the data thus far
  if(ss.fail() || ss.bad() || ss.eof())
  {
    throw 3;
  }

  ss >> od.currentAlive;
  // Make sure there were no errors while reading in the data thus far
  if(ss.fail() || ss.bad() || ss.eof())
  {
    throw 4;
  }

  ss >> od.totalExisted;
  // Make sure there were no errors while reading in the data thus far
  if(ss.fail() || ss.bad() || ss.eof())
  {
    throw 5;
  }

  ss >> od.genomeLength;
  // Make sure there were no errors while reading in the data thus far
  if(ss.fail() || ss.bad() || ss.eof())
  {
    throw 6;
  }

  ss >> od.merit;
  // Make sure there were no errors while reading in the data thus far
  if(ss.fail() || ss.bad() || ss.eof())
  {
    throw 7;
  }

  ss >> od.gestationTime;
  // Make sure there were no errors while reading in the data thus far
  if(ss.fail() || ss.bad() || ss.eof())
  {
    throw 8;
  }

  ss >> od.fitness;
  // Make sure there were no errors while reading in the data thus far
  if(ss.fail() || ss.bad() || ss.eof())
  {
    throw 9;
  }

  ss >> od.updateBorn;
  // Make sure there were no errors while reading in the data thus far
  if(ss.fail() || ss.bad() || ss.eof())
  {
    throw 10;
  }

  ss >> od.updateDeactivated;
  // Make sure there were no errors while reading in the data thus far
  if(ss.fail() || ss.bad() || ss.eof())
  {
    throw 11;
  }

  ss >> od.phyloDepth;
  // Make sure there were no errors while reading in the data thus far
  if(ss.fail() || ss.bad() || ss.eof())
  {
    throw 12;
  }

  // Read in the genome
  char genome[2048];
  ss >> genome;

  if(static_cast<int>(strlen(genome)) != od.genomeLength ||
     ((ss.fail() || ss.bad()) && !ss.eof()))
  {
    throw 13;
  }
  od.genome = genome;

  AvidaOrganism *o = 0;
  // In case I decide to put some error checking in the copy
  try
  {
    o = new AvidaOrganism(od);
  }
  catch(...)
  {
    if(o != 0)
    {
      delete o;
      o = 0;
    }
    od.genome = 0;
    throw 14;
  }

  // Must set od.genome to zero because it is assigned a static 
  //   array can should not be deleted in the destructor for od.
  od.genome = 0;

  return o;
}

const bool operator==(const AvidaOrganism &o1, const AvidaOrganism &o2)
{
  if(o1.id                == o2.id                &&
     o1.parentId          == o2.parentId          &&
     o1.parentDistance    == o2.parentDistance    &&
     o1.currentAlive      == o2.currentAlive      &&
     o1.totalExisted      == o2.totalExisted      &&
     o1.genomeLength      == o2.genomeLength      &&
     o1.merit             == o2.merit             &&
     o1.gestationTime     == o2.gestationTime     &&
     o1.fitness           == o2.fitness           &&
     o1.updateBorn        == o2.updateBorn        &&
     o1.updateDeactivated == o2.updateDeactivated &&
     o1.phyloDepth        == o2.phyloDepth        &&
     strcmp(o1.genome, o2.genome) == 0)
  {
    return true;
  }
  return false;
}

void LoadAvidaOrganisms(vector<iOrganism*> &organisms,
                        istream &in,
                        const bool isDetail) throw(pair<int,int>)
{
  char line[2048];

  // read in each line, and start counting line numbers at 1
  int i = 1;
  while(!in.eof())
  {
    // set line to the empty string
    line[0] = '\0';

    // read in a line from the file
    in.getline(line, 2047);

    // create a new organism from the line using the Parse function
    try
    {
      // Not all lines are organisms so a return of zero indicates that
      //   a valid organism was not loaded so skip.
      iOrganism *od = AvidaOrganism::Parse(line);

      // It will only be zero for comments and blank lines, otherwise it
      //   throws and exception upon error.
      if(od == 0) { continue; }

      // Insert first so if an error follows it will be deleted when
      //   the calling function cleans up the organism vector.
      organisms.push_back(od);

      // Make sure that only those who are alive are in the detail file.
      if(isDetail == false && od->GetIsAlive() == true)
      {
        throw -1;
      }
      // Make sure that those in the detail file are not dead.
      else if(isDetail == true && od->GetIsAlive() == false)
      {
        throw -2;
      }
    }
    catch(int x)
    {
      throw make_pair(i, x);
    }

    ++i;
  }

  return;
}

set<int> CheckForDuplicateIds(const vector<iOrganism*> &organisms) throw(int)
{
  // By using sets, we remove the issue of possible duplicate ids.
  set<int> ids;
  set<int> duplicates;

  vector<iOrganism*>::const_iterator i = organisms.begin();
  for(; i!= organisms.end(); ++i)
  {
    if(*i == 0) { throw 0; }

    int id = (*i)->GetId();

    // See if the id is already in the set of ids
    set<int>::iterator si = ids.find(id);
    if(si == ids.end())
    {
      // If the id is not found, add it to the set of ids
      ids.insert(id);
    }
    else
    {
      // If the id is found, add it to the set of duplicates.
      duplicates.insert(id);
    }
  }

  return duplicates;
}
