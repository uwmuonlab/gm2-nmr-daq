/********************************************************************\

  file:    shim_data_bundler.cxx
  author:  Matthias W. Smith

  about:   Look for SHPF and SHFX midas banks and convert them
           into root trees.

\********************************************************************/

//--- std includes -------------------------------------------------//
#include <string>
#include <ctime>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cassert>
using std::string;

//--- other includes -----------------------------------------------//
#include "TTree.h"
#include "TFile.h"

//--- project includes ---------------------------------------------//
#include "shim_structs.hh"

int main(int argc, char *argv[])
{
  int run_number;
  std::string datadir;

  // Declare the data structures.
  gm2::platform_t platform;
  gm2::hamar_t laser;
  gm2::capacitec_t ctec;
  gm2::scs2000_t envi;

  // Initialize the times to check for bad data.
  for (int i = 0; i < SHIM_PLATFORM_CH; ++i) {
    platform.sys_clock[i] = 0;
  }
  laser.midas_time = 0;
  ctec.midas_time = 0;
  envi.midas_time = 0;

  // And the ROOT variables.
  TFile *pf_platform = nullptr;
  TTree *pt_platform = nullptr;

  TFile *pf_laser = nullptr;
  TTree *pt_laser = nullptr;

  TFile *pf_ctec = nullptr;
  TTree *pt_ctec = nullptr;

  TFile *pf_slow = nullptr;
  TTree *pt_slow = nullptr;

  TFile *pf_all = nullptr;
  TTree *pt_sync = nullptr;
  TTree *pt_envi = nullptr;

  // All the program needs is a run number, and optionally a data dir.
  assert(argc > 0);
  run_number = std::stoi(argv[1]);

  if (argc > 2) {
    
    datadir = std::string(argv[2]);

    if (datadir[datadir.size() - 1] != '/') {
      datadir.append("/");
    }

  } else {
    
    datadir = std::string("data/");
  }

  std::stringstream ss;
  std::ios::fmtflags flags(ss.flags());

  // Load the laser data.
  ss.str("");
  ss << datadir << "rome/laser_tree_run" << std::setfill('0') << std::setw(5);
  ss << run_number << ".root";
  pf_laser = new TFile(ss.str().c_str(), "read");
  
  if (!pf_laser->IsZombie()) {
    pt_laser = (TTree *)pf_laser->Get("g2laser");
  }

  ss.str("");
  ss << datadir << "rome/cap_tree_run" << std::setfill('0') << std::setw(5);
  ss << run_number << ".root";
  pf_ctec = new TFile(ss.str().c_str(), "read");
  
  if (!pf_ctec->IsZombie()) {
    pt_ctec = (TTree *)pf_ctec->Get("g2cap");
  }

  // And the slow control data.
  ss.str("");
  ss << datadir << "rome/sc_tree_run" << std::setfill('0') << std::setw(5);
  ss << run_number << ".root";
  pf_slow = new TFile(ss.str().c_str(), "read");

  if (!pf_slow->IsZombie()) {
    pt_slow = (TTree *)pf_slow->Get("g2slow");
  }

  // And the platform data.
  ss.str("");
  ss << datadir << "root/platform_run_" << std::setfill('0') << std::setw(5);
  ss << run_number << ".root";

  pf_platform = new TFile(ss.str().c_str(), "read");
  if (!pf_platform->IsZombie()) {
    pt_platform = (TTree *)pf_platform->Get("t_shpf");
  }

  // Now get the final combined file into play.
  ss.str("");
  ss << datadir << "shim/run_" << std::setfill('0') << std::setw(5);
  ss << run_number << ".root";
  
  pf_all = new TFile(ss.str().c_str(), "recreate");
  pt_sync = new TTree("t_sync", "Synchronous Shim Data");
  pt_envi = new TTree("t_envi", "Asynchronous SCS200 Data");

  // Attach the branches to the final output
  pt_sync->Branch("platform", &platform, gm2::platform_str);
  pt_sync->Branch("laser", &laser, gm2::hamar_str);
  pt_sync->Branch("ctec", &ctec, gm2::capacitec_str);
  pt_envi->Branch("envi", &envi, gm2::scs2000_str);

  // Attach to the data files if they exist.
  if (pt_laser != nullptr) {

    pt_laser->SetBranchAddress("Timestamp", &laser.midas_time);
    pt_laser->SetBranchAddress("position_rad", &laser.r);
    pt_laser->SetBranchAddress("position_height", &laser.phi);
    pt_laser->SetBranchAddress("position_azim", &laser.z);
  }

  if (pt_ctec != nullptr) {

    pt_ctec->SetBranchAddress("Timestamp", &ctec.midas_time);
    pt_ctec->SetBranchAddress("cap_0", &ctec.outer_lo);
    pt_ctec->SetBranchAddress("cap_1", &ctec.inner_up);
    pt_ctec->SetBranchAddress("cap_2", &ctec.inner_lo);
    pt_ctec->SetBranchAddress("cap_3", &ctec.outer_up);
  }

  if (pt_platform != nullptr) {
    pt_platform->SetBranchAddress("shim_platform", &platform.sys_clock[0]);
  }

  if (pt_slow != nullptr) {
    pt_slow->SetBranchAddress("Timestamp", &envi.midas_time);
    pt_slow->SetBranchAddress("Temp0", &envi.temp[0]);
    pt_slow->SetBranchAddress("Temp1", &envi.temp[1]);
    pt_slow->SetBranchAddress("Temp2", &envi.temp[2]);
    pt_slow->SetBranchAddress("Temp3", &envi.temp[3]);
    pt_slow->SetBranchAddress("Temp4", &envi.temp[4]);
    pt_slow->SetBranchAddress("Temp5", &envi.temp[5]);
    pt_slow->SetBranchAddress("Temp6", &envi.temp[6]);
    pt_slow->SetBranchAddress("Temp7", &envi.temp[7]);
    pt_slow->SetBranchAddress("Capac0", &envi.ctec[0]);
    pt_slow->SetBranchAddress("Capac1", &envi.ctec[1]);
    pt_slow->SetBranchAddress("Capac2", &envi.ctec[2]);
    pt_slow->SetBranchAddress("Capac3", &envi.ctec[3]);
  }
  
  int num_sync_events = 0;
  if (pt_platform != nullptr) {
    num_sync_events = pt_platform->GetEntries();
  }

  if (pt_laser != nullptr) {
    int nlaser = pt_laser->GetEntries();
    if ((num_sync_events > nlaser) && (nlaser != 0)) {
      num_sync_events = pt_laser->GetEntries();
    }
  }

  if (pt_ctec != nullptr) {
    int nctec = pt_ctec->GetEntries();
    if ((num_sync_events > nctec) && (nctec != 0)) {
      num_sync_events = pt_ctec->GetEntries();
    }
  }

  for (int i = 0; i < num_sync_events; ++i) {
    
    if (pt_platform != nullptr) {
      pt_platform->GetEntry(i);
    }

    if (pt_laser != nullptr) {
      pt_laser->GetEntry(i);
    }

    if (pt_ctec != nullptr) {
      pt_ctec->GetEntry(i);
    }

    pt_sync->Fill();
  }

  if (pt_slow != nullptr) {
    for (int i = 0; i < pt_slow->GetEntries(); ++i) {
      pt_slow->GetEntry(i);
      pt_envi->Fill();
    }
  }

  pf_all->Write();
  pf_all->Close();

  pf_laser->Close(); 
  pf_ctec->Close();
  pf_slow->Close();
  pf_platform->Close();

  return 0;
}
