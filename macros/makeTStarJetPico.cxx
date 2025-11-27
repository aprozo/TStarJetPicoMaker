
//  Example of how to use TStarJetPicoMaker to produce
//  a tree of TStarJetPicoEvents

//  this macro produces TStarJetPicoEvents from STAR muDSTs.
//  Requires the STAR libraries & muDST files. Defaults
//  are set to a test production using the test.list file
//  ( 10 files located on distributed disk from y14 high
//  luminosity production )

void makeTStarJetPico(const char *filelist = "lists/test.list", const char *outputName = "test")
{
   const int nEvents = 1e9;
   const int nFiles = 1e9;
   const int trigSet = 0;
   // load STAR libraries
   gROOT->Macro("LoadLogger.C");
   gROOT->Macro("loadMuDst.C");
   gSystem->Load("StarMagField.so");
   gSystem->Load("StMagF");
   gSystem->Load("StDetectorDbMaker");
   gSystem->Load("StTpcDb");
   gSystem->Load("St_db_Maker");
   gSystem->Load("StDbUtilities");
   gSystem->Load("StMiniMcEvent");
   gSystem->Load("StMiniMcMaker");
   gSystem->Load("StDaqLib");
   gSystem->Load("StEmcRawMaker");
   gSystem->Load("StEmcADCtoEMaker");
   gSystem->Load("StEpcMaker");
   gSystem->Load("StTriggerUtilities");
   gSystem->Load("StDbBroker");
   gSystem->Load("libgeometry_Tables");
   gSystem->Load("StEEmcUtil");
   gSystem->Load("StEEmcDbMaker");
   gSystem->Load("StPreEclMaker");
   gSystem->Load("StEpcMaker");
   gSystem->Load("StPicoEvent.so");
   gSystem->Load("StPicoDstMaker.so");

   gSystem->AddDynamicPath("./libs/"); // add local libs to path
   gSystem->Load("libStRefMultCorr.so");
   gSystem->Load("libTStarJetPico.so");
   gSystem->Load("libTStarJetPicoMaker.so");

   StChain *chain = new StChain("StChain");

   StMuDstMaker *muDstMaker = new StMuDstMaker(0, 0, "", filelist, "", nFiles);

   St_db_Maker *dbMaker = new St_db_Maker("StarDb", "MySQL:StarDb");
   // StMiniMcMaker* mcMaker = new StMiniMcMaker();
   StEEmcDbMaker *eemcb = new StEEmcDbMaker("eemcDb");
   StEmcADCtoEMaker *adc = new StEmcADCtoEMaker();
   StPreEclMaker *pre_ecl = new StPreEclMaker();
   StEpcMaker *epc = new StEpcMaker();

   // get control table so we can turn off BPRS zero-suppression and save hits from "bad" caps
   controlADCtoE_st *control_table = adc->getControlTable();
   control_table->CutOff[1] = -1;
   control_table->CutOffType[1] = 0;
   control_table->DeductPedestal[1] = 2;
   adc->saveAllStEvent(kTRUE);

   cout << "DEBUG B" << endl;

   // simulates a trigger response based on an ADC value & trigger definitions
   StTriggerSimuMaker *trigsim = new StTriggerSimuMaker();
   trigsim->setMC(false); // CHANGE IF GOING BACK TO DATA!!
   trigsim->useBemc();
   trigsim->useEemc();
   trigsim->useBbc();
   trigsim->useOnlineDB();
   trigsim->bemc->setConfig(StBemcTriggerSimu::kOffline);

   TStarJetPicoMaker *jetPicoMaker = new TStarJetPicoMaker(Form("%s.root", outputName));
   jetPicoMaker->SetInputSource(TStarJetPicoMaker::InputPicoDst);
   // jetPicoMaker->ProcessMC(1);
   jetPicoMaker->SetVertexSelector(TStarJetPicoMaker::VpdOrRank);
   jetPicoMaker->SetTowerAcceptMode(TStarJetPicoMaker::RejectBadTowerStatus);
   jetPicoMaker->SetStRefMultCorrMode(TStarJetPicoMaker::FillNone);
   jetPicoMaker->EventCuts()->SetVzRange(-30, 30);
   jetPicoMaker->EventCuts()->SetRefMultRange(0, 7000);
   jetPicoMaker->SetTowerEnergyMin(0.15);
   jetPicoMaker->SetTrackEtaRange(-1.5, 1.5);
   jetPicoMaker->SetTrackFitPointMin(10);
   jetPicoMaker->SetTrackDCAMax(3.0);
   jetPicoMaker->SetTrackFlagMin(0);

   jetPicoMaker->EventCuts()->AddTrigger(450202);
   jetPicoMaker->EventCuts()->AddTrigger(450212);

   cout << "DEBUG C" << endl;

   if (chain->Init()) {
      cout << "StChain failed init: exiting" << endl;
      return;
   }
   cout << "chain initialized" << endl;

   TStopwatch total;
   TStopwatch timer;

   int i = 0;
   cout << "DEBUG E" << endl;
   while (i < nEvents && chain->Make() == kStOk) {
      // cout << "DEBUG F" << endl;
      if (i % 500 == 0) {
         cout << "done with event " << i;
         cout << "\tcpu: " << timer.CpuTime() << "\treal: " << timer.RealTime()
              << "\tratio: " << timer.CpuTime() / timer.RealTime(); //<<endl;
         timer.Start();
         // memory.PrintMem( NULL );
      }
      i++;
      chain->Clear();
   }

   chain->ls(3);
   chain->Finish();
   printf("my macro processed %i events", i);
   cout << "\tcpu: " << total.CpuTime() << "\treal: " << total.RealTime()
        << "\tratio: " << total.CpuTime() / total.RealTime() << endl;

   cout << endl;
   cout << "-------------" << endl;
   cout << "(-: Done :-) " << endl;
   cout << "-------------" << endl;
   cout << endl;
}
