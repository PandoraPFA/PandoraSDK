/**
 *  @file   LCContent/src/LCTrackClusterAssociation/TrackRecoveryInteractionsAlgorithm.cc
 * 
 *  @brief  Implementation of the track recovery interactions algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCHelpers/ClusterHelper.h"
#include "LCHelpers/ReclusterHelper.h"
#include "LCHelpers/SortingHelper.h"

#include "LCTrackClusterAssociation/TrackRecoveryInteractionsAlgorithm.h"

using namespace pandora;

namespace lc_content
{

TrackRecoveryInteractionsAlgorithm::TrackRecoveryInteractionsAlgorithm() :
    m_maxTrackClusterDistance(200.f),
    m_trackClusterDistanceCut(100.f),
    m_clusterEnergyCut(0.5f),
    m_directionCosineCut(0.9f),
    m_maxTrackAssociationChi(2.f),
    m_maxSearchLayer(19),
    m_parallelDistanceCut(100.f),
    m_minTrackClusterCosAngle(0.f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackRecoveryInteractionsAlgorithm::Run()
{
    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pTrackList));

    TrackVector trackVector(pTrackList->begin(), pTrackList->end());
    std::sort(trackVector.begin(), trackVector.end(), SortingHelper::SortTracksByEnergy);

    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

    // Loop over all unassociated tracks in the current track list
    for (TrackVector::const_iterator iterT = trackVector.begin(), iterTEnd = trackVector.end(); iterT != iterTEnd; ++iterT)
    {
        Track *pTrack = *iterT;

        // Use only unassociated tracks that can be used to form a pfo
        if (pTrack->HasAssociatedCluster() || !pTrack->CanFormPfo())
            continue;

        if (!pTrack->GetDaughterTrackList().empty())
            continue;

        // Identify best cluster to be associated with this track, based on energy consistency and proximity
        Cluster *pBestCluster(NULL);
        float minEnergyDifference(std::numeric_limits<float>::max());
        float smallestTrackClusterDistance(std::numeric_limits<float>::max());

        for (ClusterList::const_iterator iterC = pClusterList->begin(), iterCEnd = pClusterList->end(); iterC != iterCEnd; ++iterC)
        {
            Cluster *pCluster = *iterC;

            if (!pCluster->GetAssociatedTrackList().empty() || (0 == pCluster->GetNCaloHits()))
                continue;

            float trackClusterDistance(std::numeric_limits<float>::max());

            if (STATUS_CODE_SUCCESS != ClusterHelper::GetTrackClusterDistance(pTrack, pCluster, m_maxSearchLayer, m_parallelDistanceCut,
                m_minTrackClusterCosAngle, trackClusterDistance))
            {
                continue;
            }

            const float energyDifference(std::fabs(pCluster->GetHadronicEnergy() - pTrack->GetEnergyAtDca()));

            if ((trackClusterDistance < smallestTrackClusterDistance) ||
                ((trackClusterDistance == smallestTrackClusterDistance) && (energyDifference < minEnergyDifference)))
            {
                smallestTrackClusterDistance = trackClusterDistance;
                pBestCluster = pCluster;
                minEnergyDifference = energyDifference;
            }
        }

        if ((NULL == pBestCluster) || (smallestTrackClusterDistance > m_maxTrackClusterDistance))
            continue;

        // Should track be associated with "best" cluster?
        const float clusterEnergy(pBestCluster->GetTrackComparisonEnergy(this->GetPandora()));

        if ((smallestTrackClusterDistance > m_trackClusterDistanceCut) && (clusterEnergy > m_clusterEnergyCut))
        {
            const CartesianVector &trackCalorimeterPosition(pTrack->GetTrackStateAtCalorimeter().GetPosition());
            const CartesianVector &trackerEndPosition(pTrack->GetTrackStateAtEnd().GetPosition());
            const CartesianVector innerLayerCentroid(pBestCluster->GetCentroid(pBestCluster->GetInnerPseudoLayer()));

            const CartesianVector trackerToTrackCalorimeterUnitVector((trackCalorimeterPosition - trackerEndPosition).GetUnitVector());
            const CartesianVector trackerToClusterUnitVector((innerLayerCentroid - trackerEndPosition).GetUnitVector());

            const float directionCosine(trackerToClusterUnitVector.GetDotProduct(trackerToTrackCalorimeterUnitVector));

            if (directionCosine < m_directionCosineCut)
                continue;
        }

        const float trackEnergy(pTrack->GetEnergyAtDca());
        const float chi(ReclusterHelper::GetTrackClusterCompatibility(this->GetPandora(), clusterEnergy, trackEnergy));

        if (chi < m_maxTrackAssociationChi)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, pTrack, pBestCluster));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackRecoveryInteractionsAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxTrackClusterDistance", m_maxTrackClusterDistance));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackClusterDistanceCut", m_trackClusterDistanceCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClusterEnergyCut", m_clusterEnergyCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DirectionCosineCut", m_directionCosineCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxTrackAssociationChi", m_maxTrackAssociationChi));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxSearchLayer", m_maxSearchLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ParallelDistanceCut", m_parallelDistanceCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinTrackClusterCosAngle", m_minTrackClusterCosAngle));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
