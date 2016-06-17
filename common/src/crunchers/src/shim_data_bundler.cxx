/********************************************************************\

  file:    shim_data_bundler.cxx
  author:  Matthias W. Smith

  about:   Takes rome root trees and bundles them together.

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
using std::cout;
using std::endl;

//--- other includes -----------------------------------------------//
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "TTree.h"
#include "TFile.h"

//--- project includes ---------------------------------------------//
#include "shim_constants.hh"
#include "shim_structs.hh"

int main(int argc, char *argv[])
{
  int run_number;
  bool laser_swap;
  std::string datadir;
  std::string laser_point;
  std::stringstream ss;

  // Declare the data structures.
  gm2::platform_t platform;
  gm2::laser_t laser;
  gm2::capacitec_t ctec;
  gm2::metrolab_t mlab;
  gm2::mscb_cart_t cart;
  gm2::mscb_ring_t ring;
  gm2::tilt_sensor_t tilt;
  gm2::hall_probe_t hall;
  gm2::data_flags_t flags;

  // Initialize the flags, since they aren't set to false automatically.
  flags.platform_data = false;
  flags.laser_data = false;
  flags.ctec_data = false;
  flags.mlab_data = false;
  flags.hall_data = false;
  flags.tilt_data = false;
  flags.mscb_cart_data = false;
  flags.mscb_ring_data = false;
  flags.laser_p1 = false;
  flags.laser_p2 = false;
  flags.laser_swap = false;
  flags.missing_probe19 = false;

  // Initialize the times to check for bad data.
  for (int i = 0; i < SHIM_PLATFORM_CH; ++i) {
    platform.sys_clock[i] = 0;
  }
  laser.midas_time = 0;
  ctec.midas_time = 0;
  mlab.midas_time = 0;
  cart.midas_time = 0;
  ring.midas_time = 0;
  tilt.midas_time = 0;
  hall.midas_time = 0;

  // And the ROOT variables.
  TFile *pf_platform = nullptr;
  TTree *pt_platform = nullptr;

  TFile *pf_rome_laser = nullptr;
  TTree *pt_rome_laser = nullptr;

  TFile *pf_rome_ctec = nullptr;
  TTree *pt_rome_ctec = nullptr;

  TFile *pf_rome_ring = nullptr;
  TTree *pt_rome_ring = nullptr;

  TFile *pf_rome_cart = nullptr;
  TTree *pt_rome_cart = nullptr;

  TFile *pf_rome_tilt = nullptr;
  TTree *pt_rome_tilt = nullptr;

  TFile *pf_rome_hall = nullptr;
  TTree *pt_rome_hall = nullptr;

  TFile *pf_rome_mlab = nullptr;
  TTree *pt_rome_mlab = nullptr;

  TFile *pf_all = nullptr;
  TTree *pt_sync = nullptr;
  TTree *pt_cart = nullptr;
  TTree *pt_ring = nullptr;
  TTree *pt_tilt = nullptr;
  TTree *pt_hall = nullptr;
  TTree *pt_mlab = nullptr;

  // All the program needs is a run number, and optionally a data dir.
  if (argc < 2) {
    cout << "Insufficent arguments, usage:" << endl;
    cout << "./shim_data_bundler <run-number> [data-dir]" << endl;
    exit(1);
  }
  run_number = std::stoi(argv[1]);

  if (argc > 2) {
    
    datadir = std::string(argv[2]);

    if (datadir[datadir.size() - 1] != '/') {
      datadir.append("/");
    }

  } else {
    
    datadir = std::string("data/");
  }

  string attr_file("/home/newg2/Applications/gm2-nmr/");
  attr_file += string("resources/log/run_attributes.json");

  boost::property_tree::ptree pt;
  boost::property_tree::read_json(attr_file, pt);

  ss.str("");
  ss << std::setfill('0') << std::setw(5) << run_number << ".laser_point";
  laser_point = pt.get<string>(ss.str());

  ss.str("");
  ss << std::setfill('0') << std::setw(5) << run_number << ".laser_swap";
  laser_swap = pt.get<bool>(ss.str());
  
  // Load the rome_laser data.
  ss.str("");
  ss << datadir << "rome/laser_tree_run" << std::setfill('0') << std::setw(5);
  ss << run_number << ".root";
  pf_rome_laser = new TFile(ss.str().c_str(), "read");
  
  if (!pf_rome_laser->IsZombie()) {
    pt_rome_laser = (TTree *)pf_rome_laser->Get("g2laser");
  }

  ss.str("");
  ss << datadir << "rome/cap_tree_run" << std::setfill('0') << std::setw(5);
  ss << run_number << ".root";
  pf_rome_ctec = new TFile(ss.str().c_str(), "read");
  
  if (!pf_rome_ctec->IsZombie()) {
    pt_rome_ctec = (TTree *)pf_rome_ctec->Get("g2cap");
  }

  // And the slow control data.
  ss.str("");
  ss << datadir << "rome/sc_tree_run" << std::setfill('0') << std::setw(5);
  ss << run_number << ".root";
  pf_rome_cart = new TFile(ss.str().c_str(), "read");

  if (!pf_rome_cart->IsZombie()) {
    pt_rome_cart = (TTree *)pf_rome_cart->Get("g2slow");
  }

  // And the slow control data.
  ss.str("");
  ss << datadir << "rome/sc_tree_run" << std::setfill('0') << std::setw(5);
  ss << run_number << ".root";
  pf_rome_ring = new TFile(ss.str().c_str(), "read");

  if (!pf_rome_ring->IsZombie()) {
    pt_rome_ring = (TTree *)pf_rome_ring->Get("g2slow");
  }

  // And the tilt sensor data.
  ss.str("");
  ss << datadir << "rome/tilt_tree_run" << std::setfill('0') << std::setw(5);
  ss << run_number << ".root";
  pf_rome_tilt = new TFile(ss.str().c_str(), "read");

  if (!pf_rome_tilt->IsZombie()) {
    pt_rome_tilt = (TTree *)pf_rome_tilt->Get("g2tilt");
  }

  // And the hall probe platform data.
  ss.str("");
  ss << datadir << "rome/hall_probe_tree_run" << std::setfill('0');
  ss << std::setw(5) << run_number << ".root";
  pf_rome_hall = new TFile(ss.str().c_str(), "read");

  if (!pf_rome_hall->IsZombie()) {
    pt_rome_hall = (TTree *)pf_rome_hall->Get("g2hall_probe");
  }

  // And the metrolab data.
  ss.str("");
  ss << datadir << "rome/metrolab_tree_run" << std::setfill('0');
  ss << std::setw(5) << run_number << ".root";
  pf_rome_mlab = new TFile(ss.str().c_str(), "read");

  if (!pf_rome_tilt->IsZombie()) {
    pt_rome_mlab = (TTree *)pf_rome_mlab->Get("g2metrolab");
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
  pt_cart = new TTree("t_cart", "Asynchronous SCS200 Data");
  pt_ring = new TTree("t_ring", "Asynchronous SCS200 Data");
  pt_tilt = new TTree("t_tilt", "Asynchronous Tilt Sensor Data");
  pt_hall = new TTree("t_hall", "Asynchronous Hall Prob Platform Data");
  pt_mlab = new TTree("t_mlab", "Asynchronous Metrolab Data");

  // Attach the branches to the final output
  pt_sync->Branch("platform", &platform, gm2::platform_str);
  pt_sync->Branch("laser", &laser, gm2::laser_str);
  pt_sync->Branch("ctec", &ctec, gm2::capacitec_str);
  pt_sync->Branch("flags", &flags.platform_data, gm2::data_flags_str);
  pt_cart->Branch("cart", &cart, gm2::mscb_cart_str);
  pt_ring->Branch("ring", &ring, gm2::mscb_ring_str);
  pt_tilt->Branch("tilt", &tilt, gm2::tilt_sensor_str);
  pt_hall->Branch("hall", &hall, gm2::hall_probe_str);
  pt_mlab->Branch("mlab", &mlab, gm2::metrolab_str);

  if (run_number < 1475) {
    flags.missing_probe19 = true;
  }

  // Attach to the data files if they exist.
  if (pt_rome_laser != nullptr) {

    cout << "Found ROME data for laser" << endl;

    flags.laser_data = true;
    
    if (run_number < 787) {
      
      if (laser_point == string("P1")) {

        flags.laser_p1 = true;
    
        pt_rome_laser->SetBranchAddress("Timestamp", &laser.midas_time);
        pt_rome_laser->SetBranchAddress("position_rad", &laser.r_1);
        pt_rome_laser->SetBranchAddress("position_height", &laser.phi_1);
        pt_rome_laser->SetBranchAddress("position_azim", &laser.z_1);
        laser.r_2 = 0.0;
        laser.z_2 = 0.0;
        laser.phi_2 = 0.0;

      } else if (laser_point == string("P2")) {

        flags.laser_p2 = true;

        pt_rome_laser->SetBranchAddress("Timestamp", &laser.midas_time);
        pt_rome_laser->SetBranchAddress("position_rad", &laser.r_2);
        pt_rome_laser->SetBranchAddress("position_height", &laser.phi_2);
        pt_rome_laser->SetBranchAddress("position_azim", &laser.z_2);
        laser.r_1 = 0.0;
        laser.z_1 = 0.0;
        laser.phi_1 = 0.0;

      } else {

        pt_rome_laser->SetBranchAddress("Timestamp", &laser.midas_time);
        pt_rome_laser->SetBranchAddress("position_rad", &laser.r_1);
        pt_rome_laser->SetBranchAddress("position_height", &laser.phi_1);
        pt_rome_laser->SetBranchAddress("position_azim", &laser.z_1);
        laser.r_2 = 0.0;
        laser.z_2 = 0.0;
        laser.phi_2 = 0.0;
      }

    } else {

      if (laser_point == string("P1")) {
        flags.laser_p1 = true;
      }
      
      if (laser_point == string("P2")) {
        flags.laser_p2 = true;
      }
      
      if (laser_swap) {

        flags.laser_swap = true;

        pt_rome_laser->SetBranchAddress("Timestamp", &laser.midas_time);
        pt_rome_laser->SetBranchAddress("p1_rad", &laser.r_2);
        pt_rome_laser->SetBranchAddress("p1_phi", &laser.phi_2);
        pt_rome_laser->SetBranchAddress("p1_height", &laser.z_2);
        pt_rome_laser->SetBranchAddress("p2_rad", &laser.r_1);
        pt_rome_laser->SetBranchAddress("p2_phi", &laser.phi_1);
        pt_rome_laser->SetBranchAddress("p2_height", &laser.z_1);
   
      } else {

        pt_rome_laser->SetBranchAddress("Timestamp", &laser.midas_time);
        pt_rome_laser->SetBranchAddress("p1_rad", &laser.r_1);
        pt_rome_laser->SetBranchAddress("p1_phi", &laser.phi_1);
        pt_rome_laser->SetBranchAddress("p1_height", &laser.z_1);
        pt_rome_laser->SetBranchAddress("p2_rad", &laser.r_2);
        pt_rome_laser->SetBranchAddress("p2_phi", &laser.phi_2);
        pt_rome_laser->SetBranchAddress("p2_height", &laser.z_2);
   
      }
    } 
  }

  if (pt_rome_ctec != nullptr) {

    flags.ctec_data = true;

    pt_rome_ctec->SetBranchAddress("Timestamp", &ctec.midas_time);
    pt_rome_ctec->SetBranchAddress("cap_0", &ctec.outer_lo);
    pt_rome_ctec->SetBranchAddress("cap_1", &ctec.inner_up);
    pt_rome_ctec->SetBranchAddress("cap_2", &ctec.inner_lo);
    pt_rome_ctec->SetBranchAddress("cap_3", &ctec.outer_up);
  }

  if (pt_rome_mlab != nullptr) {

    cout << "Found ROME data for metrolab" << endl;

    flags.mlab_data = true;
    pt_rome_mlab->SetBranchAddress("Timestamp", &mlab.midas_time);
    pt_rome_mlab->SetBranchAddress("Mtr1", &mlab.field);
    pt_rome_mlab->SetBranchAddress("Mtr0", &mlab.state);
    pt_rome_mlab->SetBranchAddress("Mtr2", &mlab.units);
  }

  if (pt_platform != nullptr) {

    cout << "Found ROOT data for shim platform" << endl;

    flags.platform_data = true;
    pt_platform->SetBranchAddress("shim_platform", &platform.sys_clock[0]);
  }

  if (pt_rome_cart != nullptr) {

    cout << "Found ROME data for Cartronment" << endl;

    flags.mscb_cart_data = true;
    pt_rome_cart->SetBranchAddress("Timestamp", &cart.midas_time);
    pt_rome_cart->SetBranchAddress("Temp0", &cart.temp[0]);
    pt_rome_cart->SetBranchAddress("Temp1", &cart.temp[1]);
    pt_rome_cart->SetBranchAddress("Temp2", &cart.temp[2]);
    pt_rome_cart->SetBranchAddress("Temp3", &cart.temp[3]);
    pt_rome_cart->SetBranchAddress("Temp4", &cart.temp[4]);
    pt_rome_cart->SetBranchAddress("Temp5", &cart.temp[5]);
    pt_rome_cart->SetBranchAddress("Temp6", &cart.temp[6]);
    pt_rome_cart->SetBranchAddress("Temp7", &cart.temp[7]);
    pt_rome_cart->SetBranchAddress("Capac0", &cart.ctec[0]);
    pt_rome_cart->SetBranchAddress("Capac1", &cart.ctec[1]);
    pt_rome_cart->SetBranchAddress("Capac2", &cart.ctec[2]);
    pt_rome_cart->SetBranchAddress("Capac3", &cart.ctec[3]);
  }

  
  if (pt_rome_tilt != nullptr) {

    cout << "Found ROME data for Tilt Sensor" << endl;

    flags.tilt_data = true;
    pt_rome_tilt->SetBranchAddress("Timestamp", &tilt.midas_time);
    pt_rome_tilt->SetBranchAddress("Tilt0", &tilt.temp);
    pt_rome_tilt->SetBranchAddress("Tilt1", &tilt.phi);
    pt_rome_tilt->SetBranchAddress("Tilt2", &tilt.rad);
  }

  if (pt_rome_hall != nullptr) {

    cout << "Found ROME data for Hall Probe Platform" << endl;

    flags.hall_data = true;
    pt_rome_hall->SetBranchAddress("Timestamp", &hall.midas_time);
    pt_rome_hall->SetBranchAddress("HallPlatform0", &hall.temp);
    pt_rome_hall->SetBranchAddress("HallPlatform1", &hall.volt);
  }

  int num_sync_events = 0;

  if (pt_platform != nullptr) {

    int nplatform = pt_platform->GetEntries();

    if (nplatform == 0) {

      cout << "Platform TTree is empty." << endl;

    } else {

      num_sync_events = pt_platform->GetEntries();
      cout << "num_sync_events = " << num_sync_events << endl;
    }
  }

  if (pt_rome_laser != nullptr) {
    int nlaser = pt_rome_laser->GetEntries();

    if (nlaser == 0) {
      cout << "Laser TTree is empty." << endl;

    } else if ((num_sync_events == 0) || (num_sync_events > nlaser)) {
      
      num_sync_events = pt_rome_laser->GetEntries();
      cout << "num_sync_events = " << num_sync_events << endl;
    }
  }

  if (pt_rome_ctec != nullptr) {
    int nctec = pt_rome_ctec->GetEntries();

    if (nctec == 0) {
      cout << "Capacitec TTree is empty." << endl;

    } else if ((num_sync_events == 0) || (num_sync_events > nctec)) {

      num_sync_events = pt_rome_ctec->GetEntries();
      cout << "num_sync_events = " << num_sync_events << endl;
    }
  }

  for (int i = 0; i < num_sync_events; ++i) {
    
    if (pt_platform != nullptr) {
      pt_platform->GetEntry(i);

      // Check if the first event is empty as it sometimes is.
      if (i == 0) {

        bool is_empty = true;

        for (int j = 0; j < SHIM_PLATFORM_CH; ++j) {
          for (int n = 0; n < SHORT_FID_LN; ++n) {
            is_empty &= (platform.trace[j][n] == 0);

            if (!is_empty) break;
          }

          if (!is_empty) break;
        }

        if (!is_empty) continue;
      }
    }

    if (pt_rome_laser != nullptr) {
      pt_rome_laser->GetEntry(i);

      if (laser_point == string("P1")) {
        laser.phi_2 = laser.phi_1 + gm2::laser_phi_offset_p2_to_p1;

        if (laser.phi_2 >= 360.0) {
          laser.phi_2 -= 360.0;
        }

      } else if (laser_point == string("P2")) {
        laser.phi_1 = laser.phi_2 - gm2::laser_phi_offset_p2_to_p1;

        if (laser.phi_1 <= 0.0) {
          laser.phi_1 += 360.0;
        }
      }
    }

    if (pt_rome_ctec != nullptr) {
      pt_rome_ctec->GetEntry(i);
    }

    pt_sync->Fill();
  }
 
  cout << "Finished bundling sync TTree." << endl;

  if (pt_rome_cart != nullptr) {
    for (int i = 0; i < pt_rome_cart->GetEntries(); ++i) {
      pt_rome_cart->GetEntry(i);
      pt_cart->Fill();
    }
  }

  if (pt_rome_ring != nullptr) {
    for (int i = 0; i < pt_rome_ring->GetEntries(); ++i) {
      pt_rome_ring->GetEntry(i);
      pt_ring->Fill();
    }
  }

  if (pt_rome_tilt != nullptr) {

    for (int i = 0; i < pt_rome_tilt->GetEntries(); ++i) {
      pt_rome_tilt->GetEntry(i);
      pt_tilt->Fill();
    }

    cout << "Finished bundling tilt TTree." << endl;
  }

  if (pt_rome_hall != nullptr) {

    for (int i = 0; i < pt_rome_hall->GetEntries(); ++i) {
      pt_rome_hall->GetEntry(i);
      pt_hall->Fill();
    }

    cout << "Finished bundling hall TTree." << endl;
  }


  if (pt_rome_mlab != nullptr) {
    for (int i = 0; i < pt_rome_mlab->GetEntries(); ++i) {
      pt_rome_mlab->GetEntry(i);
      
      if (mlab.state == 'L') {
        
        mlab.locked = true;
        
      } else {
        
        mlab.locked = false;
      }
      
      if (mlab.units == 'T') {
        
        mlab.is_tesla = true;
        
      } else {
        
        mlab.is_tesla = false;
      }
      
      pt_mlab->Fill();
    }
    cout << "Finished bundling mlab TTree." << endl;
  }

  pf_all->Write();
  pf_all->Close();

  pf_rome_laser->Close(); 
  pf_rome_ctec->Close();
  pf_rome_cart->Close();
  pf_rome_ring->Close();
  pf_rome_tilt->Close();
  pf_rome_hall->Close();
  pf_platform->Close();

  return 0;
}