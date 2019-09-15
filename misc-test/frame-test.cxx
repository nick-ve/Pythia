/////////////////////////////////////////////////////////////////
// Code (ROOT) to test the compatibility of event generation
// in various frames, i.e. CMS, FIXT or FREE.
//
// The frame, beam particles, energies, number of events and specification
// of the produced particle spectra have to be provided by the user
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
// To run this program, just invoke ($ = prompt)
//
// $ root -b -q run-local.cc
//
// or submit a job on the cluster by invoking
//
// $ source frame-test.sh
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

 TString beamname="p";       // The beam particle name

 TString targetname="pbar"; // The target particle name

 Int_t nucl=0; // Flag to indicate nucleus-nucleus interactions
 Int_t zp=2;
 Int_t ap=4;
 Int_t zt=2;
 Int_t at=4;
 Int_t npt=4;

 TString frame="cms";       // Interaction frame (cms, fixt or free) 

// beamp=1e7, targetp=10e-6 --> Ecms=20.022
// beamp=1e9, targetp=1e-6 --> Ecms=63.2525
// beamp=1e9, targetp=100e-9 --> Ecms=20.022
// beamp=1e9, targetp=1e-9 --> Ecms=2.209
 Float_t energy=20.022;     // CM or beam energy for frame=cms or frame=fixt
 Float_t beamp=158;         // Beam momentum in GeV/c (only for frame=free)
 Float_t targetp=1e-2;     // Target momentum in GeV/c (only for frame=free)

 Int_t wxsec=0;     // Flag to indicate cross section weighting at varying energies

 Int_t mult=1;      // Flag to indicate activation of multiple interactions

 Int_t elastic=1;   // Flag to indicate activation of elastic and diffractive interactions

 Int_t randomise=0; // Flag to randomise the beam (>0) or target (<0) direction

 Float_t fact=-100;    // Factor to increase the beam (>0) or target (<0) momentum for initialisation

 Int_t nevents=1;  // Number of events to be generated for each sample

 Int_t printfreq=1; // Print some NcCollider info every "printfreq" events

 Int_t evtlist=15;    // Produce an event listing of the first n events of each sample

 Int_t fulldump=0;   // Include also the incoming particles, strings etc. in the event (1) or not (0)

 Int_t pystat=1;     // Produce Pythia statistics at the end of each sample (1) or not (0)

 /////////////////////////////////////
 // The actual simulation procedure //
 /////////////////////////////////////

 cout << " ***" << endl;
 cout << " *** Generation has been optimised for " << mode.Data() << " spectra studies."<< endl;
 cout << " ***" << endl;

 NcTreeMaker* mktree=new NcTreeMaker();
 mktree->SetOutputFile("frame-test.root","NcCollider AGN simulated data in plain ROOT tree format");
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
 cout << " === Minimal CMS energy for event generation : " << gen->GetEcmsMin() << endl;

 NcEvent* evt=0;

 Nc3Vector pbeam;
 Nc3Vector ptarget;

 pbeam.SetVector(0,0,beamp,"car");
 ptarget.SetVector(0,0,-targetp,"car");

 Int_t jrun=0;
 NcSignal evtuser; // To store event user data
 evtuser.AddNamedSlot("Ebeam");
 evtuser.AddNamedSlot("Etarget");
 Int_t igen=0; // Flag to denote successful generation of an event
 Int_t ievt=0; // Counter for successfully generated events
 NcAstrolab lab;

 evtuser.SetSignal(beamp,"Ebeam");
 evtuser.SetSignal(targetp,"Etarget");

 Int_t ier=0;
 if (!nucl)
 {
  ier=gen->Init(frame,beamname,targetname,energy,&pbeam,&ptarget,wxsec,fact);
 }
 else
 {
  ier=gen->Init(frame,zp,ap,zt,at,energy,&pbeam,&ptarget,wxsec);
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

 ievt=0;
 Int_t ntk=0;
// for (ievt=0; ievt<nevents; ievt++)
 while (ievt<nevents)
 {
  if (randomise>0)
  {
   lab.RandomPosition(pbeam,0,180,0,360);
   gen->SetMomentum(pbeam,1);
  }
  if (randomise<0)
  {
   lab.RandomPosition(ptarget,150,180,0,360);
   gen->SetMomentum(ptarget,2);
  }

  if (ievt<evtlist) // Produce Pythia listing for the first "evtlist" events of each sample
  {
   if (!fulldump)
   {
    igen=gen->MakeEvent(npt,1);
   }
   else
   {
    igen=gen->MakeEvent(npt,1,-1);
   }
  }
  else // No Pythia event listing
  {
   igen=gen->MakeEvent(npt);
  }

  if (igen<0) break;   // Error
  if (!igen) continue; // Event did not pass selection

  evt=gen->GetEvent();

  if (ievt<evtlist)
  {
   cout << " === NcEvent data." << endl;
   evt->Data();
   evt->ShowTracks();
  }

  evt->SetUserData(evtuser);

  q->ProcessObject(evt);

  ievt++;
 } // End of loop over the events for this sample

 // Printout Pythia statistics for this sample
 if (pystat) gen->Pystat(1);

 mktree->CloseTree();
}
