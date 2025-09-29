
#include <iostream>
#include <fstream>
#include <string>

// --- main function ---
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

      if (line.empty() || line[0] == '#') {
         // Skip empty lines and comments
         continue;
      }
      // replace "MuDst.root" with "miniMc.root" at the end of the line
      const std::string muSuffix = "MuDst.root";
      const std::string miniSuffix = "minimc.root";
      if (line.size() < muSuffix.size() || line.substr(line.size() - muSuffix.size()) != muSuffix) {
         // Did not match the expected suffix; warn and skip
         std::cerr << "[getMiniMcFromMuDstList] WARN line " << nLines
                   << ": not a MuDst path (no trailing \"MuDst.root\"): " << line << "\n";
         continue;
      }

      std::string miniPath = line.substr(0, line.size() - muSuffix.size()) + miniSuffix;

      // Optional: check that the file exists (ROOT-style); comment out if not desired
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
      std::cerr << "[getMiniMcFromMuDstList] ERROR: no files added; returning NULL\n";
      delete chain;
      return 0;
   }
   return chain;
}

void MakeTStarJetPico(char *filelist = "test.list", const char *nametag = "TStarJetPicoMaker_test")
{
   int nEvents = 1e3;
   int nFiles = 2;
   int trigSet = 0;
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
   TChain *mcChain = getMiniMcFromMuDstList(filelist);
   if (!mcChain) {
      cout << "No MiniMc chain returned. Exiting." << endl;
      return;
   }

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

   // making the nametag for output root files
   std::string filestr = (std::string)std::string(filelist);
   std::string front = "MuDst";
   size_t front_len = front.size();
   size_t pos_front = filestr.find(front);
   size_t pos_begin = pos_front + front_len;
   std::string back = ".list";
   size_t back_len = back.size();
   size_t pos_back = filestr.find(back);

   std::string unique_id = filestr.substr(pos_begin, pos_back - pos_begin);
   std::string unique_name = "pt-hat" + unique_id;
   TStarJetPicoMaker *jetPicoMaker =
      new TStarJetPicoMaker(Form("%s.root", unique_name), mcChain, 1, nametag, nFiles, trigSet);
   jetPicoMaker->ProcessMC(1);
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

   //  set DEBUG output level

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
   printf("my macro processed %i events in %s", i, nametag);
   cout << "\tcpu: " << total.CpuTime() << "\treal: " << total.RealTime()
        << "\tratio: " << total.CpuTime() / total.RealTime() << endl;

   cout << endl;
   cout << "-------------" << endl;
   cout << "(-: Done :-) " << endl;
   cout << "-------------" << endl;
   cout << endl;
}
