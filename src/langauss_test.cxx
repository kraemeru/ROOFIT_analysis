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
#include <RooWorkspace.h>
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
			int found2 = keyname.find("_s631");
			int found3 = keyname.find("_b0_k26");
			int found4 = keyname.find("time");
			
			//cout << found1 << endl;
			//cout << found2 << endl;
			if (( found1 != -1 ) && ( found2 != -1 ) && ( found3 != -1 ) && ( found4 == -1 )) 
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
	
	TFile *data_file = TFile::Open(argv[1]);
	TFile *out_file = new TFile("/afs/desy.de/user/k/kraemeru/roofit/langauss_test2_noFC.root", "RECREATE");
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
		
		RooWorkspace* w = new RooWorkspace("w");
		RooRealVar x("ADC", "ADC", hist_mean - 5*hist_devi, hist_mean + 5*hist_devi); // mean+-5sigma
		//w->import(x);
		w->factory("Landau::landau(x[1, 2], lmean[hist_mean, hist_mean-hist_devi, hist_mean+hist_devi], lsigma[hist_devi/2, 1, hist_devi])");
		//w->factory("Gaussian::gauss(x, lmean[hist_mean, hist_mean-hist_devi, hist_mean+hist_devi], lsigma[hist_devi/2, 0, hist_devi])");
		
		//w->factory("FCONV::lxg(x,landau,gauss)") ;
		
		RooPlot* frame = x.frame(Title(histname));
		RooDataHist datahist("datahist","datahist",x,Import(*data_histogram)) ;
		
		datahist.plotOn(frame);
		
		//RooFFTConvPdf langauss("langauss","landau (X) gauss",w::x,w::landau,w::gauss) ;
		
		////cout << x_min << " " << x_max << " " << hist_mean << " " << hist_devi << endl;
		//
		////Construct Landau
		
		//RooRealVar lmean ("lmean","mean of landau",hist_mean,hist_mean-hist_devi,hist_mean+hist_devi) ; // mean+-sigma
		//RooRealVar lsigma ("lsigma","width of landau",hist_devi/2,0,hist_devi) ; // between 0 and sigma with starting parameter sigma/2
		//RooLandau* landau = new RooLandau("landau","landau PDF",x,lmean,lsigma) ; 
		
		////Construct gauss
		//RooRealVar gmean("gmean","mean of gauss",0) ;
		//RooRealVar gsigma("gsigma","width of gauss",hist_devi/4,0,hist_devi/2) ; // between 0 and sigma/2 with starting parameter sigma/4
		//RooGaussian* gauss = new RooGaussian("gauss","gauss PDF",x,gmean,gsigma) ;
		 
		
		//RooPlot* frame = x.frame(Title(histname));
		//RooFFTConvPdf* langauss = new RooFFTConvPdf(histname, "landau (X) gauss", x, *landau, *gauss);
		
		//RooDataHist datahist("datahist","datahist",x,Import(*data_histogram)) ;
		//datahist.plotOn(frame);
		////datahist.statOn(frame,Layout(0.55,0.99,0.9)) ;
		
		//////RANGE
		////x.setRange("R1", hist_mean-hist_devi/4,hist_mean+hist_devi);
		//langauss->fitTo(datahist, Extended());
		////langauss->chi2FitTo(datahist, Range("R1")) ;
		//langauss->paramOn(frame, Layout(0.7,0.99,0.9)) ;
		////langauss.plotOn(frame, Range("R1"), LineColor(kBlack));
		//langauss->plotOn(frame, Range("Full"), LineColor(kBlack));
		////landau->fitTo(datahist) ;
		////landau->plotOn(frame, Range("Full"), LineStyle(kDashed),LineColor(kRed)) ;
		
		//TCanvas* c = new TCanvas(histname,histname,1600,1200) ;
		//frame->Draw() ;
		
		////c->SaveAs("./langauss_fit_to_beam_channel_tracker_test.png");
		////c->SaveAs("./langauss_fit_to_beam_channel_ECAL.png");
		////datahist.Write();
		//c->Write();
		////out_file->Write();
		cout << "Mean of the Histogram: " << hist_mean << endl;
		cout << "Std.Dev. of the Histogram: " << hist_devi << endl << endl;
		
	}
	
	out_file->Close();
	
  
	return 0;
}
