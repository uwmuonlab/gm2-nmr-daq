///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// MIDTFillGraph                                                             //
//                                                                           //
// Begin_Html <!--
/*-->

<!--*/
// --> End_Html
//                                                                           //
//                                                                           //
// Please note: The following information is only correct after executing    //
// the ROMEBuilder.                                                          //
//                                                                           //
// This task accesses the following folders :                                //
//     ODB                                                                   //
//                                                                           //
// This task contains the following graphs :                                 //
//    MyGraph                                                                //
//                                                                           //
// The histograms/graph are created and saved automaticaly by the task.      //
//                                                                           //
// The following method can be used to get a handle of the histogram/graph : //
//                                                                           //
// Get<Histogram/Graph Name>()                                               //
//                                                                           //
// For histogram/graph arrays use :                                          //
//                                                                           //
// Get<Histogram/Graph Name>At(Int_t index)                                  //
//                                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/* Generated header file containing necessary includes                       */
#include "generated/MIDTFillGraphGeneratedIncludes.h"

///////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the       *
 * following line will be lost next time ROMEBuilder is executed.            */
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

int   evID = 0.;
gm2::tilt_sensor_t tilt;

static TFile *fTreeFile = NULL;
static TTree *fEventTree = NULL;

ClassImp(MIDTFillGraph)

//_____________________________________________________________________________
void MIDTFillGraph::Init()
{
}

//_____________________________________________________________________________
void MIDTFillGraph::BeginOfRun()
{
   TString treeFile = "data-out/tilt_sensor_tree_";
   treeFile += Form("%05i",gAnalyzer->GetODB()->GetRunNumber());
   treeFile += ".root";

   fTreeFile = TFile::Open(treeFile.Data(), "RECREATE");   
   fTreeFile->cd();
   fEventTree = new TTree("t_tilt","Tilt Sensor Data");
   fEventTree->SetAutoSave(300000000); // autosave when 300 Mbyte written.
   fEventTree->SetMaxVirtualSize(300000000); // 300 Mbyte

   // Set branches.
   fEventTree->Branch("tilt", &tilt.midas_time, gm2::tilt_sensor_str);
}


//_____________________________________________________________________________
void MIDTFillGraph::Event()
{
  if (IsMyGraphActive()) {
    
    tilt.midas_time = gAnalyzer->GetActiveDAQ()->GetTimeStamp();

    if (gAnalyzer->GetMidasDAQ()->GetTILTBankEntries() > 0) {
      
      tilt.temp = gAnalyzer->GetMidasDAQ()->GetTILTBankAt(0);
      tilt.phi = gAnalyzer->GetMidasDAQ()->GetTILTBankAt(1);
      tilt.rad = gAnalyzer->GetMidasDAQ()->GetTILTBankAt(2);

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

