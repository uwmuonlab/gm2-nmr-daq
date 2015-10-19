/********************************************************************\

  file:    bundle_full_scan.cxx
  author:  Matthias W. Smith

  about:   Combine runs the comprise a full scan around the ring.

\********************************************************************/

//--- std includes -------------------------------------------------//
#include <string>
#include <ctime>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cassert>

//--- other includes -----------------------------------------------//
#include "TFile.h"
#include "TTree.h"

//--- project includes ---------------------------------------------//
#include "shim_dataset.hh"

using namespace std;
using namespace gm2;

int main(int argc, char *argv[])
{
  // The data i/o
  string outfile;
  string datadir;
  stringstream ss;
  vector<ShimDataset> dvec;
  vector<int> run_numbers;

  // Shim structs
  platform_t platform;
  hamar_t laser;
  scs2000_t envi;
  int run_number;

  // ROOT stuff
  TFile *pf;
  TTree *pt_sync;
  TTree *pt_envi;
  TTree *pt_run;

  // Parse the i/o files
  assert(argc > 3);
  outfile = string(argv[1]);
  datadir = string(argv[2]);

  if (datadir[datadir.size() - 1] != '/') {
    datadir += string("/");
  }

  for (int i = 3; i < argc; ++i) {
    ss.str("");
    ss << datadir << "run_" << std::setfill('0') << std::setw(5);
    ss << stoi(argv[i]) << ".root";

    cout << "loading " << ss.str() << endl;
    run_numbers.push_back(stoi(argv[i]));
    dvec.push_back(ShimDataset(ss.str()));
  }

  // Create a new ROOT file.
  pf = new TFile(outfile.c_str(), "recreate");
  pt_sync = new TTree("t_sync", "Synchronous Shim Data");
  pt_envi = new TTree("t_envi", "Asynchronous SCS200 Data");

  // Attach the branches to the new output.
  pt_sync->Branch("laser", &laser, gm2::hamar_str);
  pt_sync->Branch("platform", &platform, gm2::platform_str);
  pt_sync->Branch("run_number", &run_number, "run_number/I");
  pt_envi->Branch("envi", &envi, gm2::scs2000_str);
  pt_envi->Branch("run_number", &run_number, "run_number/I");

  for (int idx = 0; idx < dvec.size(); ++idx) {

    // Loop over the sync variables.
    for (int i = 0; i < dvec[idx].GetSyncEntries(); ++i) {
 
      platform = dvec[idx][i].platform;
      laser = dvec[idx][i].laser;
      run_number = run_numbers[idx];

      pt_sync->Fill();
    }

    // And the slow control.
    for (int i = 0; i < dvec[idx].GetEnviEntries(); ++i) {
 
      platform = dvec[idx][i].platform;
      laser = dvec[idx][i].laser;
      run_number = run_numbers[idx];

      pt_envi->Fill();
    }    
  }

  pt_sync->Write();  
  pt_envi->Write();

  pf->Write();
  pf->Close();

  return 0;
}