/**
 *  @file   LCContent/src/LCReclustering/ForceSplitTrackAssociationsAlg.cc
 * 
 *  @brief  Implementation of the force split track associations algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCReclustering/ForceSplitTrackAssociationsAlg.h"

using namespace pandora;

namespace lc_content
{

ForceSplitTrackAssociationsAlg::ForceSplitTrackAssociationsAlg() :
    m_minTrackAssociations(2)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ForceSplitTrackAssociationsAlg::Run()
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

    // Loop over clusters in the algorithm input list, looking for those with excess track associations
    for (ClusterList::const_iterator iter = pClusterList->begin(); iter != pClusterList->end();)
    {
        Cluster *pOriginalCluster = *iter;
        ++iter;

        const TrackList trackList(pOriginalCluster->GetAssociatedTrackList());

        if (trackList.size() < m_minTrackAssociations)
            continue;

        OrderedCaloHitList orderedCaloHitList(pOriginalCluster->GetOrderedCaloHitList());
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, orderedCaloHitList.Add(pOriginalCluster->GetIsolatedCaloHitList()));

        // Initialize cluster fragmentation operations
        ClusterList clusterList;
        clusterList.insert(pOriginalCluster);
        std::string originalClustersListName, fragmentClustersListName;

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::InitializeFragmentation(*this, clusterList,
            originalClustersListName, fragmentClustersListName));

        // Remove original track-cluster associations and create new track-seeded clusters for each track
        TrackToClusterMap trackToClusterMap;

        for (TrackList::const_iterator trackIter = trackList.begin(), trackIterEnd = trackList.end(); trackIter != trackIterEnd;
            ++trackIter)
        {
            Track *pTrack = *trackIter;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RemoveTrackClusterAssociation(*this, pTrack, pOriginalCluster));

            Cluster *pCluster = NULL;
            PandoraContentApi::Cluster::Parameters parameters;
            parameters.m_pTrack = pTrack;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, parameters, pCluster));

            if (!trackToClusterMap.insert(TrackToClusterMap::value_type(pTrack, pCluster)).second)
            {
                return STATUS_CODE_FAILURE;
            }
        }

        // Assign the calo hits in the original cluster to the most appropriate track
        for (OrderedCaloHitList::const_iterator listIter = orderedCaloHitList.begin(), listIterEnd = orderedCaloHitList.end();
            listIter != listIterEnd; ++listIter)
        {
            for (CaloHitList::const_iterator hitIter = listIter->second->begin(), hitIterEnd = listIter->second->end();
                hitIter != hitIterEnd; ++hitIter)
            {
                CaloHit *pCaloHit = *hitIter;
                const CartesianVector &hitPosition(pCaloHit->GetPositionVector());

                // Identify most suitable cluster for calo hit, using distance to helix fit as figure of merit
                Cluster *pBestCluster = NULL;
                float bestClusterEnergy(0.);
                float minDistanceToTrack(std::numeric_limits<float>::max());

                for (TrackToClusterMap::const_iterator mapIter = trackToClusterMap.begin(), mapIterEnd = trackToClusterMap.end();
                    mapIter != mapIterEnd; ++mapIter)
                {
                    const Helix *const pHelix(mapIter->first->GetHelixFitAtCalorimeter());

                    CartesianVector helixSeparation(0.f, 0.f, 0.f);
                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pHelix->GetDistanceToPoint(hitPosition, helixSeparation));

                    const float distanceToTrack(helixSeparation.GetMagnitude());
                    const float clusterEnergy(mapIter->second->GetHadronicEnergy());

                    if ((distanceToTrack < minDistanceToTrack) || ((distanceToTrack == minDistanceToTrack) && (clusterEnergy > bestClusterEnergy)))
                    {
                        minDistanceToTrack = distanceToTrack;
                        pBestCluster = mapIter->second;
                        bestClusterEnergy = clusterEnergy;
                    }
                }

                if (NULL == pBestCluster)
                {
                    return STATUS_CODE_FAILURE;
                }

                if (!pCaloHit->IsIsolated())
                {
                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddToCluster(*this, pBestCluster, pCaloHit));
                }
                else
                {
                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddIsolatedToCluster(*this, pBestCluster, pCaloHit));
                }
            }
        }

        // Check for any "empty" clusters and create new track-cluster associations
        for (TrackToClusterMap::iterator mapIter = trackToClusterMap.begin(); mapIter != trackToClusterMap.end();)
        {
            Cluster *pCluster = mapIter->second;

            if (0 == pCluster->GetNCaloHits())
            {
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Delete(*this, pCluster));
                trackToClusterMap.erase(mapIter++);
            }
            else
            {
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, mapIter->first, pCluster));
                ++mapIter;
            }
        }

        if (trackToClusterMap.empty())
        {
            return STATUS_CODE_FAILURE;
        }

        // End cluster fragmentation operations, automatically choose the new cluster fragments
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::EndFragmentation(*this, fragmentClustersListName,
            originalClustersListName));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ForceSplitTrackAssociationsAlg::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinTrackAssociations", m_minTrackAssociations));

    if (m_minTrackAssociations < 2)
        return STATUS_CODE_INVALID_PARAMETER;

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
