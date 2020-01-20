#ifndef __JET_MU_H_
#define __JET_MU_H_
/////
//   Include files and namespaces
/////
#include <memory>
#include <iostream>
#include <cmath>
#include <vector>
#include <TBranch.h>
#include <TTree.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <string>
#include <map>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <TRandom3.h>
#include <TBranch.h>                                                                    
#include <TClonesArray.h>
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "JetMETCorrections/Modules/interface/JetResolution.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerEventWithRefs.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "baseTree.h"
#include <random>
using namespace std;
using namespace pat;
using namespace edm;
using namespace reco;
/////
//   Class declaration
/////
class JetSelector : public  baseTree{
 public:
  JetSelector(std::string name, TTree* tree, bool debug, const edm::ParameterSet& cfg, edm::ConsumesCollector && iC);
  ~JetSelector();
  void Fill(const edm::Event& iEvent);
  void SetBranches();
  void JECInitialization();
  void Clear();
  void GetJER(pat::Jet jet, float JesSF, float rhoJER, bool AK4PFchs, float &JERScaleFactor, float &JERScaleFactorUP, float &JERScaleFactorDOWN);
  void Getjer(pat::Jet jet, float JesSF, float rhoJER, bool AK4PFchs, float &JERScaleFactor, float &JERScaleFactorUP, float &JERScaleFactorDOWN, int nEvent);
  void computeQG(const pat::Jet& jet, double& ptD_, double& mult_, double& axis1_, double& axis2_, bool useQualityCut );
 private:
  JetSelector(){};
  /////
  //   Config variables
  /////
  edm::EDGetTokenT<reco::VertexCollection> vtx_h_;
  edm::EDGetTokenT<pat::JetCollection> jets_;
  edm::EDGetTokenT<edm::View<pat::Muon> > muon_h_;
  edm::EDGetTokenT<edm::View<pat::Electron> > electron_pat_;
  edm::EDGetTokenT<edm::View<pat::Tau> > taus_;
  edm::EDGetTokenT<edm::ValueMap<float> > qgToken_;
  edm::EDGetTokenT<edm::ValueMap<float> > axis2Token_;
  edm::EDGetTokenT<edm::ValueMap<float> > ptDToken_;
  edm::EDGetTokenT<edm::ValueMap<int> > multToken_;
  edm::EDGetTokenT<pat::JetCollection> puppijets_;
  edm::EDGetTokenT<double> rhopogHandle_;
  //edm::EDGetTokenT<double> rhoJERHandle_;
  edm::FileInPath jecPayloadNamesAK4PFchsMC1_;
  edm::FileInPath jecPayloadNamesAK4PFchsMC2_;
  edm::FileInPath jecPayloadNamesAK4PFchsMC3_;
  edm::FileInPath jecPayloadNamesAK4PFchsMCUnc_;
  edm::FileInPath jecPayloadNamesAK4PFchsDATA1_;
  edm::FileInPath jecPayloadNamesAK4PFchsDATA2_;
  edm::FileInPath jecPayloadNamesAK4PFchsDATA3_;
  edm::FileInPath jecPayloadNamesAK4PFchsDATA4_;
  edm::FileInPath jecPayloadNamesAK4PFchsDATAUnc_;
  edm::FileInPath jecPayloadNamesAK4PFPuppiMC1_;
  edm::FileInPath jecPayloadNamesAK4PFPuppiMC2_;
  edm::FileInPath jecPayloadNamesAK4PFPuppiMC3_;
  edm::FileInPath jecPayloadNamesAK4PFPuppiMCUnc_;
  edm::FileInPath jecPayloadNamesAK4PFPuppiDATA1_;
  edm::FileInPath jecPayloadNamesAK4PFPuppiDATA2_;
  edm::FileInPath jecPayloadNamesAK4PFPuppiDATA3_;
  edm::FileInPath jecPayloadNamesAK4PFPuppiDATA4_;
  edm::FileInPath jecPayloadNamesAK4PFPuppiDATAUnc_;
  std::string jerAK4PFchs_;
  std::string jerAK4PFchsSF_;
  std::string jerAK4PFPuppi_;
  std::string jerAK4PFPuppiSF_;
  double _Jet_pt_min;
  double _Muon_pt_min;
  double _Muon_eta_max;
  double _patElectron_pt_min;
  double _patElectron_eta_max;
  double _Tau_pt_min;
  double _Tau_eta_max;
  bool _super_TNT;
  bool _PuppiVar;
  bool _is_data;
  int  _dataEra; // which year?
  bool _qglVar;
  /////
  //   JEC
  /////
  boost::shared_ptr<FactorizedJetCorrector>   jecAK4PFchsMC_;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_;
  
  //===========================================================================
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_AbsoluteStat;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_AbsoluteScale;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_AbsoluteFlavMap;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_AbsoluteMPFBias;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_Fragmentation;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_SinglePionECAL;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_SinglePionHCAL;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_FlavorQCD;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_TimePtEta;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_RelativeJEREC1;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_RelativeJEREC2;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_RelativeJERHF;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_RelativePtBB;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_RelativePtEC1;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_RelativePtEC2;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_RelativePtHF;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_RelativeBal;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_RelativeFSR;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_RelativeStatFSR;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_RelativeStatEC;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_RelativeStatHF;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_PileUpDataMC;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_PileUpPtRef;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_PileUpPtBB;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_PileUpPtEC1;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_PileUpPtEC2;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_PileUpPtHF;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_PileUpMuZero;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_PileUpEnvelope;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_SubTotalPileUp;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_SubTotalRelative;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_SubTotalPt;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_SubTotalScale;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_SubTotalAbsolute;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_SubTotalMC;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_TotalNoFlavor;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_TotalNoFlavorNoTime;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_FlavorZJet;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_FlavorPhotonJet;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_FlavorPureGluon;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_FlavorPureQuark;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_FlavorPureCharm;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsMCUnc_FlavorPureBottom;
  //===========================================================================
  
  boost::shared_ptr<FactorizedJetCorrector>   jecAK4PFchsDATA_;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFchsDATAUnc_;
  

  boost::shared_ptr<FactorizedJetCorrector>   jecAK4PFPuppiMC_;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFPuppiMCUnc_;
  boost::shared_ptr<FactorizedJetCorrector>   jecAK4PFPuppiDATA_;
  boost::shared_ptr<JetCorrectionUncertainty> jecAK4PFPuppiDATAUnc_;
  
  // JER
  std::mt19937 m_random_generator;
   
  /////
  //   BSM variables
  /////
  ////slimmedJets
  //Kinematics
  vector<double> Jet_pt, Jet_eta, Jet_phi, Jet_energy, Jet_mass, Jet_px, Jet_py, Jet_pz, Jet_Uncorr_pt, Jet_L1corr_pt;
  //ID
  vector<double> Jet_pfCombinedInclusiveSecondaryVertexV2BJetTags, Jet_pfCombinedMVAV2BJetTags, Jet_pfJetProbabilityBJetTags, Jet_pfCombinedCvsLJetTags, Jet_pfCombinedCvsBJetTags, Jet_pileupId, Jet_isPFJet, Jet_isCaloJet;
  vector<double> Jet_pfDeepCSVProbudsg, Jet_pfDeepCSVProbb, Jet_pfDeepCSVProbc, Jet_pfDeepCSVProbbb, Jet_pfDeepCSVProbcc;
  vector<double> Jet_pfDeepCSVBJetTags;
  vector<double> Jet_pfDeepFlavourProbuds, Jet_pfDeepFlavourProbb, Jet_pfDeepFlavourProbc, Jet_pfDeepFlavourProbbb, Jet_pfDeepFlavourProblepb, Jet_pfDeepFlavourProbg;
  vector<double> Jet_pfDeepFlavourBJetTags;
  vector<double> Jet_qg, Jet_axis2, Jet_ptD, Jet_mult, Jet_axis1 ;  
  //Energy
  vector<double> Jet_neutralHadEnergyFraction, Jet_neutralEmEnergyFraction, Jet_chargedHadronEnergyFraction, Jet_chargedEmEnergyFraction, Jet_muonEnergyFraction, Jet_electronEnergy, Jet_photonEnergy, Jet_emEnergyFraction;
  //Other prop
  vector<double> Jet_numberOfConstituents, Jet_chargedMultiplicity, Jet_vtxMass, Jet_vtxNtracks, Jet_vtx3DVal, Jet_vtx3DSig;
  //Jet Energy Corrections and Uncertainties
  vector<double> Jet_JesSF, Jet_JesSFup, Jet_JesSFdown, Jet_JerSF, Jet_JerSFup, Jet_JerSFdown; 
  //MC 
  vector<double> Jet_gen_pt, Jet_gen_eta, Jet_gen_phi, Jet_gen_en;
  vector<double> Jet_partonFlavour, Jet_hadronFlavour;
  ////slimmedJetsPuppi
  //Kinematics
  vector<double> Jet_puppi_pt, Jet_puppi_eta, Jet_puppi_phi, Jet_puppi_energy, Jet_puppi_mass, Jet_puppi_px, Jet_puppi_py, Jet_puppi_pz, Jet_puppi_Uncorr_pt;
  //ID
  vector<double> Jet_puppi_pfCombinedInclusiveSecondaryVertexV2BJetTags, Jet_puppi_pfCombinedMVAV2BJetTags, Jet_puppi_pfJetProbabilityBJetTags, Jet_puppi_pfCombinedCvsLJetTags, Jet_puppi_pfCombinedCvsBJetTags, Jet_puppi_pileupId, Jet_puppi_isPFJet, Jet_puppi_isCaloJet;
  //Match Indices
  vector<int> Jet_ele_indices, Jet_ele_number;
  vector<int> Jet_mu_indices, Jet_mu_number;
  vector<int> Jet_tau_indices, Jet_tau_number;
  //Energy
  vector<double> Jet_puppi_neutralHadEnergyFraction, Jet_puppi_neutralEmEnergyFraction, Jet_puppi_chargedHadronEnergyFraction, Jet_puppi_chargedEmEnergyFraction, Jet_puppi_muonEnergyFraction, Jet_puppi_electronEnergy, Jet_puppi_photonEnergy, Jet_puppi_emEnergyFraction;
  //Other prop
  vector<double> Jet_puppi_numberOfConstituents, Jet_puppi_chargedMultiplicity, Jet_puppi_vtxMass, Jet_puppi_vtxNtracks, Jet_puppi_vtx3DVal, Jet_puppi_vtx3DSig;
  //Jet Energy Corrections and Uncertainties
  vector<double> Jet_puppi_JesSF, Jet_puppi_JesSFup, Jet_puppi_JesSFdown, Jet_puppi_JerSF, Jet_puppi_JerSFup, Jet_puppi_JerSFdown; 
  //Factorised
  vector<double> Jet_JesSF_AbsoluteStat_up;
  vector<double> Jet_JesSF_AbsoluteStat_down;
  vector<double> Jet_JesSF_AbsoluteScale_up;
  vector<double> Jet_JesSF_AbsoluteScale_down;
  vector<double> Jet_JesSF_AbsoluteFlavMap_up;
  vector<double> Jet_JesSF_AbsoluteFlavMap_down;
  vector<double> Jet_JesSF_AbsoluteMPFBias_up;
  vector<double> Jet_JesSF_AbsoluteMPFBias_down;
  vector<double> Jet_JesSF_Fragmentation_up;
  vector<double> Jet_JesSF_Fragmentation_down;
  vector<double> Jet_JesSF_SinglePionECAL_up;
  vector<double> Jet_JesSF_SinglePionECAL_down;
  vector<double> Jet_JesSF_SinglePionHCAL_up;
  vector<double> Jet_JesSF_SinglePionHCAL_down;
  vector<double> Jet_JesSF_FlavorQCD_up;
  vector<double> Jet_JesSF_FlavorQCD_down;
  vector<double> Jet_JesSF_TimePtEta_up;
  vector<double> Jet_JesSF_TimePtEta_down;
  vector<double> Jet_JesSF_RelativeJEREC1_up;
  vector<double> Jet_JesSF_RelativeJEREC1_down;
  vector<double> Jet_JesSF_RelativeJEREC2_up;
  vector<double> Jet_JesSF_RelativeJEREC2_down;
  vector<double> Jet_JesSF_RelativeJERHF_up;
  vector<double> Jet_JesSF_RelativeJERHF_down;
  vector<double> Jet_JesSF_RelativePtBB_up;
  vector<double> Jet_JesSF_RelativePtBB_down;
  vector<double> Jet_JesSF_RelativePtEC1_up;
  vector<double> Jet_JesSF_RelativePtEC1_down;
  vector<double> Jet_JesSF_RelativePtEC2_up;
  vector<double> Jet_JesSF_RelativePtEC2_down;
  vector<double> Jet_JesSF_RelativePtHF_up;
  vector<double> Jet_JesSF_RelativePtHF_down;
  vector<double> Jet_JesSF_RelativeBal_up;
  vector<double> Jet_JesSF_RelativeBal_down;
  vector<double> Jet_JesSF_RelativeFSR_up;
  vector<double> Jet_JesSF_RelativeFSR_down;
  vector<double> Jet_JesSF_RelativeStatFSR_up; //FIXME
  vector<double> Jet_JesSF_RelativeStatFSR_down;
  vector<double> Jet_JesSF_RelativeStatEC_up;
  vector<double> Jet_JesSF_RelativeStatEC_down;
  vector<double> Jet_JesSF_RelativeStatHF_up;
  vector<double> Jet_JesSF_RelativeStatHF_down;
  vector<double> Jet_JesSF_PileUpDataMC_up;
  vector<double> Jet_JesSF_PileUpDataMC_down;
  vector<double> Jet_JesSF_PileUpPtRef_up;
  vector<double> Jet_JesSF_PileUpPtRef_down;
  vector<double> Jet_JesSF_PileUpPtBB_up;
  vector<double> Jet_JesSF_PileUpPtBB_down;
  vector<double> Jet_JesSF_PileUpPtEC1_up;
  vector<double> Jet_JesSF_PileUpPtEC1_down;
  vector<double> Jet_JesSF_PileUpPtEC2_up;
  vector<double> Jet_JesSF_PileUpPtEC2_down;
  vector<double> Jet_JesSF_PileUpPtHF_up;
  vector<double> Jet_JesSF_PileUpPtHF_down;
  vector<double> Jet_JesSF_PileUpMuZero_up;
  vector<double> Jet_JesSF_PileUpMuZero_down;
  vector<double> Jet_JesSF_PileUpEnvelope_up;
  vector<double> Jet_JesSF_PileUpEnvelope_down;
  vector<double> Jet_JesSF_SubTotalPileUp_up;
  vector<double> Jet_JesSF_SubTotalPileUp_down;
  vector<double> Jet_JesSF_SubTotalRelative_up;
  vector<double> Jet_JesSF_SubTotalRelative_down;
  vector<double> Jet_JesSF_SubTotalPt_up;
  vector<double> Jet_JesSF_SubTotalPt_down;
  vector<double> Jet_JesSF_SubTotalScale_up;
  vector<double> Jet_JesSF_SubTotalScale_down;
  vector<double> Jet_JesSF_SubTotalAbsolute_up;
  vector<double> Jet_JesSF_SubTotalAbsolute_down;
  vector<double> Jet_JesSF_SubTotalMC_up;
  vector<double> Jet_JesSF_SubTotalMC_down;
  vector<double> Jet_JesSF_TotalNoFlavor_up;
  vector<double> Jet_JesSF_TotalNoFlavor_down;
  vector<double> Jet_JesSF_TotalNoFlavorNoTime_up;
  vector<double> Jet_JesSF_TotalNoFlavorNoTime_down;
  vector<double> Jet_JesSF_FlavorZJet_up;
  vector<double> Jet_JesSF_FlavorZJet_down;
  vector<double> Jet_JesSF_FlavorPhotonJet_up;
  vector<double> Jet_JesSF_FlavorPhotonJet_down;
  vector<double> Jet_JesSF_FlavorPureGluon_up;
  vector<double> Jet_JesSF_FlavorPureGluon_down;
  vector<double> Jet_JesSF_FlavorPureQuark_up;
  vector<double> Jet_JesSF_FlavorPureQuark_down;
  vector<double> Jet_JesSF_FlavorPureCharm_up;
  vector<double> Jet_JesSF_FlavorPureCharm_down;
  vector<double> Jet_JesSF_FlavorPureBottom_up;
  vector<double> Jet_JesSF_FlavorPureBottom_down;
  //MC 
  vector<double> Jet_puppi_partonFlavour, Jet_puppi_hadronFlavour;
};
#endif
