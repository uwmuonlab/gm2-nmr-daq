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

// uncomment if you want to include headers of all folders
//#include "MIDAllFolders.h"

int   evID = 0.;
int   tStamp = 0;
float laser[6];

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
   TString treeFile = "data-out/laser_tree_run";
   treeFile += Form("%05i",gAnalyzer->GetODB()->GetRunNumber());
   treeFile += ".root";

   fTreeFile = TFile::Open(treeFile.Data(), "RECREATE");   
   fTreeFile->cd();
   fEventTree = new TTree("g2laser","SLOW CONTROL DATA");
   fEventTree->SetAutoSave(300000000); // autosave when 300 Mbyte written.
   fEventTree->SetMaxVirtualSize(300000000); // 300 Mbyte

   // Set branches.
   fEventTree->Branch("Timestamp",&tStamp,"Timestamp/I");
   fEventTree->Branch("p1_rad",&laser[0],"Laser0/F");
   fEventTree->Branch("p1_phi",&laser[1],"Laser1/F");
   fEventTree->Branch("p1_height",&laser[2],"Laser2/F");
   fEventTree->Branch("p2_rad",&laser[3],"Laser3/F");
   fEventTree->Branch("p2_phi",&laser[4],"Laser4/F");
   fEventTree->Branch("p2_height",&laser[5],"Laser5/F");
}

//_____________________________________________________________________________
void MIDTFillGraph::Event()
{
  if (IsMyGraphActive()) {

    // Tree stuff.
    tStamp = gAnalyzer->GetActiveDAQ()->GetTimeStamp();

    int N = gAnalyzer->GetMidasDAQ()->GetLTRKBankEntries();
    bool laser_in_inches = false;
    
    for (int i = 0; i < N; ++i) {
      laser[i] = gAnalyzer->GetMidasDAQ()->GetLTRKBankAt(i);

      // Check if the laser is reporting in inches.
      if (i % 3 == 0) {
        if ((laser[i] >= 272.0) && (laser[i] <= 280.0)) {
          laser_in_inches = true;
        }
      }

      // Apply corrections for angle range and inches->meters.
      if (i % 3 == 1) {

        laser[i] =  fmod(laser[i], 360.0);

      } else if (laser_in_inches) {

        laser[i] *= 0.025380710659898477;
      }
    }

    if (gAnalyzer->GetMidasDAQ()->GetLTRKBankEntries() > 0) {
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
