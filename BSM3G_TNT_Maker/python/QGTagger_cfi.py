import FWCore.ParameterSet.Config as cms

QGTagger = cms.EDProducer('QGTagger',
  srcJets		= cms.InputTag('selectedUpdatedPatJets'),
  jetsLabel		= cms.string('QGL_AK4PFchs'),
  srcRho 		= cms.InputTag('fixedGridRhoFastjetAll'),		
  srcVertexCollection	= cms.InputTag('offlinePrimaryVerticesWithBS'),
  useQualityCuts	= cms.bool(False)
)
