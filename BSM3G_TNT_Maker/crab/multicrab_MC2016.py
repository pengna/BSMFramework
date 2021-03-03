if __name__ == '__main__':
 #####
 ##   Multicrab configuration
 #####
 import sys
 from multiprocessing import Process
 from CRABClient.UserUtilities import config#, getUsernameFromSiteDB
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
##Addtional QCD Lepton enriched samples
'Legacy16V2_QCD_Pt_15to20_MuEnriched',#0
'Legacy16V2_QCD_Pt_20to30_MuEnriched',
'Legacy16V2_QCD_Pt_30to50_MuEnriched',
'Legacy16V2_QCD_Pt_50to80_MuEnriched',
'Legacy16V2_QCD_Pt_80to120_MuEnriched_v1',
'Legacy16V2_QCD_Pt_80to120_MuEnriched_ext1',
'Legacy16V2_QCD_Pt_120to170_MuEnriched',
'Legacy16V2_QCD_Pt_170to300_MuEnriched_v1',
'Legacy16V2_QCD_Pt_170to300_MuEnriched_ext1',
'Legacy16V2_QCD_Pt_300to470_MuEnriched_v1',
'Legacy16V2_QCD_Pt_300to470_MuEnriched_ext1', #10
'Legacy16V2_QCD_Pt_300to470_MuEnriched_ext2',
'Legacy16V2_QCD_Pt_470to600_MuEnriched_v1',
'Legacy16V2_QCD_Pt_470to600_MuEnriched_ext1',
'Legacy16V2_QCD_Pt_470to600_MuEnriched_ext2',
'Legacy16V2_QCD_Pt_600to800_MuEnriched_v1',
'Legacy16V2_QCD_Pt_600to800_MuEnriched_ext1',
'Legacy16V2_QCD_Pt_800to1000_MuEnriched_v1',
'Legacy16V2_QCD_Pt_800to1000_MuEnriched_ext1',
'Legacy16V2_QCD_Pt_800to1000_MuEnriched_ext2',
'Legacy16V2_QCD_Pt_1000toInf_MuEnriched_v1',#20
'Legacy16V2_QCD_Pt_1000toInf_MuEnriched_ext1',

#QCD EM enriched

'Legacy16V2_QCD_Pt_20to30_EMEnriched',
'Legacy16V2_QCD_Pt_30to50_EMEnriched_v1',
'Legacy16V2_QCD_Pt_30to50_EMEnriched_ext1',
'Legacy16V2_QCD_Pt_50to80_EMEnriched_v1',
'Legacy16V2_QCD_Pt_50to80_EMEnriched_ext1',
'Legacy16V2_QCD_Pt_80to120_EMEnriched_v1',
'Legacy16V2_QCD_Pt_80to120_EMEnriched_ext1',
'Legacy16V2_QCD_Pt_120to170_EMEnriched_v1',
'Legacy16V2_QCD_Pt_120to170_EMEnriched_ext1',#30
'Legacy16V2_QCD_Pt_170to300_EMEnriched',
'Legacy16V2_QCD_Pt_300toInf_EMEnriched',#32



## signal
#'Legacy16V2_TTHnobb', #0
#'Legacy16V2_ttHnobb',
#'Legacy16V2_TTH_ctcvcp', 
                 ]
 datasetinputs = [
#QCD Muonenriched samples
'/QCD_Pt-15to20_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM', #0
'/QCD_Pt-20to30_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM',
'/QCD_Pt-30to50_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM',
'/QCD_Pt-50to80_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM',
'/QCD_Pt-80to120_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM',
'/QCD_Pt-80to120_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM',
'/QCD_Pt-120to170_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM',
'/QCD_Pt-170to300_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM',
'/QCD_Pt-170to300_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM',
'/QCD_Pt-300to470_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM',
'/QCD_Pt-300to470_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v1/MINIAODSIM',#10
'/QCD_Pt-300to470_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext2-v2/MINIAODSIM',
'/QCD_Pt-470to600_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM',
'/QCD_Pt-470to600_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM',
'/QCD_Pt-470to600_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext2-v2/MINIAODSIM',
'/QCD_Pt-600to800_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM',
'/QCD_Pt-600to800_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM',
'/QCD_Pt-800to1000_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM',
'/QCD_Pt-800to1000_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM',
'/QCD_Pt-800to1000_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext2-v2/MINIAODSIM',
'/QCD_Pt-1000toInf_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM',#20
'/QCD_Pt-1000toInf_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM',
#QCD EM enriched samples
'/QCD_Pt-20to30_EMEnriched_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM',
'/QCD_Pt-30to50_EMEnriched_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM',
'/QCD_Pt-30to50_EMEnriched_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM',
'/QCD_Pt-50to80_EMEnriched_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM',
'/QCD_Pt-50to80_EMEnriched_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM',
'/QCD_Pt-80to120_EMEnriched_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM',
'/QCD_Pt-80to120_EMEnriched_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM',
'/QCD_Pt-120to170_EMEnriched_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM',
'/QCD_Pt-120to170_EMEnriched_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM',#30
'/QCD_Pt-170to300_EMEnriched_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM',
'/QCD_Pt-300toInf_EMEnriched_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM',#32




# signal
#'/ttHToNonbb_M125_TuneCUETP8M2_ttHtranche3_13TeV-powheg-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM', #0
#'/ttHJetToNonbb_M125_13TeV_amcatnloFXFX_madspin_pythia8_mWCutfix/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM',
#'/ttH_4f_ctcvcp_TuneCP5_13TeV_madgraph_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM', 
                ]

# samples also used in tW or bstar
# minimum lepton are set to 1 instead of 2 
tWLists = [
'Legacy16V2_QCD_Pt_15to20_MuEnriched',#0
'Legacy16V2_QCD_Pt_20to30_MuEnriched',
'Legacy16V2_QCD_Pt_30to50_MuEnriched',
'Legacy16V2_QCD_Pt_50to80_MuEnriched',
'Legacy16V2_QCD_Pt_80to120_MuEnriched_v1',
'Legacy16V2_QCD_Pt_80to120_MuEnriched_ext1',
'Legacy16V2_QCD_Pt_120to170_MuEnriched',
'Legacy16V2_QCD_Pt_170to300_MuEnriched_v1',
'Legacy16V2_QCD_Pt_170to300_MuEnriched_ext1',
'Legacy16V2_QCD_Pt_300to470_MuEnriched_v1',
'Legacy16V2_QCD_Pt_300to470_MuEnriched_ext1', #10
'Legacy16V2_QCD_Pt_300to470_MuEnriched_ext2',
'Legacy16V2_QCD_Pt_470to600_MuEnriched_v1',
'Legacy16V2_QCD_Pt_470to600_MuEnriched_ext1',
'Legacy16V2_QCD_Pt_470to600_MuEnriched_ext2',
'Legacy16V2_QCD_Pt_600to800_MuEnriched_v1',
'Legacy16V2_QCD_Pt_600to800_MuEnriched_ext1',
'Legacy16V2_QCD_Pt_800to1000_MuEnriched_v1',
'Legacy16V2_QCD_Pt_800to1000_MuEnriched_ext1',
'Legacy16V2_QCD_Pt_800to1000_MuEnriched_ext2',
'Legacy16V2_QCD_Pt_1000toInf_MuEnriched_v1',#20
'Legacy16V2_QCD_Pt_1000toInf_MuEnriched_ext1',

#QCD EM enriched

'Legacy16V2_QCD_Pt_20to30_EMEnriched',
'Legacy16V2_QCD_Pt_30to50_EMEnriched_v1',
'Legacy16V2_QCD_Pt_30to50_EMEnriched_ext1',
'Legacy16V2_QCD_Pt_50to80_EMEnriched_v1',
'Legacy16V2_QCD_Pt_50to80_EMEnriched_ext1',
'Legacy16V2_QCD_Pt_80to120_EMEnriched_v1',
'Legacy16V2_QCD_Pt_80to120_EMEnriched_ext1',
'Legacy16V2_QCD_Pt_120to170_EMEnriched_v1',
'Legacy16V2_QCD_Pt_120to170_EMEnriched_ext1',#30
'Legacy16V2_QCD_Pt_170to300_EMEnriched',
'Legacy16V2_QCD_Pt_300toInf_EMEnriched',#32

]

# baseDir
#baseDir = "/afs/cern.ch/work/b/binghuan/private/TTHLep_RunII/CMSSW_10_2_16/src/BSMFramework/"
baseDir = "/afs/cern.ch/user/p/pengn/WorkSpace/AddtionSamples/CMSSW_10_2_20_UL/src/BSMFramework/"

for d in range(0,len(datasetnames)):
#for d in range(0,2):
    print 'multicrab.py: Running datasetname: ', datasetnames[d]

    lepFilt = 2
    if datasetnames[d] in tWLists:
        lepFilt = 1
        print 'multicrab_MC2016.py: Run ', datasetnames[d], ' lepFilt 1 for tW samples '
    
    if "ctcvcp" in datasetnames[d]:
        lepFilt = 0
        print 'multicrab_MC2016.py: Run ', datasetnames[d], ' lepFilt 0 for ctcvcp samples '
    
    nameLepFilt = 'optionlepfilt={}'.format(lepFilt) 
    
    config.section_('General')
    config.General.requestName = datasetnames[d]
    config.General.workArea    = datasetnames[d]
    config.General.transferLogs = True

    config.section_('JobType')
    config.JobType.pluginName  = 'Analysis'
    # List of parameters to pass to CMSSW parameter-set configuration file:
    config.JobType.psetName    = baseDir+'BSM3G_TNT_Maker/python/miniAOD_MC2016.py'
    config.JobType.inputFiles = [(baseDir+'BSM3G_TNT_Maker/data/QG/QGL_AK4chs_94X.db')]
    config.JobType.sendExternalFolder = True
    config.JobType.maxMemoryMB = 2000 # Default == 2Gb : maximum guaranteed to run on all sites
    #config.JobType.allowUndistributedCMSSW = True
    ofParam = 'ofName=' + datasetnames[d]
    config.JobType.pyCfgParams = [nameLepFilt,
                                    ofParam]
    config.section_('Data')
    config.Data.allowNonValidInputDataset = True
    config.Data.inputDataset   = datasetinputs[d]
    config.Data.inputDBS       = 'global'
    config.Data.splitting      = 'FileBased'
    #config.Data.splitting      = 'Automatic'
    config.Data.totalUnits     = 40000 #With 'FileBased' splitting tells how many files to analyse
    config.Data.unitsPerJob    = 1
    config.Data.outLFNDirBase = '/store/user/pengn/'# First part of LFN for output files (must be /store/user/<username>/ or /store/group/<username>/  )
    config.Data.outputDatasetTag = datasetnames[d]

    print 'multicrab.py: outLFNDirBase = /store/user/pengn/'
    #config.Data.publication = True

    config.section_('Site')
    config.Site.storageSite    = 'T2_CN_Beijing'#'T2_CH_CERN' # Site to which output is permenantly copied by crab3
    print 'multicrab.py: Submitting Jobs'
    #submit(config)
    p = Process(target=submit, args=(config,))
    p.start()
    p.join()
