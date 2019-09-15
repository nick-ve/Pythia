// ROOT macro to test counts, differential and log histogram facilities of NcAstrolab 
{
 gSystem->Load("ncfspack");

 NcAstrolab lab;

 // Test function to describe a dN/dx spectrum
 TF1 func("func","1.e6*pow(x,-2.)");
 func.SetTitle("func;Energy in GeV;dN/dE");

 // Setting up the input histogram
 Int_t mode=1; // Flag to denote linear(=0)/log10(=1)/Ln(=2) scale for the X-axis
 Int_t nbins=100;
 Double_t xmin=1;
 Double_t xmax=10;

 TH1F hin=lab.GetCountsHistogram(func,nbins,xmin,xmax,mode,"");

 TCanvas cspec("cspec","cspec");
 if (!mode)
 {
  func.SetRange(xmin,xmax);
 }
 else if (mode==1)
 {
  func.SetRange(pow(10.,xmin),pow(10.,xmax));
 }
 else if (mode==2)
 {
  func.SetRange(exp(xmin),exp(xmax));
 }
 func.Draw();

 TCanvas cin("cin","cin");
 hin.Draw();

 TH1F hdiff=lab.GetDifHistogram(&hin,mode,"dN/dE");
 TCanvas cdiff("cdiff","cdiff");
 hdiff.Draw();

 TH1F hlog=lab.GetLogHistogram(&hdiff,1);
 TCanvas clog("clog","clog");
 hlog.Draw();

 TF1 f("f","pow(x,2.)");
 TH1F hdiff2=lab.GetDifHistogram(&hin,mode,"E^{2}dN/dE",&f);
 TCanvas cdiff2("cdiff2","cdiff2");
 hdiff2.Draw();

 TH1F hlog2=lab.GetLogHistogram(&hdiff2,1);
 TCanvas clog2("clog2","clog2");
 hlog2.Draw();
}
