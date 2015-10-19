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
int tilt[3];

static TFile *fTreeFile = NULL;
static TTree *fEventTree = NULL;
static TBranch *fEventBranch = NULL;

ClassImp(MIDTFillGraph)

//______________________________________________________________________________
void MIDTFillGraph::Init()
{
}

//______________________________________________________________________________
void MIDTFillGraph::BeginOfRun()
{
   TString treeFile = "data-out/tilt_tree_run";
   treeFile += Form("%05i",gAnalyzer->GetODB()->GetRunNumber(),"");
   treeFile += ".root";

   fTreeFile = TFile::Open(treeFile.Data(), "RECREATE");   
   fTreeFile->cd();
   fEventTree = new TTree("g2tilt","TILT SENSOR DATA");
   fEventTree->SetAutoSave(300000000); // autosave when 300 Mbyte written.
   fEventTree->SetMaxVirtualSize(300000000); // 300 Mbyte

   //set branches.  should generalize to use var like nTempChan, nCapacChan, etc.
   fEventTree->Branch("Timestamp",&tStamp,"Timestamp/I");
   fEventTree->Branch("Tilt0",&tilt[0],"Tilt0/I");
   fEventTree->Branch("Tilt1",&tilt[1],"Tilt1/I");
   fEventTree->Branch("Tilt2",&tilt[2],"Tilt2/I");
}

//______________________________________________________________________________
void MIDTFillGraph::Event()
{

  gStyle->SetStatH(0.2);
  gStyle->SetStatW(0.6);
  gStyle->SetCanvasColor(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetStatColor(0);
  gStyle->SetHistLineWidth(0);

   if (IsMyGraphActive()) {
	  
	  tStamp = gAnalyzer->GetActiveDAQ()->GetTimeStamp();

          //cout<<" tstamp "<<tStamp<<" number of inpt entries "<<gAnalyzer->GetMidasDAQ()->GetTILTBankEntries()<<" number of tilt entries "<<gAnalyzer->GetMidasDAQ()->GetTILTBankEntries()<<endl;     

      for (Int_t i = 0; i < gAnalyzer->GetMidasDAQ()->GetTILTBankEntries(); i++) {
         GetMyGraphAt(i)->SetPoint(GetMyGraphAt(i)->GetN(),gAnalyzer->GetActiveDAQ()->GetTimeStamp(),gAnalyzer->GetMidasDAQ()->GetTILTBankAt(i));

         if (i < gAnalyzer->GetGSP()->GetNChannels()) {//loop is redundant?
            //GetMyGraphAt(i)->SetPoint(GetMyGraphAt(i)->GetN(),gAnalyzer->GetActiveDAQ()->GetTimeStamp(),gAnalyzer->GetMidasDAQ()->GetTILTBankAt(i));
            GetMyGraphAt(i)->SetMarkerStyle(20);
            GetMyGraphAt(i)->SetMarkerColor(kRed);
            GetMyGraphAt(i)->SetLineColor(kBlue);
            GetMyGraphAt(i)->GetXaxis()->SetTimeDisplay(1);
            GetMyGraphAt(i)->GetXaxis()->SetTimeFormat("#splitline{%H:%M}{%d/%m} %F 1970-01-01 00:00:00");
            GetMyGraphAt(i)->GetXaxis()->SetTitle("");
            GetMyGraphAt(i)->GetYaxis()->SetTitle("ADC counts");
            GetMyGraphAt(i)->SetTitle(Form("Run #%d",gAnalyzer->GetODB()->GetRunNumber()));
            GetMyGraphAt(i)->GetYaxis()->CenterTitle(1);
            GetMyGraphAt(i)->GetXaxis()->SetLabelOffset(0.02);
            GetMyGraphAt(i)->GetXaxis()->SetTitleSize(0.05);
	        
	        //tree stuff
	        tilt[i] = gAnalyzer->GetMidasDAQ()->GetTILTBankAt(i);
	                 
         }
      }
      if (gAnalyzer->GetMidasDAQ()->GetTILTBankEntries()>0) fEventTree->Fill();
   }
}

//______________________________________________________________________________
void MIDTFillGraph::EndOfRun()
{

TGraph *graphs[gAnalyzer->GetMidasDAQ()->GetTILTBankEntries()];
for (int i=0; i<gAnalyzer->GetMidasDAQ()->GetTILTBankEntries(); i++){
  graphs[i] = new TGraph();
  for (int k=0; k<GetMyGraphAt(i)->GetN(); k++)
    graphs[i]->SetPoint(k,GetMyGraphAt(i)->GetX()[k],GetMyGraphAt(i)->GetY()[k]);
}

   fTreeFile->cd();

for (int i=0; i<gAnalyzer->GetMidasDAQ()->GetTILTBankEntries(); i++) graphs[i]->Write(Form("tilt_%d",i+1));

fTreeFile->Write();
fTreeFile->Close();

}

//______________________________________________________________________________
void MIDTFillGraph::Terminate()
{
}
