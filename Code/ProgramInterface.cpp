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

#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <cstring>

#include "ProgramInterface.h"

#include "PhylogeneticTree/Interface/Tree.h"
#include "PhylogeneticTree/Interface/iTreeNode.h"
#include "PhylogeneticTree/Interface/Utilities.h"
#include "PhylogeneticTree/Interface/GammaFunctions.h"
#include "PhylogeneticTree/Interface/NoncumulativeStem.h"
#include "PhylogeneticTree/Interface/Balance.h"
#include "PhylogeneticTree/Interface/NewickOutput.h"
#include "Organisms/Interface/Avida.h"
#include "Support/Interface/OutputStream.h"
#include "Support/Interface/random.h"

using namespace std;
using namespace PhylogeneticTree;

/*** Helper Functions *******************************************************/
// Set detailFilename to zero if you want no file output for specific calc.
const double CalculateGamma(const Tree &, const char * const detailFilename,
                            const double timeCutoff);
const double CalculateNCStem(const Tree &, const char * const detailFilename,
                             const double timeCutoff);
const double CalculateBalance(const Tree &,
                              const bool generateReport,
                              ostream &reportTxt, ostream &reportCsv,
                              ostream &listTxt, ostream &listCsv);
void Cleanup(Tree **, vector<iOrganism*> &);
void CreateOutput(const char *const detailFilename, const char *const extension,
                  ofstream &, ofstream &);
void LoadOrganisms(vector<iOrganism*> &organisms,
                   const char * const historicFilename,
                   const char * const detailFilename);
void PrepareTree(Tree &);
const double RunSamples(const Tree &, unsigned int samples,
                        unsigned int leavesToSample,
                        const double trueValue,
                        const double timeCutoff,
                        const int method,
                        const bool generateReport,
                        const bool generateNewick,
                        ostream &reportTxt,
                        ostream &reportCsv,
                        ostream &listTxt,
                        ostream &listCsv,
                        const char * const newickFilenameBase,
                        const char * const outFilename);

/*** Global Variables *******************************************************/
// Used to control whether console output is displayed.
OutputStream output(&cout, false);
RandomNumberGenerator rng;

/*** Main Function **********************************************************/
void Run(const char * const historicFilename,
         const char * const detailFilename,
         const bool verboseOn,
         const bool outputToFile,
         const bool generateReport,
         const bool generateNewick,
         const bool calcGamma,
         const bool calcNCStem,
         const bool calcBalance,
         unsigned int samples,
         unsigned int leavesToSample,
         unsigned int timeCutoff)
{
  // Setup output
  output.SetShowState(verboseOn);

  // Load organisms
  vector<iOrganism*> organisms;
  try { LoadOrganisms(organisms, historicFilename, detailFilename); }
  catch(int) { return; }

  // Create and process the full tree
  Tree *fullTree = 0;
  output << "Creating full tree-" << endl;
  try { fullTree = new Tree(organisms); }
  catch(int x)
  {
    output << "Failed to create tree - " << x << endl;
    Cleanup(0, organisms);
    return;
  }

  try { PrepareTree(*fullTree); }
  catch(int) { Cleanup(&fullTree, organisms); return; }

  // Create Newick Output
  if(generateNewick == true)
  {
    try { NewickOutput(*fullTree, timeCutoff, detailFilename, output); }
    catch(int) { Cleanup(&fullTree, organisms); return; }
  }

  // Create Report
  ofstream gammaReportFileTxt;
  ofstream gammaReportFileCsv;
  ofstream ncstemReportFileTxt;
  ofstream ncstemReportFileCsv;
  ofstream balanceReportFileTxt;
  ofstream balanceReportFileCsv;
  ofstream balanceListFileTxt;
  ofstream balanceListFileCsv;
  if(generateReport == true)
  {
    try
    {
      if(calcGamma)
        CreateOutput(detailFilename, "gamma.report",
                     gammaReportFileTxt, gammaReportFileCsv);
      if(calcNCStem)
        CreateOutput(detailFilename, "ncstem.report",
                     ncstemReportFileTxt, ncstemReportFileCsv);
      if(calcBalance)
        CreateOutput(detailFilename, "balance.report",
                     balanceReportFileTxt, balanceReportFileCsv);
      if(calcBalance)
        CreateOutput(detailFilename, "balance.list",
                     balanceListFileTxt, balanceListFileCsv);
    }
    catch(int)
    {
      gammaReportFileTxt.close();
      gammaReportFileCsv.close();
      ncstemReportFileTxt.close();
      ncstemReportFileCsv.close();
      balanceReportFileTxt.close();
      balanceReportFileCsv.close();
      balanceListFileTxt.close();
      balanceListFileCsv.close();
      Cleanup(&fullTree, organisms);
      return;
    }

    if(calcGamma)
    {
      gammaReportFileTxt << "#Type   Id       Gamma" << endl;
      gammaReportFileCsv << "Type, Id, Gamma" << endl;
    }

    if(calcNCStem)
    {
      ncstemReportFileTxt << "#Type   Id       NCStem" << endl;
      ncstemReportFileCsv << "Type, Id, NCStem" << endl;
    }

    if(calcBalance)
    {
      balanceReportFileTxt << "Index  IC_Min  IC_Max" << endl;
      balanceReportFileCsv << "Index, IC_Min, IC_Max" << endl;
      balanceListFileTxt << "#id  update_born  num_children  ";
      balanceListFileTxt << "[nodes_per_child]+  " << endl;
      balanceListFileCsv << "#id, update_born, num_children, ";
      balanceListFileCsv << "[nodes_per_child]+  " << endl;
    }
  }

  // Calculate gamma for the full tree
  double gammaValue = 0;
  if(calcGamma)
  {
    output << "Calculate gamma for the full tree-" << endl;
    gammaValue = CalculateGamma(*fullTree,
                                (outputToFile) ? detailFilename : 0,
                                 static_cast<double>(timeCutoff));
    if(generateReport == true)
    {
      gammaReportFileTxt << "1 0        " << gammaValue << endl;
      gammaReportFileCsv << "Full,0," << gammaValue << endl;
    }
  }

  // Calculate NCStem for the full tree
  double ncstemValue = 0;
  if(calcNCStem)
  {
    output << "Calculate noncumulative stemminess for the full tree-" << endl;
    ncstemValue = CalculateNCStem(*fullTree,
                                  (outputToFile) ? detailFilename : 0,
                                  static_cast<double>(timeCutoff));
    if(generateReport == true)
    {
      ncstemReportFileTxt << "1 0        " << ncstemValue << endl;
      ncstemReportFileCsv << "Full,0," << ncstemValue << endl;
    }
  }

  // Calculate Balance for the full tree
  double balanceValue = 0;
  if(calcBalance)
  {
    output << "Calculate balance for the full tree-" << endl;
    balanceValue = CalculateBalance(*fullTree,
                                    generateReport,
                                    balanceReportFileTxt, balanceReportFileCsv,
                                    balanceListFileTxt, balanceListFileCsv);
  }

  // Calculate samples
  if(samples != 0 && leavesToSample != 0)
  {
    try
    {
      if(calcGamma)
        RunSamples(*fullTree, samples, leavesToSample, gammaValue,
                   static_cast<double>(timeCutoff),
                   1, // method == 1 == gamma
                   generateReport, generateNewick,
                   gammaReportFileTxt, gammaReportFileCsv,
                   gammaReportFileTxt, gammaReportFileCsv,
                   detailFilename,
                   (outputToFile) ? detailFilename : 0);

      if(calcNCStem)
        RunSamples(*fullTree, samples, leavesToSample, ncstemValue,
                   static_cast<double>(timeCutoff),
                   2, // method == 2 == NCStem
                   generateReport, generateNewick,
                   ncstemReportFileTxt, ncstemReportFileCsv,
                   ncstemReportFileTxt, ncstemReportFileCsv,
                   detailFilename,
                   (outputToFile) ? detailFilename : 0);

      if(calcBalance)
        RunSamples(*fullTree, samples, leavesToSample, balanceValue,
                   static_cast<double>(timeCutoff),
                   3, // method == 3 == Balance
                   generateReport, generateNewick,
                   balanceReportFileTxt, balanceReportFileCsv,
                   balanceListFileTxt, balanceListFileCsv,
                   detailFilename,
                   (outputToFile) ? detailFilename : 0);
    }
    catch(...)
    {
      output << "Failed to examine samples." << endl;
      // Close reports
      if(generateReport == true)
      {
        gammaReportFileTxt.close();
        gammaReportFileCsv.close();
        ncstemReportFileTxt.close();
        ncstemReportFileCsv.close();
        balanceReportFileTxt.close();
        balanceReportFileCsv.close();
        balanceListFileTxt.close();
        balanceListFileCsv.close();
      }
      Cleanup(&fullTree, organisms);
    }
  }

  // Close reports
  if(generateReport == true)
  {
    gammaReportFileTxt.close();
    gammaReportFileCsv.close();
    ncstemReportFileTxt.close();
    ncstemReportFileCsv.close();
    balanceReportFileTxt.close();
    balanceReportFileCsv.close();
    balanceListFileTxt.close();
    balanceListFileCsv.close();
  }

  // Clean up data
  Cleanup(&fullTree, organisms);

  return;
}

/*** Global Function Definitions ********************************************/
const double CalculateGamma(const Tree &tree,
                            const char * const detailFilename,
                            const double timeCutoff)
{
  // Calculate gamma for the given tree
  const double value = ComputeGamma(tree,
                                    timeCutoff,
                                    detailFilename,
                                    output);
  output << endl;
  output << "Gamma = " << value << endl;
  output << endl;

  return value;
}

const double CalculateNCStem(const Tree &tree,
                             const char * const detailFilename,
                             const double timeCutoff)
{
  // Calculate noncumulative stemminess for the given tree
  const double value = NoncumulativeStem(tree,
                                         timeCutoff,
                                         detailFilename,
                                         output);
  output << endl;
  output << "NCStem = " << value << endl;
  output << endl;

  return value;
}

const double CalculateBalance(const Tree &tree,
                              const bool generateReport,
                              ostream &reportTxt, ostream &reportCsv,
                              ostream &listTxt, ostream &listCsv)
{
  try
  {
    // Calculate balance for the given tree
    const double value = ComputeBalance(tree,
                                        output,
                                        generateReport,
                                        reportTxt, reportCsv,
                                        listTxt, listCsv);

    return value;
  }
  catch(int x)
  {
    output << "Error (" << x << "): Could not compute balance." << endl;
  }

  return 0.0;
}

void Cleanup(Tree **fullTree, vector<iOrganism*> &organisms)
{
  // Clean up tree
  if(fullTree != 0 && *fullTree != 0)
  {
    delete *fullTree;
    *fullTree = 0;
  }

  // Clean up and delete organisms
  vector<iOrganism*>::iterator oi = organisms.begin();
  for(; oi != organisms.end(); ++oi)
  {
    if(*oi != 0)
    {
      delete *oi;
      *oi = 0;
    }
  }

  return;
}

void CreateOutput(const char * const detailFilename, const char *const extension,
                  ofstream &txt, ofstream &csv)
{
  if(detailFilename == 0)
  {
    output << "Failed to create report file." << endl;
    throw 1;
  }

  char reportFilename[256];
  stringstream ssOutFilename;
  ssOutFilename << detailFilename << "." << extension << '\0';
  strcpy(reportFilename, ssOutFilename.str().c_str());

  char reportFileTxt[256];
  char reportFileCsv[256];
  reportFileTxt[0] = '\0';
  reportFileCsv[0] = '\0';
  strcpy(reportFileTxt, reportFilename);
  strcpy(reportFileCsv, reportFilename);
  strcat(reportFileTxt, ".txt");
  strcat(reportFileCsv, ".csv");

  try
  {
    txt.open(reportFileTxt);
    csv.open(reportFileCsv);
    if(!txt.good() || !csv.good()) { throw 0; }
  }
  catch(...)
  {
    output << "Failed to create report file." << endl;
    if(txt.good()) { txt.close(); }
    if(csv.good()) { csv.close(); }
    throw 2;
  }

  return;
}

void LoadOrganisms(vector<iOrganism*> &organisms,
                   const char * const historicFilename,
                   const char * const detailFilename)
{
  // open input files
  ifstream detailIn(detailFilename);
  ifstream historicIn(historicFilename);
  if(!detailIn || !historicIn)
  {
    if(!historicIn)   { output << "Could not open historic file." << endl; }
    else              { historicIn.close(); }

    if(!detailIn) { output << "Could not open detail file."   << endl; }
    else          { detailIn.close(); }

    throw 1;
  }
  output << "Historic and detail files opened." << endl << endl;

  // Load in organisms from file.
  output << "Loading input files- " << endl;

  try
  {
    // load files
    output << "Loading historic file        ... ";
    LoadAvidaOrganisms(organisms, historicIn, false);
    output << "Loaded." << endl;

    output << "Loading detail file          ... ";
    LoadAvidaOrganisms(organisms, detailIn, true);
    output << "Loaded." << endl;
  }
  catch(pair<int,int> errorData)
  {
    // output the error that occurred while processing a line
    output << "Failed." << endl;
    output << errorData.first << "\t error - " << errorData.second;
    output << endl;
    output << "Abandoning build." << endl;
    Cleanup(0, organisms);

    // Close input files
    historicIn.close();
    detailIn.close();

    throw 2;
  }

  // Close input files
  historicIn.close();
  detailIn.close();

  // Check for duplicate organism ids
  output << "Checking for duplicate ids   ... ";
  try
  {
    set<int> duplicateIds = CheckForDuplicateIds(organisms);
    if(!duplicateIds.empty()) { throw 0; }
  }
  catch(int)
  {
    output << "found.  Aborting ..." << endl;
    throw 3;
  }
  output << "none." << endl;
  output << endl;

  return;
}

void PrepareTree(Tree &fullTree)
{
  // Validate all nodes, no missing nodes in the list and valid ids/parentId
  //output << "Validating tree              ... ";
  //if(Validate(fullTree) == false)
  //{
  //  output << "failed." << endl;
  //  return false;
  //}
  //output << "Complete." << endl;

  // Checking for cycles
  //output << "Checking for cycles          ... ";
  //if(CheckForCycles(fullTree) == true)
  //{
  //  output << "found." << endl;
  //  return false;
  //}
  //output << "Complete." << endl;

  // Prune leaves from the tree that are not in the final population, if any
  //output << "Prune non-living leaves      ... ";
  //if(!PruneDeadLeaves(fullTree))
  //{
  //  output << "Failed." << endl;
  //  throw 1;
  //}
  //output << "Complete." << endl;

  // Remove non-furcating nodes except the root and leaves
  output << "Removing non-furcating nodes ... ";
  RemoveNonfurcatingNodes(fullTree);
  output << "Complete." << endl;

  // Output tree information
  output << endl;
  if(output.GetShowState() == true && output.GetStream() != 0 &&
     PrintInformation(fullTree, *output.GetStream()) == false)
  {
    output << "Failed to retrieve tree information." << endl;
    throw 2;
  }

  return;
}

const double RunSamples(const Tree &fullTree,
                        unsigned int inputSamples,
                        unsigned int inputLeavesToSample,
                        const double trueValue,
                        const double timeCutoff,
                        const int method,
                        const bool generateReport,
                        const bool generateNewick,
                        ostream &reportTxt,
                        ostream &reportCsv,
                        ostream &listTxt,
                        ostream &listCsv,
                        const char * const newickFilenameBase,
                        const char * const outFilename)
{
  if(inputSamples == 0 || inputLeavesToSample == 0) { return 0; }

  double total = 0;
  unsigned int samples = inputSamples;
  unsigned int sampleSize = inputLeavesToSample;
  for(unsigned int i = 0; i < samples; ++i)
  {
    Tree *sampleTree = 0;
    try
    {
      output << "Working on sample " << i+1 << endl;

      sampleTree = Sample(fullTree, sampleSize, rng);
      if(sampleTree == 0)
      {
        output << "Failed to create a sample from the full tree." << endl;
        throw 1;
      }

      try { PrepareTree(*sampleTree); }
      catch(int)
      {
        output << "Failed to prepare sample tree." << endl;
        throw 2;
      }

      char *name = 0;
      char filename[256];
      if(outFilename)
      {
        stringstream ssOutFilename;

        ssOutFilename << outFilename;
        if(method == 1)      ssOutFilename << ".gamma";
        else if(method == 2) ssOutFilename << ".ncstem";
        else if(method == 3) ssOutFilename << ".balance";
        ssOutFilename << ".sample" << i+1 << '\0';
        strcpy(filename, ssOutFilename.str().c_str());
        name = filename;
      }

      if(generateNewick)
      {
        char newickFilename[256];
        stringstream ssOutFilename2;
        ssOutFilename2 << newickFilenameBase;
        if(method == 1)      ssOutFilename2 << ".gamma";
        else if(method == 2) ssOutFilename2 << ".ncstem";
        else if(method == 3) ssOutFilename2 << ".balance";
        ssOutFilename2 << ".sample" << i+1 << '\0';
        strcpy(newickFilename, ssOutFilename2.str().c_str());

        NewickOutput(*sampleTree, timeCutoff, newickFilename, output);
      }

      double value = 0;
      if(method == 1)
        value = CalculateGamma(*sampleTree, name, timeCutoff);
      else if(method == 2)
        value = CalculateNCStem(*sampleTree, name, timeCutoff);
      else if(method == 3)
        value = CalculateBalance(*sampleTree, generateReport,
                                 reportTxt, reportCsv,
                                 listTxt, listCsv);
      else throw 3;

      if(generateReport && (method == 1 || method == 2))
      {
        reportTxt << "0 ";
        reportTxt.width(8);
        reportTxt << left << i+1;
        reportTxt << " " << value;
        reportTxt << endl;
        reportCsv << "Sample," << i+1 << "," << value << endl;
      }
      total += value;
    }
    catch(int)
    {
      output << "Failed to sample the full tree." << endl;
    }
    if(sampleTree != 0) { delete sampleTree; }
  }

  output << "True ";

  if(method == 1) output << "Gamma";
  else if(method == 2) output << "NCStem";
  else if(method == 3) output << "Balance";

  output << " = " << trueValue << endl;
  output << "Average ";

  if(method == 1) output << "Gamma";
  else if(method == 2) output << "NCStem";
  else if(method == 3) output << "Balance";

  output << " = " << total;
  output << " / " << samples << " = ";
  double average = total / samples;
  output << average << endl;

  return average;
}

