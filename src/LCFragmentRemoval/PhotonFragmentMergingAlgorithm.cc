/**
 *  @file   LCContent/src/LCFragmentRemoval/PhotonFragmentMergingAlgorithm.cc
 *
 *  @brief  Implementation of the photon fragment merging algorithm class.
 *
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCFragmentRemoval/PhotonFragmentMergingAlgorithm.h"

using namespace pandora;

namespace lc_content
{

PhotonFragmentMergingAlgorithm::PhotonFragmentMergingAlgorithm() :
    m_centroidSeparationPhotonNeutralThresholdLow1(40.f),
    m_hitSeparationPhotonNeutralThresholdHigh3(20.f),
    m_energyRatioCandidateToMainNeutralThresholdHigh3(0.1f),
    m_weightedLayerSeparationPhotonNeutralThresholdHigh3(40.f),
    m_energyOfCandidateClusterPhotonNeutralThresholdHigh3(10.f),
    m_hitSeparationPhotonNeutralThresholdHigh4(20.f),
    m_energyRatioCandidateToMainNeutralThresholdHigh4(0.2f),
    m_weightedLayerSeparationPhotonNeutralThresholdHigh4(20.f),
    m_energyOfCandidateClusterPhotonNeutralThresholdHigh4(10.f),
    m_centroidSeparationPhotonPhotonThresholdLow3(50.f),
    m_energyOfCandidateClusterPhotonPhotonThresholdLow4(0.6f),
    m_cosineBarrelLimit(0.7f),
    m_nCaloHitsCandidatePhotonPhotonThresholdLow5(20),
    m_weightedLayerSeparationPhotonPhotonThresholdLow5(50.f),
    m_hitSeparationPhotonPhotonThresholdLow6(20.f),
    m_energyRatioCandidateToMainPhotonThresholdLow6(0.01f),
    m_weightedLayerSeparationPhotonPhotonThresholdLow6(40.f),
    m_hitSeparationPhotonPhotonThresholdHigh2(20.f),
    m_energyRatioCandidateToMainPhotonThresholdHigh2(0.02f),
    m_weightedLayerSeparationPhotonPhotonThresholdHigh2(40.f)
{
    // ATTN Different default values to those specified in the base class
    m_weightedLayerSeparationPhotonNeutralThresholdHigh1 = 20.f;
    m_weightedLayerSeparationPhotonPhotonThresholdLow1 = 30.f;
    m_energyOfCandidateClusterPhotonPhotonThresholdLow2 = 0.4f;
    m_nCaloHitsCandidatePhotonPhotonThresholdLow3 = 40.f;
    m_centroidSeparationPhotonPhotonThresholdLow4 = 60.f;
    m_nCaloHitsCandidatePhotonPhotonThresholdLow4 = 40.f;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonFragmentMergingAlgorithm::GetAffectedClusterList(const ClusterList *&pClusterList) const
{
    return PandoraContentApi::GetCurrentList(*this, pClusterList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonFragmentMergingAlgorithm::GetPhotonPhotonMergingFlag(const Parameters &parameters) const
{
    if (parameters.m_energyOfCandidateCluster < m_lowEnergyOfCandidateClusterThreshold)
    {
        return (this->IsPhotonFragmentInShowerProfile(parameters) ||
                this->IsAbsoluteLowEnergyPhotonFragment(parameters) ||
                this->IsSmallPhotonFragment1(parameters) ||
                this->IsSmallForwardPhotonFragment(parameters) ||
                this->IsSmallPhotonFragment2(parameters) ||
                this->IsRelativeLowEnergyPhotonFragment(parameters));
    }
    else
    {
        return (this->IsHighEnergyPhotonFragmentInShowerProfile(parameters) ||
                this->IsHighEnergyRelativeLowEnergyPhotonFragment(parameters));
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonFragmentMergingAlgorithm::GetPhotonNeutralMergingFlag(const Parameters &parameters) const
{
    if (parameters.m_energyOfCandidateCluster < m_lowEnergyOfCandidateClusterThreshold)
    {
        return (this->IsCloseByNeutralFragment(parameters) ||
                this->IsSmallNeutralFragment(parameters) ||
                this->IsRelativeLowEnergyNeutralFragment(parameters));
    }
    else
    {
        return (this->IsHighEnergyNeutralFragmentInShowerProfile(parameters) ||
                this->IsHighEnergyRelativeLowEnergyNeutralFragment1(parameters) ||
                this->IsHighEnergyRelativeLowEnergyNeutralFragment1(parameters) ||
                this->IsHighEnergyRelativeLowEnergyNeutralFragment2(parameters) ||
                this->IsHighEnergyRelativeLowEnergyNeutralFragment3(parameters));
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonFragmentMergingAlgorithm::DeleteClusters(const ClusterVector &/*photonClusterVec*/, const ClusterVector &/*neutralClusterVec*/,
    const ClusterVector &/*unusedClusterVec*/) const
{
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonFragmentMergingAlgorithm::IsPhotonFragmentInShowerProfile(const Parameters &parameters) const
{
    return (parameters.m_energyOfCandidatePeak > 0.f &&
            parameters.m_energyOfCandidateCluster > 0.f &&
            parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonPhotonThresholdLow1 &&
            (parameters.m_energyOfCandidatePeak + parameters.m_energyOfMainPeak) / (parameters.m_energyOfCandidateCluster + parameters.m_energyOfMainCluster) > m_minRatioTotalShowerPeakEnergyToTotalEnergyThreshold &&
            parameters.m_energyOfCandidatePeak / parameters.m_energyOfCandidateCluster < m_energyRatioCandidatePeakToClusterPhotonThresholdLow1);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonFragmentMergingAlgorithm::IsAbsoluteLowEnergyPhotonFragment(const Parameters &parameters) const
{
    return (parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonPhotonThresholdLow2 &&
            parameters.m_energyOfCandidateCluster < m_energyOfCandidateClusterPhotonPhotonThresholdLow2);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonFragmentMergingAlgorithm::IsSmallPhotonFragment1(const Parameters &parameters) const
{
    return (parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonPhotonThresholdLow3 &&
            parameters.m_nCaloHitsCandidate < m_nCaloHitsCandidatePhotonPhotonThresholdLow3 &&
            parameters.m_centroidSeparation < m_centroidSeparationPhotonPhotonThresholdLow3);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonFragmentMergingAlgorithm::IsSmallPhotonFragment2(const Parameters &parameters) const
{
    return (parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_nCaloHitsCandidate < m_nCaloHitsCandidatePhotonPhotonThresholdLow5 &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonPhotonThresholdLow5);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonFragmentMergingAlgorithm::IsSmallForwardPhotonFragment(const Parameters &parameters) const
{
    return (parameters.m_centroidSeparation < m_centroidSeparationPhotonPhotonThresholdLow4 &&
            parameters.m_nCaloHitsCandidate < m_nCaloHitsCandidatePhotonPhotonThresholdLow4 &&
            parameters.m_energyOfCandidateCluster < m_energyOfCandidateClusterPhotonPhotonThresholdLow4 &&
            std::fabs(parameters.m_cosineMain) > m_cosineBarrelLimit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonFragmentMergingAlgorithm::IsRelativeLowEnergyPhotonFragment(const Parameters &parameters) const
{
    return (parameters.m_energyOfMainCluster > 0.f &&
            parameters.m_hitSeparation < m_hitSeparationPhotonPhotonThresholdLow6 &&
            parameters.m_energyOfCandidateCluster / parameters.m_energyOfMainCluster < m_energyRatioCandidateToMainPhotonThresholdLow6 &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonPhotonThresholdLow6 &&
            parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonFragmentMergingAlgorithm::IsHighEnergyPhotonFragmentInShowerProfile(const Parameters &parameters) const
{
    return (parameters.m_energyOfCandidateCluster > 0.f &&
            parameters.m_energyOfMainCluster > 0.f &&
            parameters.m_energyOfCandidatePeak > 0.f &&
            parameters.m_energyOfMainPeak>0.f &&
            parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonPhotonThresholdHigh1 &&
            (parameters.m_energyOfCandidatePeak + parameters.m_energyOfMainPeak) / (parameters.m_energyOfCandidateCluster + parameters.m_energyOfMainCluster) > m_minRatioTotalShowerPeakEnergyToTotalEnergyThreshold &&
            ( (parameters.m_energyOfCandidatePeak / parameters.m_energyOfCandidateCluster < m_energyRatioCandidatePeakToClusterPhotonThresholdHigh1 &&
            parameters.m_energyOfMainPeak/parameters.m_energyOfMainCluster > m_energyRatioMainPeakToClusterPhotonThresholdHigh1 &&
            (parameters.m_energyOfCandidatePeak / parameters.m_energyOfCandidateCluster + m_triangularEnergyRatioCandidatePeakToClusterPhotonThresholdHigh1 * parameters.m_energyOfMainPeak / parameters.m_energyOfMainCluster) < m_triangularSumEnergyRatioCandidatePeakToClusterPhotonThresholdHigh1) ||
            (parameters.m_energyOfCandidatePeak / parameters.m_energyOfCandidateCluster < m_linearEnergyRatioCandidatePeakToClusterPhotonThresholdHigh1 &&
            parameters.m_energyOfMainPeak / parameters.m_energyOfMainCluster > m_linearEnergyRatioMainPeakToClusterPhotonThresholdHigh1) ));
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonFragmentMergingAlgorithm::IsHighEnergyRelativeLowEnergyPhotonFragment(const Parameters &parameters) const
{
    return (parameters.m_energyOfCandidateCluster > 0.f &&
            parameters.m_energyOfMainCluster > 0.f &&
            parameters.m_hitSeparation < m_hitSeparationPhotonPhotonThresholdHigh2 &&
            parameters.m_energyOfCandidateCluster / parameters.m_energyOfMainCluster < m_energyRatioCandidateToMainPhotonThresholdHigh2 &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonPhotonThresholdHigh2 &&
            parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonFragmentMergingAlgorithm::IsCloseByNeutralFragment(const Parameters &parameters) const
{
    return (parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonNeutralThresholdLow1 &&
            parameters.m_centroidSeparation < m_centroidSeparationPhotonNeutralThresholdLow1);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonFragmentMergingAlgorithm::IsSmallNeutralFragment(const Parameters &parameters) const
{
    return (parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonNeutralThresholdLow2 &&
            parameters.m_hitSeparation < m_hitSeparationPhotonNeutralThresholdLow2 &&
            parameters.m_nCaloHitsCandidate < m_nCaloHitsCandidatePhotonNeutralThresholdLow2);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonFragmentMergingAlgorithm::IsRelativeLowEnergyNeutralFragment(const Parameters &parameters) const
{
    return (parameters.m_energyOfCandidateCluster > 0.f &&
            parameters.m_energyOfMainCluster > 0.f &&
            parameters.m_hitSeparation < m_hitSeparationPhotonNeutralThresholdLow3 &&
            parameters.m_energyOfCandidateCluster / parameters.m_energyOfMainCluster < m_energyRatioCandidateToMainNeutralThresholdLow3 &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonNeutralThresholdLow3 &&
            parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonFragmentMergingAlgorithm::IsHighEnergyNeutralFragmentInShowerProfile(const Parameters &parameters) const
{
    return (parameters.m_energyOfCandidateCluster > 0.f &&
            parameters.m_energyOfMainCluster > 0.f &&
            parameters.m_energyOfCandidatePeak > 0.f &&
            parameters.m_energyOfMainPeak>0.f &&
            parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonNeutralThresholdHigh1 &&
            (parameters.m_energyOfCandidatePeak + parameters.m_energyOfMainPeak) / (parameters.m_energyOfCandidateCluster + parameters.m_energyOfMainCluster) > m_minRatioTotalShowerPeakEnergyToTotalEnergyThreshold &&
            parameters.m_energyOfCandidateCluster/parameters.m_energyOfMainCluster<m_energyRatioCandidateToMainNeutralThresholdHigh1 &&
            ( (parameters.m_energyOfCandidatePeak / parameters.m_energyOfCandidateCluster < m_energyRatioCandidatePeakToClusterNeutralThresholdHigh1 &&
            parameters.m_energyOfMainPeak / parameters.m_energyOfMainCluster > m_energyRatioMainPeakToClusterNeutralThresholdHigh1 &&
            (parameters.m_energyOfCandidatePeak / parameters.m_energyOfCandidateCluster + m_triangularEnergyRatioMainPeakToClusterNeutralThresholdHigh1 * parameters.m_energyOfMainPeak / parameters.m_energyOfMainCluster) < m_triangularSumEnergyRatioMainPeakToClusterNeutralThresholdHigh1) ||
            (parameters.m_energyOfCandidatePeak / parameters.m_energyOfCandidateCluster < m_squareEnergyRatioCandidatePeakToClusterNeutralThresholdHigh1 &&
            parameters.m_energyOfMainPeak / parameters.m_energyOfMainCluster > m_squareEnergyRatioMainPeakToClusterNeutralThresholdHigh1) ));
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonFragmentMergingAlgorithm::IsHighEnergyRelativeLowEnergyNeutralFragment1(const Parameters &parameters) const
{
    return (parameters.m_energyOfCandidateCluster > 0.f &&
            parameters.m_energyOfMainCluster > 0.f &&
            parameters.m_hitSeparation < m_hitSeparationPhotonNeutralThresholdHigh2 &&
            parameters.m_energyOfCandidateCluster / parameters.m_energyOfMainCluster < m_energyRatioCandidateToMainNeutralThresholdHigh2 &&
            parameters.m_weightedLayerSeparation<m_weightedLayerSeparationPhotonNeutralThresholdHigh2 &&
            parameters.m_weightedLayerSeparation>m_minWeightedLayerSeparation);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonFragmentMergingAlgorithm::IsHighEnergyRelativeLowEnergyNeutralFragment2(const Parameters &parameters) const
{
    return (parameters.m_energyOfCandidateCluster > 0.f &&
            parameters.m_energyOfMainCluster > 0.f &&
            parameters.m_hitSeparation < m_hitSeparationPhotonNeutralThresholdHigh3 &&
            parameters.m_energyOfCandidateCluster / parameters.m_energyOfMainCluster < m_energyRatioCandidateToMainNeutralThresholdHigh3 &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonNeutralThresholdHigh3 &&
            parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_energyOfCandidateCluster > m_energyOfCandidateClusterPhotonNeutralThresholdHigh3);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonFragmentMergingAlgorithm::IsHighEnergyRelativeLowEnergyNeutralFragment3(const Parameters &parameters) const
{
    return (parameters.m_energyOfCandidateCluster > 0.f &&
            parameters.m_energyOfMainCluster > 0.f &&
            parameters.m_hitSeparation < m_hitSeparationPhotonNeutralThresholdHigh4 &&
            parameters.m_energyOfCandidateCluster / parameters.m_energyOfMainCluster < m_energyRatioCandidateToMainNeutralThresholdHigh4 &&
            parameters.m_weightedLayerSeparation < m_weightedLayerSeparationPhotonNeutralThresholdHigh4 &&
            parameters.m_weightedLayerSeparation > m_minWeightedLayerSeparation &&
            parameters.m_energyOfCandidateCluster > m_energyOfCandidateClusterPhotonNeutralThresholdHigh4);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonFragmentMergingAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CentroidSeparationPhotonNeutralThresholdLow1", m_centroidSeparationPhotonNeutralThresholdLow1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestDistanceSeparationPhotonNeutralThresholdHigh3", m_hitSeparationPhotonNeutralThresholdHigh3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyRatioCandidateToMainNeutralThresholdHigh3", m_energyRatioCandidateToMainNeutralThresholdHigh3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "WeightedLayerSeparationPhotonNeutralThresholdHigh3", m_weightedLayerSeparationPhotonNeutralThresholdHigh3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyOfCandidateClusterPhotonNeutralThresholdHigh3", m_energyOfCandidateClusterPhotonNeutralThresholdHigh3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestDistanceSeparationPhotonNeutralThresholdHigh4", m_hitSeparationPhotonNeutralThresholdHigh4));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyRatioCandidateToMainNeutralThresholdHigh4", m_energyRatioCandidateToMainNeutralThresholdHigh4));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "WeightedLayerSeparationPhotonNeutralThresholdHigh4", m_weightedLayerSeparationPhotonNeutralThresholdHigh4));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyOfCandidateClusterPhotonNeutralThresholdHigh4", m_energyOfCandidateClusterPhotonNeutralThresholdHigh4));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CentroidSeparationPhotonPhotonThresholdLow3", m_centroidSeparationPhotonPhotonThresholdLow3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyOfCandidateClusterPhotonPhotonThresholdLow4", m_energyOfCandidateClusterPhotonPhotonThresholdLow4));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CosineBarrelLimit", m_cosineBarrelLimit));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NCaloHitsCandidatePhotonPhotonThresholdLow5", m_nCaloHitsCandidatePhotonPhotonThresholdLow5));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "WeightedLayerSeparationPhotonPhotonThresholdLow5", m_weightedLayerSeparationPhotonPhotonThresholdLow5));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestDistanceSeparationPhotonPhotonThresholdLow6", m_hitSeparationPhotonPhotonThresholdLow6));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyRatioCandidateToMainPhotonThresholdLow6", m_energyRatioCandidateToMainPhotonThresholdLow6));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "WeightedLayerSeparationPhotonPhotonThresholdLow6", m_weightedLayerSeparationPhotonPhotonThresholdLow6));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HitSeparationPhotonPhotonThresholdHigh2", m_hitSeparationPhotonPhotonThresholdHigh2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyRatioCandidateToMainPhotonThresholdHigh2", m_energyRatioCandidateToMainPhotonThresholdHigh2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "WeightedLayerSeparationPhotonPhotonThresholdHigh2", m_weightedLayerSeparationPhotonPhotonThresholdHigh2));

    return PhotonFragmentMergingBaseAlgorithm::ReadSettings(xmlHandle);
}

} // end namespace lc_content
