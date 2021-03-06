////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// MIDTFillGraph                                                              //
//                                                                            //
// Begin_Html <!--
/*-->

<!--*/
// --> End_Html
//                                                                            //
//                                                                            //
// Please note: The following information is only correct after executing     //
// the ROMEBuilder.                                                           //
//                                                                            //
// This task accesses the following folders :                                 //
//     ODB                                                                    //
//                                                                            //
// This task contains the following graphs :                                  //
//    MyGraph                                                                 //
//                                                                            //
// The histograms/graph are created and saved automaticaly by the task.       //
//                                                                            //
// The following method can be used to get a handle of the histogram/graph :  //
//                                                                            //
// Get<Histogram/Graph Name>()                                                //
//                                                                            //
// For histogram/graph arrays use :                                           //
//                                                                            //
// Get<Histogram/Graph Name>At(Int_t index)                                   //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* Generated header file containing necessary includes                        */
#include "generated/MIDTFillGraphGeneratedIncludes.h"

////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////

#include "generated/MIDAnalyzer.h"
#include "tasks/MIDTFillGraph.h"
#include "ROMEiostream.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TFile.h"
#include "TPad.h"
#include "TGraph.h"
#include <TTree.h>
#include "ROMEODBOfflineDataBase.h"
#include "MIDODB.h"
#include <TBranch.h>
#include "shim_structs.hh"

// uncomment if you want to include headers of all folders
//#include "MIDAllFolders.h"

int evID = 0.;
int run_number;
gm2::laser_t laser;

static TFile *fTreeFile = NULL;
static TTree *fEventTree = NULL;

ClassImp(MIDTFillGraph)

//______________________________________________________________________________
void MIDTFillGraph::Init()
{
}

//______________________________________________________________________________
void MIDTFillGraph::BeginOfRun()
{
  run_number = gAnalyzer->GetODB()->GetRunNumber();

  if (run_number >= 787) {
    TString treeFile = "data-out/laser_tree_";
    treeFile += Form("%05i", run_number);
    treeFile += ".root";

    fTreeFile = TFile::Open(treeFile.Data(), "RECREATE");
    fTreeFile->cd();

    fEventTree = new TTree("t_ltrk","Laser Tracker Data");
    fEventTree->Branch("laser", &laser.midas_time, gm2::laser_str);
  }
}

//_____________________________________________________________________________
void MIDTFillGraph::Event()
{
  if (IsMyGraphActive() && (run_number >= 787)) {

    // Tree stuff.
    laser.midas_time = gAnalyzer->GetActiveDAQ()->GetTimeStamp();

    int N = gAnalyzer->GetMidasDAQ()->GetLTRKBankEntries();
    bool laser_in_inches = false;

    if (gAnalyzer->GetMidasDAQ()->GetLTRKBankEntries() > 0) {

      laser.r_1 = gAnalyzer->GetMidasDAQ()->GetLTRKBankAt(0);
      laser.phi_1 = gAnalyzer->GetMidasDAQ()->GetLTRKBankAt(1);
      laser.z_1 = gAnalyzer->GetMidasDAQ()->GetLTRKBankAt(2);

      laser.r_2 = gAnalyzer->GetMidasDAQ()->GetLTRKBankAt(3);
      laser.phi_2 = gAnalyzer->GetMidasDAQ()->GetLTRKBankAt(4);
      laser.z_2 = gAnalyzer->GetMidasDAQ()->GetLTRKBankAt(5);

      // Make sure the angles are from [0, 360].

      laser.phi_1 = fmod(laser.phi_1, 360.0);
      laser.phi_2 = fmod(laser.phi_2, 360.0);

      // Check to see if the laser tracker is report inches and convert.
      laser_in_inches = false;

      if ((laser.r_1 >= 272.0) && (laser.r_1 <= 280.0)) {

        laser_in_inches = true;

      } else  if ((laser.r_2 >= 272.0) && (laser.r_2 <= 280.0)) {

        laser_in_inches = true;
      }

      // Apply corrections for inches->meters.
      if (laser_in_inches) {

        laser.r_1 *= 0.025380710659898477;
        laser.r_2 *= 0.025380710659898477;
        laser.z_1 *= 0.025380710659898477;
        laser.z_2 *= 0.025380710659898477;
      }

      fEventTree->Fill();
    }
  }
}

//_____________________________________________________________________________
void MIDTFillGraph::EndOfRun()
{
  fTreeFile->cd();
  fTreeFile->Write();
  fTreeFile->Close();
}

//______________________________________________________________________________
void MIDTFillGraph::Terminate()
{
}

