/**
 *  @file   LCContent/src/LCTopologicalAssociation/IsolatedHitMergingAlgorithm.cc
 * 
 *  @brief  Implementation of the isolated hit merging algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCHelpers/SortingHelper.h"

#include "LCTopologicalAssociation/IsolatedHitMergingAlgorithm.h"

using namespace pandora;

namespace lc_content
{

IsolatedHitMergingAlgorithm::IsolatedHitMergingAlgorithm() :
    m_shouldUseCurrentClusterList(true),
    m_minHitsInCluster(4),
    m_maxRecombinationDistance(250.f),
    m_minCosOpeningAngle(0.f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode IsolatedHitMergingAlgorithm::Run()
{
    // HACK
    const ClusterList *pInputClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pInputClusterList));

    // Read specified lists of input clusters
    ClusterList clusterList;

    if (m_shouldUseCurrentClusterList)
    {
        const ClusterList *pClusterList = NULL;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

        clusterList.insert(pClusterList->begin(), pClusterList->end());
    }

    for (StringVector::const_iterator iter = m_additionalClusterListNames.begin(), iterEnd = m_additionalClusterListNames.end(); iter != iterEnd; ++iter)
    {
        const ClusterList *pClusterList = NULL;

        if (STATUS_CODE_SUCCESS == PandoraContentApi::GetList(*this, *iter, pClusterList))
        {
            clusterList.insert(pClusterList->begin(), pClusterList->end());
        }
        else
        {
            std::cout << "IsolatedHitMergingAlgorithm: Failed to obtain cluster list " << *iter << std::endl;
        }
    }

    // Create a vector of input clusters, ordered by inner layer
    ClusterVector clusterVector(clusterList.begin(), clusterList.end());
    std::sort(clusterVector.begin(), clusterVector.end(), SortingHelper::SortClustersByInnerLayer);


    // FIRST PART - find "small" clusters, below threshold number of calo hits, delete them and associate hits with other clusters
    for (ClusterVector::iterator iterI = clusterVector.begin(), iterIEnd = clusterVector.end(); iterI != iterIEnd; ++iterI)
    {
        const Cluster *const pClusterToDelete = *iterI;

        if (NULL == pClusterToDelete)
            continue;

        const unsigned int nCaloHits(pClusterToDelete->GetNCaloHits());

        if (nCaloHits > m_minHitsInCluster)
            continue;

        if (pInputClusterList->end() == pInputClusterList->find(pClusterToDelete))
            continue;

        CaloHitList caloHitList;
        pClusterToDelete->GetOrderedCaloHitList().GetCaloHitList(caloHitList);

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Delete(*this, pClusterToDelete));
        *iterI = NULL;

        // Redistribute hits that used to be in cluster I amongst other clusters
        for (CaloHitList::const_iterator hitIter = caloHitList.begin(), hitIterEnd = caloHitList.end(); hitIter != hitIterEnd; ++hitIter)
        {
            const CaloHit *const pCaloHit = *hitIter;

            const Cluster *pBestHostCluster(NULL);
            float bestHostClusterEnergy(0.);
            float minDistance(m_maxRecombinationDistance);

            // Find the most appropriate cluster for this newly-available hit
            for (ClusterVector::const_iterator iterJ = clusterVector.begin(), iterJEnd = clusterVector.end(); iterJ != iterJEnd; ++iterJ)
            {
                const Cluster *const pNewHostCluster = *iterJ;

                if (NULL == pNewHostCluster)
                    continue;

                if (pNewHostCluster->GetNCaloHits() < nCaloHits)
                    continue;

                const float distance(this->GetDistanceToHit(pNewHostCluster, pCaloHit));
                const float hostClusterEnergy(pNewHostCluster->GetHadronicEnergy());

                // In event of equidistant host candidates, choose highest energy cluster
                if ((distance < minDistance) || ((distance == minDistance) && (hostClusterEnergy > bestHostClusterEnergy)))
                {
                    minDistance = distance;
                    pBestHostCluster = pNewHostCluster;
                    bestHostClusterEnergy = hostClusterEnergy;
                }
            }

            if (NULL != pBestHostCluster)
            {
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddIsolatedToCluster(*this, pBestHostCluster, pCaloHit));
            }
        }
    }


    // SECOND PART - loop over the remaining available isolated hits, and associate them with other clusters
    const CaloHitList *pCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pCaloHitList));

    for (CaloHitList::const_iterator hitIterI = pCaloHitList->begin(); hitIterI != pCaloHitList->end(); ++hitIterI)
    {
        const CaloHit *const pCaloHit = *hitIterI;

        if (!pCaloHit->IsIsolated() || !PandoraContentApi::IsAvailable(*this, pCaloHit))
            continue;

        const Cluster *pBestHostCluster(NULL);
        float bestHostClusterEnergy(0.);
        float minDistance(m_maxRecombinationDistance);

        // Find most appropriate cluster for this isolated hit
        for (ClusterVector::const_iterator iterJ = clusterVector.begin(), iterJEnd = clusterVector.end(); iterJ != iterJEnd; ++iterJ)
        {
            const Cluster *const pCluster = *iterJ;

            if (NULL == pCluster)
                continue;

            const float distance(this->GetDistanceToHit(pCluster, pCaloHit));
            const float hostClusterEnergy(pCluster->GetHadronicEnergy());

            if ((distance < minDistance) || ((distance == minDistance) && (hostClusterEnergy > bestHostClusterEnergy)))
            {
                minDistance = distance;
                pBestHostCluster = pCluster;
                bestHostClusterEnergy = hostClusterEnergy;
            }
        }

        if (NULL != pBestHostCluster)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddIsolatedToCluster(*this, pBestHostCluster, pCaloHit));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float IsolatedHitMergingAlgorithm::GetDistanceToHit(const Cluster *const pCluster, const CaloHit *const pCaloHit) const
{
    // Apply simple preselection using cosine of opening angle between the hit and cluster directions
    if (pCaloHit->GetExpectedDirection().GetCosOpeningAngle(pCluster->GetInitialDirection()) < m_minCosOpeningAngle)
        return std::numeric_limits<float>::max();

    float minDistanceSquared(std::numeric_limits<float>::max());
    const CartesianVector &hitPosition(pCaloHit->GetPositionVector());
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        const float distanceSquared((pCluster->GetCentroid(iter->first) - hitPosition).GetMagnitudeSquared());

        if (distanceSquared < minDistanceSquared)
            minDistanceSquared = distanceSquared;
    }

    if (minDistanceSquared < std::numeric_limits<float>::max())
        return std::sqrt(minDistanceSquared);

    return std::numeric_limits<float>::max();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode IsolatedHitMergingAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldUseCurrentClusterList", m_shouldUseCurrentClusterList));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "AdditionalClusterListNames", m_additionalClusterListNames));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitsInCluster", m_minHitsInCluster));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxRecombinationDistance", m_maxRecombinationDistance));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCosOpeningAngle", m_minCosOpeningAngle));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
