#ifndef __CINT__
#include <RooGlobalFunc.h>
#endif   
#include <iostream>
#include <iomanip>
#include <RooRealVar.h>
#include <RooDataSet.h>
#include <RooDataHist.h>
#include <RooGaussian.h>
#include <RooLandau.h>
#include <RooAddPdf.h>
#include <RooFFTConvPdf.h>
#include <TCanvas.h>
#include <RooPlot.h>
#include <TAxis.h>
#include <TH1F.h>
#include <TROOT.h>
#include <TFile.h>
#include <TStyle.h>
#include <TKey.h>
#include <TMacro.h>
#include <stdio.h>
#include <string.h>
 
using namespace RooFit; 
using namespace std;

vector<TObject*> hist_objects;
//TObject* test_object;

void loopdir(TDirectory* dir)
{
	
	TDirectory *dirsav = gDirectory;
	TIter keys_iter(dir->GetListOfKeys());
	TKey* key;
	
	while ((key = (TKey*)keys_iter()))
	{
		
		if (key->IsFolder())
		{
			dir->cd(key->GetName());
			TDirectory *subdir = gDirectory;
			loopdir(subdir);
			dirsav->cd();
			continue;
		}
		else
		{
			string keyname = key->GetName();
			
			int found1 = keyname.find("hist_");
			//int found2 = keyname.find("_s_");
			int found3 = keyname.find("_b0_k26");
			int found4 = keyname.find("time");
			
			//cout << found1 << endl;
			//cout << found2 << endl;
			if (( found1 != -1 )  && ( found3 != -1 ) && ( found4 == -1 )) 
			{	
				hist_objects.push_back(key->ReadObj());
				cout << keyname << endl;
			}
		}
	}
}


int main(int argc, char **argv)
{
	if (argc < 2) {
		cout << "Please add a root file as argument" << endl;
		return(1);
	}	
	//RooMsgService::instance().setSilentMode(true); //Quiet mode
	TFile *data_file = TFile::Open(argv[1]);
	TFile *out_file = new TFile("/afs/desy.de/user/k/kraemeru/roofit/langauss_test2.root", "RECREATE");
	//gStyle->SetOptFit(1111);
	//string histogram_name = "hist_s631_c0446_b0_k26"; //Tracker
	//string histogram_name = "hist_s890_c0064_b0_k30"; //ECAL
	loopdir(data_file);
	
	for (unsigned int i =0; i<hist_objects.size(); ++i)
	{
		const char* histname = hist_objects.at(i)->GetName();
		
		TH1F* data_histogram = (TH1F*)hist_objects.at(i);
		//double x_min = data_histogram->FindFirstBinAbove(0);
		//double x_max = data_histogram->FindLastBinAbove(0);
		double hist_mean = data_histogram->GetMean();
		double hist_devi = data_histogram->GetStdDev();
		
		if (hist_devi > 0 && data_histogram->GetEntries() > 20)
		{
		
			//cout << x_min << " " << x_max << " " << hist_mean << " " << hist_devi << endl;
			RooRealVar x("ADC", "ADC", hist_mean - 5*hist_devi, hist_mean + 5*hist_devi); // mean+-5sigma
			//Construct Landau
			
			RooRealVar lmean ("lmean","mean of landau",hist_mean,hist_mean-hist_devi,hist_mean+hist_devi) ; // mean+-sigma
			RooRealVar lsigma ("lsigma","width of landau",hist_devi/2,0,hist_devi) ; // between 0 and sigma with starting parameter sigma/2
			RooLandau* landau = new RooLandau("landau","landau PDF",x,lmean,lsigma) ; 
			
			RooRealVar lmean2 ("l2mean","mean of landau",hist_mean,hist_mean-hist_devi,hist_mean+hist_devi) ; // mean+-sigma
			RooRealVar lsigma2 ("l2sigma","width of landau",hist_devi/2,0,hist_devi) ; // between 0 and sigma with starting parameter sigma/2
			RooLandau* landau2 = new RooLandau("landau2","landau2 PDF",x,lmean2,lsigma2) ; 
			
			//Construct gauss
			RooRealVar gmean("gmean","mean of gauss",0) ;
			RooRealVar gsigma("gsigma","width of gauss",hist_devi/4,0,hist_devi/2) ; // between 0 and sigma/2 with starting parameter sigma/4
			RooGaussian* gauss = new RooGaussian("gauss","gauss PDF",x,gmean,gsigma) ;
			
			
			RooPlot* frame = x.frame(Title(histname));
			RooFFTConvPdf* langauss = new RooFFTConvPdf(histname, "landau (X) gauss", x, *landau, *gauss);
			
			RooDataHist* datahist = new RooDataHist("datahist","datahist",x,Import(*data_histogram)) ;
			
			//datahist.statOn(frame,Layout(0.55,0.99,0.9)) ;
			
			////RANGE
			datahist->plotOn(frame);
			x.setRange("RLandau", hist_mean-hist_devi/4,hist_mean+hist_devi);
			x.setRange("RLangauss", hist_mean-hist_devi,hist_mean+hist_devi);
			langauss->fitTo(*datahist, SumW2Error(kTRUE), Range("RLangauss"));// PrintEvalErrors(-1), Verbose(-1));
			langauss->paramOn(frame, Label("fit result"), Format("NEU",AutoPrecision(1)), Layout(0.70,0.99,0.95)) ; 
			langauss->plotOn(frame, Range("Full"), LineColor(kBlue));
			
			landau2->fitTo(*datahist, SumW2Error(kTRUE), Range("RLandau"));//, PrintEvalErrors(-1), Verbose(-1)) ;
			landau2->plotOn(frame, Range("Full"), LineStyle(kDashed),LineColor(kRed)) ;
			landau2->paramOn(frame, Label("fit result"), Format("NEU",AutoPrecision(1)), Layout(0.7,0.99,0.7)) ;
			
			//langauss.plotOn(frame, Range("R1"), LineColor(kBlack));
			
			
			
			TCanvas* c = new TCanvas(histname,histname,1600,1200) ;
			frame->Draw() ;
			
			//c->SaveAs("./langauss_fit_to_beam_channel_tracker_test.png");
			//c->SaveAs("./langauss_fit_to_beam_channel_ECAL.png");
			//datahist.Write();
			c->Write();
			//out_file->Write();
			cout << "Mean of the Histogram: " << hist_mean << endl;
			cout << "Std.Dev. of the Histogram: " << hist_devi << endl << endl;
		}
		
	}
	
	out_file->Close();
	
  
	return 0;
}
