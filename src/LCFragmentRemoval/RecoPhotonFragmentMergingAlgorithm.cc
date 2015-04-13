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

bool RecoPhotonFragmentMergingAlgorithm::GetPhotonPhotonMergingFlag(const Parameters &parameters) const
{
    if (parameters.m_energyOfCandidateCluster < m_lowEnergyOfCandidateClusterThreshold)
    {
        if (parameters.m_energyOfCandidatePeak > 0.f &&
            parameters.m_energyOfCandidateCluster > 0.f &&
            !parameters.m_hasCrossedGap &&
            parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonPhotonThresholdLow1 &&
            parameters.m_energyOfCandidatePeak / parameters.m_energyOfCandidateCluster < m_energyRatioCandidatePeakToClusterPhotonThresholdLow1)
        {
            return true;
        }

        if (!parameters.m_hasCrossedGap &&
            parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonPhotonThresholdLow2 &&
            parameters.m_energyOfCandidateCluster < m_energyOfCandidateClusterPhotonPhotonThresholdLow2)
        {
            return true;
        }

        if (parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonPhotonThresholdLow3 &&
            parameters.m_nCaloHitsCandidate < m_nCaloHitsCandidatePhotonPhotonThresholdLow3 &&
            parameters.m_hitSeparation < m_hitSeparationPhotonPhotonThresholdLow3)
        {
            return true;
        }

        if (parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_centroidSeparation < m_centroidSeparationPhotonPhotonThresholdLow4 &&
            parameters.m_nCaloHitsCandidate < m_nCaloHitsCandidatePhotonPhotonThresholdLow4 &&
            parameters.m_hitSeparation < m_hitSeparationPhotonPhotonThresholdLow4)
        {
            return true;
        }
    }
    else
    {
        if (parameters.m_energyOfCandidateCluster > 0.f &&
            parameters.m_energyOfMainCluster > 0.f &&
            parameters.m_energyOfCandidatePeak > 0.f &&
            parameters.m_energyOfMainPeak > 0.f &&
            !parameters.m_hasCrossedGap &&
            parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonPhotonThresholdHigh1 &&
            ( (parameters.m_energyOfCandidatePeak / parameters.m_energyOfCandidateCluster < m_energyRatioCandidatePeakToClusterPhotonThresholdHigh1 &&
            parameters.m_energyOfMainPeak / parameters.m_energyOfMainCluster > m_energyRatioMainPeakToClusterPhotonThresholdHigh1 &&
            (parameters.m_energyOfCandidatePeak / parameters.m_energyOfCandidateCluster + m_triangularEnergyRatioCandidatePeakToClusterPhotonThresholdHigh1 * parameters.m_energyOfMainPeak / parameters.m_energyOfMainCluster) < m_triangularSumEnergyRatioCandidatePeakToClusterPhotonThresholdHigh1) ||
            (parameters.m_energyOfCandidatePeak / parameters.m_energyOfCandidateCluster < m_linearEnergyRatioCandidatePeakToClusterPhotonThresholdHigh1 &&
            parameters.m_energyOfMainPeak / parameters.m_energyOfMainCluster > m_linearEnergyRatioMainPeakToClusterPhotonThresholdHigh1) ))
        {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool RecoPhotonFragmentMergingAlgorithm::GetPhotonNeutralMergingFlag(const Parameters &parameters) const
{
    if (parameters.m_energyOfCandidateCluster < m_lowEnergyOfCandidateClusterThreshold)
    {
        if (parameters.m_energyOfCandidatePeak > 0.f &&
            parameters.m_energyOfCandidateCluster > 0.f &&
            !parameters.m_hasCrossedGap &&
            parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonNeutralThresholdLow1 &&
            parameters.m_energyOfCandidatePeak / parameters.m_energyOfCandidateCluster < m_energyRatioCandidatePeakToClusterNeutralThresholdLow1)
        {
            return true;
        }

        if (parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonNeutralThresholdLow2 &&
            parameters.m_hitSeparation < m_hitSeparationPhotonNeutralThresholdLow2 &&
            parameters.m_nCaloHitsCandidate < m_nCaloHitsCandidatePhotonNeutralThresholdLow2)
        {
            return true;
        }

        if (parameters.m_energyOfCandidateCluster > 0.f &&
            parameters.m_energyOfMainCluster > 0.f &&
            parameters.m_hitSeparation < m_hitSeparationPhotonNeutralThresholdLow3 &&
            parameters.m_energyOfCandidateCluster / parameters.m_energyOfMainCluster < m_energyRatioCandidateToMainNeutralThresholdLow3 &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonNeutralThresholdLow3 &&
            parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation)
        {
            return true;
        }
    }
    else
    {
        if (parameters.m_energyOfCandidateCluster > 0.f &&
            parameters.m_energyOfMainCluster > 0.f &&
            parameters.m_energyOfCandidatePeak > 0.f &&
            parameters.m_energyOfMainPeak > 0.f &&
            !parameters.m_hasCrossedGap &&
            parameters.m_energyOfCandidateCluster / parameters.m_energyOfMainCluster < m_energyRatioCandidateToMainNeutralThresholdHigh1 &&
            parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonNeutralThresholdHigh1 &&
            ( (parameters.m_energyOfCandidatePeak / parameters.m_energyOfCandidateCluster < m_energyRatioCandidatePeakToClusterNeutralThresholdHigh1 &&
            parameters.m_energyOfMainPeak / parameters.m_energyOfMainCluster > m_energyRatioMainPeakToClusterNeutralThresholdHigh1 &&
            (parameters.m_energyOfCandidatePeak / parameters.m_energyOfCandidateCluster + m_triangularEnergyRatioMainPeakToClusterNeutralThresholdHigh1 * parameters.m_energyOfMainPeak / parameters.m_energyOfMainCluster) < m_triangularSumEnergyRatioMainPeakToClusterNeutralThresholdHigh1) ||
            (parameters.m_energyOfCandidatePeak / parameters.m_energyOfCandidateCluster < m_squareEnergyRatioCandidatePeakToClusterNeutralThresholdHigh1 &&
            parameters.m_energyOfMainPeak / parameters.m_energyOfMainCluster > m_squareEnergyRatioMainPeakToClusterNeutralThresholdHigh1) ))
        {
            return true;
        }

        if (parameters.m_energyOfCandidateCluster > 0.f &&
            parameters.m_energyOfMainCluster > 0.f &&
            parameters.m_hitSeparation < m_hitSeparationPhotonNeutralThresholdHigh2 &&
            parameters.m_energyOfCandidateCluster / parameters.m_energyOfMainCluster<m_energyRatioCandidateToMainNeutralThresholdHigh2 &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonNeutralThresholdHigh2 &&
            parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation)
        {
            return true;
        }
    }

    return false;
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
