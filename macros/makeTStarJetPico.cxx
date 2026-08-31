
//  DATA pico production — ONE macro for every stream. config selects the
//  trigger set: "JPHT" (HT2+JP0/1/2, tower-trigger folder), "MB"
//  (370001+370011, min-bias folder) or "ALL" (all six). The two-folder
//  split keeps each downstream analysis reading only its own stream.
//  vz +-80, highest-rank vertex, all other selection at Stage-2.
//  Trigger sim: kOnline (hardware DSM ruler), same config as the embedding
//  macro — one consistent ruler, no C(pT) needed downstream.

void makeTStarJetPico(const char *filelist = "lists/test.list", const char *outputName = "test",
                      const char *config = "ALL")
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

   // Internal log suppression (no stream splitting):
   //   1) drop the TRefTable::Add "SetParent must be called" spam
   //   2) silence StTriggerSimuMaker INFO chatter
   TStarJetPicoMaker::SuppressTRefTableNoise();
   TStarJetPicoMaker::SetLoggerLevel("StTriggerSimuMaker", "WARN");

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

   StTriggerSimuMaker *trigsim = new StTriggerSimuMaker();
   trigsim->setMC(false);
   trigsim->useBemc();
   trigsim->useEemc();
   trigsim->useBbc();
   trigsim->useOnlineDB();
   trigsim->bemc->setConfig(StBemcTriggerSimu::kOnline);
   // useEemc is REQUIRED even for a barrel-only study: with useOnlineDB the
   // register loader (get2009DsmRegistersFromOnlineDatabase) dereferences
   // eemc-> unconditionally for the EE101/EE001 DSM dictionary rows ->
   // segfault in InitRun if absent.

   TStarJetPicoMaker *jetPicoMaker = new TStarJetPicoMaker(Form("%s.root", outputName));
   jetPicoMaker->SetInputSource(TStarJetPicoMaker::InputMuDst);
   jetPicoMaker->ProcessMC(0); // 0 = data, 1 = MC; LoadMuDst aborts if MC requested without StMiniMcEvent
   // jetPicoMaker->SetVertexSelector(TStarJetPicoMaker::VpdOrRank);
   jetPicoMaker->SetVertexSelector(TStarJetPicoMaker::Rank);
   jetPicoMaker->SetTowerAcceptMode(
      TStarJetPicoMaker::RejectBadTowerStatus); 
   jetPicoMaker->SetStRefMultCorrMode(TStarJetPicoMaker::FillNone);

   //jetPicoMaker->SetUseEemc(true);

   jetPicoMaker->EventCuts()->SetVzRange(-80, 80);
   jetPicoMaker->EventCuts()->SetRefMultRange(0, 7000);
   jetPicoMaker->SetTowerEnergyMin(0.20);
   // Per-tower ADC quality cut
   //   require (ADC - pedestal) > 4 AND (ADC - pedestal) > 3·RMS.
   // Hot-tower handling layered with the DB tower-status flag (BTOW status != 1)
   // already checked via SetTowerAcceptMode(RejectBadTowerStatus) above; this
   // ADC cut filters transient pedestal-noise spikes that pass the energy
   // threshold but lack real signal.
   jetPicoMaker->SetTowerAdcCut(4, 3.0);
   jetPicoMaker->SetTrackEtaRange(-2.5, 2.5);
   jetPicoMaker->SetTrackFitPointMin(12);
   jetPicoMaker->SetTrackDCAMax(3.0);
   jetPicoMaker->SetTrackFlagMin(0);
   jetPicoMaker->SetTrackLastPointMin(125.0);

   const TString cfg(config);
   if (cfg == "ALL" || cfg == "JPHT") {
      jetPicoMaker->EventCuts()->AddTrigger(370531); // HT2
      jetPicoMaker->EventCuts()->AddTrigger(370601); // JP0
      jetPicoMaker->EventCuts()->AddTrigger(370611); // JP1
      jetPicoMaker->EventCuts()->AddTrigger(370621); // JP2
   }
   // jetPicoMaker->EventCuts()->AddTrigger(370982); // JP2*L2JetHigh
   // jetPicoMaker->EventCuts()->AddTrigger(370641); // AJP
   if (cfg == "ALL" || cfg == "MB") {
      jetPicoMaker->EventCuts()->AddTrigger(370001); // VPDMB
      jetPicoMaker->EventCuts()->AddTrigger(370011); // VPDMB-nobsmd
   }
   // jetPicoMaker->EventCuts()->AddTrigger(370021); // BBCMB
   // jetPicoMaker->EventCuts()->AddTrigger(370022); // BBCMB

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
