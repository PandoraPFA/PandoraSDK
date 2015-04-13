/**
 *  @file   LCContent/src/LCFragmentRemoval/PhotonFragmentMergingBaseAlgorithm.cc
 *
 *  @brief  Implementation of the photon fragment merging algorithm base class.
 *
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "Helpers/ClusterFitHelper.h"

#include "LCFragmentRemoval/PhotonFragmentMergingBaseAlgorithm.h"

#include "LCHelpers/ClusterHelper.h"
#include "LCHelpers/FragmentRemovalHelper.h"
#include "LCHelpers/SortingHelper.h"

using namespace pandora;

namespace lc_content
{

PhotonFragmentMergingBaseAlgorithm::PhotonFragmentMergingBaseAlgorithm() :
    m_transProfileMaxLayer(30),
    m_minWeightedLayerSeparation(0.f),
    m_maxWeightedLayerSeparation(80.f),
    m_lowEnergyOfCandidateClusterThreshold(1.f),
    m_weightedLayerSeparationPhotonNeutralThresholdLow1(20.f),
    m_weightedLayerSeparationPhotonNeutralThresholdLow2(50.f),
    m_hitSeparationPhotonNeutralThresholdLow2(50.f),
    m_nCaloHitsCandidatePhotonNeutralThresholdLow2(10),
    m_hitSeparationPhotonNeutralThresholdLow3(15.f),
    m_energyRatioCandidateToMainNeutralThresholdLow3(0.01f),
    m_weightedLayerSeparationPhotonNeutralThresholdLow3(40.f),
    m_weightedLayerSeparationPhotonNeutralThresholdHigh1(12.f),
    m_energyRatioCandidatePeakToClusterNeutralThresholdHigh1(0.4f),
    m_energyRatioMainPeakToClusterNeutralThresholdHigh1(0.9f),
    m_triangularEnergyRatioMainPeakToClusterNeutralThresholdHigh1(0.5f),
    m_triangularSumEnergyRatioMainPeakToClusterNeutralThresholdHigh1(0.85f),
    m_squareEnergyRatioCandidatePeakToClusterNeutralThresholdHigh1(0.1f),
    m_squareEnergyRatioMainPeakToClusterNeutralThresholdHigh1(1.5f),
    m_energyRatioCandidateToMainNeutralThresholdHigh1(0.1f),
    m_hitSeparationPhotonNeutralThresholdHigh2(20.f),
    m_energyRatioCandidateToMainNeutralThresholdHigh2(0.02f),
    m_weightedLayerSeparationPhotonNeutralThresholdHigh2(40.f),
    m_weightedLayerSeparationPhotonPhotonThresholdLow1(20.f),
    m_energyRatioCandidatePeakToClusterPhotonThresholdLow1(0.7f),
    m_weightedLayerSeparationPhotonPhotonThresholdLow2(20.f),
    m_energyOfCandidateClusterPhotonPhotonThresholdLow2(0.2f),
    m_weightedLayerSeparationPhotonPhotonThresholdLow3(30.f),
    m_nCaloHitsCandidatePhotonPhotonThresholdLow3(20),
    m_centroidSeparationPhotonPhotonThresholdLow4(30.f),
    m_nCaloHitsCandidatePhotonPhotonThresholdLow4(10),
    m_weightedLayerSeparationPhotonPhotonThresholdHigh1(15.f),
    m_energyRatioCandidatePeakToClusterPhotonThresholdHigh1(0.4f),
    m_energyRatioMainPeakToClusterPhotonThresholdHigh1(0.9f),
    m_triangularEnergyRatioCandidatePeakToClusterPhotonThresholdHigh1(0.5f),
    m_triangularSumEnergyRatioCandidatePeakToClusterPhotonThresholdHigh1(0.85f),
    m_linearEnergyRatioCandidatePeakToClusterPhotonThresholdHigh1(0.1f),
    m_linearEnergyRatioMainPeakToClusterPhotonThresholdHigh1(1.5f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonFragmentMergingBaseAlgorithm::Run()
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetAffectedClusterList(pClusterList));

    ClusterVector photonClusterVec, neutralClusterVec, unusedClusterVec;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetAffectedClusterVec(pClusterList, photonClusterVec, neutralClusterVec, unusedClusterVec));

    if (!photonClusterVec.empty())
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->MergePhotonFragmentWithPhotons(photonClusterVec));

    if (!photonClusterVec.empty() && !neutralClusterVec.empty())
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->MergeNeutralFragmentWithPhotons(photonClusterVec,neutralClusterVec));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->DeleteClusters(photonClusterVec, neutralClusterVec, unusedClusterVec));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonFragmentMergingBaseAlgorithm::GetAffectedClusterVec(const ClusterList *const pClusterList, ClusterVector &photonClusterVec,
    ClusterVector &neutralClusterVec, ClusterVector &unusedClusterVec) const
{
    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        const Cluster *const pCluster = *iter;

        if (pCluster->GetParticleIdFlag() == PHOTON)
        {
            photonClusterVec.push_back(pCluster);
        }
        else if (!pCluster->GetAssociatedTrackList().empty())
        {
            unusedClusterVec.push_back(pCluster);
        }
        else if (pCluster->GetElectromagneticEnergy() < std::numeric_limits<float>::epsilon())
        {
            unusedClusterVec.push_back(pCluster);
        }
        else if (pCluster->IsPhotonFast(this->GetPandora()))
        {
            photonClusterVec.push_back(pCluster);
        }
        else
        {
            neutralClusterVec.push_back(pCluster);
        }
    }

    std::sort(photonClusterVec.begin(), photonClusterVec.end(), SortingHelper::SortClustersByElectromagneticEnergy);
    std::sort(neutralClusterVec.begin(), neutralClusterVec.end(), SortingHelper::SortClustersByElectromagneticEnergy);
    std::sort(unusedClusterVec.begin(), unusedClusterVec.end(), SortingHelper::SortClustersByElectromagneticEnergy);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonFragmentMergingBaseAlgorithm::MergePhotonFragmentWithPhotons(ClusterVector &photonClusterVec) const
{
    for (ClusterVector::reverse_iterator iterD = photonClusterVec.rbegin(), iterDEnd = photonClusterVec.rend(); iterD != iterDEnd; ++iterD)
    {
        const Cluster *const pDaughterCluster(*iterD);

        if (NULL == pDaughterCluster)
            continue;

        const Cluster *pBestParentCluster(NULL);
        float bestClusterSeparation(std::numeric_limits<float>::max());

        for (ClusterVector::reverse_iterator iterP = iterD, iterPEnd = photonClusterVec.rend(); iterP != iterPEnd; ++iterP)
        {
            const Cluster *const pParentCluster(*iterP);

            if (NULL == pParentCluster)
                continue;

            if (pDaughterCluster == pParentCluster)
                continue;

            try
            {
                const float clusterSeparation(FragmentRemovalHelper::GetEMEnergyWeightedLayerSeparation(pParentCluster, pDaughterCluster));

                if ((clusterSeparation > m_maxWeightedLayerSeparation) ||
                    (clusterSeparation < m_minWeightedLayerSeparation) ||
                    (clusterSeparation > bestClusterSeparation))
                {
                    continue;
                }

                PhotonFragmentMergingBaseAlgorithm::Parameters parameters;
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetEvidenceForMerging(pParentCluster, pDaughterCluster, clusterSeparation, parameters));

                if (this->GetPhotonPhotonMergingFlag(parameters))
                {
                    pBestParentCluster = pParentCluster;
                    bestClusterSeparation = clusterSeparation;
                }
            }
            catch (StatusCodeException &)
            {
            }
        }

        if (NULL != pBestParentCluster)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBestParentCluster, pDaughterCluster));
            *iterD = NULL;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonFragmentMergingBaseAlgorithm::MergeNeutralFragmentWithPhotons(ClusterVector &photonClusterVec,ClusterVector &neutralClusterVec) const
{
    for (ClusterVector::reverse_iterator iterD = neutralClusterVec.rbegin(), iterDEnd =neutralClusterVec.rend(); iterD != iterDEnd; ++iterD)
    {
        const Cluster *const pDaughterCluster(*iterD);

        if (NULL == pDaughterCluster)
            continue;

        const Cluster *pBestParentCluster(NULL);
        float bestClusterSeparation(std::numeric_limits<float>::max());

        for (ClusterVector::reverse_iterator iterP = photonClusterVec.rbegin(), iterPEnd = photonClusterVec.rend(); iterP != iterPEnd; ++iterP)
        {
            const Cluster *const pParentCluster(*iterP);

            if (NULL == pParentCluster)
                continue;

            try
            {
                const float clusterSeparation(FragmentRemovalHelper::GetEMEnergyWeightedLayerSeparation(pParentCluster, pDaughterCluster));

                if ((clusterSeparation > m_maxWeightedLayerSeparation) ||
                    (clusterSeparation < m_minWeightedLayerSeparation) ||
                    (clusterSeparation > bestClusterSeparation))
                {
                    continue;
                }

                PhotonFragmentMergingBaseAlgorithm::Parameters parameters;
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetEvidenceForMerging(pParentCluster, pDaughterCluster, clusterSeparation, parameters));

                if (this->GetPhotonNeutralMergingFlag(parameters))
                {
                    pBestParentCluster = pParentCluster;
                    bestClusterSeparation = clusterSeparation;
                }
            }
            catch (StatusCodeException &)
            {
            }
        }

        if (NULL != pBestParentCluster)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBestParentCluster, pDaughterCluster));
            *iterD = NULL;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonFragmentMergingBaseAlgorithm::GetEvidenceForMerging(const Cluster *const pParentCluster, const Cluster *const pDaughterCluster,
    const float clusterSeparation, Parameters &parameters) const
{
    ShowerProfilePlugin::ShowerPeakList showerPeakList;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetShowerPeakList(pParentCluster, pDaughterCluster, showerPeakList));

    ClusterFitResult parentFitResult, daughterFitResult;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, ClusterFitHelper::FitFullCluster(pParentCluster, parentFitResult));
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_OUT_OF_RANGE, !=, ClusterFitHelper::FitFullCluster(pDaughterCluster, daughterFitResult) );

    const CartesianVector parentCentroid(FragmentRemovalHelper::GetEMEnergyWeightedPosition(pParentCluster));
    const CartesianVector daughterCentroid(FragmentRemovalHelper::GetEMEnergyWeightedPosition(pDaughterCluster));

    parameters.m_weightedLayerSeparation = clusterSeparation;
    parameters.m_energyOfMainCluster = pParentCluster->GetElectromagneticEnergy();
    parameters.m_energyOfCandidateCluster = pDaughterCluster->GetElectromagneticEnergy();
    parameters.m_energyOfMainPeak = (showerPeakList.size() > 0) ? showerPeakList.at(0).GetPeakEnergy() : 0.f;
    parameters.m_energyOfCandidatePeak = (showerPeakList.size() > 1) ? showerPeakList.at(1).GetPeakEnergy() : 0.f;
    parameters.m_hitSeparation = ClusterHelper::GetDistanceToClosestHit(pDaughterCluster, pParentCluster);
    parameters.m_centroidSeparation = (parentCentroid - daughterCentroid).GetMagnitude();
    parameters.m_nCaloHitsMain = pParentCluster->GetNCaloHits();
    parameters.m_nCaloHitsCandidate = pDaughterCluster->GetNCaloHits();
    parameters.m_cosineMain = parentFitResult.IsFitSuccessful() ? parentFitResult.GetDirection().GetCosOpeningAngle(CartesianVector(0.f, 0.f, 1.f)) :
        -std::numeric_limits<float>::max();
    parameters.m_cosineCandidate = daughterFitResult.IsFitSuccessful() ? daughterFitResult.GetDirection().GetCosOpeningAngle(CartesianVector(0.f, 0.f, 1.f)) :
        -std::numeric_limits<float>::max();
    parameters.m_hasCrossedGap = ClusterHelper::DoesClusterCrossGapRegion(this->GetPandora(), pParentCluster, 0, m_transProfileMaxLayer);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonFragmentMergingBaseAlgorithm::GetShowerPeakList(const Cluster *const pParentCluster, const Cluster *const pDaughterCluster,
    ShowerProfilePlugin::ShowerPeakList &showerPeakList) const
{
    ClusterList temporaryList;
    temporaryList.insert(pParentCluster);
    temporaryList.insert(pDaughterCluster);
    std::string originalClusterListName, peakClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::InitializeFragmentation(*this, temporaryList, originalClusterListName,
        peakClusterListName));

    const Cluster *pTempCluster = NULL;
    PandoraContentApi::Cluster::Parameters clusterParameters;
    pDaughterCluster->GetOrderedCaloHitList().GetCaloHitList(clusterParameters.m_caloHitList);
    pParentCluster->GetOrderedCaloHitList().GetCaloHitList(clusterParameters.m_caloHitList);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, clusterParameters, pTempCluster));

    const ShowerProfilePlugin *const pShowerProfilePlugin(PandoraContentApi::GetPlugins(*this)->GetShowerProfilePlugin());
    pShowerProfilePlugin->CalculateTransverseProfile(pTempCluster, m_transProfileMaxLayer, showerPeakList);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::EndFragmentation(*this, originalClusterListName, peakClusterListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonFragmentMergingBaseAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TransProfileMaxLayer", m_transProfileMaxLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinWeightedLayerSeparation", m_minWeightedLayerSeparation));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxWeightedLayerSeparation", m_maxWeightedLayerSeparation));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowEnergyOfCandidateClusterThreshold", m_lowEnergyOfCandidateClusterThreshold));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "WeightedLayerSeparationPhotonNeutralThresholdLow1", m_weightedLayerSeparationPhotonNeutralThresholdLow1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "WeightedLayerSeparationPhotonNeutralThresholdLow2", m_weightedLayerSeparationPhotonNeutralThresholdLow2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestDistanceSeparationPhotonNeutralThresholdLow2", m_hitSeparationPhotonNeutralThresholdLow2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NCaloHitsCandidatePhotonNeutralThresholdLow2", m_nCaloHitsCandidatePhotonNeutralThresholdLow2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestDistanceSeparationPhotonNeutralThresholdLow3", m_hitSeparationPhotonNeutralThresholdLow3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyRatioCandidateToMainNeutralThresholdLow3", m_energyRatioCandidateToMainNeutralThresholdLow3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "WeightedLayerSeparationPhotonNeutralThresholdLow3", m_weightedLayerSeparationPhotonNeutralThresholdLow3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "WeightedLayerSeparationPhotonNeutralThresholdHigh1", m_weightedLayerSeparationPhotonNeutralThresholdHigh1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyRatioCandidatePeakToClusterNeutralThresholdHigh1", m_energyRatioCandidatePeakToClusterNeutralThresholdHigh1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyRatioMainPeakToClusterNeutralThresholdHigh1", m_energyRatioMainPeakToClusterNeutralThresholdHigh1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TriangularEnergyRatioMainPeakToClusterNeutralThresholdHigh1", m_triangularEnergyRatioMainPeakToClusterNeutralThresholdHigh1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TriangularSumEnergyRatioMainPeakToClusterNeutralThresholdHigh1", m_triangularSumEnergyRatioMainPeakToClusterNeutralThresholdHigh1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "SquareEnergyRatioCandidatePeakToClusterNeutralThresholdHigh1", m_squareEnergyRatioCandidatePeakToClusterNeutralThresholdHigh1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "SquareEnergyRatioMainPeakToClusterNeutralThresholdHigh1", m_squareEnergyRatioMainPeakToClusterNeutralThresholdHigh1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyRatioCandidateToMainNeutralThresholdHigh1", m_energyRatioCandidateToMainNeutralThresholdHigh1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestDistanceSeparationPhotonNeutralThresholdHigh2", m_hitSeparationPhotonNeutralThresholdHigh2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyRatioCandidateToMainNeutralThresholdHigh2", m_energyRatioCandidateToMainNeutralThresholdHigh2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "WeightedLayerSeparationPhotonNeutralThresholdHigh2", m_weightedLayerSeparationPhotonNeutralThresholdHigh2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "WeightedLayerSeparationPhotonPhotonThresholdLow1", m_weightedLayerSeparationPhotonPhotonThresholdLow1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyRatioCandidatePeakToClusterPhotonThresholdLow1", m_energyRatioCandidatePeakToClusterPhotonThresholdLow1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "WeightedLayerSeparationPhotonPhotonThresholdLow2", m_weightedLayerSeparationPhotonPhotonThresholdLow2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyOfCandidateClusterPhotonPhotonThresholdLow2", m_energyOfCandidateClusterPhotonPhotonThresholdLow2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "WeightedLayerSeparationPhotonPhotonThresholdLow3", m_weightedLayerSeparationPhotonPhotonThresholdLow3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NCaloHitsCandidatePhotonPhotonThresholdLow3", m_nCaloHitsCandidatePhotonPhotonThresholdLow3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CentroidSeparationPhotonPhotonThresholdLow4", m_centroidSeparationPhotonPhotonThresholdLow4));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NCaloHitsCandidatePhotonPhotonThresholdLow4", m_nCaloHitsCandidatePhotonPhotonThresholdLow4));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "WeightedLayerSeparationPhotonPhotonThresholdHigh1", m_weightedLayerSeparationPhotonPhotonThresholdHigh1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyRatioCandidatePeakToClusterPhotonThresholdHigh1", m_energyRatioCandidatePeakToClusterPhotonThresholdHigh1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyRatioMainPeakToClusterPhotonThresholdHigh1", m_energyRatioMainPeakToClusterPhotonThresholdHigh1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TriangularEnergyRatioCandidatePeakToClusterPhotonThresholdHigh1", m_triangularEnergyRatioCandidatePeakToClusterPhotonThresholdHigh1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TriangularSumEnergyRatioCandidatePeakToClusterPhotonThresholdHigh1", m_triangularSumEnergyRatioCandidatePeakToClusterPhotonThresholdHigh1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LinearEnergyRatioCandidatePeakToClusterPhotonThresholdHigh1", m_linearEnergyRatioCandidatePeakToClusterPhotonThresholdHigh1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LinearEnergyRatioMainPeakToClusterPhotonThresholdHigh1", m_linearEnergyRatioMainPeakToClusterPhotonThresholdHigh1));

    return STATUS_CODE_SUCCESS;
}

} // end namespace lc_content
