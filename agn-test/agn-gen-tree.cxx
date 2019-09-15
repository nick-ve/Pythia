/////////////////////////////////////////////////////////////////
// Code (ROOT) for a beam dump simulation for dust obscured AGN.
// Both the p+p interactions of the jet with the dust
// and the p+gamma interactions within the jet are simulated.
//
// Beam energies, number of events and specification of the
// produced particle spectra have to be provided by the user
// as indicated at the beginning of the program.
//
// This program produces the generated event output Tree
// in the file output.root
//
// Notes :
// -------
// 1) All selected particle species appear in a separate Tree
//    in the output file.
//    However, the Tree called "Data" contains (links to) all
//    the data and as such can be used for combined analyses.
// 2) The p+p interactions have a positive run number, whereas
//    the p+gamma interactions have a negative run number.
//    The run number is available via the observable "evt.jrun".  
// 3) The beam and target momenta are available via the observables
//    "evt.Pbeam" and "evt.Ptarget", respectively.
// 4) The energy spectra can be retrieved via the observables ".p".
//    Example : "nu_mu.p" provides the nu_mu energy spectrum etc.
//
// To run this program, make sure to have also the shell script
// "agn-gen-tree.sh" and the ROOT macro "run.cc" available in
// the current directory and then just invoke ($ = prompt)
//
// $ root -b -q run-local.cc
//
// or submit a job on the IIHE cluster by invoking
//
// $ source agn-gen-tree.sh
//
// Nick van Eijndhoven, 24-mar-2016, IIHE-VUB Brussel.
/////////////////////////////////////////////////////////////////

#include <iostream>

#include "TString.h"

#include "NcCollider.h"
#include "NcTreeMaker.h"
#include "NcJob.h"
#include "NcAstrolab.h"

void user()
{

 /////////////////////
 // User input data //
 /////////////////////

 // Specify the produced particle spectra to be studied.
 // This allows reduction of Pythia memory usage by declaring various particles as stable.
 // Options for selected particle spectra are : "nu", "gamma", "neutron"
 // or any combination by a comma separated list like e.g. "nu,gamma"
 TString mode="nu,neutron,gamma";

 // Range of beam momenta
 // If beampmax<=beampmin a mono-energetic proton beam of with momentum beampmin will be used,
 // otherwise a dN/dp spectrum will be used as described by the function "beamspec".
 // In case a dN/dp spectrum is used, cross section weighting is switched on automatically.
 Float_t beampmin=2e5;  // Minimum proton beam momentum in GeV/c
 Float_t beampmax=1e7;  // Maximum proton beam momentum in GeV/c
 TF1 beamspec("beamspec","pow(x,-2.)");

 // Range of target momenta
 // If targetpmax<=targetpmin a mono-energetic photon target with momentum targetpmin will be used,
 // otherwise a dN/dp spectrum will be used as described by the function "targetspec".
 // In case a dN/dp spectrum is used, cross section weighting is switched on automatically.
 Float_t targetpmin=1e-5;  // Minimum photon target momentum in GeV/c
 Float_t targetpmax=1e-5;  // Maximum photon target momentum in GeV/c
 TF1 targetspec("targetspec","pow(x,-2.)");

 Int_t wxsec=0;     // Flag to indicate cross section weighting at varying energies

 Int_t mult=1;      // Flag to indicate activation of multiple interactions

 Int_t elastic=1;   // Flag to indicate activation of elastic and diffractive interactions

 Int_t randomise=0; // Flag to randomise the beam (>0) or target (<0) direction

 Float_t fact=0;    // Factor to increase the beam (>0) or target (<0) momentum for initialisation

 Float_t nprotons=100000; // Number of beam protons to be simulated

 Float_t pfrac=0.2;   // Fraction of beam protons used for p+gamma interactions in the jet

 Int_t printfreq=1000; // Print some NcCollider info every "printfreq" events

 Int_t evtlist=1;    // Produce an event listing of the first n events of each sample

 Int_t ntrymax=1000; // Maximum number of phase-space trials per event

 Int_t fulldump=0;   // Include also the incoming particles, strings etc. in the event (1) or not (0)

 Int_t pystat=1;     // Produce Pythia statistics at the end of each sample (1) or not (0)

 /////////////////////////////////////
 // The actual simulation procedure //
 /////////////////////////////////////

 cout << " ***" << endl;
 cout << " *** Generation has been optimised for " << mode.Data() << " spectra studies."<< endl;
 cout << " ***" << endl;

 NcTreeMaker* mktree=new NcTreeMaker();
 mktree->SetOutputFile("output.root","NcCollider AGN simulated data in plain ROOT tree format");
 if (mode.Contains("nu"))
 {
  mktree->UseTracks("nu_mu");
  mktree->UseTracks("nu_mubar");
  mktree->UseTracks("nu_e");
  mktree->UseTracks("nu_ebar");
  mktree->UseTracks("nu_tau");
  mktree->UseTracks("nu_taubar");
  mktree->UseTracks("nu",-1,1);
 }
 if (mode.Contains("gamma")) mktree->UseTracks("gamma");
 if (mode.Contains("neutron"))
 {
  mktree->UseTracks("n0");
  mktree->UseTracks("nbar0");
  mktree->UseTracks("p+");
  mktree->UseTracks("pbar-");
 }

 mktree->Select("track","p");

 mktree->Select("event","jrun");
 mktree->Select("event","user","Pbeam");
 mktree->Select("event","user","Ptarget");

 NcJob* q=new NcJob();
 q->Add(mktree);

 NcCollider* gen=new NcCollider();
 gen->SetTitle("AGN proton beamdump simulation");
 gen->SetPrintFreq(printfreq);
 gen->SetMultiple(mult);
 gen->SetElastic(elastic);

 cout << " === Multiple interaction flag : " << gen->GetMultiple() << endl;
 cout << " === Elastic and diffractive scattering flag : " << gen->GetElastic() << endl;
 cout << " === Minimal CMS energy for event generation : " << gen->GetEcmsMin() << endl;
 cout << " === Number of simulated beam protons : " << nprotons << endl;
 cout << " === Fraction of beam protons used for p+gamma interactions in the jet : " << pfrac << endl;

 NcEvent* evt=0;
 NcAstrolab lab;

 // Construct the beam momentum distribution from the specified dN/dE spectrum
 TH1F hpbeam;
 if (beampmax>beampmin)
 {
  cout << " === Beam spectrum : dN/dp=" << beamspec.GetTitle() << " within [" << beampmin << "," << beampmax << "] GeV/c" << endl; 
  beamspec.SetTitle("beamspec;Beam momentum in GeV/c;dN/dp");
  hpbeam=lab.GetCountsHistogram(beamspec,1000,beampmin,beampmax,0);
 }

 // Construct the target momentum distribution from the specified dN/dE spectrum
 TH1F hptarget;
 if (targetpmax>targetpmin)
 {
  cout << " === Target spectrum : dN/dp=" << targetspec.GetTitle() << " within [" << targetpmin << "," << targetpmax << "] GeV/c" << endl; 
  targetspec.SetTitle("targetspec;Target momentum in GeV/c;dN/dp");
  hptarget=lab.GetCountsHistogram(targetspec,1000,targetpmin,targetpmax,0);
 }

 Int_t jrun=0;
 Float_t beamp=0;
 Float_t targetp=0;
 Nc3Vector pbeam;
 Nc3Vector ptarget;
 Nc3Vector pfixed;
 NcSignal evtuser; // To store event user data
 evtuser.AddNamedSlot("Pbeam");
 evtuser.AddNamedSlot("Ptarget");
 Int_t ier=0;  // Flag to identify initialisation error
 Int_t nevents=0; // Number of events to be generated for each process
 Int_t igen=0;    // Flag to denote successful generation of an event
 Int_t ievt=0;    // Counter for successfully generated events
 Int_t ntry=0;    // Counter for phase-space trials per event

 ////////////////////////////////////////////////////////////////
 // Generate both p+p (jrun>0) and p+gamma (jrun<0) processes. //
 ////////////////////////////////////////////////////////////////

 // Initialisation of the processes
 for (Int_t k=0; k<2; k++)
 {
  beamp=beampmin;
  if (beampmax>beampmin)
  {
   beamp=beampmax;
   wxsec=1;
  }
  targetp=targetpmin;
  if (targetpmax>targetpmin)
  {
   targetp=targetpmax;
   wxsec=1;
  }
  pbeam.SetVector(0,0,beamp,"car");
  ptarget.SetVector(0,0,-targetp,"car");
  pfixed.SetVector(0,0,0,"car");

  if (!k) // p+p process
  {   
   nevents=(1.-pfrac)*nprotons;
   jrun=1;
   ier=gen->Init("free","p","p",0,&pbeam,&pfixed,wxsec,fact);
  }
  else
  {
   nevents=pfrac*nprotons;
   jrun=-1;
   ier=gen->Init("free","p","gamma",0,&pbeam,&ptarget,wxsec,fact);
  }

  if (ier) return;

  gen->SetRunNumber(jrun);

  // Define several particles as (un)stable according to selected analysis mode
  gen->SetStable(0,1,4); // Declare all mesons as stable
  if (mode.Contains("nu") || mode.Contains("gamma")) gen->SetStable(0,0,4); // Declare all mesons as unstable
  if (!(mode.Contains("gamma")))  gen->SetStable(111,1); // Declare pi0 as stable 
  if (!(mode.Contains("nu")))  gen->SetStable(211,1); // Declare pi+ and pi- as stable 
  if (mode.Contains("nu")) gen->SetStable(13,0); // Declare mu+ and mu- as unstable
  if (mode.Contains("nu") && !(mode.Contains("neutron"))) gen->SetStable(2112,0); // Declare n and nbar as unstable

  // Generation of the events for this process
  ievt=0;
  ntry=0;
  while (ievt<nevents && ntry<ntrymax)
  {
   // Pick a proton momentum from the beam momentum distribution 
   if (beampmax>beampmin)
   {
    beamp=hpbeam.GetRandom();
    pbeam.SetVector(0,0,beamp,"car");
    gen->SetMomentum(pbeam,1);
   }

   // Pick a photon momentum from the target momentum distribution 
   if (targetpmax>targetpmin)
   {
    targetp=hptarget.GetRandom();
    ptarget.SetVector(0,0,targetp,"car");
    gen->SetMomentum(ptarget,2);
   }
 
   // Randomisation of the beam or target direction
   if (randomise>0)
   {
    lab.RandomPosition(pbeam,0,180,0,360);
    gen->SetMomentum(pbeam,1);
   }
   if (randomise<0)
   {
    lab.RandomPosition(ptarget,0,180,0,360);
    gen->SetMomentum(ptarget,2);
   }

   if (ievt<evtlist) // Produce Pythia listing for the first "evtlist" events of each sample
   {
    if (!fulldump)
    {
     igen=gen->MakeEvent(0,1);
    }
    else
    {
     igen=gen->MakeEvent(0,1,-1);
    }
   }
   else // No Pythia event listing
   {
    igen=gen->MakeEvent();
   }

   if (igen<0) break;   // Error
   if (!igen) // Event did not pass selection
   {
    ntry++;
    continue;
   }

   ievt++;
   ntry=0;

   evt=gen->GetEvent();
   evtuser.SetSignal(beamp,"Pbeam");
   evtuser.SetSignal(targetp,"Ptarget");
   evt->SetUserData(evtuser);

   q->ProcessObject(evt);
  } // End of loop over the events for this process

   // Printout Pythia statistics for this event sample
   if (pystat) gen->Pystat(1);

 } // End of loop over the processes

 mktree->CloseTree();
}
