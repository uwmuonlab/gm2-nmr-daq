////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// MIDTFillGraph_SHPF                                                         //
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
//    MyGraph_SHPF                                                            //
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
#include "generated/MIDTFillGraph_SHPFGeneratedIncludes.h"

////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////

#include "generated/MIDAnalyzer.h"
#include "tasks/MIDTFillGraph_SHPF.h"
#include "ROMEiostream.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TFile.h"
#include "TPad.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TF2.h"
#include <TTree.h>
#include "ROMEODBOfflineDataBase.h"
#include "MIDODB.h"
#include <TBranch.h>
#include "TMath.h"
#include "tasks/common.hh"

const int shim_platform_size = sizeof(daq::shim_platform_st) / sizeof(uint);
static TFile *fTreeFile = NULL;
static TTree *fEventTree = NULL;
static daq::shim_platform_st platform;
static uint buffer[shim_platform_size];

ClassImp(MIDTFillGraph_SHPF)

//______________________________________________________________________________
void MIDTFillGraph_SHPF::Init()
{
}

//______________________________________________________________________________
void MIDTFillGraph_SHPF::BeginOfRun()
{
  TString treeFile = "data-out/platform_run_";
  treeFile += Form("%05i.root", gAnalyzer->GetODB()->GetRunNumber());
  
  fTreeFile = TFile::Open(treeFile.Data(), "RECREATE");   
  fTreeFile->cd();
  fEventTree = new TTree("t_shpf","Shimming Plaform Data");
  fEventTree->Branch("shim_platform", &platform.sys_clock[0], daq::shim_platform_st_string);
}

//______________________________________________________________________________
void MIDTFillGraph_SHPF::Event()
{
  if (IsMyGraph_SHPFActive()) {

    if (gAnalyzer->GetMidasDAQ()->GetSHPFBankEntries() > 0) {

      // Fill the int buffer.;
      for (Int_t i = 0; i < gAnalyzer->GetMidasDAQ()->GetSHPFBankEntries(); i++) {
        buffer[i] = gAnalyzer->GetMidasDAQ()->GetSHPFBankAt(i);
      }
      
      // Then cast it to a shim_platform_st type.
      std::copy(&buffer[0], 
                &buffer[shim_platform_size],
                (int *)(&platform.sys_clock[0]));
      
      // Fill the TTree.
      fEventTree->Fill();
    }
  }
}

//______________________________________________________________________________
void MIDTFillGraph_SHPF::EndOfRun()
{
  fTreeFile->Write();
  fTreeFile->Close();
}

//______________________________________________________________________________
void MIDTFillGraph_SHPF::Terminate()
{
}

