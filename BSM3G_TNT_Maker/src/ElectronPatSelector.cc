#include "BSMFramework/BSM3G_TNT_Maker/interface/ElectronPatSelector.h"
ElectronPatSelector::ElectronPatSelector(std::string name, TTree* tree, bool debug, const pset& iConfig, edm::ConsumesCollector && ic): 
  baseTree(name,tree,debug),
  triggerBits_(ic.consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("bits"))),
  triggerObjects_(ic.consumes<pat::TriggerObjectStandAloneCollection>(iConfig.getParameter<edm::InputTag>("objects"))),
  ebRecHitsToken_(ic.consumes<edm::SortedCollection<EcalRecHit,edm::StrictWeakOrdering<EcalRecHit>>>(iConfig.getParameter<edm::InputTag>("ebRecHits")))
{
  vtx_h_               = ic.consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"));
  beamSpot_            = ic.consumes<reco::BeamSpot>(iConfig.getParameter<edm::InputTag>("beamSpot"));
  electron_pat_        = ic.consumes<edm::View<pat::Electron> >(iConfig.getParameter<edm::InputTag>("patElectrons"));
  pfToken_             = ic.consumes<pat::PackedCandidateCollection>(edm::InputTag("packedPFCandidates"));
  jets_                = ic.consumes<pat::JetCollection >(iConfig.getParameter<edm::InputTag>("lepjets"));
  jetsToken            = ic.consumes<edm::View<pat::Jet>>(iConfig.getParameter<edm::InputTag>("jets"));
  qgToken              = ic.consumes<edm::ValueMap<float>>(edm::InputTag("QGTagger", "qgLikelihood"));
  _patElectron_pt_min  = iConfig.getParameter<double>("patElectron_pt_min");
  _patElectron_eta_max = iConfig.getParameter<double>("patElectron_eta_max");
  _vtx_ndof_min        = iConfig.getParameter<int>("vtx_ndof_min");
  _vtx_rho_max         = iConfig.getParameter<int>("vtx_rho_max");
  _vtx_position_z_max  = iConfig.getParameter<double>("vtx_position_z_max");
  _AJVar               = iConfig.getParameter<bool>("AJVar");
  _tthlepVar           = iConfig.getParameter<bool>("tthlepVar");
  _qglVar              = iConfig.getParameter<bool>("qglVar");
  _is_data             = iConfig.getParameter<bool>("is_data");
  _dataEra             = iConfig.getParameter<int>("dataEra");
  //if(_dataEra==2016){
  //    rhopogHandle_        = ic.consumes<double>(edm::InputTag("fixedGridRhoFastjetCentralNeutral"));
  //}else{
  rhopogHandle_        = ic.consumes<double>(edm::InputTag("fixedGridRhoFastjetAll"));
  //}
  SetBranches();
}
ElectronPatSelector::~ElectronPatSelector(){
  delete tree_;
}
KalmanVertexFitter vertexfitterele(true);
namespace{
  struct ByEta{
    bool operator()(const pat::PackedCandidate *c1, const pat::PackedCandidate *c2) const{
      return c1->eta()<c2->eta();
    }
    bool operator()(double c1eta, const pat::PackedCandidate *c2) const{
      return c1eta<c2->eta();
    }
    bool operator()(const pat::PackedCandidate *c1, double c2eta) const{
      return c1->eta()<c2eta;
    }
  };
}
void ElectronPatSelector::Fill(const edm::Event& iEvent, const edm::EventSetup& iSetup){
  Clear();
  /////
  //   Recall collections
  /////  
  edm::Handle<reco::VertexCollection> vtx_h;
  iEvent.getByToken(vtx_h_, vtx_h);
  reco::BeamSpot beamSpot;
  edm::Handle<reco::BeamSpot> beamSpotHandle;
  iEvent.getByToken(beamSpot_, beamSpotHandle);
  edm::Handle<edm::View<pat::Electron> > electron_pat;
  iEvent.getByToken(electron_pat_, electron_pat);
  edm::Handle<pat::PackedCandidateCollection> pcc;
  iEvent.getByToken(pfToken_, pcc);
  edm::Handle<pat::JetCollection> jets;
  iEvent.getByToken(jets_, jets);
  edm::Handle<pat::TriggerObjectStandAloneCollection> triggerObjects;
  iEvent.getByToken(triggerObjects_, triggerObjects);
  edm::Handle<edm::TriggerResults> triggerBits;
  iEvent.getByToken(triggerBits_, triggerBits);
  edm::Handle<double> rhopogHandle;
  iEvent.getByToken(rhopogHandle_,rhopogHandle);
  double rhopog = *rhopogHandle;
  edm::ESHandle<TransientTrackBuilder> ttrkbuilder;
  iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",ttrkbuilder);
  iEvent.getByToken(ebRecHitsToken_, _ebRecHits);
  /////
  //   Require a good vertex 
  /////
  //if(vtx_h->empty()) return; // skip the event if no PV found
  const reco::Vertex &firstGoodVertex = vtx_h->front();  
  //bool isgoodvtx = isGoodVertex(firstGoodVertex);
  //if(!isgoodvtx) return;
  /////
  //   Get electron information 
  /////
  for(edm::View<pat::Electron>::const_iterator el = electron_pat->begin(); el != electron_pat->end(); el++){
    //Acceptance
    if(el->pt() < _patElectron_pt_min)         continue;
    if(fabs(el->eta()) > _patElectron_eta_max) continue;  
    //Kinematics    
    patElectron_pt.push_back(el->pt());
    patElectron_eta.push_back(el->eta());
    patElectron_phi.push_back(el->phi());
    patElectron_energy.push_back(el->energy());
    patElectron_px.push_back(el->px());
    patElectron_py.push_back(el->py());
    patElectron_pz.push_back(el->pz());
    patElectron_p.push_back(el->p());
    patElectron_Et.push_back(el->caloEnergy()*sin(el->p4().theta()));
    double EleSCeta = el->superCluster()->position().eta();
    patElectron_SCeta.push_back(EleSCeta);
    float Escenergy = el->superCluster()->energy();
    patElectron_Esc.push_back(Escenergy);
    bool inCrack  = 1.4442<fabs(EleSCeta) && fabs(EleSCeta)<1.5660;
    patElectron_inCrack.push_back(inCrack);
    //Corrections
    if(el->energy()!=0) patElectron_energySF.push_back(el->userFloat("ecalTrkEnergyPostCorr")/el->energy());
    else patElectron_energySF.push_back(1.0);
    patElectron_ecalEnergyErrPostCorr.push_back(el->userFloat("ecalEnergyErrPostCorr"));
    patElectron_ecalEnergyErrPreCorr.push_back(el->userFloat("ecalEnergyErrPreCorr"));
    patElectron_ecalEnergyPostCorr.push_back(el->userFloat("ecalEnergyPostCorr"));
    patElectron_ecalEnergyPreCorr.push_back(el->userFloat("ecalEnergyPreCorr"));
    patElectron_ecalTrkEnergyErrPostCorr.push_back(el->userFloat("ecalTrkEnergyErrPostCorr"));
    patElectron_ecalTrkEnergyErrPreCorr.push_back(el->userFloat("ecalTrkEnergyErrPreCorr"));
    patElectron_ecalTrkEnergyPostCorr.push_back(el->userFloat("ecalTrkEnergyPostCorr"));
    patElectron_ecalTrkEnergyPreCorr.push_back(el->userFloat("ecalTrkEnergyPreCorr"));
    patElectron_energyScaleDown.push_back(el->userFloat("energyScaleDown"));
    patElectron_energyScaleGainDown.push_back(el->userFloat("energyScaleGainDown"));
    patElectron_energyScaleGainUp.push_back(el->userFloat("energyScaleGainUp"));
    patElectron_energyScaleStatDown.push_back(el->userFloat("energyScaleStatDown"));
    patElectron_energyScaleStatUp.push_back(el->userFloat("energyScaleStatUp"));
    patElectron_energyScaleSystDown.push_back(el->userFloat("energyScaleSystDown"));
    patElectron_energyScaleSystUp.push_back(el->userFloat("energyScaleSystUp"));
    patElectron_energyScaleUp.push_back(el->userFloat("energyScaleUp"));
    patElectron_energyScaleValue.push_back(el->userFloat("energyScaleValue"));
    patElectron_energySigmaDown.push_back(el->userFloat("energySigmaDown"));
    patElectron_energySigmaPhiDown.push_back(el->userFloat("energySigmaPhiDown"));
    patElectron_energySigmaPhiUp.push_back(el->userFloat("energySigmaPhiUp"));
    patElectron_energySigmaRhoDown.push_back(el->userFloat("energySigmaRhoDown"));
    patElectron_energySigmaRhoUp.push_back(el->userFloat("energySigmaRhoUp"));
    patElectron_energySigmaUp.push_back(el->userFloat("energySigmaUp"));
    patElectron_energySigmaValue.push_back(el->userFloat("energySigmaValue"));
    patElectron_energySmearNrSigma.push_back(el->userFloat("energySmearNrSigma"));
    //Charge
    patElectron_charge.push_back(el->charge());
    patElectron_isGsfCtfScPixChargeConsistent.push_back(el->isGsfCtfScPixChargeConsistent());
    patElectron_isGsfScPixChargeConsistent.push_back(el->isGsfScPixChargeConsistent());
    //ID
    float mvaval_nonIso  = el->userFloat("ElectronMVAEstimatorRun2Fall17NoIsoV2Values");
    passVetoId_.push_back  ( el->electronID("cutBasedElectronID-Fall17-94X-V2-veto"));
    passLooseId_.push_back ( el->electronID("cutBasedElectronID-Fall17-94X-V2-loose"));
    passMediumId_.push_back( el->electronID("cutBasedElectronID-Fall17-94X-V2-medium"));
    passTightId_.push_back ( el->electronID("cutBasedElectronID-Fall17-94X-V2-tight"));
    passMvaIsowp80Id_.push_back( el->electronID("mvaEleID-Fall17-iso-V2-wp80") );
    passMvanonIsowp80Id_.push_back( el->electronID("mvaEleID-Fall17-noIso-V2-wp80") );
    passMvaIsowp90Id_.push_back( el->electronID("mvaEleID-Fall17-iso-V2-wp90") );
    passMvanonIsowp90Id_.push_back( el->electronID("mvaEleID-Fall17-noIso-V2-wp90") );
    passMvaIsowpLooseId_.push_back( el->electronID("mvaEleID-Fall17-iso-V2-wpLoose") );
    passMvanonIsowpLooseId_.push_back( el->electronID("mvaEleID-Fall17-noIso-V2-wpLoose") );
    patElectron_mvaValue_nonIso_.push_back(el->userFloat("ElectronMVAEstimatorRun2Fall17NoIsoV2Values"));
    patElectron_mvaCategory_nonIso_.push_back(el->userInt("ElectronMVAEstimatorRun2Fall17NoIsoV2Categories"));
    patElectron_mvaValue_Iso_.push_back(el->userFloat("ElectronMVAEstimatorRun2Fall17IsoV2Values"));
    patElectron_mvaCategory_Iso_.push_back(el->userInt("ElectronMVAEstimatorRun2Fall17IsoV2Categories"));
    /*
    if(_dataEra==2016){
        passVetoOldId_.push_back  ( el->electronID("cutBasedElectronID-Summer16-80X-V1-veto"));
        passLooseOldId_.push_back ( el->electronID("cutBasedElectronID-Summer16-80X-V1-loose"));
        passMediumOldId_.push_back( el->electronID("cutBasedElectronID-Summer16-80X-V1-medium"));
        passTightOldId_.push_back ( el->electronID("cutBasedElectronID-Summer16-80X-V1-tight"));
        passMvaIsowp80OldId_.push_back( -999);
        passMvanonIsowp80OldId_.push_back(el->electronID("mvaEleID-Spring16-GeneralPurpose-V1-wp80"));
        passMvaIsowp90OldId_.push_back(-999);
        passMvanonIsowp90OldId_.push_back(el->electronID("mvaEleID-Spring16-GeneralPurpose-V1-wp90"));
        passMvaIsowpLooseOldId_.push_back( -999 );
        passMvanonIsowpLooseOldId_.push_back( -999 );
        patElectron_OldmvaValue_nonIso_.push_back(el->userFloat("ElectronMVAEstimatorRun2Spring16GeneralPurposeV1Values"));
        patElectron_OldmvaCategory_nonIso_.push_back(el->userInt("ElectronMVAEstimatorRun2Spring16GeneralPurposeV1Categories"));
        patElectron_OldmvaValue_Iso_.push_back(-999);
    }else{
        passVetoOldId_.push_back  ( el->electronID("cutBasedElectronID-Fall17-94X-V1-veto"));
        passLooseOldId_.push_back ( el->electronID("cutBasedElectronID-Fall17-94X-V1-loose"));
        passMediumOldId_.push_back( el->electronID("cutBasedElectronID-Fall17-94X-V1-medium"));
        passTightOldId_.push_back ( el->electronID("cutBasedElectronID-Fall17-94X-V1-tight"));
        passMvaIsowp80OldId_.push_back( el->electronID("mvaEleID-Fall17-iso-V1-wp80") );
        passMvanonIsowp80OldId_.push_back( el->electronID("mvaEleID-Fall17-noIso-V1-wp80") );
        passMvaIsowp90OldId_.push_back( el->electronID("mvaEleID-Fall17-iso-V1-wp90") );
        passMvanonIsowp90OldId_.push_back( el->electronID("mvaEleID-Fall17-noIso-V1-wp90") );
        passMvaIsowpLooseOldId_.push_back( el->electronID("mvaEleID-Fall17-iso-V1-wpLoose") );
        passMvanonIsowpLooseOldId_.push_back( el->electronID("mvaEleID-Fall17-noIso-V1-wpLoose") );
        patElectron_OldmvaValue_nonIso_.push_back(el->userFloat("ElectronMVAEstimatorRun2Fall17NoIsoV1Values"));
        patElectron_OldmvaCategory_nonIso_.push_back(el->userInt("ElectronMVAEstimatorRun2Fall17NoIsoV1Categories"));
        patElectron_OldmvaValue_Iso_.push_back(el->userFloat("ElectronMVAEstimatorRun2Fall17IsoV1Values"));
        patElectron_OldmvaCategory_Iso_.push_back(el->userInt("ElectronMVAEstimatorRun2Fall17IsoV1Categories"));
    }
    */
    passMvaHZZwpLooseId_.push_back( el->electronID("mvaEleID-Spring16-HZZ-V1-wpLoose") );
    patElectron_mvaValue_HZZ_.push_back(el->userFloat("ElectronMVAEstimatorRun2Spring16HZZV1Values"));
    patElectron_mvaCategory_HZZ_.push_back(el->userInt("ElectronMVAEstimatorRun2Spring16HZZV1Categories"));
    passHEEPId_.push_back  (el->electronID("heepElectronID-HEEPV70"));   
    patElectron_pdgId.push_back(el->pdgId());
    patElectron_isEcalDriven.push_back(el->ecalDriven());
    //Isolation
    //reco::GsfElectron::PflowIsolationVariables pfIso = el->pfIsolationVariables();
    //double SumChHadPt       = el->chargedHadronIsoR(0.4);
    double SumChHadPt       = el->pfIsolationVariables().sumChargedHadronPt;
    double SumNeuHadEt      = el->pfIsolationVariables().sumNeutralHadronEt;
    double SumPhotonEt      = el->pfIsolationVariables().sumPhotonEt; 
    double SumPU            = el->pfIsolationVariables().sumPUPt;
    patElectron_isoChargedHadrons.push_back( SumChHadPt );
    patElectron_isoNeutralHadrons.push_back( SumNeuHadEt );
    patElectron_isoPhotons.push_back( SumPhotonEt );
    patElectron_isoPU.push_back( SumPU );
    double SumNeutralCorrEt = std::max( 0.0, SumNeuHadEt+SumPhotonEt - 0.5*SumPU );
    double relIsoDeltaBeta = (SumChHadPt + SumNeutralCorrEt)/el->pt();
    patElectron_relIsoDeltaBeta.push_back(relIsoDeltaBeta);
    double EffArea = get_effarea(el->superCluster()->position().eta(), true);
    double SumChHadPt04       = el->chargedHadronIso();
    double SumNeuHadEt04      = el->neutralHadronIso();
    double SumPhotonEt04      = el->photonIso(); 
    //double SumPU04            = el->puChargedHadronIso();
    SumNeutralCorrEt = std::max( 0.0, SumNeuHadEt04+SumPhotonEt04 - rhopog*EffArea*16./9. );
    double relIsoRhoEA = (SumChHadPt04 + SumNeutralCorrEt)/el->pt();
    patElectron_relIsoRhoEA.push_back(relIsoRhoEA);
    patElectron_dr03EcalRecHitSumEt.push_back(el->dr03EcalRecHitSumEt());
    patElectron_dr03HcalDepth1TowerSumEt.push_back(el->dr03HcalDepth1TowerSumEt());
    patElectron_isolPtTracks.push_back(el->dr03TkSumPt());
    patElectron_ecalPFClusterIso.push_back(el->ecalPFClusterIso());
    patElectron_hcalPFClusterIso.push_back(el->hcalPFClusterIso());
    //Shape, Track related variables, other prop
    double dEtaIn = el->deltaEtaSuperClusterTrackAtVtx();


float dEtaInSeed =el->superCluster().isNonnull() && el->superCluster()->seed().isNonnull() ?  el->deltaEtaSuperClusterTrackAtVtx() - el->superCluster()->eta() + el->superCluster()->seed()->eta() : std::numeric_limits<float>::max();


    double dPhiIn = el->deltaPhiSuperClusterTrackAtVtx();
    double full5x5_sigmaIetaIeta = el->full5x5_sigmaIetaIeta();
    double hOverE = el->hcalOverEcal();
    double hOverE_new = el->hadronicOverEm();
    double ooEmooP = -999;
    if(el->ecalEnergy()==0)                   ooEmooP = 1e30;
    else if(!std::isfinite(el->ecalEnergy())) ooEmooP = 1e30;
    else                                      ooEmooP = 1.0/el->ecalEnergy() - el->eSuperClusterOverP()/el->ecalEnergy();
    patElectron_dEtaIn.push_back(dEtaIn);
    patElectron_dEtaInSeed.push_back(dEtaInSeed);
    patElectron_dPhiIn.push_back(dPhiIn);
    patElectron_full5x5_sigmaIetaIeta.push_back(full5x5_sigmaIetaIeta);
    patElectron_full5x5_e2x5Max.push_back(el->full5x5_e2x5Max());
    patElectron_full5x5_e5x5.push_back(el->full5x5_e5x5());
    patElectron_full5x5_e1x5.push_back(el->full5x5_e1x5());
    patElectron_hOverE.push_back(hOverE);
    patElectron_hOverE_new.push_back(hOverE_new);
    patElectron_ooEmooP.push_back(ooEmooP);
    passConversionVeto_.push_back(el->passConversionVeto());
    if(el->gsfTrack().isNonnull()){
      expectedMissingInnerHits.push_back(el->gsfTrack()->hitPattern().numberOfLostHits(reco::HitPattern::MISSING_INNER_HITS));
      patElectron_gsfTrack_normChi2.push_back(el->gsfTrack()->normalizedChi2());
      patElectron_gsfTrack_ndof.push_back(el->gsfTrack()->ndof());
    }else{
      expectedMissingInnerHits.push_back(-999);
      patElectron_gsfTrack_normChi2.push_back(-999);
      patElectron_gsfTrack_ndof.push_back(-999);
    }
    //IP
    if(el->gsfTrack().isNonnull()){
      patElectron_gsfTrack_dz_pv.push_back(el->gsfTrack()->dz(firstGoodVertex.position()));
      patElectron_gsfTrack_dxy_pv.push_back(el->gsfTrack()->dxy(firstGoodVertex.position()));
      patElectron_d0.push_back((-1) * el->gsfTrack()->dxy(firstGoodVertex.position()));
      patElectron_dzError.push_back(el->gsfTrack()->dzError());
      patElectron_dxyError.push_back(el->gsfTrack()->d0Error());
      patElectron_gsfTrack_vtx.push_back(el->gsfTrack()->vx());
      patElectron_gsfTrack_vty.push_back(el->gsfTrack()->vy());
      patElectron_gsfTrack_vtz.push_back(el->gsfTrack()->vz());
      if(_AJVar){ 
        if(beamSpotHandle.isValid() && el->closestCtfTrackRef().isNonnull()){//AJ vars (both pv and bs are in this if condition, tought for pv is not mandatory)
          beamSpot = *beamSpotHandle;
          math::XYZPoint point(beamSpot.x0(),beamSpot.y0(), beamSpot.z0());
          patElectron_gsfTrack_dz_bs.push_back(el->gsfTrack()->dz(point));
          patElectron_gsfTrack_dxy_bs.push_back(el->gsfTrack()->dxy(point));
          GlobalPoint thebs(beamSpot.x0(),beamSpot.y0(),beamSpot.z0()); 
          GlobalPoint thepv(firstGoodVertex.position().x(),firstGoodVertex.position().y(),firstGoodVertex.position().z());
          TrackRef eletr = el->closestCtfTrackRef(); 
          TransientTrack elecTransTkPtr = ttrkbuilder->build(eletr);
          GlobalPoint patElectron_pca_pv = elecTransTkPtr.trajectoryStateClosestToPoint(thepv).position();
          GlobalPoint patElectron_pca_bs = elecTransTkPtr.trajectoryStateClosestToPoint(thebs).position();
          patElectron_gsfTrack_PCAx_pv.push_back(patElectron_pca_pv.x());
          patElectron_gsfTrack_PCAy_pv.push_back(patElectron_pca_pv.y());
          patElectron_gsfTrack_PCAz_pv.push_back(patElectron_pca_pv.z());
          patElectron_gsfTrack_PCAx_bs.push_back(patElectron_pca_bs.x());
          patElectron_gsfTrack_PCAy_bs.push_back(patElectron_pca_bs.y());
          patElectron_gsfTrack_PCAz_bs.push_back(patElectron_pca_bs.z());
          const float elecMass = 0.000510998928;
          float elecSigma      = elecMass*1e-6;
          float chi2 = 0.0;
          float ndf  = 0.0;
          KinematicParticleFactoryFromTransientTrack pFactory;
          RefCountedKinematicParticle elecParticle = pFactory.particle(elecTransTkPtr, elecMass, chi2, ndf, elecSigma);
          patElectron_gsfTrackFitErrorMatrix_00.push_back(elecParticle->stateAtPoint(patElectron_pca_bs).kinematicParametersError().matrix()(0,0));
          patElectron_gsfTrackFitErrorMatrix_01.push_back(elecParticle->stateAtPoint(patElectron_pca_bs).kinematicParametersError().matrix()(0,1));
          patElectron_gsfTrackFitErrorMatrix_02.push_back(elecParticle->stateAtPoint(patElectron_pca_bs).kinematicParametersError().matrix()(0,2));
          patElectron_gsfTrackFitErrorMatrix_11.push_back(elecParticle->stateAtPoint(patElectron_pca_bs).kinematicParametersError().matrix()(1,1));
          patElectron_gsfTrackFitErrorMatrix_12.push_back(elecParticle->stateAtPoint(patElectron_pca_bs).kinematicParametersError().matrix()(1,2));
          patElectron_gsfTrackFitErrorMatrix_22.push_back(elecParticle->stateAtPoint(patElectron_pca_bs).kinematicParametersError().matrix()(2,2));
        }else{
          patElectron_gsfTrack_dz_bs.push_back(-998);
          patElectron_gsfTrack_dxy_bs.push_back(-998);
          patElectron_gsfTrack_PCAx_pv.push_back(-998);
          patElectron_gsfTrack_PCAy_pv.push_back(-998);
          patElectron_gsfTrack_PCAz_pv.push_back(-998);
          patElectron_gsfTrack_PCAx_bs.push_back(-998);
          patElectron_gsfTrack_PCAy_bs.push_back(-998);
          patElectron_gsfTrack_PCAz_bs.push_back(-998);
          patElectron_gsfTrackFitErrorMatrix_00.push_back(-998);
          patElectron_gsfTrackFitErrorMatrix_01.push_back(-998);
          patElectron_gsfTrackFitErrorMatrix_02.push_back(-998);
          patElectron_gsfTrackFitErrorMatrix_11.push_back(-998);
          patElectron_gsfTrackFitErrorMatrix_12.push_back(-998);
          patElectron_gsfTrackFitErrorMatrix_22.push_back(-998);
        }
      } 
    }else{
      patElectron_gsfTrack_dz_pv.push_back(-999);
      patElectron_gsfTrack_dxy_pv.push_back(-999);
      patElectron_d0.push_back(-999);
      patElectron_dzError.push_back(-999);
      patElectron_dxyError.push_back(-999);
      patElectron_gsfTrack_vtx.push_back(-999);
      patElectron_gsfTrack_vty.push_back(-999);
      patElectron_gsfTrack_vtz.push_back(-999);
      if(_AJVar){
        patElectron_gsfTrack_dz_bs.push_back(-999);
        patElectron_gsfTrack_dxy_bs.push_back(-999);
        patElectron_gsfTrack_PCAx_pv.push_back(-999);
        patElectron_gsfTrack_PCAy_pv.push_back(-999);
        patElectron_gsfTrack_PCAz_pv.push_back(-999);
        patElectron_gsfTrack_PCAx_bs.push_back(-999);
        patElectron_gsfTrack_PCAy_bs.push_back(-999);
        patElectron_gsfTrack_PCAz_bs.push_back(-999);
        patElectron_gsfTrackFitErrorMatrix_00.push_back(-999);
        patElectron_gsfTrackFitErrorMatrix_01.push_back(-999);
        patElectron_gsfTrackFitErrorMatrix_02.push_back(-999);
        patElectron_gsfTrackFitErrorMatrix_11.push_back(-999);
        patElectron_gsfTrackFitErrorMatrix_12.push_back(-999);
        patElectron_gsfTrackFitErrorMatrix_22.push_back(-999);
      }
    }
    /////
    //   TTH variables
    ///// 
    if(_tthlepVar){
      double miniIso      = 999;
      double miniIsoCh    = 999;
      double miniIsoNeu   = 999;
      double miniIsoPUsub = 999;
      //get_eleminiIso_info(*pcc,rhopog,*el,miniIso,miniIsoCh,miniIsoNeu,miniIsoPUsub);
      get_eleminiIso(rhopog,*el,miniIso,miniIsoCh,miniIsoNeu,miniIsoPUsub);
      patElectron_miniIsoRel.push_back(miniIso/el->pt());
      patElectron_miniIsoCh.push_back(miniIsoCh);
      patElectron_miniIsoNeu.push_back(miniIsoNeu);
      patElectron_miniIsoPUsub.push_back(miniIsoPUsub);
      double elejet_mindr    = 999;
      double elejet_l1corr    = 1;
      double elejetislep    = 0;
      double elejet_pt       = -1;
      double eleptOVelejetpt = -1;
      double eleptOVelejetptV2 = 1;
      double elejet_pfCombinedInclusiveSecondaryVertexV2BJetTags = -1;
      double elejet_pfDeepCSVBJetTags = -1;
      double elejet_pfDeepFlavourBJetTags = -1;
      double elejet_pfJetProbabilityBJetTag = -1;
      double elejet_pfCombinedMVABJetTags = -1;
      double elejet_qgl = -3;
      double elejetx  = -999;
      double elejety  = -999;
      double elejetz  = -999;
      double eleptrel = -999;
      int lepjetidx = -1;
      get_elejet_info(el,iEvent,iSetup, elejet_l1corr, elejetislep,
                      elejet_mindr,elejet_pt,eleptOVelejetpt,
                      elejet_pfCombinedInclusiveSecondaryVertexV2BJetTags,elejet_pfDeepCSVBJetTags,elejet_pfDeepFlavourBJetTags,
                      elejet_pfJetProbabilityBJetTag,elejet_pfCombinedMVABJetTags,elejet_qgl,
                      elejetx,elejety,elejetz,eleptrel,lepjetidx);
      patElectron_jetdr.push_back(elejet_mindr);
      patElectron_jetl1corr.push_back(elejet_l1corr);
      patElectron_jetislep.push_back(elejetislep);
      patElectron_jetidx.push_back(lepjetidx);
      if(elejet_pt<0){
          elejet_pt = 0;
          eleptOVelejetpt=1;
          eleptOVelejetptV2 = 1/(1 + relIsoRhoEA);
          eleptrel = 0;
      }
      patElectron_jetpt.push_back(elejet_pt);
      patElectron_jetptratio.push_back(eleptOVelejetpt);
      patElectron_jetptratioV2.push_back(eleptOVelejetptV2);
      patElectron_jetcsv.push_back(elejet_pfCombinedInclusiveSecondaryVertexV2BJetTags);
      patElectron_jetdeepcsv.push_back(elejet_pfDeepCSVBJetTags);
      patElectron_jetdeepflavour.push_back(elejet_pfDeepFlavourBJetTags);
      patElectron_ptrel.push_back(eleptrel);
      patElectron_elejet_pfJetProbabilityBJetTag.push_back(elejet_pfJetProbabilityBJetTag);
      patElectron_elejet_pfCombinedMVABJetTags.push_back(elejet_pfCombinedMVABJetTags);
      patElectron_elejet_qgl.push_back(elejet_qgl);
      patElectron_IP3Dsig.push_back(fabs(el->dB(pat::Electron::PV3D))/el->edB(pat::Electron::PV3D));
      /*if(el->hasUserFloat("ElectronMVAEstimatorRun2Spring15NonTrig25nsV1Values")) patElectron_eleMVASpring15NonTrig25ns.push_back(el->userFloat("ElectronMVAEstimatorRun2Spring15NonTrig25nsV1Values"));   
	else*/                                                                        patElectron_eleMVASpring15NonTrig25ns.push_back(-999); 
      bool nontrigelemva_vl = false;
      double ntelemva = mvaval_nonIso; 
      double eleta    = fabs(el->superCluster()->position().eta());
      if((eleta < 0.8                   && ntelemva > -0.70) ||
         (0.8 <= eleta && eleta < 1.479 && ntelemva > -0.83) || 
         (1.479 <= eleta && eleta < 500 && ntelemva > -0.92)         
        ){
        nontrigelemva_vl = true;
      }
      patElectron_eleMVASpring15NonTrig25ns_VL.push_back(nontrigelemva_vl);
      int pvass = pvassociation(el,*pcc);
      patElectron_pvass.push_back(pvass);
      const math::XYZVector& lepton_momentum = el->momentum();
      const math::XYZVector axis(elejetx,elejety,elejetz);
      double etarel = relativeEta(lepton_momentum,axis);
      patElectron_etarel.push_back(etarel);
      patElectron_ptOVen.push_back(el->pt()/el->energy());
      //Mass
      patElectron_elemass.push_back(el->p4().M());
      double elewmass    = get_lepWmass(el,iEvent,lepjetidx);
      double eletopmass  = get_lepTopmass(el,iEvent,lepjetidx);
      double elewtopmass = get_lepWTopmass(el,iEvent,lepjetidx);
      if(lepjetidx!=-1){
        const pat::Jet & lepjet = (*jets)[lepjetidx];
        patElectron_elejet_mass.push_back(lepjet.p4().M());
      }else{
        patElectron_elejet_mass.push_back(0);
      }
      patElectron_elejet_Wmass.push_back(elewmass);
      patElectron_elejet_Topmass.push_back(eletopmass);
      patElectron_elejet_WTopmass.push_back(elewtopmass);
      //Ele IP 
      GlobalVector elejetgv(elejetx,elejety,elejetz);
      double IP3D_val  = -9999;
      double IP3D_err  = -9999;
      double IP3D_sig  = -9999;
      double sIP3D_val = -9999;
      double sIP3D_err = -9999;
      double sIP3D_sig = -9999;
      double IP2D_val  = -9999;
      double IP2D_err  = -9999;
      double IP2D_sig  = -9999;
      double sIP2D_val = -9999;
      double sIP2D_err = -9999;
      double sIP2D_sig = -9999;
      double IP1D_val  = -9999;
      double IP1D_err  = -9999;
      double IP1D_sig  = -9999;
      double sIP1D_val = -9999;
      double sIP1D_err = -9999;
      double sIP1D_sig = -9999;
      if(el->gsfTrack().isNonnull()){
        GsfTrackRef elegsft = el->gsfTrack();
        TransientTrack elettrk = ttrkbuilder->build(elegsft);
        IP3D2D(elettrk,firstGoodVertex,elejetgv,IP3D_val,IP3D_err,IP3D_sig,sIP3D_val,sIP3D_err,sIP3D_sig,IP2D_val,IP2D_err,IP2D_sig,sIP2D_val,sIP2D_err,sIP2D_sig);
        zIP1D(elettrk,firstGoodVertex,elejetgv,IP1D_val,IP1D_err,IP1D_sig,sIP1D_val,sIP1D_err,sIP1D_sig);
      }
      //Max Lep jet IP (the maximum IP for a tracks of the lepton jet)
      double lepjetMaxIP3D_val  = IP3D_val; 
      double lepjetMaxIP3D_sig  = IP3D_sig; 
      double lepjetMaxsIP3D_val = sIP3D_val; 
      double lepjetMaxsIP3D_sig = sIP3D_sig; 
      double lepjetMaxIP2D_val  = IP2D_val; 
      double lepjetMaxIP2D_sig  = IP2D_sig; 
      double lepjetMaxsIP2D_val = sIP2D_val; 
      double lepjetMaxsIP2D_sig = sIP2D_sig; 
      double lepjetMaxIP1D_val  = IP1D_val; 
      double lepjetMaxIP1D_sig  = IP1D_sig; 
      double lepjetMaxsIP1D_val = sIP1D_val; 
      double lepjetMaxsIP1D_sig = sIP1D_sig; 
      //Av Lep jet IP (the average IP of the lepton jet tracks)   
      double lepjetAvIP3D_val  = 0; double denlepjetAvIP3D_val  = 0;
      if(IP3D_val!=-9999){lepjetAvIP3D_val   = IP3D_val; denlepjetAvIP3D_val   = 1;} 
      double lepjetAvIP3D_sig  = 0; double denlepjetAvIP3D_sig  = 0;
      if(IP3D_sig!=-9999){lepjetAvIP3D_sig   = IP3D_sig; denlepjetAvIP3D_sig   = 1;}
      double lepjetAvsIP3D_val = 0; double denlepjetAvsIP3D_val = 0;
      if(sIP3D_val!=-9999){lepjetAvsIP3D_val = sIP3D_val; denlepjetAvsIP3D_val = 1;} 
      double lepjetAvsIP3D_sig = 0; double denlepjetAvsIP3D_sig = 0;
      if(sIP3D_sig!=-9999){lepjetAvsIP3D_sig = sIP3D_sig; denlepjetAvsIP3D_sig = 1;} 
      double lepjetAvIP2D_val  = 0; double denlepjetAvIP2D_val  = 0;
      if(IP2D_val!=-9999){lepjetAvIP2D_val   = IP2D_val; denlepjetAvIP2D_val   = 1;} 
      double lepjetAvIP2D_sig  = 0; double denlepjetAvIP2D_sig  = 0;
      if(IP2D_sig!=-9999){lepjetAvIP2D_sig   = IP2D_sig; denlepjetAvIP2D_sig   = 1;} 
      double lepjetAvsIP2D_val = 0; double denlepjetAvsIP2D_val = 0;
      if(sIP2D_val!=-9999){lepjetAvsIP2D_val = sIP2D_val; denlepjetAvsIP2D_val = 1;} 
      double lepjetAvsIP2D_sig = 0; double denlepjetAvsIP2D_sig = 0;
      if(sIP2D_sig!=-9999){lepjetAvsIP2D_sig = sIP2D_sig; denlepjetAvsIP2D_sig = 1;} 
      double lepjetAvIP1D_val  = 0; double denlepjetAvIP1D_val  = 0;
      if(IP1D_val!=-9999){lepjetAvIP1D_val   = IP1D_val; denlepjetAvIP1D_val   = 1;} 
      double lepjetAvIP1D_sig  = 0; double denlepjetAvIP1D_sig  = 0;
      if(IP1D_sig!=-9999){lepjetAvIP1D_sig   = IP1D_sig; denlepjetAvIP1D_sig   = 1;} 
      double lepjetAvsIP1D_val = 0; double denlepjetAvsIP1D_val = 0;
      if(sIP1D_val!=-9999){lepjetAvsIP1D_val = sIP1D_val; denlepjetAvsIP1D_val = 1;} 
      double lepjetAvsIP1D_sig = 0; double denlepjetAvsIP1D_sig = 0;
      if(sIP1D_sig!=-9999){lepjetAvsIP1D_sig = sIP1D_sig; denlepjetAvsIP1D_sig = 1;} 
      //Get values of Max and Av IP
      if(lepjetidx!=-1){
        const pat::Jet & lepjet = (*jets)[lepjetidx]; 
        lepjetIP(lepjet,firstGoodVertex,elejetgv,*ttrkbuilder,
                 lepjetMaxIP3D_val, lepjetMaxIP3D_sig, lepjetMaxsIP3D_val, lepjetMaxsIP3D_sig, lepjetMaxIP2D_val, lepjetMaxIP2D_sig, lepjetMaxsIP2D_val, lepjetMaxsIP2D_sig, lepjetMaxIP1D_val, lepjetMaxIP1D_sig, lepjetMaxsIP1D_val, lepjetMaxsIP1D_sig,
                 lepjetAvIP3D_val, lepjetAvIP3D_sig, lepjetAvsIP3D_val, lepjetAvsIP3D_sig, lepjetAvIP2D_val, lepjetAvIP2D_sig, lepjetAvsIP2D_val, lepjetAvsIP2D_sig, lepjetAvIP1D_val, lepjetAvIP1D_sig, lepjetAvsIP1D_val, lepjetAvsIP1D_sig,
                 denlepjetAvIP3D_val, denlepjetAvIP3D_sig, denlepjetAvsIP3D_val, denlepjetAvsIP3D_sig, denlepjetAvIP2D_val, denlepjetAvIP2D_sig, denlepjetAvsIP2D_val, denlepjetAvsIP2D_sig, denlepjetAvIP1D_val, denlepjetAvIP1D_sig, denlepjetAvsIP1D_val, denlepjetAvsIP1D_sig,
                 IP3D_val   
                );                                                                    
      }
      patElectron_IP3D_val.push_back(IP3D_val);
      patElectron_IP3D_err.push_back(IP3D_err);
      patElectron_IP3D_sig.push_back(IP3D_sig);
      patElectron_sIP3D_val.push_back(sIP3D_val);
      patElectron_sIP3D_err.push_back(sIP3D_err);
      patElectron_sIP3D_sig.push_back(sIP3D_sig);
      patElectron_IP2D_val.push_back(IP2D_val);
      patElectron_IP2D_err.push_back(IP2D_err);
      patElectron_IP2D_sig.push_back(IP2D_sig);
      patElectron_sIP2D_val.push_back(sIP2D_val);
      patElectron_sIP2D_err.push_back(sIP2D_err);
      patElectron_sIP2D_sig.push_back(sIP2D_sig);
      patElectron_IP1D_val.push_back(IP1D_val);
      patElectron_IP1D_err.push_back(IP1D_err);
      patElectron_IP1D_sig.push_back(IP1D_sig);
      patElectron_sIP1D_val.push_back(sIP1D_val);
      patElectron_sIP1D_err.push_back(sIP1D_err);
      patElectron_sIP1D_sig.push_back(sIP1D_sig);
      patElectron_lepjetMaxIP3D_val.push_back(lepjetMaxIP3D_val);
      patElectron_lepjetMaxIP3D_sig.push_back(lepjetMaxIP3D_sig);
      patElectron_lepjetMaxsIP3D_val.push_back(lepjetMaxsIP3D_val);
      patElectron_lepjetMaxsIP3D_sig.push_back(lepjetMaxsIP3D_sig);
      patElectron_lepjetMaxIP2D_val.push_back(lepjetMaxIP2D_val);
      patElectron_lepjetMaxIP2D_sig.push_back(lepjetMaxIP2D_sig);
      patElectron_lepjetMaxsIP2D_val.push_back(lepjetMaxsIP2D_val);
      patElectron_lepjetMaxsIP2D_sig.push_back(lepjetMaxsIP2D_sig);
      patElectron_lepjetMaxIP1D_val.push_back(lepjetMaxIP1D_val);
      patElectron_lepjetMaxIP1D_sig.push_back(lepjetMaxIP1D_sig);
      patElectron_lepjetMaxsIP1D_val.push_back(lepjetMaxsIP1D_val);
      patElectron_lepjetMaxsIP1D_sig.push_back(lepjetMaxsIP1D_sig);
      patElectron_lepjetAvIP3D_val.push_back(denlepjetAvIP3D_val!=0   ? lepjetAvIP3D_val/denlepjetAvIP3D_val   : IP3D_val);
      patElectron_lepjetAvIP3D_sig.push_back(denlepjetAvIP3D_sig!=0   ? lepjetAvIP3D_sig/denlepjetAvIP3D_sig   : IP3D_sig);
      patElectron_lepjetAvsIP3D_val.push_back(denlepjetAvsIP3D_val!=0 ? lepjetAvsIP3D_val/denlepjetAvsIP3D_val : sIP3D_val);
      patElectron_lepjetAvsIP3D_sig.push_back(denlepjetAvsIP3D_sig!=0 ? lepjetAvsIP3D_sig/denlepjetAvsIP3D_sig : sIP3D_sig);
      patElectron_lepjetAvIP2D_val.push_back(denlepjetAvIP2D_val!=0   ? lepjetAvIP2D_val/denlepjetAvIP2D_val   : IP2D_val);
      patElectron_lepjetAvIP2D_sig.push_back(denlepjetAvIP2D_sig!=0   ? lepjetAvIP2D_sig/denlepjetAvIP2D_sig   : IP2D_sig);
      patElectron_lepjetAvsIP2D_val.push_back(denlepjetAvsIP2D_val!=0 ? lepjetAvsIP2D_val/denlepjetAvsIP2D_val : sIP2D_val);
      patElectron_lepjetAvsIP2D_sig.push_back(denlepjetAvsIP2D_sig!=0 ? lepjetAvsIP2D_sig/denlepjetAvsIP2D_sig : sIP2D_sig);
      patElectron_lepjetAvIP1D_val.push_back(denlepjetAvIP1D_val!=0   ? lepjetAvIP1D_val/denlepjetAvIP1D_val   : IP1D_val);
      patElectron_lepjetAvIP1D_sig.push_back(denlepjetAvIP1D_sig!=0   ? lepjetAvIP1D_sig/denlepjetAvIP1D_sig   : IP1D_sig);
      patElectron_lepjetAvsIP1D_val.push_back(denlepjetAvsIP1D_val!=0 ? lepjetAvsIP1D_val/denlepjetAvsIP1D_val : sIP1D_val);
      patElectron_lepjetAvsIP1D_sig.push_back(denlepjetAvsIP1D_sig!=0 ? lepjetAvsIP1D_sig/denlepjetAvsIP1D_sig : sIP1D_sig);
      //Lep jet trks
      double lepjetchtrks      = 0;
      double lepjetpvchtrks    = 0;
      double lepjetnonpvchtrks = 0;
      double lepjetndaus       = 0;
      if(lepjetidx!=-1){
        const pat::Jet & lepjet = (*jets)[lepjetidx];
        lepjetTrks(el, lepjet, firstGoodVertex, lepjetchtrks, lepjetpvchtrks, lepjetnonpvchtrks, lepjetndaus);
      }
      patElectron_lepjetchtrks.push_back(lepjetchtrks);
      patElectron_lepjetpvchtrks.push_back(lepjetpvchtrks);
      patElectron_lepjetnonpvchtrks.push_back(lepjetnonpvchtrks);
      patElectron_lepjetndaus.push_back(lepjetndaus);
      //Lep jet vtx compatibility
      double lepjetpvchi2    = 0;
      double lepjetnumno2trk = 0;
      if(lepjetidx!=-1){
        const pat::Jet & lepjet = (*jets)[lepjetidx];
        lepjetVtxCompatibility(lepjet, firstGoodVertex, *ttrkbuilder, lepjetpvchi2, lepjetnumno2trk);
      }
      patElectron_lepjetpvchi2.push_back(lepjetpvchi2);
      patElectron_lepjetnumno2trk.push_back(lepjetnumno2trk);
    }//End if TTHLep
    /////
    //   MC info
    /////
    if(!_is_data){
      const reco::GenParticle * genpart = el->genParticle();
      if(genpart){
        patElectron_gen_pt.push_back(genpart->pt());
        patElectron_gen_eta.push_back(genpart->eta());
        patElectron_gen_phi.push_back(genpart->phi());
        patElectron_gen_en.push_back(genpart->energy());
        patElectron_gen_pdgId.push_back(genpart->pdgId());
        patElectron_gen_isPromptFinalState.push_back(genpart->isPromptFinalState());
        patElectron_gen_isDirectPromptTauDecayProductFinalState.push_back(genpart->isDirectPromptTauDecayProductFinalState());
      }else{
        patElectron_gen_pt.push_back(-999);
        patElectron_gen_eta.push_back(-999);
        patElectron_gen_phi.push_back(-999);
        patElectron_gen_en.push_back(-999);
        patElectron_gen_pdgId.push_back(-999);
        patElectron_gen_isPromptFinalState.push_back(-999);
        patElectron_gen_isDirectPromptTauDecayProductFinalState.push_back(-999);
      }
    }
    //TRIGGER MATCHING - for now only HLT_Ele115_CaloIdVT_GsfTrkIdT_v* is implemented
    int isMatchedToTrigger = 0;
    //if(!_is_data) isMatchedToTrigger = MatchingToTrigger(iEvent, triggerObjects, triggerBits, el->eta(), el->phi());
    patElectron_isMatchedToTrigger.push_back(isMatchedToTrigger);
  }
}
void ElectronPatSelector::SetBranches(){
  if(debug_) std::cout<<"setting branches: calling AddBranch of baseTree"<<std::endl;
  //Kinematics     
  AddBranch(&patElectron_pt           ,"patElectron_pt");
  AddBranch(&patElectron_eta          ,"patElectron_eta");
  AddBranch(&patElectron_phi          ,"patElectron_phi");
  AddBranch(&patElectron_energy       ,"patElectron_energy");
  AddBranch(&patElectron_px           ,"patElectron_px");
  AddBranch(&patElectron_py           ,"patElectron_py");
  AddBranch(&patElectron_pz           ,"patElectron_pz");
  AddBranch(&patElectron_p            ,"patElectron_p");
  AddBranch(&patElectron_Et           ,"patElectron_Et");
  AddBranch(&patElectron_SCeta        ,"patElectron_SCeta");
  AddBranch(&patElectron_Esc        ,"patElectron_Esc");
  AddBranch(&patElectron_inCrack      ,"patElectron_inCrack");
  //Corrections
  AddBranch(&patElectron_energySF                , "patElectron_energySF");
  AddBranch(&patElectron_ecalEnergyErrPostCorr   , "patElectron_ecalEnergyErrPostCorr");
  AddBranch(&patElectron_ecalEnergyErrPreCorr    , "patElectron_ecalEnergyErrPreCorr");
  AddBranch(&patElectron_ecalEnergyPostCorr      , "patElectron_ecalEnergyPostCorr");
  AddBranch(&patElectron_ecalEnergyPreCorr       , "patElectron_ecalEnergyPreCorr");
  AddBranch(&patElectron_ecalTrkEnergyErrPostCorr, "patElectron_ecalTrkEnergyErrPostCorr");
  AddBranch(&patElectron_ecalTrkEnergyErrPreCorr , "patElectron_ecalTrkEnergyErrPreCorr");
  AddBranch(&patElectron_ecalTrkEnergyPostCorr   , "patElectron_ecalTrkEnergyPostCorr");
  AddBranch(&patElectron_ecalTrkEnergyPreCorr    , "patElectron_ecalTrkEnergyPreCorr");
  AddBranch(&patElectron_energyScaleDown         , "patElectron_energyScaleDown");
  AddBranch(&patElectron_energyScaleGainDown     , "patElectron_energyScaleGainDown");
  AddBranch(&patElectron_energyScaleGainUp       , "patElectron_energyScaleGainUp");
  AddBranch(&patElectron_energyScaleStatDown     , "patElectron_energyScaleStatDown");
  AddBranch(&patElectron_energyScaleStatUp       , "patElectron_energyScaleStatUp");
  AddBranch(&patElectron_energyScaleSystDown     , "patElectron_energyScaleSystDown");
  AddBranch(&patElectron_energyScaleSystUp       , "patElectron_energyScaleSystUp");
  AddBranch(&patElectron_energyScaleUp           , "patElectron_energyScaleUp");
  AddBranch(&patElectron_energyScaleValue        , "patElectron_energyScaleValue");
  AddBranch(&patElectron_energySigmaDown         , "patElectron_energySigmaDown");
  AddBranch(&patElectron_energySigmaPhiDown      , "patElectron_energySigmaPhiDown");
  AddBranch(&patElectron_energySigmaPhiUp        , "patElectron_energySigmaPhiUp");
  AddBranch(&patElectron_energySigmaRhoDown      , "patElectron_energySigmaRhoDown");
  AddBranch(&patElectron_energySigmaRhoUp        , "patElectron_energySigmaRhoUp");
  AddBranch(&patElectron_energySigmaUp           , "patElectron_energySigmaUp");
  AddBranch(&patElectron_energySigmaValue        , "patElectron_energySigmaValue");
  AddBranch(&patElectron_energySmearNrSigma      , "patElectron_energySmearNrSigma");
  //Charge
  AddBranch(&patElectron_charge       ,"patElectron_charge");
  AddBranch(&patElectron_isGsfCtfScPixChargeConsistent            ,"patElectron_isGsfCtfScPixChargeConsistent");
  AddBranch(&patElectron_isGsfScPixChargeConsistent               ,"patElectron_isGsfScPixChargeConsistent");
  //ID
  AddBranch(&passVetoId_              ,"patElectron_isPassVeto");          
  AddBranch(&passLooseId_             ,"patElectron_isPassLoose");
  AddBranch(&passMediumId_            ,"patElectron_isPassMedium");
  AddBranch(&passTightId_             ,"patElectron_isPassTight");
  AddBranch(&passMvaIsowp80Id_           ,"patElectron_isPassMvaIsowp80");
  AddBranch(&passMvanonIsowp80Id_        ,"patElectron_isPassMvanonIsowp80");
  AddBranch(&passMvaIsowp90Id_       ,"patElectron_isPassMvaIsowp90");
  AddBranch(&passMvanonIsowp90Id_    ,"patElectron_isPassMvanonIsowp90");
  AddBranch(&passMvaIsowpLooseId_       ,"patElectron_isPassMvaIsowpLoose");
  AddBranch(&passMvanonIsowpLooseId_    ,"patElectron_isPassMvanonIsowpLoose");
  AddBranch(&patElectron_mvaValue_nonIso_    ,"patElectron_mvaValue_nonIso");
  AddBranch(&patElectron_mvaCategory_nonIso_ ,"patElectron_mvaCategory_nonIso");
  AddBranch(&patElectron_mvaValue_Iso_    ,"patElectron_mvaValue_Iso");
  AddBranch(&patElectron_mvaCategory_Iso_ ,"patElectron_mvaCategory_Iso");
  /*
  AddBranch(&passVetoOldId_              ,"patElectron_isPassOldVeto");          
  AddBranch(&passLooseOldId_             ,"patElectron_isPassOldLoose");
  AddBranch(&passMediumOldId_            ,"patElectron_isPassOldMedium");
  AddBranch(&passTightOldId_             ,"patElectron_isPassOldTight");
  AddBranch(&passMvaIsowp80OldId_           ,"patElectron_isPassOldMvaIsowp80");
  AddBranch(&passMvanonIsowp80OldId_        ,"patElectron_isPassOldMvanonIsowp80");
  AddBranch(&passMvaIsowp90OldId_       ,"patElectron_isPassOldMvaIsowp90");
  AddBranch(&passMvanonIsowp90OldId_    ,"patElectron_isPassOldMvanonIsowp90");
  AddBranch(&passMvaIsowpLooseOldId_       ,"patElectron_isPassOldMvaIsowpLoose");
  AddBranch(&passMvanonIsowpLooseOldId_    ,"patElectron_isPassOldMvanonIsowpLoose");
  AddBranch(&patElectron_OldmvaValue_nonIso_    ,"patElectron_OldmvaValue_nonIso");
  AddBranch(&patElectron_OldmvaCategory_nonIso_ ,"patElectron_OldmvaCategory_nonIso");
  AddBranch(&patElectron_OldmvaValue_Iso_    ,"patElectron_OldmvaValue_Iso");
  AddBranch(&patElectron_OldmvaCategory_Iso_ ,"patElectron_OldmvaCategory_Iso");
  */
  AddBranch(&passHEEPId_              ,"patElectron_isPassHEEPId");
  AddBranch(&patElectron_pdgId        ,"patElectron_pdgId");
  AddBranch(&patElectron_isEcalDriven ,"patElectron_isEcalDriven");
  AddBranch(&passMvaHZZwpLooseId_       ,"patElectron_isPassMvaHZZwpLoose");
  AddBranch(&patElectron_mvaValue_HZZ_    ,"patElectron_mvaValue_HZZ");
  AddBranch(&patElectron_mvaCategory_HZZ_ ,"patElectron_mvaCategory_HZZ");
  //Isolation
  AddBranch(&patElectron_isoChargedHadrons        ,"patElectron_isoChargedHadrons");
  AddBranch(&patElectron_isoNeutralHadrons        ,"patElectron_isoNeutralHadrons");
  AddBranch(&patElectron_isoPhotons               ,"patElectron_isoPhotons");
  AddBranch(&patElectron_isoPU                    ,"patElectron_isoPU");
  AddBranch(&patElectron_relIsoDeltaBeta          ,"patElectron_relIsoDeltaBeta");
  AddBranch(&patElectron_relIsoRhoEA              ,"patElectron_relIsoRhoEA");
  AddBranch(&patElectron_dr03EcalRecHitSumEt      ,"patElectron_dr03EcalRecHitSumEt");
  AddBranch(&patElectron_dr03HcalDepth1TowerSumEt ,"patElectron_dr03HcalDepth1TowerSumEt");
  AddBranch(&patElectron_isolPtTracks             ,"patElectron_isolPtTracks");
  AddBranch(&patElectron_ecalPFClusterIso         ,"patElectron_ecalPFClusterIso");
  AddBranch(&patElectron_hcalPFClusterIso         ,"patElectron_hcalPFClusterIso");
  //Shape, Track related variables, other prop
  AddBranch(&patElectron_dEtaIn                ,"patElectron_dEtaIn");
  AddBranch(&patElectron_dEtaInSeed                ,"patElectron_dEtaInSeed");
  AddBranch(&patElectron_dPhiIn                ,"patElectron_dPhiIn");
  AddBranch(&patElectron_full5x5_sigmaIetaIeta ,"patElectron_full5x5_sigmaIetaIeta");
  AddBranch(&patElectron_full5x5_e2x5Max       ,"patElectron_full5x5_e2x5Max");
  AddBranch(&patElectron_full5x5_e5x5          ,"patElectron_full5x5_e5x5");
  AddBranch(&patElectron_full5x5_e1x5          ,"patElectron_full5x5_e1x5");
  AddBranch(&patElectron_hOverE                ,"patElectron_hOverE");
  AddBranch(&patElectron_hOverE_new                ,"patElectron_hOverE_new");
  AddBranch(&patElectron_ooEmooP               ,"patElectron_ooEmooP");
  AddBranch(&passConversionVeto_               ,"patElectron_passConversionVeto"); 
  AddBranch(&expectedMissingInnerHits          ,"patElectron_expectedMissingInnerHits");
  AddBranch(&patElectron_gsfTrack_ndof         ,"patElectron_gsfTrack_ndof");
  AddBranch(&patElectron_gsfTrack_normChi2     ,"patElectron_gsfTrack_normChi2");
  //IP
  AddBranch(&patElectron_gsfTrack_dz_pv  ,"patElectron_gsfTrack_dz_pv");
  AddBranch(&patElectron_gsfTrack_dxy_pv ,"patElectron_gsfTrack_dxy_pv");
  AddBranch(&patElectron_d0              ,"patElectron_d0");
  AddBranch(&patElectron_dzError         ,"patElectron_dzError");
  AddBranch(&patElectron_dxyError        ,"patElectron_dxyError");
  AddBranch(&patElectron_gsfTrack_vtx    ,"patElectron_gsfTrack_vtx");
  AddBranch(&patElectron_gsfTrack_vty    ,"patElectron_gsfTrack_vty");
  AddBranch(&patElectron_gsfTrack_vtz    ,"patElectron_gsfTrack_vtz");
  if(_AJVar){
    AddBranch(&patElectron_gsfTrack_dz_bs            ,"patElectron_gsfTrack_dz_bs");
    AddBranch(&patElectron_gsfTrack_dxy_bs           ,"patElectron_gsfTrack_dxy_bs");
    AddBranch(&patElectron_gsfTrack_PCAx_pv          ,"patElectron_gsfTrack_PCAx_pv");
    AddBranch(&patElectron_gsfTrack_PCAy_pv          ,"patElectron_gsfTrack_PCAy_pv");
    AddBranch(&patElectron_gsfTrack_PCAz_pv          ,"patElectron_gsfTrack_PCAz_pv");
    AddBranch(&patElectron_gsfTrack_PCAx_bs          ,"patElectron_gsfTrack_PCAx_bs");
    AddBranch(&patElectron_gsfTrack_PCAy_bs          ,"patElectron_gsfTrack_PCAy_bs");
    AddBranch(&patElectron_gsfTrack_PCAz_bs          ,"patElectron_gsfTrack_PCAz_bs");
    AddBranch(&patElectron_gsfTrackFitErrorMatrix_00 ,"patElectron_gsfTrackFitErrorMatrix_00");
    AddBranch(&patElectron_gsfTrackFitErrorMatrix_01 ,"patElectron_gsfTrackFitErrorMatrix_01");
    AddBranch(&patElectron_gsfTrackFitErrorMatrix_02 ,"patElectron_gsfTrackFitErrorMatrix_02");
    AddBranch(&patElectron_gsfTrackFitErrorMatrix_11 ,"patElectron_gsfTrackFitErrorMatrix_11");
    AddBranch(&patElectron_gsfTrackFitErrorMatrix_12 ,"patElectron_gsfTrackFitErrorMatrix_12");
    AddBranch(&patElectron_gsfTrackFitErrorMatrix_22 ,"patElectron_gsfTrackFitErrorMatrix_22");
  }
  //TTH
  if(_tthlepVar){
    AddBranch(&patElectron_miniIsoRel                   ,"patElectron_miniIsoRel");
    AddBranch(&patElectron_miniIsoCh                    ,"patElectron_miniIsoCh");
    AddBranch(&patElectron_miniIsoNeu                   ,"patElectron_miniIsoNeu");
    AddBranch(&patElectron_miniIsoPUsub                 ,"patElectron_miniIsoPUsub");
    AddBranch(&patElectron_jetdr                        ,"patElectron_jetdr");
    AddBranch(&patElectron_jetl1corr                        ,"patElectron_jetl1corr");
    AddBranch(&patElectron_jetislep                        ,"patElectron_jetislep");
    AddBranch(&patElectron_jetidx                        ,"patElectron_jetidx");
    AddBranch(&patElectron_jetpt                        ,"patElectron_jetpt");
    AddBranch(&patElectron_jetptratio                   ,"patElectron_jetptratio");
    AddBranch(&patElectron_jetptratioV2                   ,"patElectron_jetptratioV2");
    AddBranch(&patElectron_jetcsv                       ,"patElectron_jetcsv");
    AddBranch(&patElectron_jetdeepcsv                       ,"patElectron_jetdeepcsv");
    AddBranch(&patElectron_jetdeepflavour                       ,"patElectron_jetdeepflavour");
    AddBranch(&patElectron_ptrel                        ,"patElectron_ptrel");
    AddBranch(&patElectron_IP3Dsig                      ,"patElectron_IP3Dsig");
    AddBranch(&patElectron_eleMVASpring15NonTrig25ns    ,"patElectron_eleMVASpring15NonTrig25ns");
    AddBranch(&patElectron_eleMVASpring15NonTrig25ns_VL ,"patElectron_eleMVASpring15NonTrig25ns_VL");
    AddBranch(&patElectron_pvass             ,"patElectron_pvass");
    AddBranch(&patElectron_etarel            ,"patElectron_etarel");
    AddBranch(&patElectron_ptOVen            ,"patElectron_ptOVen");
    AddBranch(&patElectron_elejet_pfJetProbabilityBJetTag ,"patElectron_elejet_pfJetProbabilityBJetTag");
    AddBranch(&patElectron_elejet_pfCombinedMVABJetTags   ,"patElectron_elejet_pfCombinedMVABJetTags");
    AddBranch(&patElectron_elejet_qgl         ,"patElectron_elejet_qgl");
    AddBranch(&patElectron_elemass            ,"patElectron_elemass");
    AddBranch(&patElectron_elejet_mass        ,"patElectron_elejet_mass");
    AddBranch(&patElectron_elejet_Wmass       ,"patElectron_elejet_Wmass");
    AddBranch(&patElectron_elejet_Topmass     ,"patElectron_elejet_Topmass");
    AddBranch(&patElectron_elejet_WTopmass    ,"patElectron_elejet_WTopmass");
    //Lep jet IP ntrks
    AddBranch(&patElectron_IP3D_val           ,"patElectron_IP3D_val");
    AddBranch(&patElectron_IP3D_err           ,"patElectron_IP3D_err");
    AddBranch(&patElectron_IP3D_sig           ,"patElectron_IP3D_sig");
    AddBranch(&patElectron_IP2D_val           ,"patElectron_IP2D_val");
    AddBranch(&patElectron_IP2D_err           ,"patElectron_IP2D_err");
    AddBranch(&patElectron_IP2D_sig           ,"patElectron_IP2D_sig");
    AddBranch(&patElectron_sIP3D_val          ,"patElectron_sIP3D_val");
    AddBranch(&patElectron_sIP3D_err          ,"patElectron_sIP3D_err");
    AddBranch(&patElectron_sIP3D_sig          ,"patElectron_sIP3D_sig");
    AddBranch(&patElectron_sIP2D_val          ,"patElectron_sIP2D_val");
    AddBranch(&patElectron_sIP2D_err          ,"patElectron_sIP2D_err");
    AddBranch(&patElectron_sIP2D_sig          ,"patElectron_sIP2D_sig");
    AddBranch(&patElectron_IP1D_val           ,"patElectron_IP1D_val");
    AddBranch(&patElectron_IP1D_err           ,"patElectron_IP1D_err");
    AddBranch(&patElectron_IP1D_sig           ,"patElectron_IP1D_sig");
    AddBranch(&patElectron_sIP1D_val          ,"patElectron_sIP1D_val");
    AddBranch(&patElectron_sIP1D_err          ,"patElectron_sIP1D_err");
    AddBranch(&patElectron_sIP1D_sig          ,"patElectron_sIP1D_sig");
    AddBranch(&patElectron_lepjetMaxIP3D_val  ,"patElectron_lepjetMaxIP3D_val");
    AddBranch(&patElectron_lepjetMaxIP3D_sig  ,"patElectron_lepjetMaxIP3D_sig");
    AddBranch(&patElectron_lepjetMaxsIP3D_val ,"patElectron_lepjetMaxsIP3D_val");
    AddBranch(&patElectron_lepjetMaxsIP3D_sig ,"patElectron_lepjetMaxsIP3D_sig");
    AddBranch(&patElectron_lepjetMaxIP2D_val  ,"patElectron_lepjetMaxIP2D_val");
    AddBranch(&patElectron_lepjetMaxIP2D_sig  ,"patElectron_lepjetMaxIP2D_sig");
    AddBranch(&patElectron_lepjetMaxsIP2D_val ,"patElectron_lepjetMaxsIP2D_val");
    AddBranch(&patElectron_lepjetMaxsIP2D_sig ,"patElectron_lepjetMaxsIP2D_sig");
    AddBranch(&patElectron_lepjetMaxIP1D_val  ,"patElectron_lepjetMaxIP1D_val");
    AddBranch(&patElectron_lepjetMaxIP1D_sig  ,"patElectron_lepjetMaxIP1D_sig");
    AddBranch(&patElectron_lepjetMaxsIP1D_val ,"patElectron_lepjetMaxsIP1D_val");
    AddBranch(&patElectron_lepjetMaxsIP1D_sig ,"patElectron_lepjetMaxsIP1D_sig");
    AddBranch(&patElectron_lepjetAvIP3D_val   ,"patElectron_lepjetAvIP3D_val");
    AddBranch(&patElectron_lepjetAvIP3D_sig   ,"patElectron_lepjetAvIP3D_sig");
    AddBranch(&patElectron_lepjetAvsIP3D_val  ,"patElectron_lepjetAvsIP3D_val");
    AddBranch(&patElectron_lepjetAvsIP3D_sig  ,"patElectron_lepjetAvsIP3D_sig");
    AddBranch(&patElectron_lepjetAvIP2D_val   ,"patElectron_lepjetAvIP2D_val");
    AddBranch(&patElectron_lepjetAvIP2D_sig   ,"patElectron_lepjetAvIP2D_sig");
    AddBranch(&patElectron_lepjetAvsIP2D_val  ,"patElectron_lepjetAvsIP2D_val");
    AddBranch(&patElectron_lepjetAvsIP2D_sig  ,"patElectron_lepjetAvsIP2D_sig");
    AddBranch(&patElectron_lepjetAvIP1D_val   ,"patElectron_lepjetAvIP1D_val");
    AddBranch(&patElectron_lepjetAvIP1D_sig   ,"patElectron_lepjetAvIP1D_sig");
    AddBranch(&patElectron_lepjetAvsIP1D_val  ,"patElectron_lepjetAvsIP1D_val");
    AddBranch(&patElectron_lepjetAvsIP1D_sig  ,"patElectron_lepjetAvsIP1D_sig");
    AddBranch(&patElectron_lepjetchtrks       ,"patElectron_lepjetchtrks");
    AddBranch(&patElectron_lepjetpvchtrks     ,"patElectron_lepjetpvchtrks");
    AddBranch(&patElectron_lepjetnonpvchtrks  ,"patElectron_lepjetnonpvchtrks");
    AddBranch(&patElectron_lepjetndaus        ,"patElectron_lepjetndaus");
    AddBranch(&patElectron_lepjetpvchi2       ,"patElectron_lepjetpvchi2");
    AddBranch(&patElectron_lepjetnumno2trk    ,"patElectron_lepjetnumno2trk");
  }
  //MC info
  if(!_is_data){
    AddBranch(&patElectron_gen_pt                                      ,"patElectron_gen_pt");
    AddBranch(&patElectron_gen_eta                                     ,"patElectron_gen_eta");
    AddBranch(&patElectron_gen_phi                                     ,"patElectron_gen_phi");
    AddBranch(&patElectron_gen_en                                      ,"patElectron_gen_en");
    AddBranch(&patElectron_gen_pdgId                                   ,"patElectron_gen_pdgId");
    AddBranch(&patElectron_gen_isPromptFinalState                      ,"patElectron_gen_isPromptFinalState");
    AddBranch(&patElectron_gen_isDirectPromptTauDecayProductFinalState ,"patElectron_gen_isDirectPromptTauDecayProductFinalState");
  }
  AddBranch(&patElectron_isMatchedToTrigger,"patElectron_isMatchedToTrigger");
  if(debug_) std::cout<<"set branches"<<std::endl;
}
void ElectronPatSelector::Clear(){
  //Kinematics     
  patElectron_pt.clear();
  patElectron_eta.clear();
  patElectron_phi.clear();
  patElectron_energy.clear();
  patElectron_Esc.clear();
  patElectron_px.clear();
  patElectron_py.clear();
  patElectron_pz.clear();
  patElectron_p.clear();
  patElectron_Et.clear();
  patElectron_SCeta.clear();
  patElectron_inCrack.clear();
  //Corrections
  patElectron_energySF.clear();
  patElectron_ecalEnergyErrPostCorr.clear();
  patElectron_ecalEnergyErrPreCorr.clear();
  patElectron_ecalEnergyPostCorr.clear();
  patElectron_ecalEnergyPreCorr.clear();
  patElectron_ecalTrkEnergyErrPostCorr.clear();
  patElectron_ecalTrkEnergyErrPreCorr.clear();
  patElectron_ecalTrkEnergyPostCorr.clear();
  patElectron_ecalTrkEnergyPreCorr.clear();
  patElectron_energyScaleDown.clear();
  patElectron_energyScaleGainDown.clear();
  patElectron_energyScaleGainUp.clear();
  patElectron_energyScaleStatDown.clear();
  patElectron_energyScaleStatUp.clear();
  patElectron_energyScaleSystDown.clear();
  patElectron_energyScaleSystUp.clear();
  patElectron_energyScaleUp.clear();
  patElectron_energyScaleValue.clear();
  patElectron_energySigmaDown.clear();
  patElectron_energySigmaPhiDown.clear();
  patElectron_energySigmaPhiUp.clear();
  patElectron_energySigmaRhoDown.clear();
  patElectron_energySigmaRhoUp.clear();
  patElectron_energySigmaUp.clear();
  patElectron_energySigmaValue.clear();
  patElectron_energySmearNrSigma.clear();
  //Charge
  patElectron_charge.clear(); 
  patElectron_isGsfCtfScPixChargeConsistent.clear();
  patElectron_isGsfScPixChargeConsistent.clear();
  //ID
  passVetoId_.clear();
  passLooseId_.clear();
  passMediumId_.clear();
  passTightId_.clear();  
  passMvaIsowp80Id_.clear();
  passMvanonIsowp80Id_.clear();
  passMvaIsowp90Id_.clear();
  passMvanonIsowp90Id_.clear();
  passMvaIsowpLooseId_.clear();
  passMvanonIsowpLooseId_.clear();
  patElectron_mvaValue_nonIso_.clear();
  patElectron_mvaCategory_nonIso_.clear();
  patElectron_mvaValue_Iso_.clear();
  patElectron_mvaCategory_Iso_.clear();
  /*
  passVetoOldId_.clear();
  passLooseOldId_.clear();
  passMediumOldId_.clear();
  passTightOldId_.clear();  
  passMvaIsowp80OldId_.clear();
  passMvanonIsowp80OldId_.clear();
  passMvaIsowp90OldId_.clear();
  passMvanonIsowp90OldId_.clear();
  passMvaIsowpLooseOldId_.clear();
  passMvanonIsowpLooseOldId_.clear();
  patElectron_OldmvaValue_nonIso_.clear();
  patElectron_OldmvaCategory_nonIso_.clear();
  patElectron_OldmvaValue_Iso_.clear();
  patElectron_OldmvaCategory_Iso_.clear();
  */
  passHEEPId_.clear();
  patElectron_pdgId.clear();
  patElectron_isEcalDriven.clear();
  passMvaHZZwpLooseId_.clear();
  patElectron_mvaValue_HZZ_.clear();
  patElectron_mvaCategory_HZZ_.clear();
  //Isolation
  patElectron_isoChargedHadrons.clear();
  patElectron_isoNeutralHadrons.clear();
  patElectron_isoPhotons.clear();
  patElectron_isoPU.clear();
  patElectron_relIsoDeltaBeta.clear();
  patElectron_relIsoRhoEA.clear();
  patElectron_dr03EcalRecHitSumEt.clear();
  patElectron_dr03HcalDepth1TowerSumEt.clear();
  patElectron_isolPtTracks.clear();
  patElectron_ecalPFClusterIso.clear();
  patElectron_hcalPFClusterIso.clear();
  //Shape, Track related variables, other prop
  patElectron_dEtaIn.clear();
  patElectron_dEtaInSeed.clear();
  patElectron_dPhiIn.clear();
  patElectron_full5x5_sigmaIetaIeta.clear();
  patElectron_full5x5_e2x5Max.clear();
  patElectron_full5x5_e5x5.clear();
  patElectron_full5x5_e1x5.clear();
  patElectron_hOverE.clear();
  patElectron_hOverE_new.clear();
  patElectron_ooEmooP.clear();
  passConversionVeto_.clear();
  expectedMissingInnerHits.clear();
  patElectron_gsfTrack_ndof.clear();
  patElectron_gsfTrack_normChi2.clear();
  //IP
  patElectron_gsfTrack_dz_pv.clear();
  patElectron_gsfTrack_dxy_pv.clear();
  patElectron_d0.clear();
  patElectron_dzError.clear();
  patElectron_dxyError.clear();
  patElectron_gsfTrack_vtx.clear();
  patElectron_gsfTrack_vty.clear();
  patElectron_gsfTrack_vtz.clear();
  if(_AJVar){
    patElectron_gsfTrack_dz_bs.clear();
    patElectron_gsfTrack_dxy_bs.clear();
    patElectron_gsfTrack_PCAx_pv.clear();
    patElectron_gsfTrack_PCAy_pv.clear();
    patElectron_gsfTrack_PCAz_pv.clear();
    patElectron_gsfTrack_PCAx_bs.clear();
    patElectron_gsfTrack_PCAy_bs.clear();
    patElectron_gsfTrack_PCAz_bs.clear();
    patElectron_gsfTrackFitErrorMatrix_00.clear();
    patElectron_gsfTrackFitErrorMatrix_01.clear();
    patElectron_gsfTrackFitErrorMatrix_02.clear();
    patElectron_gsfTrackFitErrorMatrix_11.clear();
    patElectron_gsfTrackFitErrorMatrix_12.clear();
    patElectron_gsfTrackFitErrorMatrix_22.clear();
  }
  //TTH
  if(_tthlepVar){
    patElectron_miniIsoRel.clear();
    patElectron_miniIsoCh.clear();
    patElectron_miniIsoNeu.clear();
    patElectron_miniIsoPUsub.clear();
    patElectron_jetdr.clear();
    patElectron_jetl1corr.clear();
    patElectron_jetislep.clear();
    patElectron_jetidx.clear();
    patElectron_jetpt.clear();
    patElectron_jetptratio.clear();
    patElectron_jetptratioV2.clear();
    patElectron_jetcsv.clear();
    patElectron_jetdeepcsv.clear();
    patElectron_jetdeepflavour.clear();
    patElectron_ptrel.clear();
    patElectron_IP3Dsig.clear();
    patElectron_eleMVASpring15NonTrig25ns.clear();
    patElectron_eleMVASpring15NonTrig25ns_VL.clear();
    patElectron_pvass.clear();
    patElectron_etarel.clear();
    patElectron_ptOVen.clear();
    patElectron_elejet_pfJetProbabilityBJetTag.clear();
    patElectron_elejet_pfCombinedMVABJetTags.clear();
    patElectron_elejet_qgl.clear();
    patElectron_elemass.clear();
    patElectron_elejet_mass.clear();
    patElectron_elejet_Wmass.clear();
    patElectron_elejet_Topmass.clear();
    patElectron_elejet_WTopmass.clear();
    //Lep jet IP ntrks
    patElectron_IP3D_val.clear();
    patElectron_IP3D_err.clear();
    patElectron_IP3D_sig.clear();
    patElectron_IP2D_val.clear();
    patElectron_IP2D_err.clear();
    patElectron_IP2D_sig.clear();
    patElectron_sIP3D_val.clear();
    patElectron_sIP3D_err.clear();
    patElectron_sIP3D_sig.clear();
    patElectron_sIP2D_val.clear();
    patElectron_sIP2D_err.clear();
    patElectron_sIP2D_sig.clear();
    patElectron_IP1D_val.clear();
    patElectron_IP1D_err.clear();
    patElectron_IP1D_sig.clear();
    patElectron_sIP1D_val.clear();
    patElectron_sIP1D_err.clear();
    patElectron_sIP1D_sig.clear();
    patElectron_lepjetMaxIP3D_val.clear();
    patElectron_lepjetMaxIP3D_sig.clear();
    patElectron_lepjetMaxsIP3D_val.clear();
    patElectron_lepjetMaxsIP3D_sig.clear();
    patElectron_lepjetMaxIP2D_val.clear();
    patElectron_lepjetMaxIP2D_sig.clear();
    patElectron_lepjetMaxsIP2D_val.clear();
    patElectron_lepjetMaxsIP2D_sig.clear();
    patElectron_lepjetMaxIP1D_val.clear();
    patElectron_lepjetMaxIP1D_sig.clear();
    patElectron_lepjetMaxsIP1D_val.clear();
    patElectron_lepjetMaxsIP1D_sig.clear();
    patElectron_lepjetAvIP3D_val.clear();
    patElectron_lepjetAvIP3D_sig.clear();
    patElectron_lepjetAvsIP3D_val.clear();
    patElectron_lepjetAvsIP3D_sig.clear();
    patElectron_lepjetAvIP2D_val.clear();
    patElectron_lepjetAvIP2D_sig.clear();
    patElectron_lepjetAvsIP2D_val.clear();
    patElectron_lepjetAvsIP2D_sig.clear();
    patElectron_lepjetAvIP1D_val.clear();
    patElectron_lepjetAvIP1D_sig.clear();
    patElectron_lepjetAvsIP1D_val.clear();
    patElectron_lepjetAvsIP1D_sig.clear();
    patElectron_lepjetchtrks.clear();
    patElectron_lepjetpvchtrks.clear();
    patElectron_lepjetnonpvchtrks.clear();
    patElectron_lepjetndaus.clear();
    patElectron_lepjetpvchi2.clear();
    patElectron_lepjetnumno2trk.clear();
  }
  //MC info
  if(!_is_data){
    patElectron_gen_pt.clear();
    patElectron_gen_eta.clear();
    patElectron_gen_phi.clear();
    patElectron_gen_en.clear();
    patElectron_gen_pdgId.clear();
    patElectron_gen_isPromptFinalState.clear();
    patElectron_gen_isDirectPromptTauDecayProductFinalState.clear();
  }
  patElectron_isMatchedToTrigger.clear();
}
bool ElectronPatSelector::isGoodVertex(const reco::Vertex& vtx){
  if(vtx.isFake())                                   return false;
  if(vtx.ndof()<_vtx_ndof_min)                       return false;
  if(vtx.position().Rho()>_vtx_rho_max)              return false;
  if(fabs(vtx.position().Z()) > _vtx_position_z_max) return false;
  return true;
}

void ElectronPatSelector::get_eleminiIso(double rho, const pat::Electron& cand, double& miniIso, double& miniIsoCh, double& miniIsoNeu, double& miniIsoPUsub){
  // new miniIso calculation
  auto iso = cand.miniPFIsolation();
  auto chg = iso.chargedHadronIso();
  auto neu = iso.neutralHadronIso();
  auto pho = iso.photonIso();
  double ea    = get_effarea(cand.superCluster()->position().eta());
  float R = 10.0/std::min(std::max(cand.pt(), 50.0),200.0);
  ea *= std::pow(R/0.3,2);
  miniIsoCh = chg;
  miniIsoNeu = neu;
  miniIsoPUsub = std::max(0.0,neu+pho-(rho)*ea);
  miniIso = miniIsoCh + miniIsoPUsub;
}

void ElectronPatSelector::get_eleminiIso_info(const pat::PackedCandidateCollection& pcc,double rhotth, const pat::Electron& cand, double& miniIso, double& miniIsoCh, double& miniIsoNeu, double& miniIsoPUsub){
  double miniIsoConeSize = 10.0/min(max(cand.pt(), 50.),200.);
  vector<const pat::PackedCandidate *> pfc_all; pfc_all.clear();
  vector<const pat::PackedCandidate *> pfc_ch;  pfc_ch.clear();
  vector<const pat::PackedCandidate *> pfc_neu; pfc_neu.clear();
  vector<const pat::PackedCandidate *> pfc_pu;  pfc_pu.clear();
  get_chneupu_pcc(pcc,pfc_all,pfc_ch,pfc_neu,pfc_pu);
  double innerR_ch;
  double innerR_neu;
  if(cand.isEB()){
    innerR_ch  = 0.0;
    innerR_neu = 0.0;
  }else{
    innerR_ch  = 0.015;
    innerR_neu = 0.08;
  }
  miniIsoCh  = get_isosumraw(pfc_ch,  cand, miniIsoConeSize, innerR_ch,  0.0, SelfVetoPolicyEle::selfVetoNone,  0);
  miniIsoNeu = get_isosumraw(pfc_neu, cand, miniIsoConeSize, innerR_neu, 0.0, SelfVetoPolicyEle::selfVetoNone, 22)+
               get_isosumraw(pfc_neu, cand, miniIsoConeSize,        0.0, 0.0, SelfVetoPolicyEle::selfVetoNone,130);
  double effarea    = get_effarea(cand.superCluster()->position().eta());
  double correction = rhotth*effarea*pow((miniIsoConeSize/0.3),2);
  miniIsoPUsub = std::max(0.0, miniIsoNeu-correction);
  miniIso = miniIsoCh+miniIsoPUsub;
}
void ElectronPatSelector::get_chneupu_pcc(const pat::PackedCandidateCollection& pcc,vector<const pat::PackedCandidate *>& pfc_all,vector<const pat::PackedCandidate *>& pfc_ch,vector<const pat::PackedCandidate *>& pfc_neu,vector<const pat::PackedCandidate *>&pfc_pu){
  for(const pat::PackedCandidate &p : pcc){
    pfc_all.push_back(&p);
    if(p.charge()==0){
     pfc_neu.push_back(&p);
    }else{
      if((abs(p.pdgId())==211)){// || ((abs(p.pdgId()) == 11 ) || (abs(p.pdgId()) == 13 )) ){
        if(p.fromPV()>1 && fabs(p.dz())<9999){
          pfc_ch.push_back(&p);
        }else{
          pfc_pu.push_back(&p);
        }
      }
    }
  }
  std::sort(pfc_ch.begin(),  pfc_ch.end(),  ByEta());
  std::sort(pfc_neu.begin(), pfc_neu.end(), ByEta());
  std::sort(pfc_pu.begin(),  pfc_pu.end(),  ByEta());
}
double ElectronPatSelector::get_isosumraw(const std::vector<const pat::PackedCandidate *> & pcc, const pat::Electron& cand, double miniIsoConeSize, double innerR, double ptTh, SelfVetoPolicyEle::SelfVetoPolicyEle selfVeto, int pdgId){
  //Look for cand sources
  std::vector<const reco::Candidate *> vetos; vetos.clear();
  for(uint i=0, n=cand.numberOfSourceCandidatePtrs(); i<n; ++i){
    if(selfVeto == SelfVetoPolicyEle::selfVetoNone) break;
    const reco::CandidatePtr &cp = cand.sourceCandidatePtr(i);
    if(cp.isNonnull() && cp.isAvailable()){
      vetos.push_back(&*cp);
      if(selfVeto == SelfVetoPolicyEle::selfVetoFirst) break;
    }
  }
  //Get the isolation
  double isosum = 0;
  //for(std::vector<const pat::PackedCandidate *>::const_iterator pc = pcc.begin(); pc<pcc.end(); ++pc)
  float miniIsoConeSize2 = miniIsoConeSize*miniIsoConeSize, innerR2 = innerR*innerR;
  typedef std::vector<const pat::PackedCandidate *>::const_iterator IT;
  IT pccbegin = std::lower_bound(pcc.begin(), pcc.end(), cand.eta() - miniIsoConeSize, ByEta());
  IT pccend   = std::upper_bound(pccbegin,    pcc.end(), cand.eta() + miniIsoConeSize, ByEta());
  for(IT pc = pccbegin; pc<pccend; ++pc){
    //pdgId veto
    if(pdgId>0 && abs((*pc)->pdgId())!=pdgId) continue;
    //pT requirement 
    if(ptTh>0 && (*pc)->pt()<ptTh) continue;
    //cone region
    double dr2 = reco::deltaR2(**pc, cand);
    if(miniIsoConeSize2<dr2 || dr2<innerR2) continue;
    //itself veto
    if(std::find(vetos.begin(), vetos.end(),*pc)!=vetos.end()) continue;
    //add to sum
    isosum += (*pc)->pt();
  }
  return isosum;
}
double ElectronPatSelector::get_effarea(double eta, bool isPFIso){
  //https://github.com/cms-sw/cmssw/blob/CMSSW_10_4_X/RecoEgamma/ElectronIdentification/data/Fall17/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_94X.txt
  double effarea = 0;
  if(_dataEra==2016){
      // different effarea for 2016 PFIso and 2016 MiniIso for historical reason
      // https://github.com/cms-sw/cmssw/blob/master/PhysicsTools/NanoAOD/python/electrons_cff.py#L114-L120 
      if(isPFIso){
        if(abs(eta) < 1.0)        effarea = 0.1703;
        else if(abs(eta) < 1.479) effarea = 0.1715;
        else if(abs(eta) < 2.0)   effarea = 0.1213;
        else if(abs(eta) < 2.2)   effarea = 0.1230;
        else if(abs(eta) < 2.3)   effarea = 0.1635;
        else if(abs(eta) < 2.4)   effarea = 0.1937;
        else if(abs(eta) < 2.5)   effarea = 0.2393;
      }else{
        if(abs(eta) < 1.0)        effarea = 0.1752;
        else if(abs(eta) < 1.479) effarea = 0.1862;
        else if(abs(eta) < 2.0)   effarea = 0.1411;
        else if(abs(eta) < 2.2)   effarea = 0.1534;
        else if(abs(eta) < 2.3)   effarea = 0.1903;
        else if(abs(eta) < 2.4)   effarea = 0.2243;
        else if(abs(eta) < 2.5)   effarea = 0.2687;
      }
  }else{
    if(abs(eta) < 1.0)        effarea = 0.1440;
    else if(abs(eta) < 1.479) effarea = 0.1562;
    else if(abs(eta) < 2.0)   effarea = 0.1032;
    else if(abs(eta) < 2.2)   effarea = 0.0859;
    else if(abs(eta) < 2.3)   effarea = 0.1116;
    else if(abs(eta) < 2.4)   effarea = 0.1321;
    else if(abs(eta) < 2.5)   effarea = 0.1654;
  }
  return effarea;
}
void ElectronPatSelector::get_elejet_info(edm::View<pat::Electron>::const_iterator& ele, const edm::Event& iEvent, const edm::EventSetup& iSetup, double& elejet_l1corr, double& elejetislep,
                       double& elejet_mindr, double& elejet_pt, double& eleptOVelejetpt,
                       double& elejet_pfCombinedInclusiveSecondaryVertexV2BJetTags, double& elejet_pfDeepCSVBJetTags, double& elejet_pfDeepFlavourBJetTags,
                       double& elejet_pfJetProbabilityBJetTags, double& elejet_pfCombinedMVABJetTags, double& elejet_qgl,
                       double& jx, double& jy, double& jz, double& eleptrel,
                       int& lepjetidx){
  //Look for jet associated to ele
  edm::Handle<pat::JetCollection> jets;
  iEvent.getByToken(jets_, jets);
  pat::Jet elejet;
  int currjetpos = 0;
  for(const pat::Jet &j : *jets){
    pat::Jet jet = j;
    double dr = deltaR(ele->p4(),jet.p4());
    /*
    if(dr<elejet_mindr){
      elejet_mindr = dr;
      elejet       = jet;
      lepjetidx = currjetpos;
    }
    */
    //std::cout<<iEvent.id().event()  <<" Electron pt "<<ele->p4().pt()<< " Jet pt " << jet.p4().pt() <<std::endl;
    for(unsigned int i1 = 0 ; i1 < ele->numberOfSourceCandidatePtrs();i1++){
        const reco::CandidatePtr  &c1s=ele->sourceCandidatePtr(i1);
        for(unsigned int i2 = 0 ; i2 < jet.numberOfSourceCandidatePtrs();i2++) {
            const reco::CandidatePtr  &c2s=jet.sourceCandidatePtr(i2);
            if(c2s== c1s){
                elejet = jet;
                elejet_mindr = dr;
                lepjetidx = currjetpos;
                //std::cout << " closest jet is " << jet.p4()<< " lepjetidx is " << lepjetidx <<std::endl;
                break;  // take leading jet with shared source candidates
            }
        }
        if(lepjetidx >=0)break;// take leading jet with shared source candidates
    }
    currjetpos++;
    if(lepjetidx >=0)break;// take leading jet with shared source candidates
  }
  //Get info
  if(elejet.jecSetsAvailable()){
    if((elejet.correctedJet("Uncorrected").p4()-ele->p4()).R()<1e-4)elejetislep=1;
    double L1_corr = elejet.jecFactor("L1FastJet")/elejet.jecFactor("Uncorrected");
    //std::cout<<iEvent.id().event()  <<" Electron pt "<<ele->p4().pt()<<" L1Factor "<<L1_corr<<" closetJet unCorr p4"<<elejet.correctedJet("Uncorrected").p4() <<" closetJet corr0 Pt " << elejet.correctedJet(0)<<  "L1corrjet "<< elejet.correctedJet(1) << " jecFactor.L1FastJet "<< elejet.jecFactor("L1FastJet")<< " jecFactor.Uncorrected "<< elejet.jecFactor("Uncorrected") << " jet pt " << elejet.pt() <<std::endl;
    elejet.setP4((elejet.correctedJet("Uncorrected").p4()-ele->p4()*(1.0/L1_corr))*(elejet.p4().pt()/elejet.correctedJet("Uncorrected").p4().pt())+ele->p4());
    elejet_l1corr = L1_corr;
    elejet_pt       = elejet.pt();
    eleptOVelejetpt = min(ele->pt()/elejet.pt(), 1.5);
  }
  elejet_pfCombinedInclusiveSecondaryVertexV2BJetTags = max(double(elejet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags")), 0.0);
  if(elejet_pfCombinedInclusiveSecondaryVertexV2BJetTags!=elejet_pfCombinedInclusiveSecondaryVertexV2BJetTags) elejet_pfCombinedInclusiveSecondaryVertexV2BJetTags = -996;
  elejet_pfDeepCSVBJetTags = max(double(elejet.bDiscriminator("pfDeepCSVJetTags:probb") + elejet.bDiscriminator("pfDeepCSVJetTags:probbb")), 0.0);
  if(elejet_pfDeepCSVBJetTags!=elejet_pfDeepCSVBJetTags) elejet_pfDeepCSVBJetTags = -996;
  elejet_pfDeepFlavourBJetTags = max(double(elejet.bDiscriminator("pfDeepFlavourJetTags:probb") + elejet.bDiscriminator("pfDeepFlavourJetTags:probbb") + elejet.bDiscriminator("pfDeepFlavourJetTags:problepb")), 0.0);
  if(elejet_pfDeepFlavourBJetTags!=elejet_pfDeepFlavourBJetTags) elejet_pfDeepFlavourBJetTags = -996;
  elejet_pfJetProbabilityBJetTags = elejet.bDiscriminator("pfJetProbabilityBJetTags");
  if(elejet_pfJetProbabilityBJetTags!=elejet_pfJetProbabilityBJetTags) elejet_pfJetProbabilityBJetTags = -996;
  elejet_pfCombinedMVABJetTags    = elejet.bDiscriminator("pfCombinedMVABJetTags");
  if(elejet_pfCombinedMVABJetTags!=elejet_pfCombinedMVABJetTags) elejet_pfCombinedMVABJetTags = -996;
  if(_qglVar){
    edm::Handle<edm::View<pat::Jet>> jets_QGL;
    iEvent.getByToken(jetsToken,jets_QGL);
    edm::Handle<edm::ValueMap<float>> qgHandle;
    iEvent.getByToken(qgToken, qgHandle);
    for(auto jet = jets_QGL->begin();  jet != jets_QGL->end(); ++jet){
      if(distance(jets_QGL->begin(),jet)!=lepjetidx) continue;
      edm::RefToBase<pat::Jet> jetRef(edm::Ref<edm::View<pat::Jet> >(jets_QGL, jet - jets_QGL->begin()));
      elejet_qgl = (*qgHandle)[jetRef];
      break;
    }
  }
  jx = elejet.px();
  jy = elejet.py();
  jz = elejet.pz();
  TLorentzVector ele_lv = TLorentzVector(ele->px(),ele->py(),ele->pz(),ele->p4().E());
  TLorentzVector jet_lv = TLorentzVector(elejet.px(),elejet.py(),elejet.pz(),elejet.p4().E());
  eleptrel = ele_lv.Perp((jet_lv-ele_lv).Vect());
}
int ElectronPatSelector::pvassociation(edm::View<pat::Electron>::const_iterator& ele, const pat::PackedCandidateCollection& pcc){
  int pvass = -1;
  double mindr = 0.3;
  for(const pat::PackedCandidate &cpf : pcc){
    if(deltaR(ele->p4(),cpf.p4())<mindr             //dR is the standard geometrical way to associate 
       && (fabs(ele->pt()-cpf.pt())/ele->pt())<0.05   //Check in pT, because ele,tau are usually faked by jets (many trks) and dR may not be enough
       && cpf.charge()!=0 && cpf.numberOfHits()>0 //Leptons are charged and built from tracks, also to be consistent with PV tracks  
    ){
      mindr = deltaR(ele->p4(),cpf.p4());
      pvass = cpf.fromPV();
    }
  }
  return pvass;
}
double ElectronPatSelector::relativeEta(const math::XYZVector& vector, const math::XYZVector& axis){
  double etarel = 15; //Take this as a default value and in the end use min(etarel,15)
  double mag = vector.r() * axis.r();
  double dot = vector.Dot(axis);
  if((mag-dot)!=0 && (mag+dot)!=0) etarel = -log((mag-dot)/(mag+dot)) / 2;
  return etarel;
}
double ElectronPatSelector::get_lepWmass(edm::View<pat::Electron>::const_iterator& ele, const edm::Event& iEvent, int& lepjetidx){
  double lepWmass = 0;
  double minchi2  = 999;
  //Take lepjet lv 
  edm::Handle<pat::JetCollection> jets;
  iEvent.getByToken(jets_, jets);
  TLorentzVector lepjet;
  if(lepjetidx!=-1){
    const pat::Jet & jet = (*jets)[lepjetidx];
    lepjet.SetPtEtaPhiE(jet.pt(),jet.eta(),jet.phi(),jet.energy()); 
  }else{
    lepjet.SetPtEtaPhiE(ele->pt(),ele->eta(),ele->phi(),ele->energy());
  }
  //Start combinatorial with other jets
  for(const pat::Jet &j : *jets){
    pat::Jet jet = j;
    if(!(j.pt()>25 && fabs(j.eta())<2.4 && deltaR(j.p4(),ele->p4())>0.4)) continue;
    if(!(j.neutralHadronEnergyFraction()<0.99 && j.neutralEmEnergyFraction()<0.99 && (j.chargedMultiplicity() + j.neutralMultiplicity())>1
       && j.chargedHadronEnergyFraction()>0 && j.chargedEmEnergyFraction()<0.99 && j.chargedMultiplicity()>0
       && j.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags")<0.89)//0.605)
      ) continue;
    TLorentzVector jet_lv = TLorentzVector(j.px(),j.py(),j.pz(),j.p4().E());
    double currmass = (lepjet+jet_lv).M();
    double currchi2 = pow((currmass-80.385)/25,2); 
    if(currchi2<minchi2){
      minchi2 = currchi2;
      lepWmass = currmass;
    }
  }
  return lepWmass;
};
double ElectronPatSelector::get_lepTopmass(edm::View<pat::Electron>::const_iterator& ele, const edm::Event& iEvent, int& lepjetidx){
  double lepTopmass = 0;
  double minchi2  = 999;
  //Take lepjet lv 
  edm::Handle<pat::JetCollection> jets;
  iEvent.getByToken(jets_, jets);
  TLorentzVector lepjet;
  if(lepjetidx!=-1){
    const pat::Jet & jet = (*jets)[lepjetidx];
    lepjet.SetPtEtaPhiE(jet.pt(),jet.eta(),jet.phi(),jet.energy()); 
  }else{
    lepjet.SetPtEtaPhiE(ele->pt(),ele->eta(),ele->phi(),ele->energy());
  }
  //Start combinatorial with other jets (you can assume any type of jets. Instead asking non b jet would favor the case where lepjet is a b)
  for(uint j1 = 0; j1 < jets->size(); j1++){
    const pat::Jet & jet1 = (*jets)[j1];
    if(!(jet1.pt()>25 && fabs(jet1.eta())<2.4 && deltaR(jet1.p4(),ele->p4())>0.4)) continue;
    if(!(jet1.neutralHadronEnergyFraction()<0.99 && jet1.neutralEmEnergyFraction()<0.99 && (jet1.chargedMultiplicity() + jet1.neutralMultiplicity())>1
       && jet1.chargedHadronEnergyFraction()>0 && jet1.chargedEmEnergyFraction()<0.99 && jet1.chargedMultiplicity()>0
       && jet1.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags")<0.89
        )
      ) continue;
    for(uint j2 = j1+1; j2 < jets->size(); j2++){
      const pat::Jet & jet2 = (*jets)[j2];
      if(!(jet2.pt()>25 && fabs(jet2.eta())<2.4 && deltaR(jet2.p4(),ele->p4())>0.4)) continue;
      if(!(jet2.neutralHadronEnergyFraction()<0.99 && jet2.neutralEmEnergyFraction()<0.99 && (jet2.chargedMultiplicity() + jet2.neutralMultiplicity())>1
         && jet2.chargedHadronEnergyFraction()>0 && jet2.chargedEmEnergyFraction()<0.99 && jet2.chargedMultiplicity()>0
         && jet2.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags")<0.89
          )
        ) continue;
      TLorentzVector jet1_lv = TLorentzVector(jet1.px(),jet1.py(),jet1.pz(),jet1.p4().E());      
      TLorentzVector jet2_lv = TLorentzVector(jet2.px(),jet2.py(),jet2.pz(),jet2.p4().E());      
      TLorentzVector W_lv   = jet1_lv+jet2_lv;
      TLorentzVector Top_lv = W_lv+lepjet;    
      double currmassw   = W_lv.M();  
      double currmasstop = Top_lv.M(); 
      double currchi2 = pow((currmassw-80.385)/25,2) + pow((currmasstop-173.34)/50,2);
      if(currchi2<minchi2){
        minchi2 = currchi2;
        lepTopmass = currmasstop;
      }    
    }
  }
  return lepTopmass;
};
double ElectronPatSelector::get_lepWTopmass(edm::View<pat::Electron>::const_iterator& ele, const edm::Event& iEvent, int& lepjetidx){
  double lepWTopmass = 0;
  double minchi2  = 999;
  //Take lepjet lv 
  edm::Handle<pat::JetCollection> jets;
  iEvent.getByToken(jets_, jets);
  TLorentzVector lepjet;
  if(lepjetidx!=-1){
    const pat::Jet & jet = (*jets)[lepjetidx];
    lepjet.SetPtEtaPhiE(jet.pt(),jet.eta(),jet.phi(),jet.energy()); 
  }else{
    lepjet.SetPtEtaPhiE(ele->pt(),ele->eta(),ele->phi(),ele->energy());
  }
  //Start combinatorial with other jets (you can assume any type of jets. Instead asking non b jet would favor the case where lepjet is a b)
  for(uint j1 = 0; j1 < jets->size(); j1++){
    const pat::Jet & jet1 = (*jets)[j1];
    if(!(jet1.pt()>25 && fabs(jet1.eta())<2.4 && deltaR(jet1.p4(),ele->p4())>0.4)) continue;
    if(!(jet1.neutralHadronEnergyFraction()<0.99 && jet1.neutralEmEnergyFraction()<0.99 && (jet1.chargedMultiplicity() + jet1.neutralMultiplicity())>1
       && jet1.chargedHadronEnergyFraction()>0 && jet1.chargedEmEnergyFraction()<0.99 && jet1.chargedMultiplicity()>0
       && jet1.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags")<0.89
        )
      ) continue;
    TLorentzVector jet1_lv = TLorentzVector(jet1.px(),jet1.py(),jet1.pz(),jet1.p4().E());         
    TLorentzVector W_lv    = jet1_lv+lepjet;
    for(uint j2 = j1+1; j2 < jets->size(); j2++){
      const pat::Jet & jet2 = (*jets)[j2];
      if(!(jet2.pt()>25 && fabs(jet2.eta())<2.4 && deltaR(jet2.p4(),ele->p4())>0.4)) continue;
      if(!(jet2.neutralHadronEnergyFraction()<0.99 && jet2.neutralEmEnergyFraction()<0.99 && (jet2.chargedMultiplicity() + jet2.neutralMultiplicity())>1
         && jet2.chargedHadronEnergyFraction()>0 && jet2.chargedEmEnergyFraction()<0.99 && jet2.chargedMultiplicity()>0
         && jet2.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags")>0.605
          )
        ) continue;
      TLorentzVector jet2_lv = TLorentzVector(jet2.px(),jet2.py(),jet2.pz(),jet2.p4().E());      
      TLorentzVector Top_lv = W_lv+jet2_lv;    
      double currmassw   = W_lv.M();  
      double currmasstop = Top_lv.M(); 
      double currchi2 = pow((currmassw-80.385)/25,2) + pow((currmasstop-173.34)/50,2);
      if(currchi2<minchi2){
        minchi2 = currchi2;
        lepWTopmass = currmassw;
      }    
    }
  }
  return lepWTopmass;
};
void ElectronPatSelector::IP3D2D(TransientTrack ttrk, const reco::Vertex& vtx, GlobalVector gv, double& IP3D_val,double& IP3D_err,double& IP3D_sig, double& sIP3D_val,double& sIP3D_err,double& sIP3D_sig, double& IP2D_val,double& IP2D_err,double& IP2D_sig, double& sIP2D_val,double& sIP2D_err,double& sIP2D_sig){
 pair<bool, Measurement1D> currIP;
 //3D
 currIP = IPTools::absoluteImpactParameter3D(ttrk,vtx);
 if(currIP.first){
   if(currIP.second.value()==currIP.second.value())               IP3D_val = currIP.second.value();
   if(currIP.second.error()==currIP.second.error())               IP3D_err = currIP.second.error();
   if(currIP.second.significance()==currIP.second.significance()) IP3D_sig = currIP.second.significance();
 }
 //s3D
 currIP = IPTools::signedImpactParameter3D(ttrk,gv,vtx);
 if(currIP.first){
   if(currIP.second.value()==currIP.second.value())               sIP3D_val = currIP.second.value();
   if(currIP.second.error()==currIP.second.error())               sIP3D_err = currIP.second.error();
   if(currIP.second.significance()==currIP.second.significance()) sIP3D_sig = currIP.second.significance();
 }
 //2D 
 currIP = IPTools::absoluteTransverseImpactParameter(ttrk,vtx);
 if(currIP.first){
   if(currIP.second.value()==currIP.second.value())               IP2D_val = currIP.second.value();
   if(currIP.second.error()==currIP.second.error())               IP2D_err = currIP.second.error();
   if(currIP.second.significance()==currIP.second.significance()) IP2D_sig = currIP.second.significance();
 }
 //s2D 
 currIP = IPTools::signedTransverseImpactParameter(ttrk,gv,vtx);
 if(currIP.first){
   if(currIP.second.value()==currIP.second.value())               sIP2D_val = currIP.second.value();
   if(currIP.second.error()==currIP.second.error())               sIP2D_err = currIP.second.error();
   if(currIP.second.significance()==currIP.second.significance()) sIP2D_sig = currIP.second.significance();
 }
}
void ElectronPatSelector::zIP1D(TransientTrack ttrk, const reco::Vertex& vtx, GlobalVector gv, double& IP1D_val,double& IP1D_err,double& IP1D_sig, double& sIP1D_val,double& sIP1D_err,double& sIP1D_sig){
 SignedTransverseImpactParameter stip;
 pair<bool, Measurement1D> currIP = stip.zImpactParameter(ttrk,gv,vtx);
 if(currIP.first){
   if(currIP.second.value()==currIP.second.value())               IP1D_val = fabs(currIP.second.value());
   if(currIP.second.error()==currIP.second.error())               IP1D_err = fabs(currIP.second.error());
   if(currIP.second.significance()==currIP.second.significance()) IP1D_sig = fabs(currIP.second.significance());
   if(currIP.second.value()==currIP.second.value())               sIP1D_val = currIP.second.value();
   if(currIP.second.error()==currIP.second.error())               sIP1D_err = currIP.second.error();
   if(currIP.second.significance()==currIP.second.significance()) sIP1D_sig = currIP.second.significance();
 }
}
void ElectronPatSelector::lepjetIP(const pat::Jet& jet, const reco::Vertex& vtx, GlobalVector lepjetgv, const TransientTrackBuilder& ttrkbuilder,
                    double& lepjetMaxIP3D_val, double& lepjetMaxIP3D_sig, double& lepjetMaxsIP3D_val, double& lepjetMaxsIP3D_sig, double& lepjetMaxIP2D_val, double& lepjetMaxIP2D_sig, double& lepjetMaxsIP2D_val, double& lepjetMaxsIP2D_sig, double& lepjetMaxIP1D_val, double& lepjetMaxIP1D_sig, double& lepjetMaxsIP1D_val, double& lepjetMaxsIP1D_sig,
                    double& lepjetAvIP3D_val, double& lepjetAvIP3D_sig, double& lepjetAvsIP3D_val, double& lepjetAvsIP3D_sig, double& lepjetAvIP2D_val, double& lepjetAvIP2D_sig, double& lepjetAvsIP2D_val, double& lepjetAvsIP2D_sig, double& lepjetAvIP1D_val, double& lepjetAvIP1D_sig, double& lepjetAvsIP1D_val, double& lepjetAvsIP1D_sig,
                    double& denlepjetAvIP3D_val, double& denlepjetAvIP3D_sig, double& denlepjetAvsIP3D_val, double& denlepjetAvsIP3D_sig, double& denlepjetAvIP2D_val, double& denlepjetAvIP2D_sig, double& denlepjetAvsIP2D_val, double& denlepjetAvsIP2D_sig, double& denlepjetAvIP1D_val, double& denlepjetAvIP1D_sig, double& denlepjetAvsIP1D_val, double& denlepjetAvsIP1D_sig,
                    double& Lep_IP3D_val
){
 //Access jet daughters
 vector<CandidatePtr> jdaus(jet.daughterPtrVector());
 sort(jdaus.begin(), jdaus.end(), [](const reco::CandidatePtr &p1, const reco::CandidatePtr &p2) {return p1->pt() > p2->pt();});
 for(uint jd=0; jd<jdaus.size(); jd++){
  const pat::PackedCandidate &jcand = dynamic_cast<const pat::PackedCandidate &>(*jdaus[jd]);
  if(deltaR(jcand.p4(),jet.p4())>0.4) continue;
  if(!jcand.hasTrackDetails())continue;
  Track trk = Track(jcand.pseudoTrack());
  bool isgoodtrk = is_goodtrk(trk,vtx);
  //Minimal conditions for a track 
  if(isgoodtrk && jcand.charge()!=0 && jcand.fromPV()>1){
    TransientTrack ttrk = ttrkbuilder.build(&trk);
    //Current IP values 
    double IP3D_val  = -9999;
    double IP3D_err  = -9999;
    double IP3D_sig  = -9999;
    double sIP3D_val = -9999;
    double sIP3D_err = -9999;
    double sIP3D_sig = -9999;
    double IP2D_val  = -9999;
    double IP2D_err  = -9999;
    double IP2D_sig  = -9999;
    double sIP2D_val = -9999;
    double sIP2D_err = -9999;
    double sIP2D_sig = -9999;
    double IP1D_val  = -9999;
    double IP1D_err  = -9999;
    double IP1D_sig  = -9999;
    double sIP1D_val = -9999;
    double sIP1D_err = -9999;
    double sIP1D_sig = -9999;
    IP3D2D(ttrk,vtx,lepjetgv,IP3D_val,IP3D_err,IP3D_sig,sIP3D_val,sIP3D_err,sIP3D_sig,IP2D_val,IP2D_err,IP2D_sig,sIP2D_val,sIP2D_err,sIP2D_sig);
    zIP1D(ttrk,vtx,lepjetgv,IP1D_val,IP1D_err,IP1D_sig,sIP1D_val,sIP1D_err,sIP1D_sig);
    //Max Lep jet IP
    if(IP3D_val>lepjetMaxIP3D_val)   lepjetMaxIP3D_val  = IP3D_val;
    if(IP3D_sig>lepjetMaxIP3D_sig)   lepjetMaxIP3D_sig  = IP3D_sig;
    if(sIP3D_val>lepjetMaxsIP3D_val) lepjetMaxsIP3D_val = sIP3D_val;
    if(sIP3D_sig>lepjetMaxsIP3D_sig) lepjetMaxsIP3D_sig = sIP3D_sig;
    if(IP2D_val>lepjetMaxIP2D_val)   lepjetMaxIP2D_val  = IP2D_val;
    if(IP2D_sig>lepjetMaxIP2D_sig)   lepjetMaxIP2D_sig  = IP2D_sig;
    if(sIP2D_val>lepjetMaxsIP2D_val) lepjetMaxsIP2D_val = sIP2D_val;
    if(sIP2D_sig>lepjetMaxsIP2D_sig) lepjetMaxsIP2D_sig = sIP2D_sig;
    if(IP1D_val>lepjetMaxIP1D_val)   lepjetMaxIP1D_val  = IP1D_val;
    if(IP1D_sig>lepjetMaxIP1D_sig)   lepjetMaxIP1D_sig  = IP1D_sig;
    if(sIP1D_val>lepjetMaxsIP1D_val) lepjetMaxsIP1D_val = sIP1D_val;
    if(sIP1D_sig>lepjetMaxsIP1D_sig) lepjetMaxsIP1D_sig = sIP1D_sig;
    //Max Lep jet IP
    if((fabs(IP3D_val-Lep_IP3D_val)/IP3D_val)>0.01){
      if(IP3D_val!=-9999 ){
        lepjetAvIP3D_val     += IP3D_val;
        denlepjetAvIP3D_val  += 1;
      }
      if(IP3D_sig!=-9999 ){
        lepjetAvIP3D_sig     += IP3D_sig;
        denlepjetAvIP3D_sig  += 1;
      }
      if(sIP3D_val!=-9999){
        lepjetAvsIP3D_val    += sIP3D_val;
        denlepjetAvsIP3D_val += 1;
      }
      if(sIP3D_sig!=-9999){
        lepjetAvsIP3D_sig    += sIP3D_sig;
        denlepjetAvsIP3D_sig += 1;
      }
      if(IP2D_val!=-9999 ){
        lepjetAvIP2D_val     += IP2D_val;
        denlepjetAvIP2D_val  += 1;
      }
      if(IP2D_sig!=-9999 ){
        lepjetAvIP2D_sig     += IP2D_sig;
        denlepjetAvIP2D_sig  += 1;
      }
      if(sIP2D_val!=-9999){
        lepjetAvsIP2D_val    += sIP2D_val;
        denlepjetAvsIP2D_val += 1;
      }
      if(sIP2D_sig!=-9999){
        lepjetAvsIP2D_sig    += sIP2D_sig;
        denlepjetAvsIP2D_sig += 1;
      }
      if(IP1D_val!=-9999 ){
        lepjetAvIP1D_val     += IP1D_val;
        denlepjetAvIP1D_val  += 1;
      }
      if(IP1D_sig!=-9999 ){
        lepjetAvIP1D_sig     += IP1D_sig;
        denlepjetAvIP1D_sig  += 1;
      }
      if(sIP1D_val!=-9999){
        lepjetAvsIP1D_val    += sIP1D_val;
        denlepjetAvsIP1D_val += 1;
      }
      if(sIP1D_sig!=-9999){
        lepjetAvsIP1D_sig    += sIP1D_sig;
        denlepjetAvsIP1D_sig += 1;
      }
    }
  }//Ch trks 
 }//Loop on jet daus
}
bool ElectronPatSelector::is_goodtrk(Track trk,const reco::Vertex& vtx){
 bool isgoodtrk = false;
 if(trk.pt()>1 &&
   trk.hitPattern().numberOfValidHits()>=8 &&
   trk.hitPattern().numberOfValidPixelHits()>=2 &&
   trk.normalizedChi2()<5 &&
   std::abs(trk.dxy(vtx.position()))<0.2 &&
   std::abs(trk.dz(vtx.position()))<17
   ) isgoodtrk = true;
 return isgoodtrk;
}
void ElectronPatSelector::lepjetTrks(edm::View<pat::Electron>::const_iterator& ele, const pat::Jet& jet, const reco::Vertex& vtx, double& lepjetchtrks, double& lepjetpvchtrks, double& lepjetnonpvchtrks, double& lepjetndaus){
 //Access jet daughters
 vector<CandidatePtr> jdaus(jet.daughterPtrVector());
 sort(jdaus.begin(), jdaus.end(), [](const reco::CandidatePtr &p1, const reco::CandidatePtr &p2) {return p1->pt() > p2->pt();});
 for(uint jd=0; jd<jdaus.size(); jd++){
  const pat::PackedCandidate &jcand = dynamic_cast<const pat::PackedCandidate &>(*jdaus[jd]);
  if(deltaR(jcand.p4(),ele->p4())>0.4) continue;
  //if(deltaR(jcand.p4(),jet.p4())>0.4) continue;
  if(!jcand.hasTrackDetails())continue;
  Track trk = Track(jcand.pseudoTrack());
  bool isgoodtrk = is_goodtrk(trk,vtx);
  //Minimal conditions for a track 
  if(isgoodtrk && jcand.charge()!=0 && jcand.fromPV()>1){
    //Get jet trk num
    lepjetchtrks++;
    if(jcand.fromPV()==pat::PackedCandidate::PVUsedInFit){
      lepjetpvchtrks++;
    }else{
      lepjetnonpvchtrks++;
    }
  }//Ch trks 
 }//Loop on jet daus
 lepjetndaus = jdaus.size();
}
void ElectronPatSelector::lepjetVtxCompatibility(const pat::Jet& jet, const reco::Vertex& vtx, const TransientTrackBuilder& ttrkbuilder, double& lepjetpvchi2, double& lepjetnumno2tr){
 //Access jet daughters
 vector<TransientTrack> jetttrks;
 vector<CandidatePtr> jdaus(jet.daughterPtrVector());
 sort(jdaus.begin(), jdaus.end(), [](const reco::CandidatePtr &p1, const reco::CandidatePtr &p2) {return p1->pt() > p2->pt();});
 for(uint jd=0; jd<jdaus.size(); jd++){
  const pat::PackedCandidate &jcand = dynamic_cast<const pat::PackedCandidate &>(*jdaus[jd]);
  if(deltaR(jcand.p4(),jet.p4())>0.4) continue;
  if(!jcand.hasTrackDetails())continue;
  Track trk = Track(jcand.pseudoTrack());
  bool isgoodtrk = is_goodtrk(trk,vtx);
  //Minimal conditions for a track 
  if(isgoodtrk && jcand.charge()!=0 && jcand.fromPV()>1){
    TransientTrack ttrk = ttrkbuilder.build(&trk);
    jetttrks.push_back(ttrk);
  }//Ch trks 
 }//Loop on jet daus
 //LepJet vertex chi2
 TransientVertex tv;
 if(jetttrks.size()>=2) tv = vertexfitterele.vertex(jetttrks);
 if(tv.isValid()) lepjetpvchi2 = tv.totalChiSquared()/tv.degreesOfFreedom();
 double num2v = 0; double numno2v = 0;
 get_2trksinfo(jetttrks,  num2v,  numno2v);
 if((numno2v+num2v)!=0) lepjetnumno2tr = numno2v/(numno2v+num2v);
 else                   lepjetnumno2tr = 0;
}
void ElectronPatSelector::get_2trksinfo(vector<TransientTrack> ttrks, double& num2v, double& numno2v){
 for(uint t=0; t<ttrks.size(); t++){
  for(uint t2=t+1; t2<ttrks.size(); t2++){
   vector<TransientTrack> twotrks;
   twotrks.push_back(ttrks[t]);
   twotrks.push_back(ttrks[t2]);
   TransientVertex tv;
   if(ttrks.size()>=2) tv = vertexfitterele.vertex(ttrks);
   if(tv.isValid() && TMath::Prob(tv.totalChiSquared(),tv.degreesOfFreedom())>0.05){
    num2v++;
   }else{
    numno2v++;
   }
  }
 }
}
//TTHLep synch
/*
    //cout<<"Ele sel "<<distance(electron_pat->begin(),el)<<" "<<el->gsfTrack().isNonnull()<<" "<<el->pt()<<" "<<fabs(el->eta())<<" "<<fabs(el->gsfTrack()->dxy(firstGoodVertex.position()))<<" "<<fabs(el->gsfTrack()->dz(firstGoodVertex.position()))<<" "<<miniIso/el->pt()<<" "<<fabs(el->dB(pat::Electron::PV3D))/el->edB(pat::Electron::PV3D)<<" "<<nontrigelemva_vl<<" "<<el->gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS)<<" "<<el->passConversionVeto()<<endl;    
    //if(el->pt()>7) cout<<"Ele"<<setw(20)<<iEvent.id().event()<<setw(20)<<distance(electron_pat->begin(),el)<<setw(20)<<el->pt()<<setw(20)<<fabs(el->eta())<<setw(20)<<fabs(el->gsfTrack()->dxy(firstGoodVertex.position()))<<setw(20)<<fabs(el->gsfTrack()->dz(firstGoodVertex.position()))<<setw(20)<<miniIso/el->pt()<<setw(20)<<fabs(el->dB(pat::Electron::PV3D))/el->edB(pat::Electron::PV3D)<<setw(20)<<nontrigelemva_vl<<setw(20)<<el->gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS)<<setw(20)<<el->passConversionVeto()<<setw(20)<<miniIso<<setw(20)<<miniIsoCh<<setw(20)<<miniIsoPUsub<<endl;
    if(!aele && el->gsfTrack().isNonnull()
       && el->pt()>7 && fabs(el->eta())<2.5
       && fabs(el->gsfTrack()->dxy(firstGoodVertex.position()))<=0.05 && fabs(el->gsfTrack()->dz(firstGoodVertex.position()))<0.1
       && miniIso/el->pt()<0.4 && fabs(el->dB(pat::Electron::PV3D))/el->edB(pat::Electron::PV3D)<8
       && nontrigelemva_vl 
       && el->gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS)<=1 && el->passConversionVeto() 
      ){
      //cout<<setw(10)<<"event"<<setw(10)<<"pT"<<setw(10)<<"Eta"<<setw(10)<<"Phi"<<setw(10)<<"E"<<setw(5)<<"pdgID"<<setw(5)<<"charge"<<setw(15)<<"miniIso"<<setw(15)<<"miniIsoCharged"<<setw(15)<<"miniIsoNeutral"<<setw(10)<<"jetPtRel"<<setw(10)<<"jetCSV"<<setw(10)<<"jetPtRatio"<<setw(10)<<"sipi3D"<<setw(10)<<"dxy"<<setw(10)<<"dz"<<setw(21)<<"segmentCompatibility"<<endl;
      cout<<"El"<<setw(10)<<iEvent.id().event()<<setw(10)<<el->pt()<<setw(10)<<el->eta()<<setw(10)<<el->phi()<<setw(10)<<el->energy()<<setw(5)<<el->pdgId()<<setw(5)<<el->charge()<<setw(15)<<miniIso/el->pt()<<setw(15)<<miniIsoCh<<setw(15)<<miniIsoPUsub<<setw(10)<<eleptrel<<setw(10)<<elejet_pfCombinedInclusiveSecondaryVertexV2BJetTags<<setw(10)<<el->pt()/elejet_pt<<setw(10)<<fabs(el->dB(pat::Electron::PV3D))/el->edB(pat::Electron::PV3D)<<setw(10)<<fabs(el->gsfTrack()->dxy(firstGoodVertex.position()))<<setw(10)<<fabs(el->gsfTrack()->dz(firstGoodVertex.position()))<<setw(21)<<mvaval_nonTrig<<endl;
      //if(el->hasUserFloat("ElectronMVAEstimatorRun2Spring15NonTrig25nsV1Values")) cout<<setw(21)<<el->userFloat("ElectronMVAEstimatorRun2Spring15NonTrig25nsV1Values")<<endl;
      //else cout<<setw(21)<<"No Ele MVA"<<endl;
      aele = true;
      //trigelemva from miniAOD (to be used at some point) 
      //bool nontrigelemva_vl = false;
      //if(el->hasUserFloat("ElectronMVAEstimatorRun2Spring15NonTrig25nsV1Values")){
      //  double ntelemva = el->userFloat("ElectronMVAEstimatorRun2Spring15NonTrig25nsV1Values");
      //  double eleta   = fabs(el->eta());
      //  if((eleta < 0.8                           && ntelemva > -0.11) ||
      //       (0.8 <= eleta && eleta < 1.479 && ntelemva > -0.35) || 
      //       (1.479 <= eleta && eleta < 500 && ntelemva > -0.55)         
      //      ){
      //      nontrigelemva_vl = true;
      //  }
      //}
*/
//Gen info
        //cout<<setw(20)<<"pT"<<setw(20)<<"eta"<<setw(20)<<"phi"<<setw(20)<<"energy"<<endl;
        //cout<<setw(20)<<genpart->pt()<<setw(20)<<genpart->eta()<<setw(20)<<genpart->phi()<<setw(20)<<genpart->energy()<<endl;
        //cout<<setw(20)<<"isPrompt"<<setw(20)<<"isfromtau"<<setw(20)<<"pdgId"<<endl;
        //cout<<setw(20)<<genpart->isPromptFinalState()<<setw(20)<<genpart->isDirectPromptTauDecayProductFinalState()<<setw(20)<<genpart->pdgId()<<endl;
int ElectronPatSelector::MatchingToTrigger(const edm::Event& iEvent, edm::Handle<pat::TriggerObjectStandAloneCollection> triggerObjects, edm::Handle<edm::TriggerResults> triggerBits, float eta, float phi){
  const edm::TriggerNames & triggerNames = iEvent.triggerNames(*triggerBits);
  float deltaRMin = 99.;
  for (pat::TriggerObjectStandAlone obj : *triggerObjects) { 
    obj.unpackPathNames(triggerNames);
    if (obj.hasFilterLabel("hltEle115CaloIdVTGsfTrkIdTGsfDphiFilter")) {
      //for (unsigned h = 0; h < obj.filterLabels().size(); ++h) std::cout << " " << obj.filterLabels()[h];  
      //cout<<obj.pt()<<endl;
      float deltaPhi = TMath::Abs(phi-obj.phi());
      float deltaEta = eta-obj.eta();
      if(deltaPhi > TMath::Pi()) deltaPhi = TMath::TwoPi() - deltaPhi;
      float deltaR = TMath::Sqrt(deltaEta*deltaEta + deltaPhi*deltaPhi);
      if(deltaR<deltaRMin) deltaRMin=deltaR;
    }
  }
  int result = 0;
  if(deltaRMin<0.3) result = 1;
  return result;
}
