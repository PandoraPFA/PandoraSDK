/**
 *  @file   LCContent/src/LCTopologicalAssociation/MuonPhotonSeparationAlgorithm.cc
 * 
 *  @brief  Implementation of the mip-photon separation algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCTopologicalAssociation/MuonPhotonSeparationAlgorithm.h"

using namespace pandora;

namespace lc_content
{

MuonPhotonSeparationAlgorithm::MuonPhotonSeparationAlgorithm() :
    m_highEnergyMuonCut(0.f),
    m_nTransitionLayers(1)
{
    m_additionalPadWidthsFine = 0.7071f;
    m_additionalPadWidthsCoarse = 0.7071f;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MuonPhotonSeparationAlgorithm::PerformFragmentation(Cluster *const pOriginalCluster, Track *const pTrack, unsigned int showerStartLayer,
    unsigned int showerEndLayer) const
{
    ClusterList clusterList;
    clusterList.insert(pOriginalCluster);
    std::string originalClustersListName, fragmentClustersListName;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::InitializeFragmentation(*this, clusterList, originalClustersListName,
        fragmentClustersListName));

    // Make the cluster fragments
    Cluster *pMipCluster = NULL, *pPhotonCluster = NULL;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->MakeClusterFragments(showerStartLayer, showerEndLayer, pOriginalCluster,
        pMipCluster, pPhotonCluster));

    // Decide whether to keep original cluster or the fragments
    std::string clusterListToSaveName(originalClustersListName), clusterListToDeleteName(fragmentClustersListName);

    if ((NULL != pMipCluster) && (NULL != pPhotonCluster) && (pPhotonCluster->GetNCaloHits() >= m_minHitsInPhotonCluster))
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RemoveTrackClusterAssociation(*this, pTrack, pOriginalCluster));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, pTrack, pMipCluster));

        if (PandoraContentApi::GetPlugins(*this)->GetParticleId()->IsMuon(pMipCluster) && (pTrack->GetEnergyAtDca() > m_highEnergyMuonCut))
        {
            clusterListToSaveName = fragmentClustersListName;
            clusterListToDeleteName = originalClustersListName;
        }
        else
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RemoveTrackClusterAssociation(*this, pTrack, pMipCluster));
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, pTrack, pOriginalCluster));
        }
    }

    // End cluster fragmentation operations
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::EndFragmentation(*this, clusterListToSaveName,
        clusterListToDeleteName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MuonPhotonSeparationAlgorithm::MakeClusterFragments(const unsigned int showerStartLayer, const unsigned int showerEndLayer,
    Cluster *const pOriginalCluster, Cluster *&pMipCluster, Cluster *&pPhotonCluster) const
{
    Track *pTrack = *(pOriginalCluster->GetAssociatedTrackList().begin());
    OrderedCaloHitList orderedCaloHitList(pOriginalCluster->GetOrderedCaloHitList());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, orderedCaloHitList.Add(pOriginalCluster->GetIsolatedCaloHitList()));

    for (OrderedCaloHitList::const_iterator iter =  orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        CaloHit *pClosestHit(NULL);
        float closestDistance(std::numeric_limits<float>::max());

        const unsigned int iLayer = iter->first;

        // If in shower region find closest hit on track trajectory
        if (((iLayer + m_nTransitionLayers) >= showerStartLayer) && (iLayer <= (showerEndLayer + m_nTransitionLayers)))
        {
            for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
            {
                CaloHit *pCaloHit = *hitIter;
                float distance(0.f);

                PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_UNCHANGED, !=, this->GetDistanceToTrack(pOriginalCluster, 
                    pTrack, pCaloHit, distance));

                if (distance < closestDistance)
                {
                    closestDistance = distance;
                    pClosestHit = pCaloHit;
                }
            }
        }

        // Add hits to the relevant cluster fragment
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            CaloHit *pCaloHit = *hitIter;

            const bool isHitOnMipPath((pClosestHit == pCaloHit) && (closestDistance < m_genericDistanceCut));

            if (isHitOnMipPath || ((iLayer + m_nTransitionLayers) < showerStartLayer) || (iLayer > (showerEndLayer + m_nTransitionLayers)))
            {
                if (NULL == pMipCluster)
                {
                    PandoraContentApi::Cluster::Parameters parameters;
                    parameters.m_pTrack = pTrack;
                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, parameters, pMipCluster));
                }

                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddToCluster(*this, pMipCluster, pCaloHit));
            }
            else
            {
                if (NULL == pPhotonCluster)
                {
                    PandoraContentApi::Cluster::Parameters parameters;
                    parameters.m_caloHitList.insert(pCaloHit);
                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, parameters, pPhotonCluster));
                }
                else
                {
                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddToCluster(*this, pPhotonCluster, pCaloHit));
                }
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MuonPhotonSeparationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HighEnergyMuonCut", m_highEnergyMuonCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NTransitionLayers", m_nTransitionLayers));

    return MipPhotonSeparationAlgorithm::ReadSettings(xmlHandle);
}

} // namespace lc_content
