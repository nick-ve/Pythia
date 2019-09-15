// ROOT macro to interactively test Pythia threshold production
{
 gSystem->Load("ncfspack");

 NcCollider* gen=new NcCollider();
 gen->SetTitle("AGN p-gamma threshold simulation");
 gen->SetPrintFreq(1);
 gen->SetMultiple(1);
 gen->SetElastic(1);

 Nc3Vector* pbeam=new Nc3Vector();
 Nc3Vector* ptarget=new Nc3Vector();
 Nc3Vector* pfixed=new Nc3Vector();
 pbeam->SetVector(0,0,2e5,"car");
 ptarget->SetVector(0,0,-1e-5,"car");
 pfixed->SetVector(0,0,0,"car");

 gen->Init("free","p","gamma",0,pbeam,ptarget,1,0);
// gen->Init("free","p","p",0,pbeam,pfixed,1,0);

 cout << " Generate an event with :" << endl;
 cout << " pbeam->SetVector(0,0,p,\"car\");" << endl;
 cout << " gen->SetMomentum(*pbeam,1);" << endl;
 cout << " gen->MakeEvent(0,1,0);" << endl;
}