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
static TBranch *fEventBranch = NULL;

ClassImp(MIDTFillGraph)

//______________________________________________________________________________
void MIDTFillGraph::Init()
{
}

//______________________________________________________________________________
void MIDTFillGraph::BeginOfRun()
{
   TString treeFile = "data-out/laser_tree_run";
   treeFile += Form("%05i",gAnalyzer->GetODB()->GetRunNumber(),"");
   treeFile += ".root";

   fTreeFile = TFile::Open(treeFile.Data(), "RECREATE");   
   fTreeFile->cd();
   fEventTree = new TTree("g2laser","SLOW CONTROL DATA");
   fEventTree->SetAutoSave(300000000); // autosave when 300 Mbyte written.
   fEventTree->SetMaxVirtualSize(300000000); // 300 Mbyte

   //set branches.  should generalize to use var like nTempChan, nCapacChan, etc.
   fEventTree->Branch("Timestamp",&tStamp,"Timestamp/I");
   fEventTree->Branch("p1_rad",&laser[0],"Laser0/F");
   fEventTree->Branch("p1_phi",&laser[1],"Laser1/F");
   fEventTree->Branch("p1_height",&laser[2],"Laser2/F");
   fEventTree->Branch("p2_rad",&laser[3],"Laser3/F");
   fEventTree->Branch("p2_phi",&laser[4],"Laser4/F");
   fEventTree->Branch("p2_height",&laser[5],"Laser5/F");
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
          //cout<<" time "<<tStamp<<" ltrk entries: "<<gAnalyzer->GetMidasDAQ()->GetLTRKBankEntries()<<endl;     
          //cout<<" NChannels "<<gAnalyzer->GetGSP()->GetNChannels()<<endl;
 
      for (Int_t i = 0; i < gAnalyzer->GetMidasDAQ()->GetLTRKBankEntries(); i++) {
         //cout<<"i is "<<i<<" out of "<<gAnalyzer->GetMidasDAQ()->GetLTRKBankEntries()<<" value "<<gAnalyzer->GetMidasDAQ()->GetLTRKBankAt(i)<<endl;
         GetMyGraphAt(i)->SetPoint(GetMyGraphAt(i)->GetN(),gAnalyzer->GetActiveDAQ()->GetTimeStamp(),gAnalyzer->GetMidasDAQ()->GetLTRKBankAt(i));

         if (i < gAnalyzer->GetGSP()->GetNChannels()) {//loop is redundant?
            //GetMyGraphAt(i)->SetPoint(GetMyGraphAt(i)->GetN(),gAnalyzer->GetActiveDAQ()->GetTimeStamp(),gAnalyzer->GetMidasDAQ()->GetLTRKBankAt(i));
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
	        laser[i] = gAnalyzer->GetMidasDAQ()->GetLTRKBankAt(i);
	                 
         }
      }

      if (gAnalyzer->GetMidasDAQ()->GetLTRKBankEntries() > 0) {
        fEventTree->Fill();
      }
   }
}

//______________________________________________________________________________
void MIDTFillGraph::EndOfRun()
{

TGraph *graphs[gAnalyzer->GetMidasDAQ()->GetLTRKBankEntries()];
int n = 0;
for (int i=0; i<gAnalyzer->GetMidasDAQ()->GetLTRKBankEntries(); i++){
  graphs[i] = new TGraph();
  for (int k=0; k<GetMyGraphAt(i)->GetN(); k++)
    graphs[i]->SetPoint(k,GetMyGraphAt(i)->GetX()[k],GetMyGraphAt(i)->GetY()[k]);
}

   fTreeFile->cd();

for (int i=0; i<gAnalyzer->GetMidasDAQ()->GetLTRKBankEntries(); i++){
   if (i==0) graphs[i]->Write("p1_r");
   if (i==1) graphs[i]->Write("p1_y");
   if (i==2) graphs[i]->Write("p1_azim");
   if (i==3) graphs[i]->Write("p2_r");
   if (i==4) graphs[i]->Write("p2_y");
   if (i==5) graphs[i]->Write("p2_azim");
}
fTreeFile->Write();
fTreeFile->Close();

}

//______________________________________________________________________________
void MIDTFillGraph::Terminate()
{
}
