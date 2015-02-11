/**
 *  @file   LCContent/src/LCClustering/ForcedClusteringAlgorithm.cc
 * 
 *  @brief  Implementation of the forced clustering algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCClustering/ForcedClusteringAlgorithm.h"

using namespace pandora;

namespace lc_content
{

ForcedClusteringAlgorithm::ForcedClusteringAlgorithm() :
    m_shouldRunStandardClusteringAlgorithm(false),
    m_shouldClusterIsolatedHits(false),
    m_shouldAssociateIsolatedHits(false)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ForcedClusteringAlgorithm::Run()
{
    // Read current track list
    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pTrackList));

    if (pTrackList->empty())
        return STATUS_CODE_INVALID_PARAMETER;

    // Read current calo hit list
    const CaloHitList *pCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pCaloHitList));

    if (pCaloHitList->empty())
        return STATUS_CODE_INVALID_PARAMETER;

    // Make new track-seeded clusters and populate track distance info vector
    TrackDistanceInfoVector trackDistanceInfoVector;

    for (TrackList::const_iterator iter = pTrackList->begin(), iterEnd = pTrackList->end(); iter != iterEnd; ++iter)
    {
        const Track *const pTrack = *iter;
        const Helix *const pHelix(pTrack->GetHelixFitAtCalorimeter());
        const float trackEnergy(pTrack->GetEnergyAtDca());

        const Cluster *pCluster = NULL;
        PandoraContentApi::Cluster::Parameters parameters;
        parameters.m_pTrack = pTrack;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, parameters, pCluster));

        for (CaloHitList::const_iterator hitIter = pCaloHitList->begin(), hitIterEnd = pCaloHitList->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const CaloHit *const pCaloHit = *hitIter;

            if ((m_shouldClusterIsolatedHits || !pCaloHit->IsIsolated()) && PandoraContentApi::IsAvailable(*this, pCaloHit))
            {
                CartesianVector helixSeparation(0.f, 0.f, 0.f);
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pHelix->GetDistanceToPoint(pCaloHit->GetPositionVector(), helixSeparation));

                trackDistanceInfoVector.push_back(TrackDistanceInfo(pCaloHit, pCluster, trackEnergy, helixSeparation.GetMagnitude()));
            }
        }
    }

    std::sort(trackDistanceInfoVector.begin(), trackDistanceInfoVector.end(), ForcedClusteringAlgorithm::SortByDistanceToTrack);

    // Work along ordered list of calo hits, adding to the clusters until each cluster energy matches associated track energy.
    for (TrackDistanceInfoVector::const_iterator iter = trackDistanceInfoVector.begin(), iterEnd = trackDistanceInfoVector.end(); iter != iterEnd; ++iter)
    {
        const Cluster *const pCluster = iter->GetCluster();
        const CaloHit *const pCaloHit = iter->GetCaloHit();
        const float trackEnergy = iter->GetTrackEnergy();

        if ((pCluster->GetHadronicEnergy() < trackEnergy) && PandoraContentApi::IsAvailable(*this, pCaloHit))
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddToCluster(*this, pCluster, pCaloHit));
        }
    }

    // Deal with remaining hits. Either run standard clustering algorithm, or crudely collect together into one cluster
    if (m_shouldRunStandardClusteringAlgorithm)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_standardClusteringAlgorithmName));
    }
    else
    {
        PandoraContentApi::Cluster::Parameters remnantParameters;
        CaloHitList &remnantCaloHitList(remnantParameters.m_caloHitList);

        for (CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
        {
            if ((m_shouldClusterIsolatedHits || !(*iter)->IsIsolated()) && PandoraContentApi::IsAvailable(*this, *iter))
                remnantCaloHitList.insert(*iter);
        }

        if (!remnantCaloHitList.empty())
        {
            const Cluster *pRemnantCluster = NULL;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, remnantParameters, pRemnantCluster));
        }
    }

    // If specified, associate isolated hits with the newly formed clusters
    if (m_shouldAssociateIsolatedHits)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_isolatedHitAssociationAlgorithmName));
    }

    // Delete any empty clusters
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RemoveEmptyClusters());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ForcedClusteringAlgorithm::RemoveEmptyClusters() const
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

    ClusterList clusterDeletionList;

    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        if (0 == (*iter)->GetNCaloHits())
            clusterDeletionList.insert(*iter);
    }

    if (!clusterDeletionList.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Delete(*this, &clusterDeletionList));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ForcedClusteringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldRunStandardClusteringAlgorithm", m_shouldRunStandardClusteringAlgorithm));

    if (m_shouldRunStandardClusteringAlgorithm)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle, "StandardClustering",
            m_standardClusteringAlgorithmName));
    }

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldClusterIsolatedHits", m_shouldClusterIsolatedHits));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldAssociateIsolatedHits", m_shouldAssociateIsolatedHits));

    if (m_shouldAssociateIsolatedHits)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle, "IsolatedHitAssociation",
            m_isolatedHitAssociationAlgorithmName));
    }

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
