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
# signal
#'Legacy17V2_TTHnobb_v1', #0
#'Legacy17V2_TTHnobb_ext', 
#'Legacy17V2_ttHnobb',
#'Legacy17V2_TTH_ctcvcp',

#QCD Mu enriched samples
'Legacy17V2_QCD_Pt_15To20_MuEnriched',#0
'Legacy17V2_QCD_Pt_20To30_MuEnriched',
'Legacy17V2_QCD_Pt_30To50_MuEnriched',
'Legacy17V2_QCD_Pt_50To80_MuEnriched',
'Legacy17V2_QCD_Pt_80To120_MuEnriched',
'Legacy17V2_QCD_Pt_120To170_MuEnriched',
'Legacy17V2_QCD_Pt_170To300_MuEnriched',
'Legacy17V2_QCD_Pt_300To470_MuEnriched',
'Legacy17V2_QCD_Pt_470To600_MuEnriched',
'Legacy17V2_QCD_Pt_600To800_MuEnriched',
'Legacy17V2_QCD_Pt_800To1000_MuEnriched',#10
'Legacy17V2_QCD_Pt_1000ToInf_MuEnriched',
 


                ]
 datasetinputs = [
# signal
#'/ttHToNonbb_M125_TuneCP5_13TeV-powheg-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM',#0
#'/ttHToNonbb_M125_TuneCP5_13TeV-powheg-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14_ext1-v2/MINIAODSIM',
#'/ttHJetToNonbb_M125_TuneCP5_13TeV_amcatnloFXFX_madspin_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM',
#'/TTH_4f_ctcvcp_TuneCP5_13TeV_madgraph_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM',
                
'/QCD_Pt-15to20_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM',#0
'/QCD_Pt-20to30_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM',
'/QCD_Pt-30to50_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM',
'/QCD_Pt-50to80_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM',
'/QCD_Pt-80to120_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM',
'/QCD_Pt-120to170_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM',
'/QCD_Pt-170to300_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM',
'/QCD_Pt-300to470_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM',
'/QCD_Pt-470to600_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM',
'/QCD_Pt-600to800_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM',
'/QCD_Pt-800to1000_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM',#10
'/QCD_Pt-1000toInf_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM',





]

# samples also used in tW or bstar
# minimum lepton are set to 1 instead of 2 
tWLists = [

'Legacy17V2_QCD_Pt_15To20_MuEnriched',#0
'Legacy17V2_QCD_Pt_20To30_MuEnriched',
'Legacy17V2_QCD_Pt_30To50_MuEnriched',
'Legacy17V2_QCD_Pt_50To80_MuEnriched',
'Legacy17V2_QCD_Pt_80To120_MuEnriched',
'Legacy17V2_QCD_Pt_120To170_MuEnriched',
'Legacy17V2_QCD_Pt_170To300_MuEnriched',
'Legacy17V2_QCD_Pt_300To470_MuEnriched',
'Legacy17V2_QCD_Pt_470To600_MuEnriched',
'Legacy17V2_QCD_Pt_600To800_MuEnriched',
'Legacy17V2_QCD_Pt_800To1000_MuEnriched',#10
'Legacy17V2_QCD_Pt_1000ToInf_MuEnriched',

]

# baseDir
#baseDir = "/afs/cern.ch/work/b/binghuan/private/TTHLep_RunII/CMSSW_10_2_16/src/BSMFramework/"
baseDir = "/afs/cern.ch/user/p/pengn/WorkSpace/AddtionSamples/CMSSW_10_2_20_UL/src/BSMFramework/"

for d in range(0,len(datasetnames)):
#for d in range(129,131):
    print 'multicrab.py: Running datasetname: ', datasetnames[d]
    
    lepFilt = 2
    if datasetnames[d] in tWLists:
        lepFilt = 1
        print 'multicrab_MC2017.py: Run ', datasetnames[d], ' lepFilt 1 '
    
    if "ctcvcp" in datasetnames[d]:
        lepFilt = 0
        print 'multicrab_MC2017.py: Run ', datasetnames[d], ' lepFilt 0 for ctcvcp samples '
    
    
    nameLepFilt = 'optionlepfilt={}'.format(lepFilt) 
    
    config.section_('General')
    config.General.requestName = datasetnames[d]
    config.General.workArea    = datasetnames[d]
    config.General.transferLogs = True

    config.section_('JobType')
    config.JobType.pluginName  = 'Analysis'
    # List of parameters to pass to CMSSW parameter-set configuration file:
    config.JobType.psetName    = baseDir+'BSM3G_TNT_Maker/python/miniAOD_MC2017.py'
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
