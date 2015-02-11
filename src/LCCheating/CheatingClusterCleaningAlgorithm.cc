/**
 *  @file   LCContent/src/LCCheating/CheatingClusterCleaningAlgorithm.cc
 * 
 *  @brief  Implementation of the cheating cluster cleaning algorithm class
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCCheating/CheatingClusterCleaningAlgorithm.h"

using namespace pandora;

namespace lc_content
{

StatusCode CheatingClusterCleaningAlgorithm::Run()
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

    // Examine clusters, reducing each to just the hits corresponding to the main mc particle
    for (ClusterList::const_iterator itCluster = pClusterList->begin(), itClusterEnd = pClusterList->end(); itCluster != itClusterEnd; ++itCluster )
    {
        try
        {
            const Cluster *const pCluster = *itCluster;
            const MCParticle *const pMainMCParticle(MCParticleHelper::GetMainMCParticle(pCluster));

            // Remove all calo hits that do not correspond to the cluster main mc particle
            OrderedCaloHitList orderedCaloHitList(pCluster->GetOrderedCaloHitList());

            for (OrderedCaloHitList::const_iterator itLyr = orderedCaloHitList.begin(), itLyrEnd = orderedCaloHitList.end(); itLyr != itLyrEnd; ++itLyr)
            {
                for (CaloHitList::const_iterator hitIter = itLyr->second->begin(), hitIterEnd = itLyr->second->end(); hitIter != hitIterEnd; ++hitIter)
                {
                    try
                    {
                        const CaloHit *const pCaloHit = *hitIter;
                        const MCParticle *const pMCParticle(MCParticleHelper::GetMainMCParticle(pCaloHit));

                        if (pMainMCParticle != pMCParticle)
                        {
                            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RemoveFromCluster(*this, pCluster, pCaloHit));
                        }
                    }
                    catch (StatusCodeException &)
                    {
                    }
                }
            }

            // Repeat for isolated hits
            CaloHitList isolatedCaloHitList(pCluster->GetIsolatedCaloHitList());

            for (CaloHitList::const_iterator hitIter = isolatedCaloHitList.begin(), hitIterEnd = isolatedCaloHitList.end(); hitIter != hitIterEnd; ++hitIter)
            {
                try
                {
                    const CaloHit *const pCaloHit = *hitIter;
                    const MCParticle *const pMCParticle(MCParticleHelper::GetMainMCParticle(pCaloHit));

                    if (pMainMCParticle != pMCParticle)
                    {
                        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RemoveIsolatedFromCluster(*this, pCluster, pCaloHit));
                    }
                }
                catch (StatusCodeException &)
                {
                }
            }
        }
        catch (StatusCodeException &)
        {
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CheatingClusterCleaningAlgorithm::ReadSettings(const TiXmlHandle /*xmlHandle*/)
{
    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
