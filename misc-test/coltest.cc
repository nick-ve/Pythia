// ROOT macro to test various aspects of NcCollider
{
 gSystem->Load("ncfspack");

 NcVersion ver;
 ver.Data();

 NcCollider gen;

 gen.SetSpectatorPmin(1e10); // Suppress storage of spectator tracks

 Int_t mlist=1;  // Pythia event listing flag (0:no listing 1:event listing)
 Int_t medit=1;  // Pythia event editing flag (-1:no editing  0:no incoming particles 1:only stable particles)

 Int_t nucl=1; // Flag to indicate nucleus-nucleus interactions
 Int_t zp=2;
 Int_t ap=4;
 Int_t zt=2;
 Int_t at=4;
 Int_t npt=4;

 Float_t ecms=70;
 Float_t pzbeam=100;

 Int_t wxsec=1; // Flag to indicate cross section weighting at varying energies

 Int_t nevents=2; // Number of events to be generated

 Nc3Vector pbeam;
 Nc3Vector ptarget;

 pbeam.SetVector(0,0,pzbeam,"car");
 ptarget.SetVector(0,0,-10,"car");

 NcEvent* evt=0;

 Int_t jevt=0;
 Int_t igen=0;
/******
 if (nucl)
 {
  gen.Init("cms",zp,ap,zt,at,fabs(ecms));
 }
 else
 { 
  gen.Init("cms","p","n",fabs(ecms));
 }

 jevt=0;
 while (jevt<nevents)
 {
  igen=gen.MakeEvent(npt,mlist,medit);
cout << " jevt:" << jevt << " igen:" << igen << endl;
  if (igen<0) break;   // Error
  if (!igen) continue; // Event did not pass selection

  jevt++;

  evt=gen.GetEvent();
  if (evt)
  {
   evt->GetDevice(1)->Data();
   evt->GetDevice(2)->Data();
  }
 }


 if (nucl)
 {
  gen.Init("fixt",zp,ap,zt,at,pzbeam);
 }
 else
 { 
  gen.Init("fixt","p","n",pzbeam);
 }

 jevt=0;
 while (jevt<nevents)
 {
  igen=gen.MakeEvent(npt,mlist,medit);
cout << " jevt:" << jevt << " igen:" << igen << endl;
  if (igen<0) break;   // Error
  if (!igen) continue; // Event did not pass selection

  jevt++;

  evt=gen.GetEvent();
  if (evt)
  {
   evt->GetDevice(1)->Data();
   evt->GetDevice(2)->Data();
  }
 }
**********************/

 if (nucl)
 {
  gen.Init("free",zp,ap,zt,at,ecms,&pbeam,&ptarget,wxsec);
 }
 else
 { 
  gen.Init("free","p","n",ecms,&pbeam,&ptarget,wxsec);
 }

 jevt=0;
 while (jevt<nevents)
 {
  igen=gen.MakeEvent(npt,mlist,medit);
//cout << " jevt:" << jevt << " igen:" << igen << endl;
  if (igen<0) break;   // Error
  if (!igen) continue; // Event did not pass selection

  jevt++;

  evt=gen.GetEvent();
  if (evt)
  {
   evt->Data();
   evt->ShowTracks();
  }
 }

 pzbeam=pzbeam+10.;
 pbeam.SetVector(0,0,pzbeam,"car");
 gen.SetMomentum(pbeam,1);

 jevt=0;
 while (jevt<nevents)
 {
  igen=gen.MakeEvent(npt,mlist,medit);
//cout << " jevt:" << jevt << " igen:" << igen << endl;
  if (igen<0) break;   // Error
  if (!igen) continue; // Event did not pass selection

  jevt++;

  evt=gen.GetEvent();
  if (evt)
  {
   evt->Data();
   evt->ShowTracks();
  }
 }

 pzbeam=pzbeam+10.;
 pbeam.SetVector(0,0,pzbeam,"car");
 gen.SetMomentum(pbeam,1);

 jevt=0;
 while (jevt<nevents)
 {
  igen=gen.MakeEvent(npt,mlist,medit);
//cout << " jevt:" << jevt << " igen:" << igen << endl;
  if (igen<0) break;   // Error
  if (!igen) continue; // Event did not pass selection

  jevt++;

  evt=gen.GetEvent();
  if (evt)
  {
   evt->Data();
   evt->ShowTracks();
  }
 }
}
