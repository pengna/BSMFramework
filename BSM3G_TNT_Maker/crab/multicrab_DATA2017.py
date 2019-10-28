if __name__ == '__main__':
 #####
 ##   Multicrab configuration
 #####
 import sys
 from multiprocessing import Process
 from CRABClient.UserUtilities import config, getUsernameFromSiteDB
 config = config()
 from CRABAPI.RawCommand import crabCommand
 from CRABClient.ClientExceptions import ClientException
 from httplib import HTTPException
 config.General.workArea = 'Crab_projects'

 def submit(config):
  try:
   crabCommand('submit', config = config)
  except HTTPException as hte:
   print "Failed submitting task: %s" % (hte.headers)
  except ClientException as cle:
   print "Failed submitting task: %s" % (cle)
 #####
 ##   Crab configuration
 #####
 datasetnames  = [
'Legacy17V2_SEleBlockB',
'Legacy17V2_SEleBlockC',
'Legacy17V2_SEleBlockD',
'Legacy17V2_SEleBlockE',
'Legacy17V2_SEleBlockF',
'Legacy17V2_SMuBlockB',
'Legacy17V2_SMuBlockC',
'Legacy17V2_SMuBlockD',
'Legacy17V2_SMuBlockE',
'Legacy17V2_SMuBlockF',
'Legacy17V2_DblEGBlockB',
'Legacy17V2_DblEGBlockC',
'Legacy17V2_DblEGBlockD',
'Legacy17V2_DblEGBlockE',
'Legacy17V2_DblEGBlockF',
'Legacy17V2_DblMuBlockB',
'Legacy17V2_DblMuBlockC',
'Legacy17V2_DblMuBlockD',
'Legacy17V2_DblMuBlockE',
'Legacy17V2_DblMuBlockF',
'Legacy17V2_MuEGBlockB',
'Legacy17V2_MuEGBlockC',
'Legacy17V2_MuEGBlockD',
'Legacy17V2_MuEGBlockE',
'Legacy17V2_MuEGBlockF',
                 ]
 datasetinputs = [
 # SingleElectron dataset : AT LEAST 1 high-energy electron in the event.
 '/SingleElectron/Run2017B-31Mar2018-v1/MINIAOD',
 '/SingleElectron/Run2017C-31Mar2018-v1/MINIAOD',
 '/SingleElectron/Run2017D-31Mar2018-v1/MINIAOD',
 '/SingleElectron/Run2017E-31Mar2018-v1/MINIAOD',
 '/SingleElectron/Run2017F-31Mar2018-v1/MINIAOD',
 # SingleMuon dataset : AT LEAST 1 high-energy muon in the event.
 '/SingleMuon/Run2017B-31Mar2018-v1/MINIAOD',
 '/SingleMuon/Run2017C-31Mar2018-v1/MINIAOD',
 '/SingleMuon/Run2017D-31Mar2018-v1/MINIAOD',
 '/SingleMuon/Run2017E-31Mar2018-v1/MINIAOD',
 '/SingleMuon/Run2017F-31Mar2018-v1/MINIAOD',
 # DoubleEG dataset : AT LEAST 2 high-energy electron in the event.
 '/DoubleEG/Run2017B-31Mar2018-v1/MINIAOD',
 '/DoubleEG/Run2017C-31Mar2018-v1/MINIAOD',
 '/DoubleEG/Run2017D-31Mar2018-v1/MINIAOD',
 '/DoubleEG/Run2017E-31Mar2018-v1/MINIAOD',
 '/DoubleEG/Run2017F-31Mar2018-v1/MINIAOD',
 # DoubleMuon dataset : AT LEAST 2 high-energy muon in the event.
 '/DoubleMuon/Run2017B-31Mar2018-v1/MINIAOD',
 '/DoubleMuon/Run2017C-31Mar2018-v1/MINIAOD',
 '/DoubleMuon/Run2017D-31Mar2018-v1/MINIAOD',
 '/DoubleMuon/Run2017E-31Mar2018-v1/MINIAOD',
 '/DoubleMuon/Run2017F-31Mar2018-v1/MINIAOD',
 # MuonEG dataset : AT LEAST 1 high-energy electron and 1 high-energy muon in the event.
 '/MuonEG/Run2017B-31Mar2018-v1/MINIAOD',
 '/MuonEG/Run2017C-31Mar2018-v1/MINIAOD',
 '/MuonEG/Run2017D-31Mar2018-v1/MINIAOD',
 '/MuonEG/Run2017E-31Mar2018-v1/MINIAOD',
 '/MuonEG/Run2017F-31Mar2018-v1/MINIAOD',
                ]

JECBlockB = [
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L1FastJet_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L2Relative_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L3Absolute_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L2L3Residual_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_Uncertainty_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L1FastJet_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L2Relative_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L3Absolute_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L2L3Residual_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_Uncertainty_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L1FastJet_AK8PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L2Relative_AK8PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L3Absolute_AK8PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L2L3Residual_AK8PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_Uncertainty_AK8PFchs.txt',
]

JECBlockC = [
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L1FastJet_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L2Relative_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L3Absolute_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L2L3Residual_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_Uncertainty_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L1FastJet_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L2Relative_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L3Absolute_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L2L3Residual_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_Uncertainty_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L1FastJet_AK8PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L2Relative_AK8PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L3Absolute_AK8PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L2L3Residual_AK8PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_Uncertainty_AK8PFchs.txt',
]

JECBlockD = [
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L1FastJet_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2Relative_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L3Absolute_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2L3Residual_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_Uncertainty_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L1FastJet_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2Relative_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L3Absolute_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2L3Residual_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_Uncertainty_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L1FastJet_AK8PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2Relative_AK8PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L3Absolute_AK8PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2L3Residual_AK8PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_Uncertainty_AK8PFchs.txt',
]

JECBlockE = [
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L1FastJet_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2Relative_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L3Absolute_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2L3Residual_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_Uncertainty_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L1FastJet_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2Relative_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L3Absolute_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2L3Residual_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_Uncertainty_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L1FastJet_AK8PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2Relative_AK8PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L3Absolute_AK8PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2L3Residual_AK8PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_Uncertainty_AK8PFchs.txt',
]

JECBlockF = [
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L1FastJet_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L2Relative_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L3Absolute_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L2L3Residual_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_Uncertainty_AK4PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L1FastJet_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L2Relative_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L3Absolute_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L2L3Residual_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_Uncertainty_AK4PFPuppi.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L1FastJet_AK8PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L2Relative_AK8PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L3Absolute_AK8PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L2L3Residual_AK8PFchs.txt',
'BSMFramework/BSM3G_TNT_Maker/data/JEC/DATA/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_Uncertainty_AK8PFchs.txt',
]


# baseDir
baseDir = "/afs/cern.ch/work/b/binghuan/private/TTHLep_RunII/CMSSW_10_2_16/src/BSMFramework/"

goodRunsLists = [
(baseDir+'BSM3G_TNT_Maker/data/JSON/Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON_v1.txt'),
]

for d in range(0,len(datasetnames)):
#for d in range(10,len(datasetnames)):
#for d in range(0,10):
#for d in [4,9,14,19,24]:
    print 'multicrab.py: Running datasetname: ', datasetnames[d]
    JECFiles = []
    tempJSON = ''
    if 'BlockB' in datasetnames[d]:
        print 'multicrab.py: Run Block B'
        JECFiles = JECBlockB
        tempJSON = goodRunsLists[0]
    if 'BlockC' in datasetnames[d]:
        print 'multicrab.py: Run Block C'
        JECFiles = JECBlockC
        tempJSON = goodRunsLists[0]
    if 'BlockD' in datasetnames[d]:
        print 'multicrab.py: Run Block D'
        JECFiles = JECBlockD
        tempJSON = goodRunsLists[0]
    if 'BlockE' in datasetnames[d]:
        print 'multicrab.py: Run Block E'
        JECFiles = JECBlockE
        tempJSON = goodRunsLists[0]
    if 'BlockF' in datasetnames[d]:
        print 'multicrab.py: Run Block F'
        JECFiles = JECBlockF
        tempJSON = goodRunsLists[0]

    print 'multicrab.py: JSON File = ', tempJSON
    try:
        testJECFiles = JECFiles[14]
    except(IndexError):
        print 'multicrab.py: Failed to access JEC list element.'
        print 'multicrab.py: Not eneough JEC files proivided.'
        sys.exit()
    try:
        testJSON = goodRunsLists[0]
    except(IndexError):
        print 'multicrab.py: Failed to access JSON list element.'
        print 'multicrab.py: Not eneough JSON files proivided.'
        sys.exit()

    nameJECAK4PFchsDATA1 = "optionJECAK4PFchsDATA1="+JECFiles[0]
    nameJECAK4PFchsDATA2 = "optionJECAK4PFchsDATA2="+JECFiles[1]
    nameJECAK4PFchsDATA3 = "optionJECAK4PFchsDATA3="+JECFiles[2]
    nameJECAK4PFchsDATA4 = "optionJECAK4PFchsDATA4="+JECFiles[3]
    nameJECAK4PFchsDATAUnc = "optionJECAK4PFchsDATAUnc="+JECFiles[4]
    nameJECAK4PFPuppiDATA1 = "optionJECAK4PFPuppiDATA1="+JECFiles[5]
    nameJECAK4PFPuppiDATA2 = "optionJECAK4PFPuppiDATA2="+JECFiles[6]
    nameJECAK4PFPuppiDATA3 = "optionJECAK4PFPuppiDATA3="+JECFiles[7]
    nameJECAK4PFPuppiDATA4 = "optionJECAK4PFPuppiDATA4="+JECFiles[8]
    nameJECAK4PFPuppiDATAUnc = "optionJECAK4PFPuppiDATAUnc="+JECFiles[9]
    nameJECAK8PFchsDATA1 = "optionJECAK8PFchsDATA1="+JECFiles[10]
    nameJECAK8PFchsDATA2 = "optionJECAK8PFchsDATA2="+JECFiles[11]
    nameJECAK8PFchsDATA3 = "optionJECAK8PFchsDATA3="+JECFiles[12]
    nameJECAK8PFchsDATA4 = "optionJECAK8PFchsDATA4="+JECFiles[13]
    nameJECAK8PFchsDATAUnc = "optionJECAK8PFchsDATAUnc="+JECFiles[14]

    config.section_('General')
    config.General.requestName = datasetnames[d]
    config.General.workArea    = datasetnames[d]

    config.section_('JobType')
    config.JobType.pluginName  = 'Analysis'
    # List of parameters to pass to CMSSW parameter-set configuration file:
    config.JobType.psetName    = baseDir+'BSM3G_TNT_Maker/python/miniAOD_RD2017.py'
    config.JobType.inputFiles = [(baseDir+'BSM3G_TNT_Maker/data/QG/QGL_AK4chs_94X.db')]
    config.JobType.allowUndistributedCMSSW = True
    config.JobType.sendExternalFolder = True
    ofParam = 'ofName=' + datasetnames[d]
    config.JobType.pyCfgParams = [nameJECAK4PFchsDATA1,
                                  nameJECAK4PFchsDATA2,
                                  nameJECAK4PFchsDATA3,
                                  nameJECAK4PFchsDATA4,
                                  nameJECAK4PFchsDATAUnc,
                                  nameJECAK4PFPuppiDATA1,
                                  nameJECAK4PFPuppiDATA2,
                                  nameJECAK4PFPuppiDATA3,
                                  nameJECAK4PFPuppiDATA4,
                                  nameJECAK4PFPuppiDATAUnc,
                                  nameJECAK8PFchsDATA1,
                                  nameJECAK8PFchsDATA2,
                                  nameJECAK8PFchsDATA3,
                                  nameJECAK8PFchsDATA4,
                                  nameJECAK8PFchsDATAUnc,
                                  ofParam
                                  ]

    config.section_('Data')
    config.Data.inputDataset   = datasetinputs[d]
    config.Data.inputDBS       = 'global'
    config.Data.splitting      = 'LumiBased'
    config.Data.unitsPerJob    = 30
    #config.Data.splitting      = 'Automatic'
    #config.Data.unitsPerJob    = 180
    # Golden
    config.Data.lumiMask       = tempJSON
    config.Data.outLFNDirBase = '/store/user/binghuan/'
    print 'multicrab.py: outLFNDirBase = /store/user/binghuan/'
    #config.Data.publication = True

    config.section_('Site')
    config.Site.storageSite    = 'T2_CN_Beijing'#'T2_CH_CERN'
    print 'multicrab.py: Submitting Jobs'
    #submit(config)
    p = Process(target=submit, args=(config,))
    p.start()
    p.join()
