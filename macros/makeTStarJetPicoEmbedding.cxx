//  Embedding pico production for pp200 GeV Run-12.
//  Trigger sim: kOnline (hardware DSM ruler) — the SAME config as the data
//  macros, so no measured trigger-efficiency correction is needed downstream.
//  Differs from makeTStarJetPico.cxx (data) in:
//    - reads StMiniMcTree alongside MuDst (mcChain via getMiniMcFromMuDstList)
//    - constructs TStarJetPicoMaker with the (mcChain, …) overload
//    - ProcessMC(1) so the JetTreeMc particle-level branch is filled
//    - cuts kept aligned with Dmitry where applicable (same as the data
//      macro): TowerEnergyMin=0.20, SetTowerAdcCut(4, 3.0),
//      TrackFitPointMin=12, |η_track|<2.5, DCA<3 cm
//    - Vz kept at ±80: the embedding vertex was thrown with σ_z = 45 cm,
//      offset -1.1 cm (GenerateVertex.C in request 20212001/20235003)

#include <iostream>
#include <fstream>
#include <string>

TChain *getMiniMcFromMuDstList(const char *filelist)
{
   if (!filelist) {
      std::cerr << "[getMiniMcFromMuDstList] ERROR: filelist is NULL\n";
      return 0;
   }
   std::ifstream in(filelist);
   if (!in.is_open()) {
      std::cerr << "[getMiniMcFromMuDstList] ERROR: cannot open list: " << filelist << "\n";
      return 0;
   }

   TChain *chain = new TChain("StMiniMcTree");
   std::string line;
   Long64_t nAdded = 0;
   Long64_t nLines = 0;

   while (std::getline(in, line)) {
      ++nLines;
      if (line.empty() || line[0] == '#')
         continue;

      const std::string muSuffix = "MuDst.root";
      const std::string miniSuffix = "minimc.root";
      if (line.size() < muSuffix.size() || line.substr(line.size() - muSuffix.size()) != muSuffix) {
         std::cerr << "[getMiniMcFromMuDstList] WARN line " << nLines << ": not a MuDst path: " << line << "\n";
         continue;
      }
      std::string miniPath = line.substr(0, line.size() - muSuffix.size()) + miniSuffix;
      if (gSystem->AccessPathName(miniPath.c_str())) {
         std::cerr << "[getMiniMcFromMuDstList] WARN: file not found -> " << miniPath << "\n";
         continue;
      }
      chain->Add(miniPath.c_str());
      ++nAdded;
   }

   std::cout << "[getMiniMcFromMuDstList] Added " << nAdded << " MiniMc files from " << nLines << " lines in "
             << filelist << "\n";
   if (nAdded == 0) {
      delete chain;
      return 0;
   }
   return chain;
}

void makeTStarJetPicoEmbedding(const char *filelist = "lists/test.list", const char *outputName = "test")
{
   const int nEvents = 1e9;
   const int nFiles = 1e9;
   const int trigSet = 0;

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

   gSystem->AddDynamicPath("./libs/");
   gSystem->Load("libStRefMultCorr.so");
   gSystem->Load("libTStarJetPico.so");
   gSystem->Load("libTStarJetPicoMaker.so");

   TStarJetPicoMaker::SuppressTRefTableNoise();
   TStarJetPicoMaker::SetLoggerLevel("StTriggerSimuMaker", "WARN");

   StChain *chain = new StChain("StChain");

   TChain *mcChain = getMiniMcFromMuDstList(filelist);
   if (!mcChain) {
      std::cout << "No MiniMc chain returned. Exiting." << std::endl;
      return;
   }

   StMuDstMaker *muDstMaker = new StMuDstMaker(0, 0, "", filelist, "", nFiles);

   St_db_Maker *dbMaker = new St_db_Maker("StarDb", "MySQL:StarDb");
   StEEmcDbMaker *eemcb = new StEEmcDbMaker("eemcDb");
   StEmcADCtoEMaker *adc = new StEmcADCtoEMaker();
   StPreEclMaker *pre_ecl = new StPreEclMaker();
   StEpcMaker *epc = new StEpcMaker();

   controlADCtoE_st *control_table = adc->getControlTable();
   control_table->CutOff[1] = -1;
   control_table->CutOffType[1] = 0;
   control_table->DeductPedestal[1] = 2;
   adc->saveAllStEvent(kTRUE);

   StTriggerSimuMaker *trigsim = new StTriggerSimuMaker();
   trigsim->setMC(false);
   trigsim->useBemc();
   trigsim->useEemc();
   trigsim->useBbc();
   trigsim->useOnlineDB();
   trigsim->bemc->setConfig(StBemcTriggerSimu::kOnline); // hardware DSM ruler, same as the data macros

   TStarJetPicoMaker *jetPicoMaker =
      new TStarJetPicoMaker(Form("%s.root", outputName), mcChain, 1, outputName, nFiles, trigSet);
   jetPicoMaker->ProcessMC(1);
   // jetPicoMaker->SetVertexSelector(TStarJetPicoMaker::VpdOrRank);
   jetPicoMaker->SetVertexSelector(TStarJetPicoMaker::Rank);
   jetPicoMaker->SetTowerAcceptMode(TStarJetPicoMaker::RejectBadTowerStatus);
   jetPicoMaker->SetStRefMultCorrMode(TStarJetPicoMaker::FillNone);
   jetPicoMaker->EventCuts()->SetVzRange(-80, 80);
   jetPicoMaker->EventCuts()->SetRefMultRange(0, 7000);
   // Dmitry-aligned cuts:
   jetPicoMaker->SetTowerEnergyMin(0.20);
   jetPicoMaker->SetTowerAdcCut(4, 3.0);
   jetPicoMaker->SetTrackEtaRange(-2.5, 2.5);
   jetPicoMaker->SetTrackFitPointMin(12);
   jetPicoMaker->SetTrackDCAMax(3.0);
   jetPicoMaker->SetTrackFlagMin(0);

   jetPicoMaker->SetTrackLastPointMin(125.0);

   if (chain->Init()) {
      std::cout << "StChain failed init: exiting" << std::endl;
      return;
   }

   TStopwatch total;
   TStopwatch timer;
   int i = 0;
   while (i < nEvents && chain->Make() == kStOk) {
      if (i % 500 == 0) {
         std::cout << "done with event " << i << "\tcpu: " << timer.CpuTime() << "\treal: " << timer.RealTime()
                   << "\tratio: " << timer.CpuTime() / timer.RealTime();
         timer.Start();
      }
      ++i;
      chain->Clear();
   }

   chain->ls(3);
   chain->Finish();
   std::cout << "processed " << i << " events"
             << "\tcpu: " << total.CpuTime() << "\treal: " << total.RealTime() << std::endl;
   std::cout << "-------------\n(-: Done :-)\n-------------" << std::endl;
}
