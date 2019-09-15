/////////////////////////////////////////////////////////////////
// Code (ROOT) for a beam dump simulation for dust obscured AGN.
// This program is designed to provide a comparison with a simulation
// made by Tobias Winchen.
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
// 3) All the produced samples have an equal amount of events.
//    So in case one wants to scale to a specific proton energy spectrum,
//    one has to introduce the proper energy weight in the analysis.
//    The beam energy is available in the observable "evt.Ebeam".
//    Example : Weighting for an E^-2 spectrum is obtained by using
//    weight=1./(evt.Ebeam*evt.Ebeam) for each (histogram) entry.
// 4) The energy spectra can be retrieved via the observables ".p".
//    Example : "nu_mu.p" provides the nu_mu energy spectrum etc.
//
// To run this program, select "tobi-test.cxx" in the file "run-local.cc"
// and then just invoke ($ = prompt)
//
// $ root -b -q run-local.cc
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

 const Int_t nsamples=1; // The number of different target gamma energy samples
 Float_t energies[nsamples]={1e-8}; // Various gamma target energies in GeV

 Float_t ebeam=50e6; // Proton beam energy in GeV

 Int_t wxsec=0;     // Flag to indicate cross section weighting at varying energies

 Int_t mult=1;      // Flag to indicate activation of multiple interactions

 Int_t elastic=1;   // Flag to indicate activation of elastic and diffractive interactions

 Int_t randomise=0; // Flag to randomise the beam (>0) or target (<0) direction

 Float_t fact=-100;    // Factor to increase the beam (>0) or target (<0) momentum for initialisation

 Int_t nevents=10;  // Number of events to be generated for each sample

 Int_t printfreq=1; // Print some NcCollider info every "printfreq" events

 Int_t evtlist=10;    // Produce an event listing of the first n events of each sample

 Int_t fulldump=0;   // Include also the incoming particles, strings etc. in the event (1) or not (0)

 Int_t pystat=1;     // Produce Pythia statistics at the end of each sample (1) or not (0)

 /////////////////////////////////////
 // The actual simulation procedure //
 /////////////////////////////////////

 cout << " ***" << endl;
 cout << " *** NcCollider run for " << nsamples << " gamma target energy samples with " << nevents << " event(s) per sample." << endl;
 cout << " *** Generated gamma target energies (in GeV) :";
 for (Int_t i=0; i<nsamples; i++)
 {
  cout << " " << energies[i];
 }
 cout << endl;
 cout << " *** Generation has been optimised for " << mode.Data() << " spectra studies."<< endl;
 cout << " ***" << endl;

 NcTreeMaker* mktree=new NcTreeMaker();
 mktree->SetOutputFile("tobi-test.root","NcCollider AGN simulated data in plain ROOT tree format");
 if (mode.Contains("nu"))
 {
  mktree->UseTracks("nu_mu");
  mktree->UseTracks("nu_mubar");
  mktree->UseTracks("nu_e");
  mktree->UseTracks("nu_ebar");
  mktree->UseTracks("nu_tau");
  mktree->UseTracks("nu_taubar");
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
 mktree->Select("event","user","Ebeam");
 mktree->Select("event","user","Etarget");

 NcJob* q=new NcJob();
 q->Add(mktree);

 NcCollider* gen=new NcCollider();
 gen->SetTitle("AGN proton beamdump simulation");
 gen->SetPrintFreq(printfreq);
 gen->SetMultiple(mult);
 gen->SetElastic(elastic);

 cout << " === Multiple interaction flag : " << gen->GetMultiple() << endl;
 cout << " === Elastic and diffractive scattering flag : " << gen->GetElastic() << endl;

 NcEvent* evt=0;
///// Float_t ebeam=0;
 Float_t etarget=0;

 Nc3Vector pbeam;
 Nc3Vector ptarget;

 pbeam.SetVector(0,0,ebeam,"car");

 Int_t jrun=0;
 NcSignal evtuser; // To store event user data
 evtuser.AddNamedSlot("Ebeam");
 evtuser.AddNamedSlot("Etarget");
 Int_t igen=0; // Flag to denote successful generation of an event
 Int_t ievt=0; // Counter for successfully generated events
 NcAstrolab lab;

 // Generate both p+p (jrun>0) and p+gamma (jrun<0) processes
 for (Int_t k=1; k<2; k++)
 {
  // Loop over the various (energy) samples
  for (Int_t isample=0; isample<nsamples; isample++)
  {
   etarget=energies[isample]; // Gamma target energy in GeV
   ptarget.SetVector(0,0,-etarget,"car");

   evtuser.SetSignal(ebeam,"Ebeam");
   evtuser.SetSignal(etarget,"Etarget");

   jrun=isample+1;

   if (!k) // p+p process
   {   
    gen->Init("fixt","p","p",ebeam);
   }
   else
   {
    jrun=-jrun;
    // Initialise at higher energy to prevent threshold problems
    gen->Init("free","p","gamma",0,&pbeam,&ptarget,wxsec,fact);
   }

   gen->SetRunNumber(jrun);

   // Define several particles as (un)stable according to selected analysis mode
   gen->SetStable(0,1,4); // Declare all mesons as stable
   if (mode.Contains("nu") || mode.Contains("gamma")) gen->SetStable(0,0,4); // Declare all mesons as unstable
   if (!(mode.Contains("gamma")))  gen->SetStable(111,1); // Declare pi0 as stable 
   if (!(mode.Contains("nu")))  gen->SetStable(211,1); // Declare pi+ and pi- as stable 
   if (mode.Contains("nu")) gen->SetStable(13,0); // Declare mu+ and mu- as unstable
   if (mode.Contains("nu") && !(mode.Contains("neutron"))) gen->SetStable(2112,0); // Declare n and nbar as unstable

   ievt=0;
//   for (Int_t ievt=0; ievt<nevents; ievt++)
   while (ievt<nevents)
   {
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
    if (!igen) continue; // Event did not pass selection

    ievt++;

    evt=gen->GetEvent();
    evt->SetUserData(evtuser);

    q->ProcessObject(evt);
   } // End of loop over the events for this sample

   // Printout Pythia statistics for this sample
   if (pystat) gen->Pystat(1);

  } // End of loop over the samples
 } // End of loop over the processes

 mktree->CloseTree();
}
