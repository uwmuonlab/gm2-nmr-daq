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

TFile *fTreeFile = NULL;
TTree *fEventTree = NULL;
gm2::laser_t laser;
int run_number;

ClassImp(MIDTFillGraph)

//______________________________________________________________________________
void MIDTFillGraph::Init()
{
}

//______________________________________________________________________________
void MIDTFillGraph::BeginOfRun()
{
  // Get the run number.  This module become obsolete after run number 787.
  run_number = gAnalyzer->GetODB()->GetRunNumber();

  if (run_number < 787) {
    TString treeFile = "data-out/laser_tree_";
    treeFile += Form("%05i", run_number);
    treeFile += ".root";

    fTreeFile = TFile::Open(treeFile.Data(), "RECREATE");
    fTreeFile->cd();
    fEventTree = new TTree("t_ltrk","Laser Tracker Data");

    // Set branches.
    fEventTree->Branch("laser", &laser.midas_time, gm2::laser_str);
  }
}

//______________________________________________________________________________
void MIDTFillGraph::Event()
{
  if (IsMyGraphActive() && (run_number < 787)) {

    laser.midas_time = gAnalyzer->GetActiveDAQ()->GetTimeStamp();

    int N = gAnalyzer->GetMidasDAQ()->GetLTRKBankEntries();

    if (N > 0) {

      if (gAnalyzer->GetODB()->GetRunNumber() < 662) {
        laser.r_1 = gAnalyzer->GetMidasDAQ()->GetLTRKBankAt(0);
        laser.z_1 = gAnalyzer->GetMidasDAQ()->GetLTRKBankAt(2);
        laser.phi_1 = gAnalyzer->GetMidasDAQ()->GetLTRKBankAt(1);

      } else {
        laser.r_1 = gAnalyzer->GetMidasDAQ()->GetLTRKBankAt(0);
        laser.z_1 = gAnalyzer->GetMidasDAQ()->GetLTRKBankAt(1);
        laser.phi_1 = gAnalyzer->GetMidasDAQ()->GetLTRKBankAt(2);
      }

      laser.r_2 = 0.0;
      laser.z_2 = 0.0;
      laser.phi_2 = 0.0;

      fEventTree->Fill();
    }
  }
}

//______________________________________________________________________________
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

