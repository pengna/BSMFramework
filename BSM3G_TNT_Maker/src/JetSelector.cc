#include "BSMFramework/BSM3G_TNT_Maker/interface/JetSelector.h"
JetSelector::JetSelector(std::string name, TTree* tree, bool debug, const pset& iConfig, edm::ConsumesCollector && ic):
  baseTree(name,tree,debug)
{
  vtx_h_        = ic.consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"));
  jets_         = ic.consumes<pat::JetCollection >(iConfig.getParameter<edm::InputTag>("jets"));
  muon_h_             = ic.consumes<edm::View<pat::Muon> >(iConfig.getParameter<edm::InputTag>("muons"));
  electron_pat_        = ic.consumes<edm::View<pat::Electron> >(iConfig.getParameter<edm::InputTag>("patElectrons"));
  taus_                   = ic.consumes<edm::View<pat::Tau> >(iConfig.getParameter<edm::InputTag>("taus"));
  puppijets_    = ic.consumes<pat::JetCollection >(iConfig.getParameter<edm::InputTag>("jetsPUPPI"));
  qgToken_      = ic.consumes<edm::ValueMap<float>>(edm::InputTag("QGTagger", "qgLikelihood"));
  axis2Token_   = ic.consumes<edm::ValueMap<float>>(edm::InputTag("QGTagger", "axis2"));
  ptDToken_     = ic.consumes<edm::ValueMap<float>>(edm::InputTag("QGTagger", "ptD"));
  multToken_    = ic.consumes<edm::ValueMap<int>>(edm::InputTag("QGTagger", "mult"));
  rhopogHandle_ = ic.consumes<double>(edm::InputTag("fixedGridRhoFastjetAll"));
  //rhoJERHandle_ = ic.consumes<double>(edm::InputTag("fixedGridRhoAll"));
  jecPayloadNamesAK4PFchsMC1_   = iConfig.getParameter<edm::FileInPath>("jecPayloadNamesAK4PFchsMC1");
  jecPayloadNamesAK4PFchsMC2_   = iConfig.getParameter<edm::FileInPath>("jecPayloadNamesAK4PFchsMC2");
  jecPayloadNamesAK4PFchsMC3_   = iConfig.getParameter<edm::FileInPath>("jecPayloadNamesAK4PFchsMC3");
  jecPayloadNamesAK4PFchsMCUnc_ = iConfig.getParameter<edm::FileInPath>("jecPayloadNamesAK4PFchsMCUnc");
  jecPayloadNamesAK4PFchsDATA1_   = iConfig.getParameter<edm::FileInPath>("jecPayloadNamesAK4PFchsDATA1");
  jecPayloadNamesAK4PFchsDATA2_   = iConfig.getParameter<edm::FileInPath>("jecPayloadNamesAK4PFchsDATA2");
  jecPayloadNamesAK4PFchsDATA3_   = iConfig.getParameter<edm::FileInPath>("jecPayloadNamesAK4PFchsDATA3");
  jecPayloadNamesAK4PFchsDATA4_   = iConfig.getParameter<edm::FileInPath>("jecPayloadNamesAK4PFchsDATA4");
  jecPayloadNamesAK4PFchsDATAUnc_ = iConfig.getParameter<edm::FileInPath>("jecPayloadNamesAK4PFchsDATAUnc");
  jecPayloadNamesAK4PFPuppiMC1_   = iConfig.getParameter<edm::FileInPath>("jecPayloadNamesAK4PFPuppiMC1");
  jecPayloadNamesAK4PFPuppiMC2_   = iConfig.getParameter<edm::FileInPath>("jecPayloadNamesAK4PFPuppiMC2");
  jecPayloadNamesAK4PFPuppiMC3_   = iConfig.getParameter<edm::FileInPath>("jecPayloadNamesAK4PFPuppiMC3");
  jecPayloadNamesAK4PFPuppiMCUnc_ = iConfig.getParameter<edm::FileInPath>("jecPayloadNamesAK4PFPuppiMCUnc");
  jecPayloadNamesAK4PFPuppiDATA1_   = iConfig.getParameter<edm::FileInPath>("jecPayloadNamesAK4PFPuppiDATA1");
  jecPayloadNamesAK4PFPuppiDATA2_   = iConfig.getParameter<edm::FileInPath>("jecPayloadNamesAK4PFPuppiDATA2");
  jecPayloadNamesAK4PFPuppiDATA3_   = iConfig.getParameter<edm::FileInPath>("jecPayloadNamesAK4PFPuppiDATA3");
  jecPayloadNamesAK4PFPuppiDATA4_   = iConfig.getParameter<edm::FileInPath>("jecPayloadNamesAK4PFPuppiDATA4");
  jecPayloadNamesAK4PFPuppiDATAUnc_ = iConfig.getParameter<edm::FileInPath>("jecPayloadNamesAK4PFPuppiDATAUnc");
  jerAK4PFchs_     = iConfig.getParameter<edm::FileInPath>("jerAK4PFchs").fullPath();
  jerAK4PFchsSF_   = iConfig.getParameter<edm::FileInPath>("jerAK4PFchsSF").fullPath();
  jerAK4PFPuppi_   = iConfig.getParameter<edm::FileInPath>("jerAK4PFPuppi").fullPath();
  jerAK4PFPuppiSF_ = iConfig.getParameter<edm::FileInPath>("jerAK4PFPuppiSF").fullPath();
  _Jet_pt_min     = iConfig.getParameter<double>("Jet_pt_min");
  _Muon_pt_min        = iConfig.getParameter<double>("Muon_pt_min");
  _Muon_eta_max       = iConfig.getParameter<double>("Muon_eta_max");
  _patElectron_pt_min  = iConfig.getParameter<double>("patElectron_pt_min");
  _patElectron_eta_max = iConfig.getParameter<double>("patElectron_eta_max");
  _Tau_pt_min     	  = iConfig.getParameter<double>("Tau_pt_min");
  _Tau_eta_max    	  = iConfig.getParameter<double>("Tau_eta_max");
  _super_TNT      = iConfig.getParameter<bool>("super_TNT");
  _is_data = iConfig.getParameter<bool>("is_data");
  _PuppiVar = iConfig.getParameter<bool>("PuppiVar");
  _qglVar             = iConfig.getParameter<bool>("qglVar");
  _dataEra             = iConfig.getParameter<int>("dataEra");
  JECInitialization();
  SetBranches();
}
JetSelector::~JetSelector(){
  delete tree_;
}
void JetSelector::Fill(const edm::Event& iEvent){
  Clear();
  /////
  //   Recall collections
  /////  
  edm::Handle<reco::VertexCollection> vertices;
  iEvent.getByToken(vtx_h_, vertices);
  edm::Handle<pat::JetCollection> jets; 
  iEvent.getByToken(jets_, jets);                                         
  edm::Handle<edm::View<pat::Muon> > muon_h;
  iEvent.getByToken(muon_h_, muon_h);
  edm::Handle<edm::View<pat::Electron> > electron_pat;
  iEvent.getByToken(electron_pat_, electron_pat);
  edm::Handle<edm::View<pat::Tau> > taus;
  iEvent.getByToken(taus_, taus);
  
  edm::Handle<pat::JetCollection> puppijets;                                       
  iEvent.getByToken(puppijets_, puppijets); 
  edm::Handle<edm::ValueMap<float>> qgHandle;
  iEvent.getByToken(qgToken_, qgHandle);
  edm::Handle<edm::ValueMap<float>> axis2Handle;
  iEvent.getByToken(axis2Token_, axis2Handle);
  edm::Handle<edm::ValueMap<float>> ptDHandle;
  iEvent.getByToken(ptDToken_, ptDHandle);
  edm::Handle<edm::ValueMap<int>> multHandle;
  iEvent.getByToken(multToken_, multHandle);
  edm::Handle<double> rhoHandle;
  iEvent.getByToken(rhopogHandle_,rhoHandle);
  double rho = *rhoHandle;
  //edm::Handle<double> rhoJERHandle;
  //iEvent.getByToken(rhoJERHandle_,rhoJERHandle);
  //double rhoJER = *rhoJERHandle;
  /////
  //   Get jet information
  /////  
  //bool ajet = false;
  ////slimmedJets
  int ij = 0;
  int nEvent = iEvent.id().event();
  // random seed for stochastic method
  std::uint32_t m_nomVar = 1;
  unsigned int runNum_uint = static_cast<unsigned int>(iEvent.id().run());
  unsigned int lumiNum_uint = static_cast<unsigned int>(iEvent.id().luminosityBlock());
  unsigned int evNum_uint = static_cast<unsigned int>(iEvent.id().event());
  unsigned int jet0eta = uint32_t((*jets).empty() ? 0 : (*jets)[0].eta() / 0.01);
  std::uint32_t seed = m_nomVar + jet0eta + (lumiNum_uint << 10) + (runNum_uint << 20) + evNum_uint;
  m_random_generator.seed(seed);
 
  for(const pat::Jet &j : *jets){ 
    //Acceptance
    if(j.pt()<_Jet_pt_min){ij++; continue;}
    //Kinematics
    Jet_pt.push_back(j.pt());  
    Jet_eta.push_back(j.eta());       
    Jet_phi.push_back(j.phi());       
    Jet_energy.push_back(j.energy());
    Jet_mass.push_back(j.mass()); 
    Jet_px.push_back(j.px());   
    Jet_py.push_back(j.py());          
    Jet_pz.push_back(j.pz());          
    Jet_Uncorr_pt.push_back(j.correctedJet("Uncorrected").pt()); 
    Jet_L1corr_pt.push_back(j.correctedJet(1).pt());                
    //double L1_corr = j.jecFactor("L1FastJet")/j.jecFactor("Uncorrected");
    //std::cout<<iEvent.id().event()  <<"jet pt "<<j.pt()<<" L1Factor "<<L1_corr<<" closetJet unCorr p4"<<j.correctedJet("Uncorrected").p4() <<" closetJet corr0 Pt " << j.correctedJet(0)<<  "L1corrjet "<< j.correctedJet(1) << " jecFactor.L1FastJet "<< j.jecFactor("L1FastJet")<< " jecFactor.Uncorrected "<< j.jecFactor("Uncorrected") <<std::endl;
    
    //ID
    Jet_pfCombinedInclusiveSecondaryVertexV2BJetTags.push_back(j.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags"));
    Jet_pfCombinedMVAV2BJetTags.push_back(j.bDiscriminator("pfCombinedMVAV2BJetTags"));
    Jet_pfJetProbabilityBJetTags.push_back(j.bDiscriminator("pfJetProbabilityBJetTags"));
    Jet_pfCombinedCvsLJetTags.push_back(j.bDiscriminator("pfCombinedCvsLJetTags"));
    Jet_pfCombinedCvsBJetTags.push_back(j.bDiscriminator("pfCombinedCvsBJetTags"));
    Jet_pfDeepCSVBJetTags.push_back(j.bDiscriminator("pfDeepCSVJetTags:probb") + j.bDiscriminator("pfDeepCSVJetTags:probbb"));
    Jet_pfDeepCSVProbb.push_back(j.bDiscriminator("pfDeepCSVJetTags:probb"));
    Jet_pfDeepCSVProbbb.push_back(j.bDiscriminator("pfDeepCSVJetTags:probbb"));
    Jet_pfDeepCSVProbc.push_back(j.bDiscriminator("pfDeepCSVJetTags:probc"));
    Jet_pfDeepCSVProbcc.push_back(j.bDiscriminator("pfDeepCSVJetTags:probcc"));
    Jet_pfDeepCSVProbudsg.push_back(j.bDiscriminator("pfDeepCSVJetTags:probudsg"));
    Jet_pfDeepFlavourBJetTags.push_back(j.bDiscriminator("pfDeepFlavourJetTags:probb") + j.bDiscriminator("pfDeepFlavourJetTags:probbb")+j.bDiscriminator("pfDeepFlavourJetTags:problepb"));
    Jet_pfDeepFlavourProbb.push_back(j.bDiscriminator("pfDeepFlavourJetTags:probb"));
    Jet_pfDeepFlavourProbbb.push_back(j.bDiscriminator("pfDeepFlavourJetTags:probbb"));
    Jet_pfDeepFlavourProblepb.push_back(j.bDiscriminator("pfDeepFlavourJetTags:problepb"));
    Jet_pfDeepFlavourProbc.push_back(j.bDiscriminator("pfDeepFlavourJetTags:probc"));
    Jet_pfDeepFlavourProbuds.push_back(j.bDiscriminator("pfDeepFlavourJetTags:probuds"));
    Jet_pfDeepFlavourProbg.push_back(j.bDiscriminator("pfDeepFlavourJetTags:probg"));
    Jet_pileupId.push_back(j.userFloat("pileupJetId:fullDiscriminant"));
    Jet_isPFJet.push_back(j.isPFJet());
    Jet_isCaloJet.push_back(j.isCaloJet());
    if(_qglVar){
        edm::Ref<pat::JetCollection> jetRef(jets, ij);
        Jet_qg.push_back((*qgHandle)[jetRef]);
        double axis1 = -1;
        double axis2 = -1;
        double ptD = -1;
        double mult = -1;
        computeQG(j, ptD, mult, axis1, axis2, false);
        Jet_axis1.push_back(axis1);
        Jet_axis2.push_back(axis2);
        Jet_ptD.push_back(ptD);
        Jet_mult.push_back(mult);
    }else{
        Jet_qg.push_back(-999);
        Jet_axis1.push_back(-999);
        Jet_axis2.push_back(-999);
        Jet_ptD.push_back(-999);
        Jet_mult.push_back(-999);
    }
    //Match Indices
    //match muons
    int currmupos = 0;
    int mu_number = 0;
    for(edm::View<pat::Muon>::const_iterator mu = muon_h->begin(); mu != muon_h->end(); mu++){
        //Acceptance 
        if(mu->pt() < _Muon_pt_min)         continue;
        if(fabs(mu->eta()) > _Muon_eta_max) continue; 
        bool isMatch = false; 
        for(unsigned int i1 = 0 ; i1 < mu->numberOfSourceCandidatePtrs();i1++){
            const reco::CandidatePtr  &c1s=mu->sourceCandidatePtr(i1);
            for(unsigned int i2 = 0 ; i2 < j.numberOfSourceCandidatePtrs();i2++) {
                const reco::CandidatePtr  &c2s=j.sourceCandidatePtr(i2);
                if(c2s== c1s){
                    isMatch = true;
                    break;
                }
            }
            if(isMatch)break;
        }
        if(isMatch){
            Jet_mu_indices.push_back(currmupos);
            mu_number++;
            //std::cout<< " Jet p4 "<< j.p4()<< " Match to Muon p4 "<< mu->p4() << " mu_number " << mu_number << " currmupos "<< currmupos << std::endl;
        }
        currmupos++;
    }
    //std::cout << " mu_number outside the Muon match loop " << mu_number << std::endl;
    Jet_mu_number.push_back(mu_number);
    //match electrons
    int currelepos = 0;
    int ele_number = 0;
    for(edm::View<pat::Electron>::const_iterator el = electron_pat->begin(); el != electron_pat->end(); el++){
        //Acceptance 
        if(el->pt() < _patElectron_pt_min)         continue;
        if(fabs(el->eta()) > _patElectron_eta_max) continue;  
        bool isMatch = false; 
        for(unsigned int i1 = 0 ; i1 < el->numberOfSourceCandidatePtrs();i1++){
            const reco::CandidatePtr  &c1s=el->sourceCandidatePtr(i1);
            for(unsigned int i2 = 0 ; i2 < j.numberOfSourceCandidatePtrs();i2++) {
                const reco::CandidatePtr  &c2s=j.sourceCandidatePtr(i2);
                if(c2s== c1s){
                    isMatch = true;
                    break;
                }
            }
            if(isMatch)break;
        }
        if(isMatch){
            Jet_ele_indices.push_back(currelepos);
            ele_number ++;
        }
        currelepos++;
    }
    Jet_ele_number.push_back(ele_number);
    //match taus
    int currtaupos = 0;
    int tau_number = 0;
    for(edm::View<pat::Tau>::const_iterator tau = taus->begin(); tau != taus->end(); tau++){
        //Acceptance 
        if(tau->pt() < _Tau_pt_min) continue;
        if(fabs(tau->eta()) > _Tau_eta_max) continue;
        bool isMatch = false; 
        for(unsigned int i1 = 0 ; i1 < tau->numberOfSourceCandidatePtrs();i1++){
            const reco::CandidatePtr  &c1s=tau->sourceCandidatePtr(i1);
            for(unsigned int i2 = 0 ; i2 < j.numberOfSourceCandidatePtrs();i2++) {
                const reco::CandidatePtr  &c2s=j.sourceCandidatePtr(i2);
                if(c2s== c1s){
                    isMatch = true;
                    break;
                }
            }
            if(isMatch)break;
        }
        if(isMatch){
            Jet_tau_indices.push_back(currtaupos);
            tau_number ++;
        }
        currtaupos++;
    }
    Jet_tau_number.push_back(tau_number);
    //
    //Energy
    if(j.isPFJet() || j.isJPTJet()){
        Jet_neutralHadEnergyFraction.push_back(j.neutralHadronEnergyFraction());                               
        Jet_neutralEmEnergyFraction.push_back(j.neutralEmEnergyFraction());                                   
        Jet_chargedHadronEnergyFraction.push_back(j.chargedHadronEnergyFraction());                               
        Jet_chargedEmEnergyFraction.push_back(j.chargedEmEnergyFraction());                              
        Jet_muonEnergyFraction.push_back(j.muonEnergyFraction());                                  
        Jet_electronEnergy.push_back(j.electronEnergy());                               
        Jet_photonEnergy.push_back(j.photonEnergy());                                 
        Jet_numberOfConstituents.push_back(j.chargedMultiplicity() + j.neutralMultiplicity());                                  
        Jet_chargedMultiplicity.push_back(j.chargedMultiplicity());
    }else{
        Jet_neutralHadEnergyFraction.push_back(-999);
        Jet_neutralEmEnergyFraction.push_back(-999);
        Jet_chargedHadronEnergyFraction.push_back(-999);
        Jet_chargedEmEnergyFraction.push_back(-999);
        Jet_muonEnergyFraction.push_back(-999);
        Jet_electronEnergy.push_back(-999);
        Jet_photonEnergy.push_back(-999);
        Jet_numberOfConstituents.push_back(-999);
        Jet_chargedMultiplicity.push_back(-999);
    }
    if(j.isCaloJet()) Jet_emEnergyFraction.push_back(j.emEnergyFraction());
    else              Jet_emEnergyFraction.push_back(-999);
    //Other prop
    Jet_vtxMass.push_back(-99);//j.userFloat("vtxMass"));
    Jet_vtxNtracks.push_back(-99);//j.userFloat("vtxNtracks"));
    Jet_vtx3DVal.push_back(-99);//j.userFloat("vtx3DVal"));
    Jet_vtx3DSig.push_back(-99);//j.userFloat("vtx3DSig"));
    //Jet Energy Corrections and Uncertainties
    double corrAK4PFchs     = 1;
    double corrUpAK4PFchs   = 1;
    double corrDownAK4PFchs = 1;
    //==================================Factorised JES Unc.=========================================
    double corrUpAK4PFchs_AbsoluteStat_up   = 1;
    double corrDownAK4PFchs_AbsoluteStat_down = 1;
    double corrUpAK4PFchs_AbsoluteScale_up   = 1;
    double corrDownAK4PFchs_AbsoluteScale_down = 1;
    double corrUpAK4PFchs_AbsoluteFlavMap_up   = 1;
    double corrDownAK4PFchs_AbsoluteFlavMap_down = 1;
    double corrUpAK4PFchs_AbsoluteMPFBias_up   = 1;
    double corrDownAK4PFchs_AbsoluteMPFBias_down = 1;
    double corrUpAK4PFchs_Fragmentation_up   = 1;
    double corrDownAK4PFchs_Fragmentation_down = 1;
    double corrUpAK4PFchs_SinglePionECAL_up   = 1;
    double corrDownAK4PFchs_SinglePionECAL_down = 1;
    double corrUpAK4PFchs_SinglePionHCAL_up   = 1;
    double corrDownAK4PFchs_SinglePionHCAL_down = 1;
    double corrUpAK4PFchs_FlavorQCD_up   = 1;
    double corrDownAK4PFchs_FlavorQCD_down = 1;
    double corrUpAK4PFchs_TimePtEta_up   = 1;
    double corrDownAK4PFchs_TimePtEta_down = 1;
    double corrUpAK4PFchs_RelativeJEREC1_up   = 1;
    double corrDownAK4PFchs_RelativeJEREC1_down = 1;
    double corrUpAK4PFchs_RelativeJEREC2_up   = 1;
    double corrDownAK4PFchs_RelativeJEREC2_down = 1;
    double corrUpAK4PFchs_RelativeJERHF_up   = 1;
    double corrDownAK4PFchs_RelativeJERHF_down = 1;
    double corrUpAK4PFchs_RelativePtBB_up   = 1;
    double corrDownAK4PFchs_RelativePtBB_down = 1;
    double corrUpAK4PFchs_RelativePtEC1_up   = 1;
    double corrDownAK4PFchs_RelativePtEC1_down = 1;
    double corrUpAK4PFchs_RelativePtEC2_up   = 1;
    double corrDownAK4PFchs_RelativePtEC2_down = 1;
    double corrUpAK4PFchs_RelativePtHF_up   = 1;
    double corrDownAK4PFchs_RelativePtHF_down = 1;
    double corrUpAK4PFchs_RelativeBal_up   = 1;
    double corrDownAK4PFchs_RelativeBal_down = 1;
    double corrUpAK4PFchs_RelativeFSR_up   = 1;
    double corrDownAK4PFchs_RelativeFSR_down = 1;
    double corrUpAK4PFchs_RelativeStatFSR_up   = 1;
    double corrDownAK4PFchs_RelativeStatFSR_down = 1;
    double corrUpAK4PFchs_RelativeStatEC_up   = 1;
    double corrDownAK4PFchs_RelativeStatEC_down = 1;
    double corrUpAK4PFchs_RelativeStatHF_up   = 1;
    double corrDownAK4PFchs_RelativeStatHF_down = 1;
    double corrUpAK4PFchs_PileUpDataMC_up   = 1;
    double corrDownAK4PFchs_PileUpDataMC_down = 1;
    double corrUpAK4PFchs_PileUpPtRef_up   = 1;
    double corrDownAK4PFchs_PileUpPtRef_down = 1;
    double corrUpAK4PFchs_PileUpPtBB_up   = 1;
    double corrDownAK4PFchs_PileUpPtBB_down = 1;
    double corrUpAK4PFchs_PileUpPtEC1_up   = 1;
    double corrDownAK4PFchs_PileUpPtEC1_down = 1;
    double corrUpAK4PFchs_PileUpPtEC2_up   = 1;
    double corrDownAK4PFchs_PileUpPtEC2_down = 1;
    double corrUpAK4PFchs_PileUpPtHF_up   = 1;
    double corrDownAK4PFchs_PileUpPtHF_down = 1;
    double corrUpAK4PFchs_PileUpMuZero_up   = 1;
    double corrDownAK4PFchs_PileUpMuZero_down = 1;
    double corrUpAK4PFchs_PileUpEnvelope_up   = 1;
    double corrDownAK4PFchs_PileUpEnvelope_down = 1;
    double corrUpAK4PFchs_SubTotalPileUp_up   = 1;
    double corrDownAK4PFchs_SubTotalPileUp_down = 1;
    double corrUpAK4PFchs_SubTotalRelative_up   = 1;
    double corrDownAK4PFchs_SubTotalRelative_down = 1;
    double corrUpAK4PFchs_SubTotalPt_up   = 1;
    double corrDownAK4PFchs_SubTotalPt_down = 1;
    double corrUpAK4PFchs_SubTotalScale_up   = 1;
    double corrDownAK4PFchs_SubTotalScale_down = 1;
    double corrUpAK4PFchs_SubTotalAbsolute_up   = 1;
    double corrDownAK4PFchs_SubTotalAbsolute_down = 1;
    double corrUpAK4PFchs_SubTotalMC_up   = 1;
    double corrDownAK4PFchs_SubTotalMC_down = 1;
    double corrUpAK4PFchs_TotalNoFlavor_up   = 1;
    double corrDownAK4PFchs_TotalNoFlavor_down = 1;
    double corrUpAK4PFchs_TotalNoFlavorNoTime_up   = 1;
    double corrDownAK4PFchs_TotalNoFlavorNoTime_down = 1;
    double corrUpAK4PFchs_FlavorZJet_up   = 1;
    double corrDownAK4PFchs_FlavorZJet_down = 1;
    double corrUpAK4PFchs_FlavorPhotonJet_up   = 1;
    double corrDownAK4PFchs_FlavorPhotonJet_down = 1;
    double corrUpAK4PFchs_FlavorPureGluon_up   = 1;
    double corrDownAK4PFchs_FlavorPureGluon_down = 1;
    double corrUpAK4PFchs_FlavorPureQuark_up   = 1;
    double corrDownAK4PFchs_FlavorPureQuark_down = 1;
    double corrUpAK4PFchs_FlavorPureCharm_up   = 1;
    double corrDownAK4PFchs_FlavorPureCharm_down = 1;
    double corrUpAK4PFchs_FlavorPureBottom_up   = 1;
    double corrDownAK4PFchs_FlavorPureBottom_down = 1;
    //===========================================================================
    
    reco::Candidate::LorentzVector uncorrJetAK4PFchs = j.correctedP4(0);
    if(!_is_data){
      jecAK4PFchsMC_->setJetEta( uncorrJetAK4PFchs.eta()    );
      jecAK4PFchsMC_->setJetPt ( uncorrJetAK4PFchs.pt()     );
      jecAK4PFchsMC_->setJetE  ( uncorrJetAK4PFchs.energy() );
      jecAK4PFchsMC_->setRho	( rho  );
      jecAK4PFchsMC_->setNPV	( vertices->size()  );
      jecAK4PFchsMC_->setJetA  ( j.jetArea()	     );
      corrAK4PFchs = jecAK4PFchsMC_->getCorrection();
      jecAK4PFchsMCUnc_->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_->getUncertainty(1)));
      jecAK4PFchsMCUnc_->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_->getUncertainty(-1)) );
      //std::cout << " uncorr jet pt : " << uncorrJetAK4PFchs.pt() << " uncorr jet eta : "<< uncorrJetAK4PFchs.eta() << " jet pt " << j.pt() << " jet eta " << j.eta()<<std::endl;
      
            //================================================Factorised JES Unc.===================================================
      jecAK4PFchsMCUnc_AbsoluteStat->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_AbsoluteStat->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_AbsoluteStat_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_AbsoluteStat->getUncertainty(1)));
      jecAK4PFchsMCUnc_AbsoluteStat->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_AbsoluteStat->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_AbsoluteStat_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_AbsoluteStat->getUncertainty(-1)) );


      jecAK4PFchsMCUnc_AbsoluteScale->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_AbsoluteScale->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_AbsoluteScale_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_AbsoluteScale->getUncertainty(1)));
      jecAK4PFchsMCUnc_AbsoluteScale->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_AbsoluteScale->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_AbsoluteScale_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_AbsoluteScale->getUncertainty(-1)) );


      jecAK4PFchsMCUnc_AbsoluteFlavMap->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_AbsoluteFlavMap->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_AbsoluteFlavMap_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_AbsoluteFlavMap->getUncertainty(1)));
      jecAK4PFchsMCUnc_AbsoluteFlavMap->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_AbsoluteFlavMap->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );

      corrDownAK4PFchs_AbsoluteFlavMap_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_AbsoluteFlavMap->getUncertainty(-1)) );

      jecAK4PFchsMCUnc_AbsoluteMPFBias->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_AbsoluteMPFBias->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_AbsoluteMPFBias_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_AbsoluteMPFBias->getUncertainty(1)));
      jecAK4PFchsMCUnc_AbsoluteMPFBias->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_AbsoluteMPFBias->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );

      corrDownAK4PFchs_AbsoluteMPFBias_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_AbsoluteMPFBias->getUncertainty(-1)) );

      jecAK4PFchsMCUnc_Fragmentation->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_Fragmentation->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_Fragmentation_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_Fragmentation->getUncertainty(1)));
      jecAK4PFchsMCUnc_Fragmentation->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_Fragmentation->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_Fragmentation_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_Fragmentation->getUncertainty(-1)) );


      jecAK4PFchsMCUnc_SinglePionECAL->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_SinglePionECAL->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_SinglePionECAL_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_SinglePionECAL->getUncertainty(1)));
      jecAK4PFchsMCUnc_SinglePionECAL->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_SinglePionECAL->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );

      corrDownAK4PFchs_SinglePionECAL_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_SinglePionECAL->getUncertainty(-1)) );

      jecAK4PFchsMCUnc_SinglePionHCAL->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_SinglePionHCAL->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_SinglePionHCAL_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_SinglePionHCAL->getUncertainty(1)));
      jecAK4PFchsMCUnc_SinglePionHCAL->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_SinglePionHCAL->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_SinglePionHCAL_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_SinglePionHCAL->getUncertainty(-1)) );

      jecAK4PFchsMCUnc_FlavorQCD->setJetEta( uncorrJetAK4PFchs.eta() );

      jecAK4PFchsMCUnc_FlavorQCD->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_FlavorQCD_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_FlavorQCD->getUncertainty(1)));
      jecAK4PFchsMCUnc_FlavorQCD->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_FlavorQCD->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_FlavorQCD_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_FlavorQCD->getUncertainty(-1)) );

      jecAK4PFchsMCUnc_TimePtEta->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_TimePtEta->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_TimePtEta_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_TimePtEta->getUncertainty(1)));
      jecAK4PFchsMCUnc_TimePtEta->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_TimePtEta->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_TimePtEta_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_TimePtEta->getUncertainty(-1)) );


      jecAK4PFchsMCUnc_RelativeJEREC1->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativeJEREC1->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_RelativeJEREC1_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_RelativeJEREC1->getUncertainty(1)));
      jecAK4PFchsMCUnc_RelativeJEREC1->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativeJEREC1->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_RelativeJEREC1_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_RelativeJEREC1->getUncertainty(-1)) );

      jecAK4PFchsMCUnc_RelativeJEREC2->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativeJEREC2->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_RelativeJEREC2_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_RelativeJEREC2->getUncertainty(1)));
      jecAK4PFchsMCUnc_RelativeJEREC2->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativeJEREC2->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );

      corrDownAK4PFchs_RelativeJEREC2_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_RelativeJEREC2->getUncertainty(-1)) );

      jecAK4PFchsMCUnc_RelativeJERHF->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativeJERHF->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_RelativeJERHF_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_RelativeJERHF->getUncertainty(1)));
      jecAK4PFchsMCUnc_RelativeJERHF->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativeJERHF->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_RelativeJERHF_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_RelativeJERHF->getUncertainty(-1)) );


      jecAK4PFchsMCUnc_RelativePtBB->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativePtBB->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_RelativePtBB_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_RelativePtBB->getUncertainty(1)));
      jecAK4PFchsMCUnc_RelativePtBB->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativePtBB->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_RelativePtBB_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_RelativePtBB->getUncertainty(-1)) );


      jecAK4PFchsMCUnc_RelativePtEC1->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativePtEC1->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_RelativePtEC1_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_RelativePtEC1->getUncertainty(1)));
      jecAK4PFchsMCUnc_RelativePtEC1->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativePtEC1->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_RelativePtEC1_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_RelativePtEC1->getUncertainty(-1)) );


      jecAK4PFchsMCUnc_RelativePtEC2->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativePtEC2->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_RelativePtEC2_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_RelativePtEC2->getUncertainty(1)));
      jecAK4PFchsMCUnc_RelativePtEC2->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativePtEC2->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_RelativePtEC2_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_RelativePtEC2->getUncertainty(-1)) );


      jecAK4PFchsMCUnc_RelativePtHF->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativePtHF->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_RelativePtHF_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_RelativePtHF->getUncertainty(1)));
      jecAK4PFchsMCUnc_RelativePtHF->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativePtHF->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_RelativePtHF_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_RelativePtHF->getUncertainty(-1)) );


      jecAK4PFchsMCUnc_RelativeBal->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativeBal->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_RelativeBal_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_RelativeBal->getUncertainty(1)));
      jecAK4PFchsMCUnc_RelativeBal->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativeBal->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_RelativeBal_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_RelativeBal->getUncertainty(-1)) );


      jecAK4PFchsMCUnc_RelativeFSR->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativeFSR->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_RelativeFSR_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_RelativeFSR->getUncertainty(1)));
      jecAK4PFchsMCUnc_RelativeFSR->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativeFSR->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_RelativeFSR_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_RelativeFSR->getUncertainty(-1)) );


      jecAK4PFchsMCUnc_RelativeStatFSR->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativeStatFSR->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_RelativeStatFSR_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_RelativeStatFSR->getUncertainty(1)));
      jecAK4PFchsMCUnc_RelativeStatFSR->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativeStatFSR->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );

      corrDownAK4PFchs_RelativeStatFSR_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_RelativeStatFSR->getUncertainty(-1)) );

      jecAK4PFchsMCUnc_RelativeStatEC->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativeStatEC->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_RelativeStatEC_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_RelativeStatEC->getUncertainty(1)));
      jecAK4PFchsMCUnc_RelativeStatEC->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativeStatEC->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );

      corrDownAK4PFchs_RelativeStatEC_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_RelativeStatEC->getUncertainty(-1)) );

      jecAK4PFchsMCUnc_RelativeStatHF->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativeStatHF->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_RelativeStatHF_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_RelativeStatHF->getUncertainty(1)));
      jecAK4PFchsMCUnc_RelativeStatHF->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_RelativeStatHF->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );

      corrDownAK4PFchs_RelativeStatHF_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_RelativeStatHF->getUncertainty(-1)) );

      jecAK4PFchsMCUnc_PileUpDataMC->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_PileUpDataMC->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_PileUpDataMC_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_PileUpDataMC->getUncertainty(1)));
      jecAK4PFchsMCUnc_PileUpDataMC->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_PileUpDataMC->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_PileUpDataMC_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_PileUpDataMC->getUncertainty(-1)) );


      jecAK4PFchsMCUnc_PileUpPtRef->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_PileUpPtRef->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_PileUpPtRef_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_PileUpPtRef->getUncertainty(1)));
      jecAK4PFchsMCUnc_PileUpPtRef->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_PileUpPtRef->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_PileUpPtRef_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_PileUpPtRef->getUncertainty(-1)) );


      jecAK4PFchsMCUnc_PileUpPtBB->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_PileUpPtBB->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_PileUpPtBB_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_PileUpPtBB->getUncertainty(1)));
      jecAK4PFchsMCUnc_PileUpPtBB->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_PileUpPtBB->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_PileUpPtBB_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_PileUpPtBB->getUncertainty(-1)) );


      jecAK4PFchsMCUnc_PileUpPtEC1->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_PileUpPtEC1->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_PileUpPtEC1_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_PileUpPtEC1->getUncertainty(1)));
      jecAK4PFchsMCUnc_PileUpPtEC1->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_PileUpPtEC1->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_PileUpPtEC1_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_PileUpPtEC1->getUncertainty(-1)) );


      jecAK4PFchsMCUnc_PileUpPtEC2->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_PileUpPtEC2->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_PileUpPtEC2_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_PileUpPtEC2->getUncertainty(1)));
      jecAK4PFchsMCUnc_PileUpPtEC2->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_PileUpPtEC2->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_PileUpPtEC2_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_PileUpPtEC2->getUncertainty(-1)) );


      jecAK4PFchsMCUnc_PileUpPtHF->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_PileUpPtHF->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_PileUpPtHF_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_PileUpPtHF->getUncertainty(1)));
      jecAK4PFchsMCUnc_PileUpPtHF->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_PileUpPtHF->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_PileUpPtHF_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_PileUpPtHF->getUncertainty(-1)) );


      jecAK4PFchsMCUnc_PileUpMuZero->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_PileUpMuZero->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_PileUpMuZero_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_PileUpMuZero->getUncertainty(1)));
      jecAK4PFchsMCUnc_PileUpMuZero->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_PileUpMuZero->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_PileUpMuZero_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_PileUpMuZero->getUncertainty(-1)) );


      jecAK4PFchsMCUnc_PileUpEnvelope->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_PileUpEnvelope->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_PileUpEnvelope_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_PileUpEnvelope->getUncertainty(1)));
      jecAK4PFchsMCUnc_PileUpEnvelope->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_PileUpEnvelope->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );

      corrDownAK4PFchs_PileUpEnvelope_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_PileUpEnvelope->getUncertainty(-1)) );

      jecAK4PFchsMCUnc_SubTotalPileUp->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_SubTotalPileUp->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_SubTotalPileUp_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_SubTotalPileUp->getUncertainty(1)));
      jecAK4PFchsMCUnc_SubTotalPileUp->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_SubTotalPileUp->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );

      corrDownAK4PFchs_SubTotalPileUp_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_SubTotalPileUp->getUncertainty(-1)) );

      jecAK4PFchsMCUnc_SubTotalRelative->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_SubTotalRelative->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_SubTotalRelative_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_SubTotalRelative->getUncertainty(1)));
      jecAK4PFchsMCUnc_SubTotalRelative->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_SubTotalRelative->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );

      corrDownAK4PFchs_SubTotalRelative_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_SubTotalRelative->getUncertainty(-1)) );

      jecAK4PFchsMCUnc_SubTotalPt->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_SubTotalPt->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_SubTotalPt_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_SubTotalPt->getUncertainty(1)));
      jecAK4PFchsMCUnc_SubTotalPt->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_SubTotalPt->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_SubTotalPt_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_SubTotalPt->getUncertainty(-1)) );


      jecAK4PFchsMCUnc_SubTotalScale->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_SubTotalScale->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_SubTotalScale_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_SubTotalScale->getUncertainty(1)));
      jecAK4PFchsMCUnc_SubTotalScale->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_SubTotalScale->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_SubTotalScale_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_SubTotalScale->getUncertainty(-1)) );


      jecAK4PFchsMCUnc_SubTotalAbsolute->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_SubTotalAbsolute->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_SubTotalAbsolute_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_SubTotalAbsolute->getUncertainty(1)));
      jecAK4PFchsMCUnc_SubTotalAbsolute->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_SubTotalAbsolute->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );

      corrDownAK4PFchs_SubTotalAbsolute_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_SubTotalAbsolute->getUncertainty(-1)) );

      jecAK4PFchsMCUnc_SubTotalMC->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_SubTotalMC->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_SubTotalMC_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_SubTotalMC->getUncertainty(1)));
      jecAK4PFchsMCUnc_SubTotalMC->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_SubTotalMC->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_SubTotalMC_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_SubTotalMC->getUncertainty(-1)) );


      jecAK4PFchsMCUnc_TotalNoFlavor->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_TotalNoFlavor->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_TotalNoFlavor_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_TotalNoFlavor->getUncertainty(1)));
      jecAK4PFchsMCUnc_TotalNoFlavor->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_TotalNoFlavor->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_TotalNoFlavor_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_TotalNoFlavor->getUncertainty(-1)) );


      jecAK4PFchsMCUnc_TotalNoFlavorNoTime->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_TotalNoFlavorNoTime->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_TotalNoFlavorNoTime_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_TotalNoFlavorNoTime->getUncertainty(1)));
      jecAK4PFchsMCUnc_TotalNoFlavorNoTime->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_TotalNoFlavorNoTime->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );

      corrDownAK4PFchs_TotalNoFlavorNoTime_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_TotalNoFlavorNoTime->getUncertainty(-1)) );

      jecAK4PFchsMCUnc_FlavorZJet->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_FlavorZJet->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_FlavorZJet_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_FlavorZJet->getUncertainty(1)));
      jecAK4PFchsMCUnc_FlavorZJet->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_FlavorZJet->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs_FlavorZJet_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_FlavorZJet->getUncertainty(-1)) );


      jecAK4PFchsMCUnc_FlavorPhotonJet->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_FlavorPhotonJet->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_FlavorPhotonJet_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_FlavorPhotonJet->getUncertainty(1)));
      jecAK4PFchsMCUnc_FlavorPhotonJet->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_FlavorPhotonJet->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );

      corrDownAK4PFchs_FlavorPhotonJet_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_FlavorPhotonJet->getUncertainty(-1)) );

      jecAK4PFchsMCUnc_FlavorPureGluon->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_FlavorPureGluon->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_FlavorPureGluon_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_FlavorPureGluon->getUncertainty(1)));
      jecAK4PFchsMCUnc_FlavorPureGluon->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_FlavorPureGluon->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );

      corrDownAK4PFchs_FlavorPureGluon_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_FlavorPureGluon->getUncertainty(-1)) );

      jecAK4PFchsMCUnc_FlavorPureQuark->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_FlavorPureQuark->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_FlavorPureQuark_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_FlavorPureQuark->getUncertainty(1)));
      jecAK4PFchsMCUnc_FlavorPureQuark->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_FlavorPureQuark->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );

      corrDownAK4PFchs_FlavorPureQuark_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_FlavorPureQuark->getUncertainty(-1)) );

      jecAK4PFchsMCUnc_FlavorPureCharm->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_FlavorPureCharm->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_FlavorPureCharm_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_FlavorPureCharm->getUncertainty(1)));
      jecAK4PFchsMCUnc_FlavorPureCharm->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_FlavorPureCharm->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );

      corrDownAK4PFchs_FlavorPureCharm_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_FlavorPureCharm->getUncertainty(-1)) );

      jecAK4PFchsMCUnc_FlavorPureBottom->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_FlavorPureBottom->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs_FlavorPureBottom_up = corrAK4PFchs * (1 + fabs(jecAK4PFchsMCUnc_FlavorPureBottom->getUncertainty(1)));
      jecAK4PFchsMCUnc_FlavorPureBottom->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsMCUnc_FlavorPureBottom->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );

      corrDownAK4PFchs_FlavorPureBottom_down = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsMCUnc_FlavorPureBottom->getUncertainty(-1)) );
    } else {
      jecAK4PFchsDATA_->setJetEta( uncorrJetAK4PFchs.eta()    );
      jecAK4PFchsDATA_->setJetPt ( uncorrJetAK4PFchs.pt()     );
      jecAK4PFchsDATA_->setJetE  ( uncorrJetAK4PFchs.energy() );
      jecAK4PFchsDATA_->setRho	( rho  );
      jecAK4PFchsDATA_->setNPV	( vertices->size()  );
      jecAK4PFchsDATA_->setJetA  ( j.jetArea()	     );
      corrAK4PFchs = jecAK4PFchsDATA_->getCorrection();
      jecAK4PFchsDATAUnc_->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsDATAUnc_->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrUpAK4PFchs = corrAK4PFchs * (1 + fabs(jecAK4PFchsDATAUnc_->getUncertainty(1)));
      jecAK4PFchsDATAUnc_->setJetEta( uncorrJetAK4PFchs.eta() );
      jecAK4PFchsDATAUnc_->setJetPt( corrAK4PFchs * uncorrJetAK4PFchs.pt() );
      corrDownAK4PFchs = corrAK4PFchs * ( 1 - fabs(jecAK4PFchsDATAUnc_->getUncertainty(-1)) );
    }
    //std::cout<<iEvent.id()<<" "<< j.pt() << " "  << j.correctedP4(1).pt()  <<" JesSF "<< corrAK4PFchs << " JesSFup "<< corrUpAK4PFchs << " JesSFdown "<< corrDownAK4PFchs<< std::endl;
    Jet_JesSF.push_back(corrAK4PFchs);
    Jet_JesSFup.push_back(corrUpAK4PFchs);
    Jet_JesSFdown.push_back(corrDownAK4PFchs);
    
    //================================== Store Factorised JEC info =============================================
    Jet_JesSF_AbsoluteStat_up.push_back(corrUpAK4PFchs_AbsoluteStat_up);
    Jet_JesSF_AbsoluteStat_down.push_back(corrDownAK4PFchs_AbsoluteStat_down);
    Jet_JesSF_AbsoluteScale_up.push_back(corrUpAK4PFchs_AbsoluteScale_up);
    Jet_JesSF_AbsoluteScale_down.push_back(corrDownAK4PFchs_AbsoluteScale_down);
    Jet_JesSF_AbsoluteFlavMap_up.push_back(corrUpAK4PFchs_AbsoluteFlavMap_up);
    Jet_JesSF_AbsoluteFlavMap_down.push_back(corrDownAK4PFchs_AbsoluteFlavMap_down);
    Jet_JesSF_AbsoluteMPFBias_up.push_back(corrUpAK4PFchs_AbsoluteMPFBias_up);
    Jet_JesSF_AbsoluteMPFBias_down.push_back(corrDownAK4PFchs_AbsoluteMPFBias_down);
    Jet_JesSF_Fragmentation_up.push_back(corrUpAK4PFchs_Fragmentation_up);
    Jet_JesSF_Fragmentation_down.push_back(corrDownAK4PFchs_Fragmentation_down);
    Jet_JesSF_SinglePionECAL_up.push_back(corrUpAK4PFchs_SinglePionECAL_up);
    Jet_JesSF_SinglePionECAL_down.push_back(corrDownAK4PFchs_SinglePionECAL_down);
    Jet_JesSF_SinglePionHCAL_up.push_back(corrUpAK4PFchs_SinglePionHCAL_up);
    Jet_JesSF_SinglePionHCAL_down.push_back(corrDownAK4PFchs_SinglePionHCAL_down);
    Jet_JesSF_FlavorQCD_up.push_back(corrUpAK4PFchs_FlavorQCD_up);
    Jet_JesSF_FlavorQCD_down.push_back(corrDownAK4PFchs_FlavorQCD_down);
    Jet_JesSF_TimePtEta_up.push_back(corrUpAK4PFchs_TimePtEta_up);
    Jet_JesSF_TimePtEta_down.push_back(corrDownAK4PFchs_TimePtEta_down);
    Jet_JesSF_RelativeJEREC1_up.push_back(corrUpAK4PFchs_RelativeJEREC1_up);
    Jet_JesSF_RelativeJEREC1_down.push_back(corrDownAK4PFchs_RelativeJEREC1_down);
    Jet_JesSF_RelativeJEREC2_up.push_back(corrUpAK4PFchs_RelativeJEREC2_up);
    Jet_JesSF_RelativeJEREC2_down.push_back(corrDownAK4PFchs_RelativeJEREC2_down);
    Jet_JesSF_RelativeJERHF_up.push_back(corrUpAK4PFchs_RelativeJERHF_up);
    Jet_JesSF_RelativeJERHF_down.push_back(corrDownAK4PFchs_RelativeJERHF_down);
    Jet_JesSF_RelativePtBB_up.push_back(corrUpAK4PFchs_RelativePtBB_up);
    Jet_JesSF_RelativePtBB_down.push_back(corrDownAK4PFchs_RelativePtBB_down);
    Jet_JesSF_RelativePtEC1_up.push_back(corrUpAK4PFchs_RelativePtEC1_up);
    Jet_JesSF_RelativePtEC1_down.push_back(corrDownAK4PFchs_RelativePtEC1_down);
    Jet_JesSF_RelativePtEC2_up.push_back(corrUpAK4PFchs_RelativePtEC2_up);
    Jet_JesSF_RelativePtEC2_down.push_back(corrDownAK4PFchs_RelativePtEC2_down);
    Jet_JesSF_RelativePtHF_up.push_back(corrUpAK4PFchs_RelativePtHF_up);
    Jet_JesSF_RelativePtHF_down.push_back(corrDownAK4PFchs_RelativePtHF_down);
    Jet_JesSF_RelativeBal_up.push_back(corrUpAK4PFchs_RelativeBal_up);
    Jet_JesSF_RelativeBal_down.push_back(corrDownAK4PFchs_RelativeBal_down);
    Jet_JesSF_RelativeFSR_up.push_back(corrUpAK4PFchs_RelativeFSR_up);
    Jet_JesSF_RelativeFSR_down.push_back(corrDownAK4PFchs_RelativeFSR_down);
    Jet_JesSF_RelativeStatFSR_up.push_back(corrUpAK4PFchs_RelativeStatFSR_up);
    Jet_JesSF_RelativeStatFSR_down.push_back(corrDownAK4PFchs_RelativeStatFSR_down);
    Jet_JesSF_RelativeStatEC_up.push_back(corrUpAK4PFchs_RelativeStatEC_up);
    Jet_JesSF_RelativeStatEC_down.push_back(corrDownAK4PFchs_RelativeStatEC_down);
    Jet_JesSF_RelativeStatHF_up.push_back(corrUpAK4PFchs_RelativeStatHF_up);
    Jet_JesSF_RelativeStatHF_down.push_back(corrDownAK4PFchs_RelativeStatHF_down);
    Jet_JesSF_PileUpDataMC_up.push_back(corrUpAK4PFchs_PileUpDataMC_up);
    Jet_JesSF_PileUpDataMC_down.push_back(corrDownAK4PFchs_PileUpDataMC_down);
    Jet_JesSF_PileUpPtRef_up.push_back(corrUpAK4PFchs_PileUpPtRef_up);
    Jet_JesSF_PileUpPtRef_down.push_back(corrDownAK4PFchs_PileUpPtRef_down);
    Jet_JesSF_PileUpPtBB_up.push_back(corrUpAK4PFchs_PileUpPtBB_up);
    Jet_JesSF_PileUpPtBB_down.push_back(corrDownAK4PFchs_PileUpPtBB_down);
    Jet_JesSF_PileUpPtEC1_up.push_back(corrUpAK4PFchs_PileUpPtEC1_up);
    Jet_JesSF_PileUpPtEC1_down.push_back(corrDownAK4PFchs_PileUpPtEC1_down);
    Jet_JesSF_PileUpPtEC2_up.push_back(corrUpAK4PFchs_PileUpPtEC2_up);
    Jet_JesSF_PileUpPtEC2_down.push_back(corrDownAK4PFchs_PileUpPtEC2_down);
    Jet_JesSF_PileUpPtHF_up.push_back(corrUpAK4PFchs_PileUpPtHF_up);
    Jet_JesSF_PileUpPtHF_down.push_back(corrDownAK4PFchs_PileUpPtHF_down);
    Jet_JesSF_PileUpMuZero_up.push_back(corrUpAK4PFchs_PileUpMuZero_up);
    Jet_JesSF_PileUpMuZero_down.push_back(corrDownAK4PFchs_PileUpMuZero_down);
    Jet_JesSF_PileUpEnvelope_up.push_back(corrUpAK4PFchs_PileUpEnvelope_up);
    Jet_JesSF_PileUpEnvelope_down.push_back(corrDownAK4PFchs_PileUpEnvelope_down);
    Jet_JesSF_SubTotalPileUp_up.push_back(corrUpAK4PFchs_SubTotalPileUp_up);
    Jet_JesSF_SubTotalPileUp_down.push_back(corrDownAK4PFchs_SubTotalPileUp_down);
    Jet_JesSF_SubTotalRelative_up.push_back(corrUpAK4PFchs_SubTotalRelative_up);
    Jet_JesSF_SubTotalRelative_down.push_back(corrDownAK4PFchs_SubTotalRelative_down);
    Jet_JesSF_SubTotalPt_up.push_back(corrUpAK4PFchs_SubTotalPt_up);
    Jet_JesSF_SubTotalPt_down.push_back(corrDownAK4PFchs_SubTotalPt_down);
    Jet_JesSF_SubTotalScale_up.push_back(corrUpAK4PFchs_SubTotalScale_up);
    Jet_JesSF_SubTotalScale_down.push_back(corrDownAK4PFchs_SubTotalScale_down);
    Jet_JesSF_SubTotalAbsolute_up.push_back(corrUpAK4PFchs_SubTotalAbsolute_up);
    Jet_JesSF_SubTotalAbsolute_down.push_back(corrDownAK4PFchs_SubTotalAbsolute_down);
    Jet_JesSF_SubTotalMC_up.push_back(corrUpAK4PFchs_SubTotalMC_up);
    Jet_JesSF_SubTotalMC_down.push_back(corrDownAK4PFchs_SubTotalMC_down);
    Jet_JesSF_TotalNoFlavor_up.push_back(corrUpAK4PFchs_TotalNoFlavor_up);
    Jet_JesSF_TotalNoFlavor_down.push_back(corrDownAK4PFchs_TotalNoFlavor_down);
    Jet_JesSF_TotalNoFlavorNoTime_up.push_back(corrUpAK4PFchs_TotalNoFlavorNoTime_up);
    Jet_JesSF_TotalNoFlavorNoTime_down.push_back(corrDownAK4PFchs_TotalNoFlavorNoTime_down);
    Jet_JesSF_FlavorZJet_up.push_back(corrUpAK4PFchs_FlavorZJet_up);
    Jet_JesSF_FlavorZJet_down.push_back(corrDownAK4PFchs_FlavorZJet_down);
    Jet_JesSF_FlavorPhotonJet_up.push_back(corrUpAK4PFchs_FlavorPhotonJet_up);
    Jet_JesSF_FlavorPhotonJet_down.push_back(corrDownAK4PFchs_FlavorPhotonJet_down);
    Jet_JesSF_FlavorPureGluon_up.push_back(corrUpAK4PFchs_FlavorPureGluon_up);
    Jet_JesSF_FlavorPureGluon_down.push_back(corrDownAK4PFchs_FlavorPureGluon_down);
    Jet_JesSF_FlavorPureQuark_up.push_back(corrUpAK4PFchs_FlavorPureQuark_up);
    Jet_JesSF_FlavorPureQuark_down.push_back(corrDownAK4PFchs_FlavorPureQuark_down);
    Jet_JesSF_FlavorPureCharm_up.push_back(corrUpAK4PFchs_FlavorPureCharm_up);
    Jet_JesSF_FlavorPureCharm_down.push_back(corrDownAK4PFchs_FlavorPureCharm_down);
    Jet_JesSF_FlavorPureBottom_up.push_back(corrUpAK4PFchs_FlavorPureBottom_up);
    Jet_JesSF_FlavorPureBottom_down.push_back(corrDownAK4PFchs_FlavorPureBottom_down);
    //===============================================================================
    //JER scale factor and uncertainties
    float JERScaleFactor     = 1; 
    float JERScaleFactorUP   = 1;
    float JERScaleFactorDOWN = 1;
    //if(!_is_data) GetJER(j, corrAK4PFchs, rho, true, JERScaleFactor, JERScaleFactorUP, JERScaleFactorDOWN);
    if(!_is_data) Getjer(j, corrAK4PFchs, rho, true, JERScaleFactor, JERScaleFactorUP, JERScaleFactorDOWN, nEvent);
    Jet_JerSF.push_back(JERScaleFactor);
    Jet_JerSFup.push_back(JERScaleFactorUP);
    Jet_JerSFdown.push_back(JERScaleFactorDOWN);
    //MC
    if(!_is_data) {
      Jet_partonFlavour.push_back(j.partonFlavour());
      Jet_hadronFlavour.push_back(j.hadronFlavour());
    }
    /////
    //   TTH variables
    /////
    /*
    cout<<setiosflags(ios::fixed)<<setprecision(5);
    if(!ajet){
      cout<<setw(20)<<iEvent.id().event()<<setw(20)<<j.pt()<<setw(20)<<j.eta()<<setw(20)<<j.phi()<<setw(20)<<j.energy()<<setw(20)<<j.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags")<<setw(20)<<j.correctedJet("Uncorrected").pt()<<setw(20)<<j.correctedJet("Uncorrected").energy()<<setw(20)<<j.correctedJet("L1FastJet").pt()<<setw(20)<<j.correctedJet("L1FastJet").energy()<<setw(20)<<j.correctedJet("L2Relative").pt()<<setw(20)<<j.correctedJet("L2Relative").energy()<<setw(20)<<j.correctedJet("L3Absolute").pt()<<setw(20)<<j.correctedJet("L3Absolute").energy()<<setw(20)<<j.jecFactor("Uncorrected")<<setw(20)<<j.jecFactor("L1FastJet")<<setw(20)<<j.jecFactor("L2Relative")<<setw(20)<<j.jecFactor("L3Absolute")<<setw(20)<<endl;
      ajet = true;
    }
    */
   ij++;
  } 
  ////slimmedJetsPuppi
  if(_PuppiVar){
    for(const pat::Jet &j : *puppijets){ 
      //Acceptance
      if(j.pt() < _Jet_pt_min) continue;
      //Kinematics
      Jet_puppi_pt.push_back(j.pt());  
      Jet_puppi_eta.push_back(j.eta());       
      Jet_puppi_phi.push_back(j.phi());       
      Jet_puppi_energy.push_back(j.energy());
      Jet_puppi_mass.push_back(j.mass()); 
      Jet_puppi_px.push_back(j.px());   
      Jet_puppi_py.push_back(j.py());          
      Jet_puppi_pz.push_back(j.pz());          
      Jet_puppi_Uncorr_pt.push_back(j.correctedJet("Uncorrected").pt());                
      //ID
      Jet_puppi_pfCombinedInclusiveSecondaryVertexV2BJetTags.push_back(j.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags"));
      Jet_puppi_pfCombinedMVAV2BJetTags.push_back(j.bDiscriminator("pfCombinedMVAV2BJetTags"));
      Jet_puppi_pfJetProbabilityBJetTags.push_back(j.bDiscriminator("pfJetProbabilityBJetTags"));
      Jet_puppi_pfCombinedCvsLJetTags.push_back(j.bDiscriminator("pfCombinedCvsLJetTags"));
      Jet_puppi_pfCombinedCvsBJetTags.push_back(j.bDiscriminator("pfCombinedCvsBJetTags"));
      Jet_puppi_pileupId.push_back(j.userFloat("pileupJetId:fullDiscriminant"));
      Jet_puppi_isPFJet.push_back(j.isPFJet());
      Jet_puppi_isCaloJet.push_back(j.isCaloJet());
      //Energy
      Jet_puppi_neutralHadEnergyFraction.push_back(j.neutralHadronEnergyFraction());                               
      Jet_puppi_neutralEmEnergyFraction.push_back(j.neutralEmEnergyFraction());                                   
      Jet_puppi_chargedHadronEnergyFraction.push_back(j.chargedHadronEnergyFraction());                               
      Jet_puppi_chargedEmEnergyFraction.push_back(j.chargedEmEnergyFraction());                              
      Jet_puppi_muonEnergyFraction.push_back(j.muonEnergyFraction());                                  
      Jet_puppi_electronEnergy.push_back(j.electronEnergy());                               
      Jet_puppi_photonEnergy.push_back(j.photonEnergy());                                 
      if(j.isCaloJet()) Jet_puppi_emEnergyFraction.push_back(j.emEnergyFraction());
      else              Jet_puppi_emEnergyFraction.push_back(-999);
      //Other prop
      Jet_puppi_numberOfConstituents.push_back(j.chargedMultiplicity() + j.neutralMultiplicity());                                  
      Jet_puppi_chargedMultiplicity.push_back(j.chargedMultiplicity());
      Jet_puppi_vtxMass.push_back(-99);//j.userFloat("vtxMass"));
      Jet_puppi_vtxNtracks.push_back(-99);//j.userFloat("vtxNtracks"));
      Jet_puppi_vtx3DVal.push_back(-99);//j.userFloat("vtx3DVal"));
      Jet_puppi_vtx3DSig.push_back(-99);//j.userFloat("vtx3DSig"));
      //Jet Energy Corrections and Uncertainties
      double corrAK4PFPuppi     = 1;
      double corrUpAK4PFPuppi   = 1;
      double corrDownAK4PFPuppi = 1;
      reco::Candidate::LorentzVector uncorrJetAK4PFPuppi = j.correctedP4(0);
      if(!_is_data){
	jecAK4PFPuppiMC_->setJetEta( uncorrJetAK4PFPuppi.eta()    );
	jecAK4PFPuppiMC_->setJetPt ( uncorrJetAK4PFPuppi.pt()     );
	jecAK4PFPuppiMC_->setJetE  ( uncorrJetAK4PFPuppi.energy() );
	jecAK4PFPuppiMC_->setRho	( rho  );
	jecAK4PFPuppiMC_->setNPV	( vertices->size()  );
	jecAK4PFPuppiMC_->setJetA  ( j.jetArea()	     );
	corrAK4PFPuppi = jecAK4PFPuppiMC_->getCorrection();
	jecAK4PFPuppiMCUnc_->setJetEta( uncorrJetAK4PFPuppi.eta() );
	jecAK4PFPuppiMCUnc_->setJetPt( corrAK4PFPuppi * uncorrJetAK4PFPuppi.pt() );
	corrUpAK4PFPuppi = corrAK4PFPuppi * (1 + fabs(jecAK4PFPuppiMCUnc_->getUncertainty(1)));
	jecAK4PFPuppiMCUnc_->setJetEta( uncorrJetAK4PFPuppi.eta() );
	jecAK4PFPuppiMCUnc_->setJetPt( corrAK4PFPuppi * uncorrJetAK4PFPuppi.pt() );
	corrDownAK4PFPuppi = corrAK4PFPuppi * ( 1 - fabs(jecAK4PFPuppiMCUnc_->getUncertainty(-1)) );
      } else {
	jecAK4PFPuppiDATA_->setJetEta( uncorrJetAK4PFPuppi.eta()    );
	jecAK4PFPuppiDATA_->setJetPt ( uncorrJetAK4PFPuppi.pt()     );
	jecAK4PFPuppiDATA_->setJetE  ( uncorrJetAK4PFPuppi.energy() );
	jecAK4PFPuppiDATA_->setRho	( rho  );
	jecAK4PFPuppiDATA_->setNPV	( vertices->size()  );
	jecAK4PFPuppiDATA_->setJetA  ( j.jetArea()	     );
	corrAK4PFPuppi = jecAK4PFPuppiDATA_->getCorrection();
	jecAK4PFPuppiDATAUnc_->setJetEta( uncorrJetAK4PFPuppi.eta() );
	jecAK4PFPuppiDATAUnc_->setJetPt( corrAK4PFPuppi * uncorrJetAK4PFPuppi.pt() );
	corrUpAK4PFPuppi = corrAK4PFPuppi * (1 + fabs(jecAK4PFPuppiDATAUnc_->getUncertainty(1)));
	jecAK4PFPuppiDATAUnc_->setJetEta( uncorrJetAK4PFPuppi.eta() );
	jecAK4PFPuppiDATAUnc_->setJetPt( corrAK4PFPuppi * uncorrJetAK4PFPuppi.pt() );
	corrDownAK4PFPuppi = corrAK4PFPuppi * ( 1 - fabs(jecAK4PFPuppiDATAUnc_->getUncertainty(-1)) );
      }
      Jet_puppi_JesSF.push_back(corrAK4PFPuppi);
      Jet_puppi_JesSFup.push_back(corrUpAK4PFPuppi);
      Jet_puppi_JesSFdown.push_back(corrDownAK4PFPuppi);
      //JER scale factor and uncertainties
      float JERScaleFactor     = 1; 
      float JERScaleFactorUP   = 1;
      float JERScaleFactorDOWN = 1;
      //if(!_is_data) GetJER(j, corrAK4PFPuppi, rho, false, JERScaleFactor, JERScaleFactorUP, JERScaleFactorDOWN);
      if(!_is_data) Getjer(j, corrAK4PFPuppi, rho, false, JERScaleFactor, JERScaleFactorUP, JERScaleFactorDOWN, nEvent);
      Jet_puppi_JerSF.push_back(JERScaleFactor);
      Jet_puppi_JerSFup.push_back(JERScaleFactorUP);
      Jet_puppi_JerSFdown.push_back(JERScaleFactorDOWN);
      //delete jecUnc;
      //MC
      if(!_is_data) {
	Jet_puppi_partonFlavour.push_back(j.partonFlavour());
	Jet_puppi_hadronFlavour.push_back(j.hadronFlavour());
      } 
    }
  }
}
void JetSelector::JECInitialization(){
  //AK4chs - MC: Get the factorized jet corrector parameters. 
  //std::cout<< " JEC Initialize  " << std::endl;
  std::vector<std::string> jecPayloadNamesAK4PFchsMC_;
  jecPayloadNamesAK4PFchsMC_.push_back(jecPayloadNamesAK4PFchsMC1_.fullPath());
  jecPayloadNamesAK4PFchsMC_.push_back(jecPayloadNamesAK4PFchsMC2_.fullPath());
  jecPayloadNamesAK4PFchsMC_.push_back(jecPayloadNamesAK4PFchsMC3_.fullPath());
  std::vector<JetCorrectorParameters> vParAK4PFchsMC;
  for ( std::vector<std::string>::const_iterator payloadBegin = jecPayloadNamesAK4PFchsMC_.begin(),
	  payloadEnd = jecPayloadNamesAK4PFchsMC_.end(), ipayload = payloadBegin; ipayload != payloadEnd; ++ipayload ) {
    JetCorrectorParameters pars(*ipayload);
    vParAK4PFchsMC.push_back(pars);
  }
  //std::cout<< " JEC Initialize jecAK4PFchsMC_ " << std::endl;
  jecAK4PFchsMC_    = boost::shared_ptr<FactorizedJetCorrector>  ( new FactorizedJetCorrector(vParAK4PFchsMC) );
  //jecAK4PFchsMCUnc_ = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(jecPayloadNamesAK4PFchsMCUnc_.fullPath()) );
  jecAK4PFchsMCUnc_   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "Total"))));
  //std::cout<< " JEC Initialize Start Components " << std::endl;
  
  jecAK4PFchsMCUnc_AbsoluteStat   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "AbsoluteStat"))));
  jecAK4PFchsMCUnc_AbsoluteScale   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "AbsoluteScale"))));
  jecAK4PFchsMCUnc_AbsoluteFlavMap   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "AbsoluteFlavMap"))));
  jecAK4PFchsMCUnc_AbsoluteMPFBias   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "AbsoluteMPFBias"))));
  jecAK4PFchsMCUnc_Fragmentation   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "Fragmentation"))));
  jecAK4PFchsMCUnc_SinglePionECAL   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "SinglePionECAL"))));
  jecAK4PFchsMCUnc_SinglePionHCAL   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "SinglePionHCAL"))));
  jecAK4PFchsMCUnc_FlavorQCD   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "FlavorQCD"))));
  jecAK4PFchsMCUnc_TimePtEta   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "TimePtEta"))));
  jecAK4PFchsMCUnc_RelativeJEREC1   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "RelativeJEREC1"))));
  jecAK4PFchsMCUnc_RelativeJEREC2   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "RelativeJEREC2"))));
  jecAK4PFchsMCUnc_RelativeJERHF   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "RelativeJERHF"))));
  jecAK4PFchsMCUnc_RelativePtBB   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "RelativePtBB"))));
  jecAK4PFchsMCUnc_RelativePtEC1   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "RelativePtEC1"))));
  jecAK4PFchsMCUnc_RelativePtEC2   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "RelativePtEC2"))));
  jecAK4PFchsMCUnc_RelativePtHF   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "RelativePtHF"))));
  jecAK4PFchsMCUnc_RelativeBal   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "RelativeBal"))));
  jecAK4PFchsMCUnc_RelativeFSR   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "RelativeFSR"))));
  jecAK4PFchsMCUnc_RelativeStatFSR   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "RelativeStatFSR"))));
  jecAK4PFchsMCUnc_RelativeStatEC   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "RelativeStatEC"))));
  jecAK4PFchsMCUnc_RelativeStatHF   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "RelativeStatHF"))));
  jecAK4PFchsMCUnc_PileUpDataMC   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "PileUpDataMC"))));
  jecAK4PFchsMCUnc_PileUpPtRef   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "PileUpPtRef"))));
  jecAK4PFchsMCUnc_PileUpPtBB   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "PileUpPtBB"))));
  jecAK4PFchsMCUnc_PileUpPtEC1   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "PileUpPtEC1"))));
  jecAK4PFchsMCUnc_PileUpPtEC2   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "PileUpPtEC2"))));
  jecAK4PFchsMCUnc_PileUpPtHF   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "PileUpPtHF"))));
  jecAK4PFchsMCUnc_PileUpMuZero   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "PileUpMuZero"))));
  jecAK4PFchsMCUnc_PileUpEnvelope   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "PileUpEnvelope"))));
  jecAK4PFchsMCUnc_SubTotalPileUp   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "SubTotalPileUp"))));
  jecAK4PFchsMCUnc_SubTotalRelative   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "SubTotalRelative"))));
  jecAK4PFchsMCUnc_SubTotalPt   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "SubTotalPt"))));
  jecAK4PFchsMCUnc_SubTotalScale   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "SubTotalScale"))));
  jecAK4PFchsMCUnc_SubTotalAbsolute   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "SubTotalAbsolute"))));
  jecAK4PFchsMCUnc_SubTotalMC   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "SubTotalMC"))));
  jecAK4PFchsMCUnc_TotalNoFlavor   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "TotalNoFlavor"))));
  jecAK4PFchsMCUnc_TotalNoFlavorNoTime   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "TotalNoFlavorNoTime"))));
  jecAK4PFchsMCUnc_FlavorZJet   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "FlavorZJet"))));
  jecAK4PFchsMCUnc_FlavorPhotonJet   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "FlavorPhotonJet"))));
  jecAK4PFchsMCUnc_FlavorPureGluon   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "FlavorPureGluon"))));
  jecAK4PFchsMCUnc_FlavorPureQuark   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "FlavorPureQuark"))));
  jecAK4PFchsMCUnc_FlavorPureCharm   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "FlavorPureCharm"))));
  jecAK4PFchsMCUnc_FlavorPureBottom   = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(*(new JetCorrectorParameters(jecPayloadNamesAK4PFchsMCUnc_.fullPath(), "FlavorPureBottom"))));
  
  //AK4chs - DATA: Get the factorized jet corrector parameters. 
  std::vector<std::string> jecPayloadNamesAK4PFchsDATA_;
  jecPayloadNamesAK4PFchsDATA_.push_back(jecPayloadNamesAK4PFchsDATA1_.fullPath());
  jecPayloadNamesAK4PFchsDATA_.push_back(jecPayloadNamesAK4PFchsDATA2_.fullPath());
  jecPayloadNamesAK4PFchsDATA_.push_back(jecPayloadNamesAK4PFchsDATA3_.fullPath());
  jecPayloadNamesAK4PFchsDATA_.push_back(jecPayloadNamesAK4PFchsDATA4_.fullPath());
  std::vector<JetCorrectorParameters> vParAK4PFchsDATA;
  for ( std::vector<std::string>::const_iterator payloadBegin = jecPayloadNamesAK4PFchsDATA_.begin(),
	  payloadEnd = jecPayloadNamesAK4PFchsDATA_.end(), ipayload = payloadBegin; ipayload != payloadEnd; ++ipayload ) {
    JetCorrectorParameters pars(*ipayload);
    vParAK4PFchsDATA.push_back(pars);
  }
  jecAK4PFchsDATA_    = boost::shared_ptr<FactorizedJetCorrector>  ( new FactorizedJetCorrector(vParAK4PFchsDATA) );
  jecAK4PFchsDATAUnc_ = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(jecPayloadNamesAK4PFchsDATAUnc_.fullPath()) );
  //AK4Puppi - MC: Get the factorized jet corrector parameters. 
  std::vector<std::string> jecPayloadNamesAK4PFPuppiMC_;
  jecPayloadNamesAK4PFPuppiMC_.push_back(jecPayloadNamesAK4PFPuppiMC1_.fullPath());
  jecPayloadNamesAK4PFPuppiMC_.push_back(jecPayloadNamesAK4PFPuppiMC2_.fullPath());
  jecPayloadNamesAK4PFPuppiMC_.push_back(jecPayloadNamesAK4PFPuppiMC3_.fullPath());
  std::vector<JetCorrectorParameters> vParAK4PFPuppiMC;
  for ( std::vector<std::string>::const_iterator payloadBegin = jecPayloadNamesAK4PFPuppiMC_.begin(),
	  payloadEnd = jecPayloadNamesAK4PFPuppiMC_.end(), ipayload = payloadBegin; ipayload != payloadEnd; ++ipayload ) {
    JetCorrectorParameters pars(*ipayload);
    vParAK4PFPuppiMC.push_back(pars);
  }
  jecAK4PFPuppiMC_    = boost::shared_ptr<FactorizedJetCorrector>  ( new FactorizedJetCorrector(vParAK4PFPuppiMC) );
  jecAK4PFPuppiMCUnc_ = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(jecPayloadNamesAK4PFPuppiMCUnc_.fullPath()) );
  //AK4Puppi - DATA: Get the factorized jet corrector parameters. 
  std::vector<std::string> jecPayloadNamesAK4PFPuppiDATA_;
  jecPayloadNamesAK4PFPuppiDATA_.push_back(jecPayloadNamesAK4PFPuppiDATA1_.fullPath());
  jecPayloadNamesAK4PFPuppiDATA_.push_back(jecPayloadNamesAK4PFPuppiDATA2_.fullPath());
  jecPayloadNamesAK4PFPuppiDATA_.push_back(jecPayloadNamesAK4PFPuppiDATA3_.fullPath());
  jecPayloadNamesAK4PFPuppiDATA_.push_back(jecPayloadNamesAK4PFPuppiDATA4_.fullPath());
  std::vector<JetCorrectorParameters> vParAK4PFPuppiDATA;
  for ( std::vector<std::string>::const_iterator payloadBegin = jecPayloadNamesAK4PFPuppiDATA_.begin(),
	  payloadEnd = jecPayloadNamesAK4PFPuppiDATA_.end(), ipayload = payloadBegin; ipayload != payloadEnd; ++ipayload ) {
    JetCorrectorParameters pars(*ipayload);
    vParAK4PFPuppiDATA.push_back(pars);
  }
  jecAK4PFPuppiDATA_    = boost::shared_ptr<FactorizedJetCorrector>  ( new FactorizedJetCorrector(vParAK4PFPuppiDATA) );
  jecAK4PFPuppiDATAUnc_ = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(jecPayloadNamesAK4PFPuppiDATAUnc_.fullPath()) );
}
void JetSelector::SetBranches(){
  if(debug_) std::cout<<"setting branches: calling AddBranch of baseTree"<<std::endl;
  ////slimmedJets
  //Kinematics
  AddBranch(&Jet_pt        ,"Jet_pt");
  AddBranch(&Jet_eta       ,"Jet_eta");
  AddBranch(&Jet_phi       ,"Jet_phi");
  AddBranch(&Jet_energy    ,"Jet_energy");
  AddBranch(&Jet_mass      ,"Jet_mass");
  AddBranch(&Jet_px        ,"Jet_px");
  AddBranch(&Jet_py        ,"Jet_py");
  AddBranch(&Jet_pz        ,"Jet_pz");
  AddBranch(&Jet_Uncorr_pt ,"Jet_Uncorr_pt");
  AddBranch(&Jet_L1corr_pt ,"Jet_L1corr_pt");
  //ID
  AddBranch(&Jet_pfCombinedInclusiveSecondaryVertexV2BJetTags ,"Jet_pfCombinedInclusiveSecondaryVertexV2BJetTags");
  AddBranch(&Jet_pfCombinedMVAV2BJetTags                      ,"Jet_pfCombinedMVAV2BJetTags");
  AddBranch(&Jet_pfJetProbabilityBJetTags                     ,"Jet_pfJetProbabilityBJetTags");
  AddBranch(&Jet_pfCombinedCvsLJetTags                        ,"Jet_pfCombinedCvsLJetTags");
  AddBranch(&Jet_pfCombinedCvsBJetTags                        ,"Jet_pfCombinedCvsBJetTags");
  AddBranch(&Jet_pfDeepCSVBJetTags                           ,"Jet_pfDeepCSVBJetTags");
  AddBranch(&Jet_pfDeepCSVProbb                             ,"Jet_pfDeepCSVProbb");
  AddBranch(&Jet_pfDeepCSVProbbb                            ,"Jet_pfDeepCSVProbbb");
  AddBranch(&Jet_pfDeepCSVProbc                             ,"Jet_pfDeepCSVProbc");
  AddBranch(&Jet_pfDeepCSVProbcc                            ,"Jet_pfDeepCSVProbcc");
  AddBranch(&Jet_pfDeepCSVProbudsg                          ,"Jet_pfDeepCSVProbudsg");
  AddBranch(&Jet_pfDeepFlavourBJetTags                      ,"Jet_pfDeepFlavourBJetTags");
  AddBranch(&Jet_pfDeepFlavourProbb                             ,"Jet_pfDeepFlavourProbb");
  AddBranch(&Jet_pfDeepFlavourProbbb                             ,"Jet_pfDeepFlavourProbbb");
  AddBranch(&Jet_pfDeepFlavourProblepb                             ,"Jet_pfDeepFlavourProblepb");
  AddBranch(&Jet_pfDeepFlavourProbc                             ,"Jet_pfDeepFlavourProbc");
  AddBranch(&Jet_pfDeepFlavourProbuds                             ,"Jet_pfDeepFlavourProbuds");
  AddBranch(&Jet_pfDeepFlavourProbg                             ,"Jet_pfDeepFlavourProbg");
  AddBranch(&Jet_pileupId                                     ,"Jet_pileupId");
  AddBranch(&Jet_isPFJet                                      ,"Jet_isPFJet");
  AddBranch(&Jet_isCaloJet                                    ,"Jet_isCaloJet");
  AddBranch(&Jet_qg               ,"Jet_qg");
  AddBranch(&Jet_axis1            ,"Jet_axis1");
  AddBranch(&Jet_axis2            ,"Jet_axis2");
  AddBranch(&Jet_ptD              ,"Jet_ptD");
  AddBranch(&Jet_mult             ,"Jet_mult");
  //Match Indices
  AddBranch(&Jet_ele_indices             ,"Jet_ele_indices");
  AddBranch(&Jet_ele_number             ,"Jet_ele_number");
  AddBranch(&Jet_mu_indices             ,"Jet_mu_indices");
  AddBranch(&Jet_mu_number             ,"Jet_mu_number");
  AddBranch(&Jet_tau_indices             ,"Jet_tau_indices");
  AddBranch(&Jet_tau_number             ,"Jet_tau_number");
  //Energy
  AddBranch(&Jet_neutralHadEnergyFraction    ,"Jet_neutralHadEnergyFraction");
  AddBranch(&Jet_neutralEmEnergyFraction     ,"Jet_neutralEmEnergyFraction");
  AddBranch(&Jet_chargedHadronEnergyFraction ,"Jet_chargedHadronEnergyFraction");
  AddBranch(&Jet_chargedEmEnergyFraction     ,"Jet_chargedEmEnergyFraction");
  AddBranch(&Jet_muonEnergyFraction          ,"Jet_muonEnergyFraction");
  AddBranch(&Jet_electronEnergy              ,"Jet_electronEnergy");
  AddBranch(&Jet_photonEnergy                ,"Jet_photonEnergy");
  AddBranch(&Jet_emEnergyFraction            ,"Jet_emEnergyFraction");
  //Other prop
  AddBranch(&Jet_numberOfConstituents ,"Jet_numberOfConstituents");
  AddBranch(&Jet_chargedMultiplicity  ,"Jet_chargedMultiplicity");
  AddBranch(&Jet_vtxMass              ,"Jet_vtxMass");
  AddBranch(&Jet_vtxNtracks           ,"Jet_vtxNtracks");
  AddBranch(&Jet_vtx3DVal             ,"Jet_vtx3DVal");
  AddBranch(&Jet_vtx3DSig             ,"Jet_vtx3DSig");
  //Jet Energy Corrections and Uncertainties
  AddBranch(&Jet_JesSF                ,"Jet_JesSF");
  AddBranch(&Jet_JesSFup              ,"Jet_JesSFup");
  AddBranch(&Jet_JesSFdown            ,"Jet_JesSFdown");
  AddBranch(&Jet_JerSF                ,"Jet_JerSF");
  AddBranch(&Jet_JerSFup              ,"Jet_JerSFup");
  AddBranch(&Jet_JerSFdown            ,"Jet_JerSFdown");
  AddBranch(&Jet_JesSF_AbsoluteStat_up                ,"Jet_JesSF_AbsoluteStat_up");
  AddBranch(&Jet_JesSF_AbsoluteStat_down                ,"Jet_JesSF_AbsoluteStat_down");
  AddBranch(&Jet_JesSF_AbsoluteScale_up                ,"Jet_JesSF_AbsoluteScale_up");
  AddBranch(&Jet_JesSF_AbsoluteScale_down                ,"Jet_JesSF_AbsoluteScale_down");
  AddBranch(&Jet_JesSF_AbsoluteFlavMap_up                ,"Jet_JesSF_AbsoluteFlavMap_up");
  AddBranch(&Jet_JesSF_AbsoluteFlavMap_down                ,"Jet_JesSF_AbsoluteFlavMap_down");
  AddBranch(&Jet_JesSF_AbsoluteMPFBias_up                ,"Jet_JesSF_AbsoluteMPFBias_up");
  AddBranch(&Jet_JesSF_AbsoluteMPFBias_down                ,"Jet_JesSF_AbsoluteMPFBias_down");
  AddBranch(&Jet_JesSF_Fragmentation_up                ,"Jet_JesSF_Fragmentation_up");
  AddBranch(&Jet_JesSF_Fragmentation_down                ,"Jet_JesSF_Fragmentation_down");
  AddBranch(&Jet_JesSF_SinglePionECAL_up               ,"Jet_JesSF_SinglePionECAL_up");
  AddBranch(&Jet_JesSF_SinglePionECAL_down               ,"Jet_JesSF_SinglePionECAL_down");
  AddBranch(&Jet_JesSF_SinglePionHCAL_up                ,"Jet_JesSF_SinglePionHCAL_up");
  AddBranch(&Jet_JesSF_SinglePionHCAL_down                ,"Jet_JesSF_SinglePionHCAL_down");
  AddBranch(&Jet_JesSF_FlavorQCD_up                ,"Jet_JesSF_FlavorQCD_up");
  AddBranch(&Jet_JesSF_FlavorQCD_down                ,"Jet_JesSF_FlavorQCD_down");
  AddBranch(&Jet_JesSF_TimePtEta_up                ,"Jet_JesSF_TimePtEta_up");
  AddBranch(&Jet_JesSF_TimePtEta_down                ,"Jet_JesSF_TimePtEta_down");
  AddBranch(&Jet_JesSF_RelativeJEREC1_up                ,"Jet_JesSF_RelativeJEREC1_up");
  AddBranch(&Jet_JesSF_RelativeJEREC1_down                ,"Jet_JesSF_RelativeJEREC1_down");
  AddBranch(&Jet_JesSF_RelativeJEREC2_up                ,"Jet_JesSF_RelativeJEREC2_up");
  AddBranch(&Jet_JesSF_RelativeJEREC2_down                ,"Jet_JesSF_RelativeJEREC2_down");
  AddBranch(&Jet_JesSF_RelativeJERHF_up                ,"Jet_JesSF_RelativeJERHF_up");
  AddBranch(&Jet_JesSF_RelativeJERHF_down                ,"Jet_JesSF_RelativeJERHF_down");
  AddBranch(&Jet_JesSF_RelativePtBB_up                ,"Jet_JesSF_RelativePtBB_up");
  AddBranch(&Jet_JesSF_RelativePtBB_down                ,"Jet_JesSF_RelativePtBB_down");
  AddBranch(&Jet_JesSF_RelativePtEC1_up                ,"Jet_JesSF_RelativePtEC1_up");
  AddBranch(&Jet_JesSF_RelativePtEC1_down                ,"Jet_JesSF_RelativePtEC1_down");
  AddBranch(&Jet_JesSF_RelativePtEC2_up                ,"Jet_JesSF_RelativePtEC2_up");
  AddBranch(&Jet_JesSF_RelativePtEC2_down                ,"Jet_JesSF_RelativePtEC2_down");
  AddBranch(&Jet_JesSF_RelativePtHF_up                ,"Jet_JesSF_RelativePtHF_up");
  AddBranch(&Jet_JesSF_RelativePtHF_down                ,"Jet_JesSF_RelativePtHF_down");
  AddBranch(&Jet_JesSF_RelativeBal_up                ,"Jet_JesSF_RelativeBal_up");
  AddBranch(&Jet_JesSF_RelativeBal_down                ,"Jet_JesSF_RelativeBal_down");
  AddBranch(&Jet_JesSF_RelativeFSR_up                ,"Jet_JesSF_RelativeFSR_up");
  AddBranch(&Jet_JesSF_RelativeFSR_down                ,"Jet_JesSF_RelativeFSR_down");
  AddBranch(&Jet_JesSF_RelativeStatFSR_up                ,"Jet_JesSF_RelativeStatFSR_up");
  AddBranch(&Jet_JesSF_RelativeStatFSR_down                ,"Jet_JesSF_RelativeStatFSR_down");
  AddBranch(&Jet_JesSF_RelativeStatEC_up                ,"Jet_JesSF_RelativeStatEC_up");
  AddBranch(&Jet_JesSF_RelativeStatEC_down                ,"Jet_JesSF_RelativeStatEC_down");
  AddBranch(&Jet_JesSF_RelativeStatHF_up                ,"Jet_JesSF_RelativeStatHF_up");
  AddBranch(&Jet_JesSF_RelativeStatHF_down                ,"Jet_JesSF_RelativeStatHF_down");
  AddBranch(&Jet_JesSF_PileUpDataMC_up                ,"Jet_JesSF_PileUpDataMC_up");
  AddBranch(&Jet_JesSF_PileUpDataMC_down                ,"Jet_JesSF_PileUpDataMC_down");
  AddBranch(&Jet_JesSF_PileUpPtRef_up                ,"Jet_JesSF_PileUpPtRef_up");
  AddBranch(&Jet_JesSF_PileUpPtRef_down                ,"Jet_JesSF_PileUpPtRef_down");
  AddBranch(&Jet_JesSF_PileUpPtBB_up                ,"Jet_JesSF_PileUpPtBB_up");
  AddBranch(&Jet_JesSF_PileUpPtBB_down                ,"Jet_JesSF_PileUpPtBB_down");
  AddBranch(&Jet_JesSF_PileUpPtEC1_up                ,"Jet_JesSF_PileUpPtEC1_up");
  AddBranch(&Jet_JesSF_PileUpPtEC1_down                ,"Jet_JesSF_PileUpPtEC1_down");
  AddBranch(&Jet_JesSF_PileUpPtEC2_up                ,"Jet_JesSF_PileUpPtEC2_up");
  AddBranch(&Jet_JesSF_PileUpPtEC2_down                ,"Jet_JesSF_PileUpPtEC2_down");
  AddBranch(&Jet_JesSF_PileUpPtHF_up                ,"Jet_JesSF_PileUpPtHF_up");
  AddBranch(&Jet_JesSF_PileUpPtHF_down                ,"Jet_JesSF_PileUpPtHF_down");
  AddBranch(&Jet_JesSF_PileUpMuZero_up                ,"Jet_JesSF_PileUpMuZero_up");
  AddBranch(&Jet_JesSF_PileUpMuZero_down                ,"Jet_JesSF_PileUpMuZero_down");
  AddBranch(&Jet_JesSF_PileUpEnvelope_up                ,"Jet_JesSF_PileUpEnvelope_up");
  AddBranch(&Jet_JesSF_PileUpEnvelope_down                ,"Jet_JesSF_PileUpEnvelope_down");
  AddBranch(&Jet_JesSF_SubTotalPileUp_up                ,"Jet_JesSF_SubTotalPileUp_up");
  AddBranch(&Jet_JesSF_SubTotalPileUp_down                ,"Jet_JesSF_SubTotalPileUp_down");
  AddBranch(&Jet_JesSF_SubTotalRelative_up                ,"Jet_JesSF_SubTotalRelative_up");
  AddBranch(&Jet_JesSF_SubTotalRelative_down               ,"Jet_JesSF_SubTotalRelative_down");
  AddBranch(&Jet_JesSF_SubTotalPt_up                ,"Jet_JesSF_SubTotalPt_up");
  AddBranch(&Jet_JesSF_SubTotalPt_down                ,"Jet_JesSF_SubTotalPt_down");
  AddBranch(&Jet_JesSF_SubTotalScale_up                ,"Jet_JesSF_SubTotalScale_up");
  AddBranch(&Jet_JesSF_SubTotalScale_down                ,"Jet_JesSF_SubTotalScale_down");
  AddBranch(&Jet_JesSF_SubTotalAbsolute_up                ,"Jet_JesSF_SubTotalAbsolute_up");
  AddBranch(&Jet_JesSF_SubTotalAbsolute_down                ,"Jet_JesSF_SubTotalAbsolute_down");
  AddBranch(&Jet_JesSF_SubTotalMC_up                ,"Jet_JesSF_SubTotalMC_up");
  AddBranch(&Jet_JesSF_SubTotalMC_down                ,"Jet_JesSF_SubTotalMC_down");
  AddBranch(&Jet_JesSF_TotalNoFlavor_up                ,"Jet_JesSF_TotalNoFlavor_up");
  AddBranch(&Jet_JesSF_TotalNoFlavor_down                ,"Jet_JesSF_TotalNoFlavor_down");
  AddBranch(&Jet_JesSF_TotalNoFlavorNoTime_up                ,"Jet_JesSF_TotalNoFlavorNoTime_up");
  AddBranch(&Jet_JesSF_TotalNoFlavorNoTime_down                ,"Jet_JesSF_TotalNoFlavorNoTime_down");
  AddBranch(&Jet_JesSF_FlavorZJet_up                ,"Jet_JesSF_FlavorZJet_up");
  AddBranch(&Jet_JesSF_FlavorZJet_down                ,"Jet_JesSF_FlavorZJet_down");
  AddBranch(&Jet_JesSF_FlavorPhotonJet_up                ,"Jet_JesSF_FlavorPhotonJet_up");
  AddBranch(&Jet_JesSF_FlavorPhotonJet_down                ,"Jet_JesSF_FlavorPhotonJet_down");
  AddBranch(&Jet_JesSF_FlavorPureGluon_up                ,"Jet_JesSF_FlavorPureGluon_up");
  AddBranch(&Jet_JesSF_FlavorPureGluon_down                ,"Jet_JesSF_FlavorPureGluon_down");
  AddBranch(&Jet_JesSF_FlavorPureQuark_up                ,"Jet_JesSF_FlavorPureQuark_up");
  AddBranch(&Jet_JesSF_FlavorPureQuark_down                ,"Jet_JesSF_FlavorPureQuark_down");
  AddBranch(&Jet_JesSF_FlavorPureCharm_up                ,"Jet_JesSF_FlavorPureCharm_up");
  AddBranch(&Jet_JesSF_FlavorPureCharm_down                ,"Jet_JesSF_FlavorPureCharm_down");
  AddBranch(&Jet_JesSF_FlavorPureBottom_up                ,"Jet_JesSF_FlavorPureBottom_up");
  AddBranch(&Jet_JesSF_FlavorPureBottom_down                ,"Jet_JesSF_FlavorPureBottom_down");
  //MC
  if(!_is_data) {
    AddBranch(&Jet_partonFlavour        ,"Jet_partonFlavour");
    AddBranch(&Jet_hadronFlavour        ,"Jet_hadronFlavour");
  }
  ////slimmedJetsPuppi
  if(_PuppiVar){
    //Kinematics
    AddBranch(&Jet_puppi_pt        ,"Jet_puppi_pt");
    AddBranch(&Jet_puppi_eta       ,"Jet_puppi_eta");
    AddBranch(&Jet_puppi_phi       ,"Jet_puppi_phi");
    AddBranch(&Jet_puppi_energy    ,"Jet_puppi_energy");
    AddBranch(&Jet_puppi_mass      ,"Jet_puppi_mass");
    AddBranch(&Jet_puppi_px        ,"Jet_puppi_px");
    AddBranch(&Jet_puppi_py        ,"Jet_puppi_py");
    AddBranch(&Jet_puppi_pz        ,"Jet_puppi_pz");
    AddBranch(&Jet_puppi_Uncorr_pt ,"Jet_puppi_Uncorr_pt");
    //ID
    AddBranch(&Jet_puppi_pfCombinedInclusiveSecondaryVertexV2BJetTags ,"Jet_puppi_pfCombinedInclusiveSecondaryVertexV2BJetTags");
    AddBranch(&Jet_puppi_pfCombinedMVAV2BJetTags                      ,"Jet_puppi_pfCombinedMVAV2BJetTags");
    AddBranch(&Jet_puppi_pfJetProbabilityBJetTags                     ,"Jet_puppi_pfJetProbabilityBJetTags");
    AddBranch(&Jet_puppi_pfCombinedCvsLJetTags                        ,"Jet_puppi_pfCombinedCvsLJetTags");
    AddBranch(&Jet_puppi_pfCombinedCvsBJetTags                        ,"Jet_puppi_pfCombinedCvsBJetTags");
    AddBranch(&Jet_puppi_pileupId                                     ,"Jet_puppi_pileupId");
    AddBranch(&Jet_puppi_isPFJet                                      ,"Jet_puppi_isPFJet");
    AddBranch(&Jet_puppi_isCaloJet                                    ,"Jet_puppi_isCaloJet");
    //Energy
    AddBranch(&Jet_puppi_neutralHadEnergyFraction    ,"Jet_puppi_neutralHadEnergyFraction");
    AddBranch(&Jet_puppi_neutralEmEnergyFraction     ,"Jet_puppi_neutralEmEnergyFraction");
    AddBranch(&Jet_puppi_chargedHadronEnergyFraction ,"Jet_puppi_chargedHadronEnergyFraction");
    AddBranch(&Jet_puppi_chargedEmEnergyFraction     ,"Jet_puppi_chargedEmEnergyFraction");
    AddBranch(&Jet_puppi_muonEnergyFraction          ,"Jet_puppi_muonEnergyFraction");
    AddBranch(&Jet_puppi_electronEnergy              ,"Jet_puppi_electronEnergy");
    AddBranch(&Jet_puppi_photonEnergy                ,"Jet_puppi_photonEnergy");
    AddBranch(&Jet_puppi_emEnergyFraction            ,"Jet_puppi_emEnergyFraction");
    //Other prop
    AddBranch(&Jet_puppi_numberOfConstituents ,"Jet_puppi_numberOfConstituents");
    AddBranch(&Jet_puppi_chargedMultiplicity  ,"Jet_puppi_chargedMultiplicity");
    AddBranch(&Jet_puppi_vtxMass              ,"Jet_puppi_vtxMass");
    AddBranch(&Jet_puppi_vtxNtracks           ,"Jet_puppi_vtxNtracks");
    AddBranch(&Jet_puppi_vtx3DVal             ,"Jet_puppi_vtx3DVal");
    AddBranch(&Jet_puppi_vtx3DSig             ,"Jet_puppi_vtx3DSig");
    //Jet Energy Corrections and Uncertainties
    AddBranch(&Jet_puppi_JesSF                ,"Jet_puppi_JesSF");
    AddBranch(&Jet_puppi_JesSFup              ,"Jet_puppi_JesSFup");
    AddBranch(&Jet_puppi_JesSFdown            ,"Jet_puppi_JesSFdown");
    AddBranch(&Jet_puppi_JerSF                ,"Jet_puppi_JerSF");
    AddBranch(&Jet_puppi_JerSFup              ,"Jet_puppi_JerSFup");
    AddBranch(&Jet_puppi_JerSFdown            ,"Jet_puppi_JerSFdown");
    //MC
    if(!_is_data) {
      AddBranch(&Jet_puppi_partonFlavour        ,"Jet_puppi_partonFlavour");
      AddBranch(&Jet_puppi_hadronFlavour        ,"Jet_puppi_hadronFlavour");
    }
  }
  if(debug_) std::cout<<"set branches"<<std::endl;
}
void JetSelector::Clear(){
  ////slimmedJets
  //Kinematics
  Jet_pt.clear();
  Jet_eta.clear();
  Jet_phi.clear();
  Jet_energy.clear();
  Jet_mass.clear();
  Jet_px.clear();
  Jet_py.clear();
  Jet_pz.clear();
  Jet_Uncorr_pt.clear();
  Jet_L1corr_pt.clear();
  //ID
  Jet_pfCombinedInclusiveSecondaryVertexV2BJetTags.clear();
  Jet_pfCombinedMVAV2BJetTags.clear();
  Jet_pfJetProbabilityBJetTags.clear();
  Jet_pfCombinedCvsLJetTags.clear();
  Jet_pfCombinedCvsBJetTags.clear();
  Jet_pfDeepCSVBJetTags.clear();
  Jet_pfDeepCSVProbb.clear();
  Jet_pfDeepCSVProbbb.clear();
  Jet_pfDeepCSVProbc.clear();
  Jet_pfDeepCSVProbcc.clear();
  Jet_pfDeepCSVProbudsg.clear();
  Jet_pfDeepFlavourBJetTags.clear();
  Jet_pfDeepFlavourProbb.clear();
  Jet_pfDeepFlavourProbbb.clear();
  Jet_pfDeepFlavourProblepb.clear();
  Jet_pfDeepFlavourProbc.clear();
  Jet_pfDeepFlavourProbuds.clear();
  Jet_pfDeepFlavourProbg.clear();
  Jet_pileupId.clear();
  Jet_isPFJet.clear();
  Jet_isCaloJet.clear();
  Jet_qg.clear();
  Jet_axis2.clear();
  Jet_axis1.clear();
  Jet_ptD.clear();
  Jet_mult.clear();
  //Match Indices
  Jet_ele_indices.clear();
  Jet_ele_number.clear();
  Jet_mu_indices.clear();
  Jet_mu_number.clear();
  Jet_tau_indices.clear();
  Jet_tau_number.clear();
  //Energy
  Jet_neutralHadEnergyFraction.clear();
  Jet_neutralEmEnergyFraction.clear();
  Jet_chargedHadronEnergyFraction.clear();
  Jet_chargedEmEnergyFraction.clear();
  Jet_muonEnergyFraction.clear();
  Jet_electronEnergy.clear();
  Jet_photonEnergy.clear();
  Jet_emEnergyFraction.clear();
  //Other prop
  Jet_numberOfConstituents.clear();
  Jet_chargedMultiplicity.clear();
  Jet_vtxMass.clear();
  Jet_vtxNtracks.clear();
  Jet_vtx3DVal.clear();
  Jet_vtx3DSig.clear();
  //Jet Energy Corrections and Uncertainties
  Jet_JesSF.clear();
  Jet_JesSFup.clear();
  Jet_JesSFdown.clear();
  Jet_JesSF_AbsoluteStat_up.clear();
  Jet_JesSF_AbsoluteStat_down.clear();
  Jet_JesSF_AbsoluteScale_up.clear();
  Jet_JesSF_AbsoluteScale_down.clear();
  Jet_JesSF_AbsoluteFlavMap_up.clear();
  Jet_JesSF_AbsoluteFlavMap_down.clear();
  Jet_JesSF_AbsoluteMPFBias_up.clear();
  Jet_JesSF_AbsoluteMPFBias_down.clear();
  Jet_JesSF_Fragmentation_up.clear();
  Jet_JesSF_Fragmentation_down.clear();
  Jet_JesSF_SinglePionECAL_up.clear();
  Jet_JesSF_SinglePionECAL_down.clear();
  Jet_JesSF_SinglePionHCAL_up.clear();
  Jet_JesSF_SinglePionHCAL_down.clear();
  Jet_JesSF_FlavorQCD_up.clear();
  Jet_JesSF_FlavorQCD_down.clear();
  Jet_JesSF_TimePtEta_up.clear();
  Jet_JesSF_TimePtEta_down.clear();
  Jet_JesSF_RelativeJEREC1_up.clear();
  Jet_JesSF_RelativeJEREC1_down.clear();
  Jet_JesSF_RelativeJEREC2_up.clear();
  Jet_JesSF_RelativeJEREC2_down.clear();
  Jet_JesSF_RelativeJERHF_up.clear();
  Jet_JesSF_RelativeJERHF_down.clear();
  Jet_JesSF_RelativePtBB_up.clear();
  Jet_JesSF_RelativePtBB_down.clear();
  Jet_JesSF_RelativePtEC1_up.clear();
  Jet_JesSF_RelativePtEC1_down.clear();
  Jet_JesSF_RelativePtEC2_up.clear();
  Jet_JesSF_RelativePtEC2_down.clear();
  Jet_JesSF_RelativePtHF_up.clear();
  Jet_JesSF_RelativePtHF_down.clear();
  Jet_JesSF_RelativeBal_up.clear();
  Jet_JesSF_RelativeBal_down.clear();
  Jet_JesSF_RelativeFSR_up.clear();
  Jet_JesSF_RelativeFSR_down.clear();
  Jet_JesSF_RelativeStatFSR_up.clear();
  Jet_JesSF_RelativeStatFSR_down.clear();
  Jet_JesSF_RelativeStatEC_up.clear();
  Jet_JesSF_RelativeStatEC_down.clear();
  Jet_JesSF_RelativeStatHF_up.clear();
  Jet_JesSF_RelativeStatHF_down.clear();
  Jet_JesSF_PileUpDataMC_up.clear();
  Jet_JesSF_PileUpDataMC_down.clear();
  Jet_JesSF_PileUpPtRef_up.clear();
  Jet_JesSF_PileUpPtRef_down.clear();
  Jet_JesSF_PileUpPtBB_up.clear();
  Jet_JesSF_PileUpPtBB_down.clear();
  Jet_JesSF_PileUpPtEC1_up.clear();
  Jet_JesSF_PileUpPtEC1_down.clear();
  Jet_JesSF_PileUpPtEC2_up.clear();
  Jet_JesSF_PileUpPtEC2_down.clear();
  Jet_JesSF_PileUpPtHF_up.clear();
  Jet_JesSF_PileUpPtHF_down.clear();
  Jet_JesSF_PileUpMuZero_up.clear();
  Jet_JesSF_PileUpMuZero_down.clear();
  Jet_JesSF_PileUpEnvelope_up.clear();
  Jet_JesSF_PileUpEnvelope_down.clear();
  Jet_JesSF_SubTotalPileUp_up.clear();
  Jet_JesSF_SubTotalPileUp_down.clear();
  Jet_JesSF_SubTotalRelative_up.clear();
  Jet_JesSF_SubTotalRelative_down.clear();
  Jet_JesSF_SubTotalPt_up.clear();
  Jet_JesSF_SubTotalPt_down.clear();
  Jet_JesSF_SubTotalScale_up.clear();
  Jet_JesSF_SubTotalScale_down.clear();
  Jet_JesSF_SubTotalAbsolute_up.clear();
  Jet_JesSF_SubTotalAbsolute_down.clear();
  Jet_JesSF_SubTotalMC_up.clear();
  Jet_JesSF_SubTotalMC_down.clear();
  Jet_JesSF_TotalNoFlavor_up.clear();
  Jet_JesSF_TotalNoFlavor_down.clear();
  Jet_JesSF_TotalNoFlavorNoTime_up.clear();
  Jet_JesSF_TotalNoFlavorNoTime_down.clear();
  Jet_JesSF_FlavorZJet_up.clear();
  Jet_JesSF_FlavorZJet_down.clear();
  Jet_JesSF_FlavorPhotonJet_up.clear();
  Jet_JesSF_FlavorPhotonJet_down.clear();
  Jet_JesSF_FlavorPureGluon_up.clear();
  Jet_JesSF_FlavorPureGluon_down.clear();
  Jet_JesSF_FlavorPureQuark_up.clear();
  Jet_JesSF_FlavorPureQuark_down.clear();
  Jet_JesSF_FlavorPureCharm_up.clear();
  Jet_JesSF_FlavorPureCharm_down.clear();
  Jet_JesSF_FlavorPureBottom_up.clear();
  Jet_JesSF_FlavorPureBottom_down.clear();
  Jet_JerSF.clear();
  Jet_JerSFup.clear();
  Jet_JerSFdown.clear(); 
  //MC
  if(!_is_data) {
    Jet_partonFlavour.clear();
    Jet_hadronFlavour.clear();
  }
  ////slimmedJetsPuppi
  if(_PuppiVar){
    //Kinematics
    Jet_puppi_pt.clear();
    Jet_puppi_eta.clear();
    Jet_puppi_phi.clear();
    Jet_puppi_energy.clear();
    Jet_puppi_mass.clear();
    Jet_puppi_px.clear();
    Jet_puppi_py.clear();
    Jet_puppi_pz.clear();
    Jet_puppi_Uncorr_pt.clear();
    //ID
    Jet_puppi_pfCombinedInclusiveSecondaryVertexV2BJetTags.clear();
    Jet_puppi_pfCombinedMVAV2BJetTags.clear();
    Jet_puppi_pfJetProbabilityBJetTags.clear();
    Jet_puppi_pfCombinedCvsLJetTags.clear();
    Jet_puppi_pfCombinedCvsBJetTags.clear();
    Jet_puppi_pileupId.clear();
    Jet_puppi_isPFJet.clear();
    Jet_puppi_isCaloJet.clear();
    //Energy
    Jet_puppi_neutralHadEnergyFraction.clear();
    Jet_puppi_neutralEmEnergyFraction.clear();
    Jet_puppi_chargedHadronEnergyFraction.clear();
    Jet_puppi_chargedEmEnergyFraction.clear();
    Jet_puppi_muonEnergyFraction.clear();
    Jet_puppi_electronEnergy.clear();
    Jet_puppi_photonEnergy.clear();
    Jet_puppi_emEnergyFraction.clear();
    //Other prop
    Jet_puppi_numberOfConstituents.clear();
    Jet_puppi_chargedMultiplicity.clear();
    Jet_puppi_vtxMass.clear();
    Jet_puppi_vtxNtracks.clear();
    Jet_puppi_vtx3DVal.clear();
    Jet_puppi_vtx3DSig.clear();
    //Corrections/Systematics
    Jet_puppi_JesSF.clear();
    Jet_puppi_JesSFup.clear();
    Jet_puppi_JesSFdown.clear();
    Jet_puppi_JerSF.clear();
    Jet_puppi_JerSFup.clear();
    Jet_puppi_JerSFdown.clear(); 
    //MC
    if(!_is_data) {
      Jet_puppi_partonFlavour.clear();
      Jet_puppi_hadronFlavour.clear();
    }
  }
}

void JetSelector::Getjer(pat::Jet jet, float JesSF, float rhoJER, bool AK4PFchs, float &JERScaleFactor, float &JERScaleFactorUP, float &JERScaleFactorDOWN, int nEvent){
  double cFactorJER = 1.0; 
  double cFactorJERdown = 1.0;
  double cFactorJERup = 1.0;
  //https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution#JER_Scaling_factors_and_Unce_AN1
  
  //double recoJetPt = jet.pt();//(jet.correctedJet("Uncorrected").pt())*JesSF;
  double recoJetPt = (jet.correctedJet("Uncorrected").pt())*JesSF;
  JME::JetResolution resolution;
  JME::JetResolutionScaleFactor res_sf;
  if(AK4PFchs){
    resolution = JME::JetResolution(jerAK4PFchs_);
    res_sf = JME::JetResolutionScaleFactor(jerAK4PFchsSF_);
  } else {
    resolution = JME::JetResolution(jerAK4PFPuppi_);
    res_sf = JME::JetResolutionScaleFactor(jerAK4PFPuppiSF_);
  }
  JME::JetParameters parameters;
  parameters.setJetPt(jet.pt());
  parameters.setJetEta(jet.eta());
  parameters.setRho(rhoJER);
  float relpterr = resolution.getResolution(parameters);
  cFactorJER = res_sf.getScaleFactor(parameters);
  cFactorJERup = res_sf.getScaleFactor(parameters, Variation::UP);
  cFactorJERdown = res_sf.getScaleFactor(parameters, Variation::DOWN);
  if(jet.genJet()){
    // scale method
    double genJetPt = jet.genJet()->pt();
    double diffPt    = recoJetPt - genJetPt;
    if(genJetPt>0. && deltaR(jet.eta(),jet.phi(),jet.genJet()->eta(),jet.genJet()->phi())<0.2
     && (abs(jet.pt()-jet.genJet()->pt())<3*relpterr*jet.pt())) {
        JERScaleFactor     = (std::max(0., genJetPt + cFactorJER*diffPt))/recoJetPt;
        JERScaleFactorUP   = (std::max(0., genJetPt + cFactorJERup*diffPt))/recoJetPt;
        JERScaleFactorDOWN = (std::max(0., genJetPt + cFactorJERdown*diffPt))/recoJetPt;
    } else {
        JERScaleFactor     = 1.;
        JERScaleFactorUP   = 1.;
        JERScaleFactorDOWN = 1.;
    }
  }else{
    // stochastic method
    // https://github.com/cms-sw/cmssw/blob/CMSSW_8_0_25/PhysicsTools/PatUtils/interface/SmearedJetProducerT.h#L239-L247
    if(cFactorJER>1){
        double sigma = relpterr * std::sqrt(cFactorJER*cFactorJER-1);
        //sigma = 1.0; // set to 1 for debugging
        std::normal_distribution<> d(0, sigma);
        JERScaleFactor = (std::max(0., 1. + d(m_random_generator)));
        //std::cout<<" Event " << nEvent << " Get jer stochastic method : setting sigma to 1 "<< " jet pt "<< jet.pt() << " jet eta " << jet.eta() << " jerSF " << JERScaleFactor  <<std::endl;
    }else{
        JERScaleFactor = 1.;
    }
    if(cFactorJERup>1){
        double sigma = relpterr * std::sqrt(cFactorJERup*cFactorJERup-1);
        std::normal_distribution<> d(0, sigma);
        JERScaleFactorUP = (std::max(0., 1. + d(m_random_generator)));
    }else{
        JERScaleFactorUP = 1.;
    }
    if(cFactorJERdown>1){
        double sigma = relpterr * std::sqrt(cFactorJERdown*cFactorJERdown-1);
        std::normal_distribution<> d(0, sigma);
        JERScaleFactorDOWN = (std::max(0., 1. + d(m_random_generator)));
    }else{
        JERScaleFactorDOWN = 1.;
    }
  }
}

void JetSelector::GetJER(pat::Jet jet, float JesSF, float rhoJER, bool AK4PFchs, float &JERScaleFactor, float &JERScaleFactorUP, float &JERScaleFactorDOWN){
  if(!jet.genJet()) return;
  double jetEta=fabs(jet.eta());
  double cFactorJER = 1.0; 
  double cFactorJERdown = 1.0;
  double cFactorJERup = 1.0;
  //https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution#JER_Scaling_factors_and_Unce_AN1
  
  if(_dataEra==2016){
      // Summer16_25nsV1
      if( jetEta<0.522 ){ 
        cFactorJER = 1.1595; 
        cFactorJERdown = 1.1595-0.0645;
        cFactorJERup   = 1.1595+0.0645; 
      } else if( jetEta<0.783 ){ 
        cFactorJER = 1.1948; 
        cFactorJERdown = 1.1948-0.0652;
        cFactorJERup   = 1.1948+0.0652; 
      } else if( jetEta<1.131 ){ 
        cFactorJER = 1.1464; 
        cFactorJERdown = 1.1464-0.0632;
        cFactorJERup   = 1.1464+0.0632; 
      } else if( jetEta<1.305 ){ 
        cFactorJER = 1.1609; 
        cFactorJERdown = 1.1609-0.1025;
        cFactorJERup   = 1.1609+0.1025; 
      } else if( jetEta<1.740 ){ 
        cFactorJER = 1.1278; 
        cFactorJERdown = 1.1278-0.0986;
        cFactorJERup   = 1.1278+0.0986; 
      } else if( jetEta<1.930 ){ 
        cFactorJER = 1.1000; 
        cFactorJERdown = 1.1000-0.1079;
        cFactorJERup   = 1.1000+0.1079; 
      } else if( jetEta<2.043 ){ 
        cFactorJER = 1.1426; 
        cFactorJERdown = 1.1426-0.1214;
        cFactorJERup   = 1.1426+0.1214; 
      } else if( jetEta<2.322 ){ 
        cFactorJER = 1.1512; 
        cFactorJERdown = 1.1512-0.1140;
        cFactorJERup   = 1.1512+0.1140; 
      } else if( jetEta<2.5 ){ 
        cFactorJER = 1.2963; 
        cFactorJERdown = 1.2963-0.2371;
        cFactorJERup   = 1.2963+0.2371; 
      } else if( jetEta<2.853 ){ 
        cFactorJER = 1.3418; 
        cFactorJERdown = 1.3418-0.2091;
        cFactorJERup   = 1.3418+0.2091; 
      } else if( jetEta<2.964 ){ 
        cFactorJER = 1.7788; 
        cFactorJERdown = 1.7788-0.2008;
        cFactorJERup   = 1.7788+0.2008; 
      } else if( jetEta<3.139 ){ 
        cFactorJER = 1.1869; 
        cFactorJERdown = 1.1869-0.1243;
        cFactorJERup   = 1.1869+0.1243; 
      } else if( jetEta<5.191 ){ 
        cFactorJER = 1.1922; 
        cFactorJERdown = 1.1922-0.1488;
        cFactorJERup   = 1.1922+0.1488;
      }
  }else if(_dataEra==2017){
      // Fall17_V3
      if( jetEta<0.522 ){ 
        cFactorJER = 1.1432; 
        cFactorJERdown = 1.1432-0.0222;
        cFactorJERup   = 1.1432+0.0222; 
      } else if( jetEta<0.783 ){ 
        cFactorJER = 1.1815; 
        cFactorJERdown = 1.1815-0.0484;
        cFactorJERup   = 1.1815+0.0484; 
      } else if( jetEta<1.131 ){ 
        cFactorJER = 1.0989; 
        cFactorJERdown = 1.0989-0.0456;
        cFactorJERup   = 1.0989+0.0456; 
      } else if( jetEta<1.305 ){ 
        cFactorJER = 1.1137; 
        cFactorJERdown = 1.1137-0.1397;
        cFactorJERup   = 1.1137+0.1397; 
      } else if( jetEta<1.740 ){ 
        cFactorJER = 1.1307; 
        cFactorJERdown = 1.1307-0.1470;
        cFactorJERup   = 1.1307+0.1470; 
      } else if( jetEta<1.930 ){ 
        cFactorJER = 1.1600; 
        cFactorJERdown = 1.1600-0.0976;
        cFactorJERup   = 1.1600+0.0976; 
      } else if( jetEta<2.043 ){ 
        cFactorJER = 1.2393; 
        cFactorJERdown = 1.2393-0.1909;
        cFactorJERup   = 1.2393+0.1909; 
      } else if( jetEta<2.322 ){ 
        cFactorJER = 1.2604; 
        cFactorJERdown = 1.2604-0.1501;
        cFactorJERup   = 1.2604+0.1501; 
      } else if( jetEta<2.5 ){ 
        cFactorJER = 1.4085; 
        cFactorJERdown = 1.4085-0.2020;
        cFactorJERup   = 1.4085+0.2020; 
      } else if( jetEta<2.853 ){ 
        cFactorJER = 1.9909; 
        cFactorJERdown = 1.9909-0.5684;
        cFactorJERup   = 1.9909+0.5684; 
      } else if( jetEta<2.964 ){ 
        cFactorJER = 2.2923; 
        cFactorJERdown = 2.2923-0.3743;
        cFactorJERup   = 2.2923+0.3743; 
      } else if( jetEta<3.139 ){ 
        cFactorJER = 1.2696; 
        cFactorJERdown = 1.2696-0.1089;
        cFactorJERup   = 1.2696+0.1089; 
      } else if( jetEta<5.191 ){ 
        cFactorJER = 1.1542; 
        cFactorJERdown = 1.1542-0.1524;
        cFactorJERup   = 1.1542+0.1524;
      }
  }else if(_dataEra==2018){
      // Autumn18_v1
      // Temporary version to be used  for Moriond 2019 analyses
      // Averaged over all runs
      if( jetEta<0.522 ){ 
        cFactorJER = 1.15; 
        cFactorJERdown = 1.15-0.043;
        cFactorJERup   = 1.15+0.043; 
      } else if( jetEta<0.783 ){ 
        cFactorJER = 1.134; 
        cFactorJERdown = 1.134-0.08;
        cFactorJERup   = 1.134+0.08; 
      } else if( jetEta<1.131 ){ 
        cFactorJER = 1.102; 
        cFactorJERdown = 1.102-0.052;
        cFactorJERup   = 1.102+0.052; 
      } else if( jetEta<1.305 ){ 
        cFactorJER = 1.134; 
        cFactorJERdown = 1.134-0.112;
        cFactorJERup   = 1.134+0.112; 
      } else if( jetEta<1.740 ){ 
        cFactorJER = 1.104; 
        cFactorJERdown = 1.104-0.211;
        cFactorJERup   = 1.104+0.211; 
      } else if( jetEta<1.930 ){ 
        cFactorJER = 1.149; 
        cFactorJERdown = 1.149-0.159;
        cFactorJERup   = 1.149+0.159; 
      } else if( jetEta<2.043 ){ 
        cFactorJER = 1.148; 
        cFactorJERdown = 1.148-0.209;
        cFactorJERup   = 1.148+0.209; 
      } else if( jetEta<2.322 ){ 
        cFactorJER = 1.114; 
        cFactorJERdown = 1.114-0.191;
        cFactorJERup   = 1.114+0.191; 
      } else if( jetEta<2.5 ){ 
        cFactorJER = 1.347; 
        cFactorJERdown = 1.347-0.274;
        cFactorJERup   = 1.347+0.274; 
      } else if( jetEta<2.853 ){ 
        cFactorJER = 2.137; 
        cFactorJERdown = 2.137-0.524;
        cFactorJERup   = 2.137+0.524; 
      } else if( jetEta<2.964 ){ 
        cFactorJER = 1.65; 
        cFactorJERdown = 1.65-0.941;
        cFactorJERup   = 1.65+0.941; 
      } else if( jetEta<3.139 ){ 
        cFactorJER = 1.225; 
        cFactorJERdown = 1.225-0.194;
        cFactorJERup   = 1.225+0.194; 
      } else if( jetEta<5.191 ){ 
        cFactorJER = 1.082; 
        cFactorJERdown = 1.082-0.198;
        cFactorJERup   = 1.082+0.198;
      }
  }else{
    std::cout<<" ERROR dataEra must be 2016/2017/2018 " <<std::endl;
  }
  //double recoJetPt = jet.pt();//(jet.correctedJet("Uncorrected").pt())*JesSF;
  double recoJetPt = (jet.correctedJet("Uncorrected").pt())*JesSF;
  double genJetPt  = jet.genJet()->pt();
  double diffPt    = recoJetPt - genJetPt;
  JME::JetResolution resolution;
  JME::JetResolutionScaleFactor res_sf;
  if(AK4PFchs){
    resolution = JME::JetResolution(jerAK4PFchs_);
    res_sf = JME::JetResolutionScaleFactor(jerAK4PFchsSF_);
  } else {
    resolution = JME::JetResolution(jerAK4PFPuppi_);
    res_sf = JME::JetResolutionScaleFactor(jerAK4PFPuppiSF_);
  }
  JME::JetParameters parameters;
  parameters.setJetPt(jet.pt());
  parameters.setJetEta(jet.eta());
  parameters.setRho(rhoJER);
  float relpterr = resolution.getResolution(parameters);
  if(genJetPt>0. && deltaR(jet.eta(),jet.phi(),jet.genJet()->eta(),jet.genJet()->phi())<0.2
     && (abs(jet.pt()-jet.genJet()->pt())<3*relpterr*jet.pt())) {
    JERScaleFactor     = (std::max(0., genJetPt + cFactorJER*diffPt))/recoJetPt;
    JERScaleFactorUP   = (std::max(0., genJetPt + cFactorJERup*diffPt))/recoJetPt;
    JERScaleFactorDOWN = (std::max(0., genJetPt + cFactorJERdown*diffPt))/recoJetPt;
  } else {
    JERScaleFactor     = 1.;
    JERScaleFactorUP   = 1.;
    JERScaleFactorDOWN = 1.;
  } 
}
void JetSelector::computeQG(const pat::Jet& jet, double& ptD_, double& mult_, double& axis1_, double& axis2_, bool useQualityCut){
  float sum_weight = 0., sum_deta = 0., sum_dphi = 0., sum_deta2 = 0., sum_dphi2 = 0., sum_detadphi = 0., sum_pt = 0.;
  int mult = 0;
  //Access jet daughters
  std::vector<const pat::PackedCandidate*> daughters_;
  // get all constituents
  for (unsigned idau=0; idau<jet.numberOfDaughters(); ++idau){
    daughters_.push_back(dynamic_cast<const pat::PackedCandidate*>(jet.daughter(idau)));
  }
  for(const auto *daughter : daughters_){
    auto part = static_cast<const pat::PackedCandidate*>(daughter);
    if(part->charge()){
      if(!(part->fromPV() > 1 && part->trackHighPurity())) continue;
      if(useQualityCut){
        if((part->dz()*part->dz())/(part->dzError()*part->dzError()) > 25.) continue;
        if((part->dxy()*part->dxy())/(part->dxyError()*part->dxyError()) < 25.) ++mult;
      } else ++mult;
    } else {
      if(part->pt() < 1.0) continue;
      ++mult;
    }

    float deta = daughter->eta() - jet.eta();
    float dphi = reco::deltaPhi(daughter->phi(), jet.phi());
    float partPt = daughter->pt();
    float weight = partPt*partPt;

    sum_weight += weight;
    sum_pt += partPt;
    sum_deta += deta*weight;
    sum_dphi += dphi*weight;
    sum_deta2 += deta*deta*weight;
    sum_detadphi += deta*dphi*weight;
    sum_dphi2 += dphi*dphi*weight;
  }

  //Calculate axis2 and ptD
  float a = 0., b = 0., c = 0.;
  float ave_deta = 0., ave_dphi = 0., ave_deta2 = 0., ave_dphi2 = 0.;
  if(sum_weight > 0){
    ave_deta = sum_deta/sum_weight;
    ave_dphi = sum_dphi/sum_weight;
    ave_deta2 = sum_deta2/sum_weight;
    ave_dphi2 = sum_dphi2/sum_weight;
    a = ave_deta2 - ave_deta*ave_deta;
    b = ave_dphi2 - ave_dphi*ave_dphi;
    c = -(sum_detadphi/sum_weight - ave_deta*ave_dphi);
  }
  float delta = sqrt(fabs((a-b)*(a-b)+4*c*c));
  axis1_ = (a+b+delta > 0 ?  sqrt(0.5*(a+b+delta)) : 0);
  axis2_ = (a+b-delta > 0 ?  sqrt(0.5*(a+b-delta)) : 0);
  ptD_   = (sum_weight > 0 ? sqrt(sum_weight)/sum_pt : 0);
  mult_ = mult;
};
