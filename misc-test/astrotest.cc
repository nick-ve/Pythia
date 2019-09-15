// ROOT macro to test the differential c.q. log histogram generation of NcAstrolab
{
 gSystem->Load("ncfspack");

 NcAstrolab lab;

 // Test function to describe a dN/dx spectrum
 TF1 func("func","pow(x,-2.)");

 // Setting up the input histogram
 Double_t fact=1000; // Factor to increase counts per bin 
 Int_t xlog=1; // Flag to denote log10 scale for the X-axis
 Int_t nbins=100;
 Double_t xmin=1;
 Double_t xmax=10;
 if (xlog)
 {
  xmin=log10(xmin);
  xmax=log10(xmax);
 }
 Double_t step=(xmax-xmin)/double(nbins);
 Double_t* xbins=new Double_t[nbins+1];
 Double_t x=xmin;
 for (Int_t ibin=0; ibin<=nbins; ibin++)
 {
  xbins[ibin]=x;
  x=x+step;
 }
 TH1F* hin=0;
 if (!xlog)
 {
  hin=new TH1F("hin","input histo;E;N",nbins,xbins);
 }
 else
 {
  hin=new TH1F("hin","input histo;^{10}Log(E);N",nbins,xbins);
 }

 // Filling the input histogram
 Double_t xval,dx,N;
 x=xmin;
 for (Int_t ibin=1; ibin<=nbins; ibin++)
 {
  if (!xlog)
  {
   xval=x;
   dx=xbins[ibin]-xbins[ibin-1];
   N=func.Eval(x)*dx*fact;
  }
  else
  {
   xval=pow(10.,x);
   dx=pow(10.,xbins[ibin])-pow(10.,xbins[ibin-1]);
   N=func.Eval(xval)*dx*fact;
  }
  hin->Fill(x,N);
  x=x+step;
 }

/**********
 TFile* f=new TFile("agn-nu-tree400.root");
 TTree* data=(TTree*)f->Get("Data");

 TH1F* hin2=new TH1F("hin2","Neutrino data;^{10}log(E);N",100,0,9);
 hin2.Sumw2();

 data->Draw("log10(nu_mu.p)>>hin2","(evt.jrun>0)*1./(evt.Ebeam*evt.Ebeam)");
//// data->Draw("log10(nu_mu.p)>>hin2","(evt.jrun>0)");

 TCanvas cin("cin","cin");
 hin2->Draw();
**********/

 TH1F hdiff=lab.GetDifHistogram(hin,1,"dN/dE");
// TH1F hdiff=lab.GetDifHistogram(hin2,1);
 TCanvas cdiff("cdiff","cdiff");
 hdiff.Draw();

 TH1F hlog=lab.GetLogHistogram(&hdiff,1);
 TCanvas clog("clog","clog");
 hlog.Draw();
}
