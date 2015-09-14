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

#include <iostream>
#include <cstring>
#include <cstdlib>

#include "ProgramInterface.h"

using namespace std;

void HowTo(void);

int main(int argc, char **argv)
{
  if(argc == 1) { HowTo(); return 0; }

  bool calcGamma   = false;
  bool calcNCStem  = false;
  bool calcBalance = false;

  bool verboseOn = false;
  bool outputToFile = false;
  bool generateReport = false;
  bool generateNewick = false;

  unsigned int samples = 0;
  unsigned int leavesToSample = 0;
  unsigned int timeCutoff = 0;
  char *historicFilename = 0;
  char *detailFilename = 0;

  for(int i=1; i<argc; i++)
  {
    if(strcmp(argv[i], "-h") == 0)
    {
      if(argc <= i+1) { HowTo(); return 0; }
      historicFilename = argv[i+1];
      ++i;
    }
    else if(strcmp(argv[i], "-d") == 0)
    {
      if(argc <= i+1) { HowTo(); return 0; }
      detailFilename = argv[i+1]; ++i;
    }
    else if(strcmp(argv[i], "-v") == 0)
    {
      verboseOn = true;
    }
    else if(strcmp(argv[i], "-f") == 0)
    {
      outputToFile = true;
    }
    else if(strcmp(argv[i], "-r") == 0)
    {
      generateReport = true;
    }
    else if(strcmp(argv[i], "-n") == 0)
    {
      generateNewick = true;
    }
    else if(strcmp(argv[i], "-b") == 0)
    {
      calcBalance = true;
    }
    else if(strcmp(argv[i], "-g") == 0)
    {
      calcGamma = true;
    }
    else if(strcmp(argv[i], "-ncstem") == 0)
    {
      calcNCStem = true;
    }
    else if(strcmp(argv[i], "-s") == 0)
    {
      if(argc <= i+1) { HowTo(); return 0; }
      samples = atoi(argv[i+1]);
      ++i;
    }
    else if(strcmp(argv[i], "-l") == 0)
    {
      if(argc <= i+1) { HowTo(); return 0; }
      leavesToSample = atoi(argv[i+1]);
      ++i;
    }
    else if(strcmp(argv[i], "-t") == 0)
    {
      if(argc <= i+1) { HowTo(); return 0; }
      timeCutoff = atoi(argv[i+1]);
      ++i;
    }
    else { HowTo(); return 0; }
  }

  Run(historicFilename, detailFilename, verboseOn, outputToFile, 
      generateReport, generateNewick, calcGamma, calcNCStem, calcBalance,
      samples, leavesToSample, timeCutoff);

  return 0;
}

void HowTo(void)
{
  cout << "Incorrect inputs to this program:" << endl;
  cout << "TreeLoader [options]" << endl;
  cout << "  -h [historic_file]" << endl;
  cout << "  -d [detail_file]" << endl;
  cout << "  -t [time cutoff]" << endl;
  cout << endl;
  cout << "  -s [how_many_samples]            optional" << endl;
  cout << "  -l [quantity_leafs_to_sample]    optional" << endl;
  cout << endl;
  cout << "  -g                               (run gamma calculation)" << endl;
  cout << "  -ncstem                          (run NC Stem calculation)" << endl;
  cout << "  -b                               (run balance calculation)" << endl;
  cout << endl;
  cout << "  -v                               (verbose output on)" << endl;
  cout << "  -f                               (generator output files)" << endl;
  cout << "  -r                               (generate report files)" << endl;
  cout << "  -n                               (generate newick files)" << endl;

  return;
}
