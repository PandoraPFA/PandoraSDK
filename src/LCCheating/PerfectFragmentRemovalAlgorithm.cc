/**
 *  @file   LCContent/src/LCCheating/PerfectFragmentRemovalAlgorithm.cc
 * 
 *  @brief  Implementation of the perfect fragment removal algorithm class
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCCheating/PerfectFragmentRemovalAlgorithm.h"

#include "LCHelpers/SortingHelper.h"

using namespace pandora;

namespace lc_content
{

PerfectFragmentRemovalAlgorithm::PerfectFragmentRemovalAlgorithm() :
    m_shouldMergeChargedClusters(false)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PerfectFragmentRemovalAlgorithm::Run()
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

    // Separate into charged and neutral cluster lists
    ClusterVector chargedClusters, neutralClusters;

    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        if (!pCluster->GetAssociatedTrackList().empty())
        {
            chargedClusters.push_back(pCluster);
        }
        else
        {
            neutralClusters.push_back(pCluster);
        }
    }

    std::sort(chargedClusters.begin(), chargedClusters.end(), SortingHelper::SortClustersByHadronicEnergy);
    std::sort(neutralClusters.begin(), neutralClusters.end(), SortingHelper::SortClustersByHadronicEnergy);

    // Merge charged clusters sharing same mc particle
    MCParticleToClusterMap mcParticleToClusterMap;

    for (ClusterVector::iterator iter = chargedClusters.begin(), iterEnd = chargedClusters.end(); iter != iterEnd; ++iter)
    {
        try
        {
            Cluster *pCluster = *iter;

            if (NULL == pCluster)
                continue;

            const MCParticle *const pMainMCParticle(MCParticleHelper::GetMainMCParticle(pCluster));

            MCParticleToClusterMap::iterator itMCParticle = mcParticleToClusterMap.find(pMainMCParticle);

            if (itMCParticle == mcParticleToClusterMap.end())
            {
                (void) mcParticleToClusterMap.insert(MCParticleToClusterMap::value_type(pMainMCParticle, pCluster));
            }
            else if (m_shouldMergeChargedClusters)
            {
                Cluster *pMainChargedCluster = itMCParticle->second;
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pMainChargedCluster, pCluster));
                *iter = NULL;
            }
        }
        catch (StatusCodeException &)
        {
        }
    }

    // Merge neutral clusters with relevant charged clusters
    for (ClusterVector::iterator iter = neutralClusters.begin(), iterEnd = neutralClusters.end(); iter != iterEnd; ++iter)
    {
        try
        {
            Cluster *pCluster = *iter;

            if (NULL == pCluster)
                continue;

            const MCParticle *const pMainMCParticle(MCParticleHelper::GetMainMCParticle(pCluster));

            MCParticleToClusterMap::iterator itMCParticle = mcParticleToClusterMap.find(pMainMCParticle);

            if (itMCParticle != mcParticleToClusterMap.end())
            {
                Cluster *pMainChargedCluster = itMCParticle->second;
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pMainChargedCluster, pCluster));
                *iter = NULL;
            }
        }
        catch (StatusCodeException &)
        {
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PerfectFragmentRemovalAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldMergeChargedClusters", m_shouldMergeChargedClusters));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
