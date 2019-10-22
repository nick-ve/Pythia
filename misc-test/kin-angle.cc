// ROOT macro to investigate the energy dependence of the
// kinematic opening angle between a muon-neutrino and the
// muon produced in a CC interaction.
{
 gSystem->Load("ncfspack");

 Double_t E=1000; // Neutrino energy in GeV
 Int_t nevt=1e4;

 NcCollider* gen=new NcCollider();
 gen->SetTitle("neutrino-lepton kinematic opening angle simulation");
 gen->SetPrintFreq(nevt/10);
/// gen->SetMultiple(1);
/// gen->SetElastic(1);

 gen->Init("fixt","numu","p",E);

 // Declare all leptons, mesons and baryons as stable
 gen->SetStable(0,1,2);
 gen->SetStable(0,1,4);
 gen->SetStable(0,1,5);

 NcEvent* evt=0;
 TObjArray tracks;
 NcTrack* tx=0;
 Double_t ang=0;
 TH1F* h=new TH1F("h","Kinematic opening angle;Angle in degrees;Counts",900,0,90); 

 for (Int_t i=0; i<nevt; i++)
 {
  gen->MakeEvent();
  evt=gen->GetEvent();
  if (!evt) continue;
  evt->GetTracks("mu-",0,&tracks);
  tx=(NcTrack*)tracks.At(0);
  if (!tx) continue;
  ang=tx->GetX(2,"sph","deg");
  h->Fill(ang);
 }
 
 h->Draw();
}