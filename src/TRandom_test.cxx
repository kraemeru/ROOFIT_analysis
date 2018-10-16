#include <iostream>
#include <iomanip>
#include <TRandom.h>
#include <TRandom3.h>
#include <TROOT.h>
#include <TH1F.h>
#include <THStack.h>
#include <TCanvas.h>

using namespace std;

int main()
{
	double random;
	TRandom *lndu = new TRandom3();
	lndu->SetSeed(0);
	TH1F *testhist = new TH1F("test", "test; x; y", 100, 26000, 60000);
	TH1F *testhist2 = new TH1F("test", "test; x; y", 100, 26000, 60000);
	TH1F *testhist3 = new TH1F("test", "test; x; y", 100, 26000, 60000);
	THStack *stacktest = new THStack("stacktest", "test stacked histograms");
	for (int i =0; i < 100000; ++i)
	{
		random = lndu->Landau(30000 ,700);
		testhist->Fill(random);
		if (i%10 == 0) 
		{
			random = lndu->Landau(35000 ,700);
			testhist2->Fill(random);
		}
		if (i%100 == 0) 
		{
			random = lndu->Landau(40000 ,700);
			testhist3->Fill(random);
		}
		//cout << random << endl;
		
	}
	TCanvas* c = new TCanvas("test","test",800,600) ;

	testhist->SetFillColor(kBlue);
	testhist2->SetFillColor(kRed);
	testhist3->SetFillColor(kGreen);
	stacktest->Add(testhist);
	stacktest->Add(testhist2);
	stacktest->Add(testhist3);
	stacktest->Draw();
	
	
	
	c->SaveAs("./testrandom.png");
	
	return 0;
}
