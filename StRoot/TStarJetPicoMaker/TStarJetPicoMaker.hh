/*        TStarJetPicoMaker
   Produces TStarJetPicoEvent trees from STAR muDSTs,
   given a set of event parameters and quality cuts.
   Capable of producing both daq streams and MC streams.
 
   The output trees include:
   - event & run environment information
   - one primary vertex (selection criteria below)
   - primary tracks with respect to primary vertex
   - EMCal tower hits
   - EMCal trigger information
 */

#ifndef TSTARJETPICOMAKER__HH
#define TSTARJETPICOMAKER__HH

#include "TFile.h"
#include "TTree.h"

#include "StMaker.h"
#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StPicoDstMaker/StPicoDstMaker.h"
#include "StPicoEvent/StPicoDst.h"
#include "StPicoEvent/StPicoEvent.h"
#include "StTriggerUtilities/StTriggerSimuMaker.h"
#include "StMcEvent/StMcEventTypes.hh"
#include "StMiniMcEvent/StMiniMcEvent.h"
#include "StMiniMcMaker/StMiniMcMaker.h"
#include "StEmcUtil/geometry/StEmcGeom.h"
#include "StEmcUtil/filters/StEmcFilter.h"
#include "StEmcUtil/projection/StEmcPosition.h"

#include "TDatabasePDG.h"

#include "TStarJetEventCuts.hh"
#include "TStarJetV0Cuts.hh"

#include "TChain.h"

#include <string>
#include <vector>


class TStarJetPicoEvent;
class StRefMultCorr;
class StBemcTables;
class StEmcCollection;
class StEmcCluster;

/*  Used to store track & tower matching 
    information between computation steps
 */
struct BemcMatch {
  Int_t globalId;
  Int_t trackId;
  Double_t trackEta;
  Double_t trackPhi;
  Double_t matchEta;
  Double_t matchPhi;
  Int_t matchedTowerId; // Dmitry-style: STAR-DB tower ID the track projects to.

  BemcMatch() : globalId(-1), trackId(-1), trackEta(0.0), trackPhi(0.0), matchEta(0.0), matchPhi(0.0), matchedTowerId(-1) {};
  BemcMatch(int id, int trkId, double trackEta, double trackPhi, double matchEta, double matchPhi, int towerId = -1) :
  globalId(id), trackId(trkId), trackEta(trackEta), trackPhi(trackPhi), matchEta(matchEta), matchPhi(matchPhi), matchedTowerId(towerId) {};

};

class TStarJetPicoMaker : public StMaker {
  
public:
  enum inputMode{NotSet=0, InputMuDst=1, InputPicoDst=2};
  enum vertexMode{VpdOrRank=0, Vpd=1, Rank=2};
  enum towerMode{AcceptAllTowers=0, RejectBadTowerStatus=1};
  enum refMultCorrectionMode{FillNone=0, FillGRefAndRefMultCorr=1, FillGRefMultCorr=2, FillGRefMultCorrP16ID=3, FillGRefMultCorrVPDMB30=4, FillGRefMultCorrVPDNoVtx=5, FillRefMultCorr=6};
  
  /* if input mode is not set by user, the reader will search the chain
     for a muDst or picoDst - if both are present for some
     reason, it will error out.
   */
  TStarJetPicoMaker(std::string outFileName, TChain *mcTree=nullptr, inputMode input = NotSet, std::string name = "TStarJetPicoMaker", int nFiles = 1, int trigSet = 0);
  ~TStarJetPicoMaker();
    
    // loads a new chain
    bool LoadTree(TChain* chain);
  
  /* the default methods called by the StChain. 
     Init(): called once to initialize before Make() is called
     Make(): called for each event, produces output TStarJetPicoEvent
             data structures for every event that passes event cuts
     Clear(): called after each Make() to reset for next event
     Finish(): called once at the end to save data to disk and clean up.
   */
  Int_t Init();
  Int_t Make();
  void  Clear(Option_t* option = "");
  Int_t Finish();
  
  /* WARNING: I have not had an opportunity to test this, it is merely
     copied from the old Maker.
     If true, maker expects input to have both monte-carlo & GEANT data,
     and will produce a TTree for each. Currently only works for muDst
   */
  void ProcessMC(bool flag) {mMakeMC = flag;}
  
  /*  *********DEPRICATED USE WITH CAUTION*********
        included for backwards compatibility, but
        has not been tested by us.
   */
  void ProcessV0s(bool flag) {mMakeV0 = flag;}
   
  /* returns pointer to TStarJetEventCuts intstance to allow
     direct setting of cut parameters by user.
   */
  TStarJetEventCuts* EventCuts() {return mEventSelector;}
  
  /* returns pointer to TStarJetv0Cuts instance to allow direct
     setting of cut parameters by user.
   */
  TStarJetV0Cuts*    V0Cuts() {return mV0Selector;}
  
  /* defines the input source (currently muDst or picoDst).
     If its not set, the reader will attempt to find the 
     source at Init() time
   */
  void SetInputSource(inputMode input) {mInputMode = input;}
  
  /* defines how a primary vertex is selected if using
     muDst input. PicoDsts only have one vertex.
     Default uses VPD if available, highest rank otherwise.
   */
  void SetVertexSelector(vertexMode vtx) {mVertexMode = vtx;}
  
  /* define if tower status should be used to reject towers, or if all
   towers should be accepted - default is to accept all towers, then
   generate a bad tower list for the entire data set.
   */
  void SetTowerAcceptMode(towerMode mode) {mTowerStatusMode = mode;}
  
  
  /* sets whether to do refmult/grefmult corrections for
   the reproduction. Be careful - the lookup is done only
   by run number, so if there are multiple productions of
   the same data, it can't discriminate and its possible
   StRefMultCorr will use a bad correction parameterization.
   Also - some runs have different parameterizations depending
   on trigger selection.
   */
  void SetStRefMultCorrMode(refMultCorrectionMode corrMode) {mRefMultCorrMode = corrMode;}
  
  /* if selecting vertices with the Vpd, can change the
     dVz requirement used from 3 cm
   */
  void SetVertexdVz(double dVz) {mdVzMax = dVz;}
  
  /* set minimum track flag value for primary tracks (default = 0) */
  void SetTrackFlagMin(int mMin) {mTrackFlagMin = mMin;}
  
  /* set maximum track DCA for primary tracks (default = 3) */
  void SetTrackDCAMax(double mMax) {mTrackDCAMax = mMax;}
  
  /* set eta range for primary tracks (default = -1.5, 1.5) */
  void SetTrackEtaRange(double mMin, double mMax) {mTrackEtaMin = mMin; mTrackEtaMax = mMax;}
  
  /* set minimum number of fit points required for a track
     (default = 10) 
   */
  void SetTrackFitPointMin(int mMin) {mTrackFitPointMin = mMin;}
  
  /* set minimum radial coordinate of last fit hit (cm). Mirrors Dmitry's
     StjTrackCutLastPoint(125) — drops short helix-curling tracks.
     Default = 0 (disabled, original behaviour preserved). Set to 125 to
     enforce Dmitry's cut at production time without altering pico schema.
   */
  void SetTrackLastPointMin(double mMin) {mTrackLastPointMin = mMin;}

  /* set the minimum tower energy to be reconstructed (default = 0.15) */
  void SetTowerEnergyMin(double mMin) {mTowerEnergyMin = mMin;}

  /* set the BEMC tower ADC quality cut. Mirrors Dmitry's StjTowerEnergyCutAdc(min,sigma):
   *   require (ADC - pedestal) > min  AND  (ADC - pedestal) > sigma * RMS
   * Default = (4, 3.0). Set min<=0 (or sigma<=0) to disable.
   */
  void SetTowerAdcCut(int min, double sigma) {mTowerAdcMin = min; mTowerAdcSigma = sigma;}


  /* Raise a named log4cxx logger's threshold so messages below `level`
     are dropped at the source (no awk filtering needed). `level` is one
     of "DEBUG","INFO","WARN","ERROR","FATAL" — case-sensitive.
     Example:  TStarJetPicoMaker::SetLoggerLevel("StTriggerSimuMaker","WARN");
   */
  static void SetLoggerLevel(const char* loggerName, const char* level);

  /* Install a chained ROOT error handler that swallows the TRefTable::Add
     "SetParent must be called" spam emitted on every Fill while forwarding
     all other errors to STAR's handler.
   */
  static void SuppressTRefTableNoise();

  private:
  
  Int_t  InitInput();
  void   InitMakers();
  void   InitOutput();
  bool   LoadEvent();
  
  Int_t MakeMuDst();
  Int_t MakePicoDst();
  
  Int_t LoadMuDst();
  Int_t LoadPicoDst();
  
  Int_t  SelectVertex();
  Bool_t MatchToVpd();
  
  Bool_t  MuProcessPrimaryTracks();
  Bool_t  PicoProcessPrimaryTracks();
  Float_t ComputeXY(const StThreeVectorF& pos, const StPhysicalHelixD &helix);
  
  Bool_t MuProcessBEMC();
  Bool_t PicoProcessBEMC();
  Int_t  MuFindSMDClusterHits(StEmcCollection* coll, Double_t eta, Double_t phi, Int_t detectorID);
  
  void MuProcessTriggerObjects();
  void PicoProcessTriggerObjects();
  
  Bool_t MuFillHeader();
  Bool_t PicoFillHeader();
  
  Double_t MuGetReactionPlane();
  Double_t PicoGetReactionPlane();
  
  void MuProcessMCEvent();
  
  void MuProcessV0s();
  
  /* selects events on data quality and physical 
     characteristics, such as centrality
   */
  TStarJetEventCuts* mEventSelector;
  
  /* selects secondary vertices based on data quality
     and daughter selection from global tracks
   */
  TStarJetV0Cuts* mV0Selector;
  
  std::string mOutFileName;
  TFile* mOutFile;
  
    unsigned current_;
    
    TChain* chain_;

  TDatabasePDG* pdg;
    
  /* output TTrees, only attempts to fill mMCTree
     if mMakeMC flag is set
   */
  TTree* mTree;
  TTree* mMCTree;
  TStarJetPicoEvent* mEvent;
  TStarJetPicoEvent* mMCEvent;
  
  StMuDstMaker*       mMuDstMaker;
  StMuDst*            mMuDst;
  StMuEvent*          mMuInputEvent;
  StPicoDstMaker*     mPicoDstMaker;
  StPicoDst*          mPicoDst;
  StPicoEvent*        mPicoInputEvent;
  StTriggerSimuMaker* mTriggerSimu;
  StTriggerSimuMaker*             mTriggerSimuHW; // kOnline instance (hardware-equivalent JP patches)
  //StMcEvent*          mStMCEvent;
    
  StMiniMcEvent* mStMiniMcEvent;
  
  StEmcPosition*         mEMCPosition;
  StEmcGeom*             mBEMCGeom;
  StEmcFilter*           mEMCFilter;
  StBemcTables*          mBemcTables;
  std::vector<BemcMatch> mBemcMatchedTracks;
  
  StRefMultCorr*      mRefMultCorr;
  StRefMultCorr*      mGRefMultCorr;
  
  unsigned mCallsToMake, mNEvents, mNAcceptedEvents;
  
  bool   mMakeV0;
  bool   mMakeMC;
  
  inputMode  mInputMode;
  
  vertexMode mVertexMode;
  
  towerMode  mTowerStatusMode;


  refMultCorrectionMode mRefMultCorrMode;
  
  Double_t mdVzMax;
  Int_t    mTrackFlagMin;
  Double_t mTrackDCAMax;
  Double_t mTrackEtaMin;
  Double_t mTrackEtaMax;
  Int_t    mTrackFitPointMin;
  Double_t mTrackLastPointMin; ///< Dmitry's StjTrackCutLastPoint(125): require lastPoint.Perp() > min. Default 0 = off.
  Double_t mTowerEnergyMin;
  Int_t    mTowerAdcMin;     ///< Dmitry's StjTowerEnergyCutAdc(min, sigma): require (ADC-ped) > min
  Double_t mTowerAdcSigma;   ///< Dmitry's StjTowerEnergyCutAdc(min, sigma): require (ADC-ped) > sigma * RMS

  ClassDef(TStarJetPicoMaker,1)
};

#endif /* TStarJetPicoMaker */
