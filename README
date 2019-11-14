####################################################

This code is based on a preliminary version of the BSM3G code initiated by:
Andres Florez (Los Andes), Alfredo Gurrola (Vanderbilt) and Amandeep Kalsi (Panjab) 
(See https://github.com/florez/NtupleMaker_740)

It has been extended and further developed by Francesco Romeo (IHEP) and Aniello Spiezia (IHEP).
It has been modified for RunII analysis by Binghuan Li (IHEP)
This version of code is used for 2016 miniAOD v3 && 2017 miniAOD v2 and v1 & 2018 miniAOD  Ntuple production:
####################################################


cmsrel CMSSW_10_2_16
cd CMSSW_10_2_16/src
cmsenv

git cms-init

# set merge limit variable
git config merge.renameLimit 999999

## set up Egamma Tools
## https://twiki.cern.ch/twiki/bin/view/CMS/EgammaMiniAODV2#2018_Preliminary_Energy_Correcti 
git cms-merge-topic cms-egamma:EgammaPostRecoTools #just adds in an extra file to have a setup function to make things easier
git cms-merge-topic cms-egamma:PhotonIDValueMapSpeedup1029 #optional but speeds up the photon ID value module so things fun faster
git cms-merge-topic cms-egamma:slava77-btvDictFix_10210 #fixes the Run2018D dictionary issue, see https://github.com/cms-sw/cmssw/issues/26182
#now to add the scale and smearing for 2018 (eventually this will not be necessary in later releases but is harmless to do regardless)
git cms-addpkg EgammaAnalysis/ElectronTools
rm EgammaAnalysis/ElectronTools/data -rf
git clone git@github.com:cms-data/EgammaAnalysis-ElectronTools.git EgammaAnalysis/ElectronTools/data
#now build everything
scram b -j 8
         

## set up MET corrections with EE nosie fix
# https://twiki.cern.ch/twiki/bin/view/CMS/MissingETUncertaintyPrescription#Instructions_for_9_4_X_X_9_for_2
git cms-merge-topic cms-met:METFixEE2017_949_v2_backport_to_102X  
scram b -j 8

## set up MetFilters
# https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2#How_to_run_ecal_BadCalibReducedM
git cms-addpkg RecoMET/METFilters
scram b

## set up DeepTau v2p1
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuidePFTauID#Running_of_the_DNN_based_tau_ID
git cms-merge-topic -u cms-tau-pog:CMSSW_10_2_X_tau-pog_DeepTau2017v2p1_nanoAOD
scram b -j 4

## set up JEC for 2018
# https://github.com/cms-sw/cmssw/pull/28098
git cms-merge-topic 28098
scram b -j 4

## set BSM Framework
git clone https://github.com/BinghuanLi/BSMFramework.git
cd BSMFramework/
git checkout CMSSW_10_2_16
cd ..
scram b -j 8


## To Run ##
cd BSMFramework/BSM3G_TNT_Maker/python/
cmsRun miniAOD_MC(RD)2017(2016)(2018).py 

## set up the crab environment and the grid certificate
## this needs be done every time you start a new terminal
voms-proxy-init --voms cms
source /cvmfs/cms.cern.ch/crab3/crab.sh

## To Submit Crab jobs ###
cd BSMFramework/BSM3G_TNT_Maker/crab/
python multicrab_MC(DATA)2016(2017)(2018).py

## Job Recover ######
#####################
1. <crab kill -d> to kill the job and make sure the jobs are in killed status before you move to next step
2. <crab report -d> to create the notFinishedLumis.json
3. Feed this json file to multicrab_MC(DATA).py : 
    set config.Data.lumiMask to this json file
    see multicrab_DATA2018.py for example.
    For convenience, I create a directory crab_dataInfo and move my crab task directory into it.



## Notice ###
- Use < crab status -d > instead of TaskMonitors to check job information 
  < crab status -d > may show information contradicting TaskMonitors
  This is due to automatic splitting:
  https://twiki.cern.ch/twiki/bin/view/CMSPublic/CRAB3FAQ#What_is_the_Automatic_splitting
  

- Please change the absolute path of baseDir in multicrab_MC(DATA)2016(2017)(2018).py
- Please set outLFNDirBase to your DIR path at IHEP Tier 2 farm
  Highly suggest to create an empty DIR as output for the IHEP Tier 2 to Tier 3 Ntuple Handlings

- Always check HLT, make sure
    HLT you want is contained in TriggerSelector
    ifevtriggers in miniADO_RD(MC).py is set to true (false)

 
