#include <iostream>
#include <iomanip>
#include <TRandom.h>
#include <TRandom3.h>
#include <TROOT.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TFile.h>

using namespace std;

int main()
{
	double random;
	TFile *rFile = new TFile("./toydata/toylandau.root" ,"recreate");
	rFile->cd();
	TRandom *lndu = new TRandom3();
	lndu->SetSeed(0);
	TH1F *toydata = new TH1F("landau_toydata", "landau_toydata; Charge; #Entries", 100, 26000, 60000);
	//TH1F *testhist2 = new TH1F("test", "test; x; y", 100, 26000, 60000);
	//TH1F *testhist3 = new TH1F("test", "test; x; y", 100, 26000, 60000);
	//THStack *stacktest = new THStack("stacktest", "test stacked histograms");
	for (int i =0; i < 100000; ++i)
	{
		random = lndu->Landau(30000 ,700);
		toydata->Fill(random);
		if (i%10 == 0) 
		{
			random = lndu->Landau(35000 ,700);
			toydata->Fill(random);
		}
		//if (i%100 == 0) 
		//{
			//random = lndu->Landau(40000 ,700);
			//toydata->Fill(random);
		//}
		//cout << random << endl;
		
	}
	rFile->Write();
	rFile->Close();
	
	return 0;
}
