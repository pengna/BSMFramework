#ifndef __TRIGGER_H_ 
#define __TRIGGER_H_
#include <memory>
/////
//   User include files
/////
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
//#include "HLTrigger/Muon/interface/HLTMuonIsoFilter.h"
#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Common/interface/TriggerResultsByName.h"
#include "HLTrigger/HLTcore/interface/HLTEventAnalyzerAOD.h"
#include "DataFormats/HLTReco/interface/TriggerRefsCollections.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"
#include "DataFormats/PatCandidates/interface/TriggerPath.h"
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include "DataFormats/PatCandidates/interface/TriggerAlgorithm.h"
#include <TTree.h>
#include <string>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include "baseTree.h"
#include <TBranch.h>
//namespaces
using namespace std;
using namespace edm;
class TriggerSelector : public baseTree{
 public:
  TriggerSelector(std::string name, TTree* tree, bool debug, const edm::ParameterSet& cfg);
  ~TriggerSelector();
  virtual void startTrigger (edm::EventSetup const& , edm::Run const &);
  void Fill(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  void SetBranches();
  void Clear();
 private:
  TriggerSelector(){};
  /////
  //   Config variables
  /////
  //vector<int> Trigger_decision;
  //vector <string> Trigger_names;
  HLTConfigProvider hltConfig_;
  edm::InputTag triggerBits_;
  double _maxtriggerversion;
  bool _is_data;
  bool _reHLT;
  int HLT_PFHT650_WideJetMJJ900DEtaJJ1p5;
  int HLT_PFHT650_WideJetMJJ950DEtaJJ1p5;
  int HLT_PFHT800;
  int HLT_PFHT900;
  int HLT_PFJet450;
  int HLT_PFJet500;
  int HLT_AK8PFJet450;
  int HLT_AK8PFJet500;
  int HLT_AK8PFJet360_TrimMass30;
  int HLT_AK8PFHT700_TrimR0p1PT0p03Mass50;
  int HLT_Ele27_eta2p1_WPTight_Gsf;
  int HLT_Ele30_WPTight_Gsf;
  int HLT_Ele27_WPTight_Gsf;
  int HLT_Ele25_eta2p1_WPTight_Gsf;
  int HLT_Ele115_CaloIdVT_GsfTrkIdT;
  int HLT_DoubleEle24_22_eta2p1_WPLoose_Gsf;
  int HLT_DoubleEle33_CaloIdL_MW;
  int HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_MW;
  int HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ;
  int HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL;
  int HLT_IsoMu22;
  int HLT_IsoTkMu22;
  int HLT_IsoMu24;
  int HLT_IsoTkMu24;
  int HLT_IsoMu22_eta2p1;
  int HLT_IsoTkMu22_eta2p1;
  int HLT_Mu50;
  int HLT_OldMu100;
  int HLT_TkMu50;
  int HLT_TkMu100;
  int HLT_DoubleMu33NoFiltersNoVtx;
  int HLT_DoubleMu23NoFiltersNoVtxDisplaced;
  int HLT_Mu30_TkMu11;
  int HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ;
  int HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8;
  int HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ;
  int HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL;
  int HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ;
  int HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL;
  int HLT_Ele32_WPTight_Gsf;
  int HLT_Ele35_WPTight_Gsf;
  int HLT_IsoMu27;
  int HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL;
  int HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ;
  int HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL;
  int HLT_Mu8_DiEle12_CaloIdL_TrackIdL;
  int HLT_DiMu9_Ele9_CaloIdL_TrackIdL_DZ;
  int HLT_TripleMu_12_10_5;
  // add HLT
  int HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ;
  int HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL;
  int HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ;
  int HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL;
  int HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ;
  int HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8;
  int HLT_Mu8_TrkIsoVVL_Ele17_CaloIdL_TrackIdL_IsoVL;
  int HLT_Mu17_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL;
  int HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL;
  int HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_DZ;
  int HLT_TripleMu_10_5_5_DZ;
  int HLT_Mu8_DiEle12_CaloIdL_TrackIdL_DZ;
  int HLT_DiMu9_Ele9_CaloIdL_TrackIdL; 
  int HLT_Ele27_eta2p1_WPLoose_Gsf;
  // bstar
  int HLT_Ele32_WPTight_Gsf_L1DoubleEG;
  int HLT_Photon200;
  int HLT_Photon175;
  int HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165;
  int HLT_Ele45_CaloIdVT_GsfTrkIdT_PFJet200_PFJet50;
  int HLT_Ele27_WP85_Gsf;
};
#endif
