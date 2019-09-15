///////////////////////////////////////////////////////////////////
// Generic ROOT macro to process IceCube data in IcePack format. //
// The actual C++ code to be processed should be available in    //
// a file called user.cxx with "void user()" as main function.   //
//                                                               //
// To execute the user code, just invoke ($ = prompt)            //
// $ root -b -q run.cc                                           //
//                                                               //
//--- Nick van Eijndhoven 19-aug-2013 IIHE-VUB, Brussels         //
///////////////////////////////////////////////////////////////////
{
 gSystem->Load("ncfspack");
 gSystem->Load("icepack");

 // Load and compile the user code
 gROOT->LoadMacro("user.cxx+");

 // Execute the user code
 user();
}
