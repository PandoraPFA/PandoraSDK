/**
 *  @file   LCContent/include/LCContent.h
 * 
 *  @brief  Header file detailing content for use with particle flow reconstruction at an e+e- linear collider
 * 
 *  $Log: $
 */
#ifndef LINEAR_COLLIDER_CONTENT_H
#define LINEAR_COLLIDER_CONTENT_H 1

#include "LCCheating/CheatingClusterCleaningAlgorithm.h"
#include "LCCheating/CheatingTrackToClusterMatching.h"
#include "LCCheating/PerfectClusteringAlgorithm.h"
#include "LCCheating/PerfectFragmentRemovalAlgorithm.h"
#include "LCCheating/PerfectParticleFlowAlgorithm.h"

#include "LCClustering/ClusteringParentAlgorithm.h"
#include "LCClustering/ConeClusteringAlgorithm.h"
#include "LCClustering/ForcedClusteringAlgorithm.h"

#include "LCFragmentRemoval/MainFragmentRemovalAlgorithm.h"
#include "LCFragmentRemoval/MergeSplitPhotonsAlgorithm.h"
#include "LCFragmentRemoval/NeutralFragmentRemovalAlgorithm.h"
#include "LCFragmentRemoval/PhotonFragmentRemovalAlgorithm.h"
#include "LCFragmentRemoval/BeamHaloMuonRemovalAlgorithm.h"

#include "LCMonitoring/ClusterComparisonAlgorithm.h"
#include "LCMonitoring/DumpPfosMonitoringAlgorithm.h"
#include "LCMonitoring/EfficiencyMonitoringAlgorithm.h"
#include "LCMonitoring/VisualMonitoringAlgorithm.h"

#include "LCParticleId/FinalParticleIdAlgorithm.h"
#include "LCParticleId/MuonReconstructionAlgorithm.h"
#include "LCParticleId/PhotonReconstructionAlgorithm.h"
#include "LCParticleId/PhotonRecoveryAlgorithm.h"

#include "LCPfoConstruction/CLICPfoSelectionAlgorithm.h"
#include "LCPfoConstruction/PfoCreationAlgorithm.h"
#include "LCPfoConstruction/PfoCreationParentAlgorithm.h"
#include "LCPfoConstruction/V0PfoCreationAlgorithm.h"

#include "LCPlugins/LCBFieldPlugin.h"
#include "LCPlugins/LCEnergyCorrectionPlugins.h"
#include "LCPlugins/LCParticleIdPlugins.h"
#include "LCPlugins/LCPseudoLayerPlugin.h"
#include "LCPlugins/LCShowerProfilePlugin.h"

#include "LCReclustering/ExitingTrackAlg.h"
#include "LCReclustering/ForceSplitTrackAssociationsAlg.h"
#include "LCReclustering/ResolveTrackAssociationsAlg.h"
#include "LCReclustering/SplitMergedClustersAlg.h"
#include "LCReclustering/SplitTrackAssociationsAlg.h"
#include "LCReclustering/TrackDrivenAssociationAlg.h"
#include "LCReclustering/TrackDrivenMergingAlg.h"

#include "LCTopologicalAssociation/BackscatteredTracksAlgorithm.h"
#include "LCTopologicalAssociation/BackscatteredTracks2Algorithm.h"
#include "LCTopologicalAssociation/BrokenTracksAlgorithm.h"
#include "LCTopologicalAssociation/ConeBasedMergingAlgorithm.h"
#include "LCTopologicalAssociation/IsolatedHitMergingAlgorithm.h"
#include "LCTopologicalAssociation/LoopingTracksAlgorithm.h"
#include "LCTopologicalAssociation/MipPhotonSeparationAlgorithm.h"
#include "LCTopologicalAssociation/MuonPhotonSeparationAlgorithm.h"
#include "LCTopologicalAssociation/MuonClusterAssociationAlgorithm.h"
#include "LCTopologicalAssociation/ProximityBasedMergingAlgorithm.h"
#include "LCTopologicalAssociation/ShowerMipMergingAlgorithm.h"
#include "LCTopologicalAssociation/ShowerMipMerging2Algorithm.h"
#include "LCTopologicalAssociation/ShowerMipMerging3Algorithm.h"
#include "LCTopologicalAssociation/ShowerMipMerging4Algorithm.h"
#include "LCTopologicalAssociation/SoftClusterMergingAlgorithm.h"
#include "LCTopologicalAssociation/TopologicalAssociationParentAlgorithm.h"

#include "LCTrackClusterAssociation/LoopingTrackAssociationAlgorithm.h"
#include "LCTrackClusterAssociation/TrackClusterAssociationAlgorithm.h"
#include "LCTrackClusterAssociation/TrackRecoveryAlgorithm.h"
#include "LCTrackClusterAssociation/TrackRecoveryHelixAlgorithm.h"
#include "LCTrackClusterAssociation/TrackRecoveryInteractionsAlgorithm.h"

#include "LCUtility/CaloHitPreparationAlgorithm.h"
#include "LCUtility/ClusterPreparationAlgorithm.h"
#include "LCUtility/EventPreparationAlgorithm.h"
#include "LCUtility/PfoPreparationAlgorithm.h"
#include "LCUtility/TrackPreparationAlgorithm.h"

/**
 *  @brief  LCContent class
 */
class LCContent
{
public:
    #define LC_ALGORITHM_LIST(d)                                                                                                \
        d("CheatingClusterCleaning",                lc_content::CheatingClusterCleaningAlgorithm::Factory)                      \
        d("CheatingTrackToClusterMatching",         lc_content::CheatingTrackToClusterMatching::Factory)                        \
        d("PerfectClustering",                      lc_content::PerfectClusteringAlgorithm::Factory)                            \
        d("PerfectFragmentRemoval",                 lc_content::PerfectFragmentRemovalAlgorithm::Factory)                       \
        d("PerfectParticleFlow",                    lc_content::PerfectParticleFlowAlgorithm::Factory)                          \
        d("ClusteringParent",                       lc_content::ClusteringParentAlgorithm::Factory)                             \
        d("ConeClustering",                         lc_content::ConeClusteringAlgorithm::Factory)                               \
        d("ForcedClustering",                       lc_content::ForcedClusteringAlgorithm::Factory)                             \
        d("MainFragmentRemoval",                    lc_content::MainFragmentRemovalAlgorithm::Factory)                          \
        d("MergeSplitPhotons",                      lc_content::MergeSplitPhotonsAlgorithm::Factory)                            \
        d("NeutralFragmentRemoval",                 lc_content::NeutralFragmentRemovalAlgorithm::Factory)                       \
        d("PhotonFragmentRemoval",                  lc_content::PhotonFragmentRemovalAlgorithm::Factory)                        \
        d("BeamHaloMuonRemoval",                    lc_content::BeamHaloMuonRemovalAlgorithm::Factory)                          \
        d("ClusterComparison",                      lc_content::ClusterComparisonAlgorithm::Factory)                            \
        d("DumpPfosMonitoring",                     lc_content::DumpPfosMonitoringAlgorithm::Factory)                           \
        d("EfficiencyMonitoring",                   lc_content::EfficiencyMonitoringAlgorithm::Factory)                         \
        d("VisualMonitoring",                       lc_content::VisualMonitoringAlgorithm::Factory)                             \
        d("FinalParticleId",                        lc_content::FinalParticleIdAlgorithm::Factory)                              \
        d("MuonReconstruction",                     lc_content::MuonReconstructionAlgorithm::Factory)                           \
        d("PhotonReconstruction",                   lc_content::PhotonReconstructionAlgorithm::Factory)                         \
        d("PhotonRecovery",                         lc_content::PhotonRecoveryAlgorithm::Factory)                               \
        d("CLICPfoSelection",                       lc_content::CLICPfoSelectionAlgorithm::Factory)                             \
        d("PfoCreation",                            lc_content::PfoCreationAlgorithm::Factory)                                  \
        d("PfoCreationParent",                      lc_content::PfoCreationParentAlgorithm::Factory)                            \
        d("V0PfoCreation",                          lc_content::V0PfoCreationAlgorithm::Factory)                                \
        d("ExitingTrack",                           lc_content::ExitingTrackAlg::Factory)                                       \
        d("ForceSplitTrackAssociations",            lc_content::ForceSplitTrackAssociationsAlg::Factory)                        \
        d("ResolveTrackAssociations",               lc_content::ResolveTrackAssociationsAlg::Factory)                           \
        d("SplitMergedClusters",                    lc_content::SplitMergedClustersAlg::Factory)                                \
        d("SplitTrackAssociations",                 lc_content::SplitTrackAssociationsAlg::Factory)                             \
        d("TrackDrivenAssociation",                 lc_content::TrackDrivenAssociationAlg::Factory)                             \
        d("TrackDrivenMerging",                     lc_content::TrackDrivenMergingAlg::Factory)                                 \
        d("BackscatteredTracks",                    lc_content::BackscatteredTracksAlgorithm::Factory)                          \
        d("BackscatteredTracks2",                   lc_content::BackscatteredTracks2Algorithm::Factory)                         \
        d("BrokenTracks",                           lc_content::BrokenTracksAlgorithm::Factory)                                 \
        d("ConeBasedMerging",                       lc_content::ConeBasedMergingAlgorithm::Factory)                             \
        d("IsolatedHitMerging",                     lc_content::IsolatedHitMergingAlgorithm::Factory)                           \
        d("LoopingTracks",                          lc_content::LoopingTracksAlgorithm::Factory)                                \
        d("MipPhotonSeparation",                    lc_content::MipPhotonSeparationAlgorithm::Factory)                          \
        d("MuonPhotonSeparation",                   lc_content::MuonPhotonSeparationAlgorithm::Factory)                         \
        d("MuonClusterAssociation",                 lc_content::MuonClusterAssociationAlgorithm::Factory)                       \
        d("ProximityBasedMerging",                  lc_content::ProximityBasedMergingAlgorithm::Factory)                        \
        d("ShowerMipMerging",                       lc_content::ShowerMipMergingAlgorithm::Factory)                             \
        d("ShowerMipMerging2",                      lc_content::ShowerMipMerging2Algorithm::Factory)                            \
        d("ShowerMipMerging3",                      lc_content::ShowerMipMerging3Algorithm::Factory)                            \
        d("ShowerMipMerging4",                      lc_content::ShowerMipMerging4Algorithm::Factory)                            \
        d("SoftClusterMerging",                     lc_content::SoftClusterMergingAlgorithm::Factory)                           \
        d("TopologicalAssociationParent",           lc_content::TopologicalAssociationParentAlgorithm::Factory)                 \
        d("LoopingTrackAssociation",                lc_content::LoopingTrackAssociationAlgorithm::Factory)                      \
        d("TrackRecovery",                          lc_content::TrackRecoveryAlgorithm::Factory)                                \
        d("TrackRecoveryHelix",                     lc_content::TrackRecoveryHelixAlgorithm::Factory)                           \
        d("TrackRecoveryInteractions",              lc_content::TrackRecoveryInteractionsAlgorithm::Factory)                    \
        d("TrackClusterAssociation",                lc_content::TrackClusterAssociationAlgorithm::Factory)                      \
        d("CaloHitPreparation",                     lc_content::CaloHitPreparationAlgorithm::Factory)                           \
        d("ClusterPreparation",                     lc_content::ClusterPreparationAlgorithm::Factory)                           \
        d("EventPreparation",                       lc_content::EventPreparationAlgorithm::Factory)                             \
        d("PfoPreparation",                         lc_content::PfoPreparationAlgorithm::Factory)                               \
        d("TrackPreparation",                       lc_content::TrackPreparationAlgorithm::Factory)

    #define LC_ENERGY_CORRECTION_LIST(d)                                                                                        \
        d("CleanClusters",          pandora::HADRONIC,      lc_content::LCEnergyCorrectionPlugins::CleanCluster)                \
        d("ScaleHotHadrons",        pandora::HADRONIC,      lc_content::LCEnergyCorrectionPlugins::ScaleHotHadrons)             \
        d("MuonCoilCorrection",     pandora::HADRONIC,      lc_content::LCEnergyCorrectionPlugins::MuonCoilCorrection)

    #define LC_PARTICLE_ID_LIST(d)                                                                                              \
        d("LCEmShowerId",                           lc_content::LCParticleIdPlugins::LCEmShowerId)                              \
        d("LCPhotonId",                             lc_content::LCParticleIdPlugins::LCPhotonId)                                \
        d("LCElectronId",                           lc_content::LCParticleIdPlugins::LCElectronId)                              \
        d("LCMuonId",                               lc_content::LCParticleIdPlugins::LCMuonId)

    /**
     *  @brief  Register all the linear collider algorithms with pandora
     * 
     *  @param  pandora the pandora instance with which to register content
     */
    static pandora::StatusCode RegisterAlgorithms(const pandora::Pandora &pandora);

    /**
     *  @brief  Register the basic (no configuration required on user side) linear collider plugins with pandora
     * 
     *  @param  pandora the pandora instance with which to register content
     */
    static pandora::StatusCode RegisterBasicPlugins(const pandora::Pandora &pandora);

    /**
     *  @brief  Register the b field plugin (note user side configuration) with pandora
     * 
     *  @param  pandora the pandora instance with which to register content
     *  @param  innerBField the bfield in the main tracker, ecal and hcal, units Tesla
     *  @param  muonBarrelBField the bfield in the muon barrel, units Tesla
     *  @param  muonEndCapBField the bfield in the muon endcap, units Tesla
     */
    static pandora::StatusCode RegisterBFieldPlugin(const pandora::Pandora &pandora, const float innerBField, const float muonBarrelBField,
        const float muonEndCapBField);

    /**
     *  @brief  Register the non linearity energy correction plugin (note user side configuration) with pandora
     * 
     *  @param  pandora the pandora instance with which to register content
     *  @param  name the name/label associated with the energy correction plugin
     *  @param  energyCorrectionType the energy correction type
     *  @param  inputEnergyCorrectionPoints the input energy points for energy correction
     *  @param  outputEnergyCorrectionPoints the output energy points for energy correction
     */
    static pandora::StatusCode RegisterNonLinearityEnergyCorrection(const pandora::Pandora &pandora, const std::string &name,
        const pandora::EnergyCorrectionType energyCorrectionType, const pandora::FloatVector &inputEnergyCorrectionPoints,
        const pandora::FloatVector &outputEnergyCorrectionPoints);
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::StatusCode LCContent::RegisterAlgorithms(const pandora::Pandora &pandora)
{
    LC_ALGORITHM_LIST(PANDORA_REGISTER_ALGORITHM);

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::StatusCode LCContent::RegisterBasicPlugins(const pandora::Pandora &pandora)
{
    LC_ENERGY_CORRECTION_LIST(PANDORA_REGISTER_ENERGY_CORRECTION);
    LC_PARTICLE_ID_LIST(PANDORA_REGISTER_PARTICLE_ID);

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::SetPseudoLayerPlugin(pandora, new lc_content::LCPseudoLayerPlugin));
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::SetShowerProfilePlugin(pandora, new lc_content::LCShowerProfilePlugin));

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::StatusCode LCContent::RegisterBFieldPlugin(const pandora::Pandora &pandora, const float innerBField,
    const float muonBarrelBField, const float muonEndCapBField)
{
    return PandoraApi::SetBFieldPlugin(pandora, new lc_content::LCBFieldPlugin(innerBField, muonBarrelBField, muonEndCapBField));
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::StatusCode LCContent::RegisterNonLinearityEnergyCorrection(const pandora::Pandora &pandora, const std::string &name,
    const pandora::EnergyCorrectionType energyCorrectionType, const pandora::FloatVector &inputEnergyCorrectionPoints,
    const pandora::FloatVector &outputEnergyCorrectionPoints)
{
    return PandoraApi::RegisterEnergyCorrectionPlugin(pandora, name, energyCorrectionType,
        new lc_content::LCEnergyCorrectionPlugins::NonLinearityCorrection(inputEnergyCorrectionPoints, outputEnergyCorrectionPoints));
}

#endif // #ifndef LINEAR_COLLIDER_CONTENT_H
