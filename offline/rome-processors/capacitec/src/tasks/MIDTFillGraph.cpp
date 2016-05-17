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

int   evID = 0.;
gm2::capacitec_t ctec;

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
   TString treeFile = "data-out/capacitec_tree_";
   treeFile += Form("%05i", gAnalyzer->GetODB()->GetRunNumber());
   treeFile += ".root";

   fTreeFile = TFile::Open(treeFile.Data(), "RECREATE");   
   fTreeFile->cd();

   fEventTree = new TTree("t_ctec","Capacitec Sensor Data");
   fEventTree->Branch("ctec", &ctec.midas_time, gm2::capacitec_str);
}

//______________________________________________________________________________
void MIDTFillGraph::Event()
{
  if (gAnalyzer->GetMidasDAQ()->GetCTECBankEntries() > 0) {

    // Tree stuff.
    ctec.midas_time = gAnalyzer->GetActiveDAQ()->GetTimeStamp();
    ctec.outer_lo = gAnalyzer->GetMidasDAQ()->GetCTECBankAt(0);
    ctec.inner_up = gAnalyzer->GetMidasDAQ()->GetCTECBankAt(1);
    ctec.inner_lo = gAnalyzer->GetMidasDAQ()->GetCTECBankAt(2);
    ctec.outer_up = gAnalyzer->GetMidasDAQ()->GetCTECBankAt(3);

    // Sometimes capacitecs get a constant offset and need to be reset.
    if (ctec.inner_lo < 0.0) {
      ctec.inner_lo += 10.0;
    }

    if (ctec.inner_up < 0.0) {
      ctec.inner_up += 10.0;
    }

    if (ctec.outer_lo < 0.0) {
      ctec.outer_lo += 10.0;
    }

    if (ctec.outer_up < 0.0) {
      ctec.outer_up += 10.0;
    }

    fEventTree->Fill();
  }
}

//______________________________________________________________________________
void MIDTFillGraph::EndOfRun()
{
  fTreeFile->Write();
  fTreeFile->Close();
}

//______________________________________________________________________________
void MIDTFillGraph::Terminate()
{
}
