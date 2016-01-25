/**
 *  @file   LCContent/src/LCFragmentRemoval/RecoPhotonFragmentMergingAlgorithm.cc
 *
 *  @brief  Implementation of the reco photon fragment merging algorithm class.
 *
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCFragmentRemoval/RecoPhotonFragmentMergingAlgorithm.h"

using namespace pandora;

namespace lc_content
{

RecoPhotonFragmentMergingAlgorithm::RecoPhotonFragmentMergingAlgorithm() :
    m_energyRatioCandidatePeakToClusterNeutralThresholdLow1(0.7f),
    m_hitSeparationPhotonPhotonThresholdLow3(13.f),
    m_hitSeparationPhotonPhotonThresholdLow4(35.f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode RecoPhotonFragmentMergingAlgorithm::GetAffectedClusterList(const ClusterList *&pClusterList) const
{
    // Take non-photon clusters in current list, save into a separate list, process using topological associations, then re-merge into original list
    const ClusterList *pInputClusterList = NULL;
    std::string inputClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pInputClusterList, inputClusterListName));

    ClusterList nonPhotonClusterList;
    for (ClusterList::const_iterator iter = pInputClusterList->begin(), iterEnd = pInputClusterList->end(); iter != iterEnd; ++iter)
    {
        const Cluster *const pCluster(*iter);

        if (pCluster->GetParticleIdFlag() != PHOTON)
            nonPhotonClusterList.insert(pCluster);
    }

    if (!nonPhotonClusterList.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList(*this, m_nonPhotonClusterListName, nonPhotonClusterList));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<Cluster>(*this, m_nonPhotonClusterListName));

        for (StringVector::const_iterator iter = m_associationAlgorithms.begin(), iterEnd = m_associationAlgorithms.end(); iter != iterEnd; ++iter)
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, *iter));
    }

    const ClusterList *pCurrentClusterList = NULL;
    std::string currentClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pCurrentClusterList, currentClusterListName));

    if (!pCurrentClusterList->empty())
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList<Cluster>(*this, inputClusterListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<Cluster>(*this, inputClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool RecoPhotonFragmentMergingAlgorithm::GetPhotonPhotonMergingFlag(const Parameters &parameters) const
{
    if (parameters.m_energyOfCandidateCluster < m_lowEnergyOfCandidateClusterThreshold)
    {
        return (this->IsPhotonFragmentInShowerProfile(parameters) ||
                this->IsAbsoluteLowEnergyPhotonFragment(parameters) ||
                this->IsSmallPhotonFragment1(parameters) ||
                this->IsSmallPhotonFragment2(parameters));
    }
    else
    {
        return (this->IsHighEnergyPhotonFragmentInShowerProfile(parameters));
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool RecoPhotonFragmentMergingAlgorithm::GetPhotonNeutralMergingFlag(const Parameters &parameters) const
{
    if (parameters.m_energyOfCandidateCluster < m_lowEnergyOfCandidateClusterThreshold)
    {
        return (this->IsNeutralFragmentInShowerProfile(parameters) ||
                this->IsSmallNeutralFragment(parameters) ||
                this->IsRelativeLowEnergyNeutralFragment(parameters));
    }
    else
    {
        return (this->IsHighEnergyNeutralFragmentInShowerProfile(parameters) ||
                this->IsHighEnergyRelativeLowEnergyNeutralFragment(parameters));
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode RecoPhotonFragmentMergingAlgorithm::DeleteClusters(const ClusterVector &photonClusterVec, const ClusterVector &neutralClusterVec,
    const ClusterVector &unusedClusterVec) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->DeleteClusters(photonClusterVec, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->DeleteClusters(neutralClusterVec, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->DeleteClusters(unusedClusterVec, false));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode RecoPhotonFragmentMergingAlgorithm::DeleteClusters(const ClusterVector &clusterVec, const bool nonFixedPhotonsOnly) const
{
    for (ClusterVector::const_iterator iter = clusterVec.begin(), iterEnd = clusterVec.end(); iter != iterEnd; ++iter)
    {
        const Cluster *const pCluster = *iter;

        if (pCluster == NULL)
            continue;

        if (!nonFixedPhotonsOnly || (PHOTON != pCluster->GetParticleIdFlag()))
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Delete(*this, pCluster));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool RecoPhotonFragmentMergingAlgorithm::IsPhotonFragmentInShowerProfile(const Parameters &parameters) const
{
    return (parameters.m_energyOfCandidateCluster > 0.f &&
            parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonPhotonThresholdLow1 &&
            (parameters.m_energyOfMainPeak) / (parameters.m_energyOfCandidateCluster + parameters.m_energyOfMainCluster) > m_minRatioTotalShowerPeakEnergyToTotalEnergyThreshold &&
            ( (parameters.m_energyOfCandidatePeak < std::numeric_limits<float>::min()) ||
            ( parameters.m_energyOfCandidatePeak / parameters.m_energyOfCandidateCluster < m_smallCandidateFractionThresholdLow &&
            parameters.m_energyOfMainPeak > parameters.m_energyOfMainCluster
            ) ) ) ;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool RecoPhotonFragmentMergingAlgorithm::IsAbsoluteLowEnergyPhotonFragment(const Parameters &parameters) const
{
    return (!parameters.m_hasCrossedGap &&
            parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonPhotonThresholdLow2 &&
            parameters.m_energyOfCandidateCluster < m_energyOfCandidateClusterPhotonPhotonThresholdLow2);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool RecoPhotonFragmentMergingAlgorithm::IsSmallPhotonFragment1(const Parameters &parameters) const
{
    return (parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonPhotonThresholdLow3 &&
            parameters.m_nCaloHitsCandidate < m_nCaloHitsCandidatePhotonPhotonThresholdLow3 &&
            parameters.m_hitSeparation < m_hitSeparationPhotonPhotonThresholdLow3);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool RecoPhotonFragmentMergingAlgorithm::IsSmallPhotonFragment2(const Parameters &parameters) const
{
    return (parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_centroidSeparation < m_centroidSeparationPhotonPhotonThresholdLow4 &&
            parameters.m_nCaloHitsCandidate < m_nCaloHitsCandidatePhotonPhotonThresholdLow4 &&
            parameters.m_hitSeparation < m_hitSeparationPhotonPhotonThresholdLow4);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool RecoPhotonFragmentMergingAlgorithm::IsHighEnergyPhotonFragmentInShowerProfile(const Parameters &parameters) const
{
    return (parameters.m_energyOfCandidateCluster > 0.f &&
            parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonPhotonThresholdLow1 &&
            (parameters.m_energyOfMainPeak) / (parameters.m_energyOfCandidateCluster + parameters.m_energyOfMainCluster) > m_minRatioTotalShowerPeakEnergyToTotalEnergyThreshold &&
            ( (parameters.m_energyOfCandidatePeak < std::numeric_limits<float>::min()) ||
            ( parameters.m_energyOfCandidatePeak / parameters.m_energyOfCandidateCluster < m_smallCandidateFractionThresholdLow &&
            parameters.m_energyOfMainPeak > parameters.m_energyOfMainCluster
            ))) ;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool RecoPhotonFragmentMergingAlgorithm::IsNeutralFragmentInShowerProfile(const Parameters &parameters) const
{
    return (parameters.m_energyOfCandidateCluster > 0.f &&
            parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonNeutralThresholdLow1 &&
            (parameters.m_energyOfMainPeak) / (parameters.m_energyOfCandidateCluster + parameters.m_energyOfMainCluster) > m_minRatioTotalShowerPeakEnergyToTotalEnergyThreshold &&
            ( (parameters.m_energyOfCandidatePeak < std::numeric_limits<float>::min()) ||
            ( parameters.m_energyOfCandidatePeak / parameters.m_energyOfCandidateCluster < m_smallCandidateFractionThresholdLow &&
            parameters.m_energyOfMainPeak > parameters.m_energyOfMainCluster
            ))) ;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool RecoPhotonFragmentMergingAlgorithm::IsSmallNeutralFragment(const Parameters &parameters) const
{
    return (parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonNeutralThresholdLow2 &&
            parameters.m_hitSeparation < m_hitSeparationPhotonNeutralThresholdLow2 &&
            parameters.m_nCaloHitsCandidate < m_nCaloHitsCandidatePhotonNeutralThresholdLow2);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool RecoPhotonFragmentMergingAlgorithm::IsRelativeLowEnergyNeutralFragment(const Parameters &parameters) const
{
    return (parameters.m_energyOfCandidateCluster > 0.f &&
            parameters.m_energyOfMainCluster > 0.f &&
            parameters.m_hitSeparation < m_hitSeparationPhotonNeutralThresholdLow3 &&
            parameters.m_energyOfCandidateCluster / parameters.m_energyOfMainCluster < m_energyRatioCandidateToMainNeutralThresholdLow3 &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonNeutralThresholdLow3 &&
            parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool RecoPhotonFragmentMergingAlgorithm::IsHighEnergyNeutralFragmentInShowerProfile(const Parameters &parameters) const
{
    return (parameters.m_energyOfCandidateCluster > 0.f &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonNeutralThresholdLow1 &&
            (parameters.m_energyOfMainPeak) / (parameters.m_energyOfCandidateCluster + parameters.m_energyOfMainCluster) > m_minRatioTotalShowerPeakEnergyToTotalEnergyThreshold &&
            ( (parameters.m_energyOfCandidatePeak < std::numeric_limits<float>::min()) ||
            ( parameters.m_energyOfCandidatePeak / parameters.m_energyOfCandidateCluster < m_smallCandidateFractionThresholdLow &&
            parameters.m_energyOfMainPeak > parameters.m_energyOfMainCluster
            ) ) ) ;
}
//------------------------------------------------------------------------------------------------------------------------------------------

bool RecoPhotonFragmentMergingAlgorithm::IsHighEnergyRelativeLowEnergyNeutralFragment(const Parameters &parameters) const
{
    return (parameters.m_energyOfCandidateCluster > 0.f &&
            parameters.m_energyOfMainCluster > 0.f &&
            parameters.m_hitSeparation < m_hitSeparationPhotonNeutralThresholdHigh2 &&
            parameters.m_energyOfCandidateCluster / parameters.m_energyOfMainCluster<m_energyRatioCandidateToMainNeutralThresholdHigh2 &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonNeutralThresholdHigh2 &&
            parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode RecoPhotonFragmentMergingAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ProcessAlgorithmList(*this, xmlHandle,
        "associationAlgorithms", m_associationAlgorithms));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "NonPhotonClusterListName", m_nonPhotonClusterListName));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyRatioCandidatePeakToClusterNeutralThresholdLow1", m_energyRatioCandidatePeakToClusterNeutralThresholdLow1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestDistanceSeparationPhotonPhotonThresholdLow3", m_hitSeparationPhotonPhotonThresholdLow3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestDistanceSeparationPhotonPhotonThresholdLow4", m_hitSeparationPhotonPhotonThresholdLow4));

    return PhotonFragmentMergingBaseAlgorithm::ReadSettings(xmlHandle);
}

} // end namespace lc_content
