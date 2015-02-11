/**
 *  @file   PandoraSDK/src/Helpers/MCParticleHelper.cc
 * 
 *  @brief  Implementation of the mc particle helper class.
 * 
 *  $Log: $
 */

#include "Helpers/MCParticleHelper.h"

#include "Objects/Cluster.h"
#include "Objects/MCParticle.h"
#include "Objects/OrderedCaloHitList.h"

namespace pandora
{

template <>
const MCParticle *MCParticleHelper::GetMainMCParticle(const CaloHit *const pCaloHit)
{
    float bestWeight(0.f);
    const MCParticle *pBestMCParticle = NULL;
    const MCParticleWeightMap &hitMCParticleWeightMap(pCaloHit->GetMCParticleWeightMap());

    for (MCParticleWeightMap::const_iterator iter = hitMCParticleWeightMap.begin(), iterEnd = hitMCParticleWeightMap.end(); iter != iterEnd; ++iter)
    {
        if (iter->second > bestWeight)
        {
            bestWeight = iter->second;
            pBestMCParticle = iter->first;
        }
    }

    if (NULL == pBestMCParticle)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return pBestMCParticle;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
const MCParticle *MCParticleHelper::GetMainMCParticle(const Cluster *const pCluster)
{
    MCParticleWeightMap mcParticleWeightMap;
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const CaloHit *const pCaloHit = *hitIter;
            const MCParticleWeightMap &hitMCParticleWeightMap(pCaloHit->GetMCParticleWeightMap());

            for (MCParticleWeightMap::const_iterator weightIter = hitMCParticleWeightMap.begin(), weightIterEnd = hitMCParticleWeightMap.end();
                weightIter != weightIterEnd; ++weightIter)
            {
                const float weight(weightIter->second);
                mcParticleWeightMap[weightIter->first] += weight;
            }
        }
    }

    float bestWeight(0.f);
    const MCParticle *pBestMCParticle(NULL);

    for (MCParticleWeightMap::const_iterator iter = mcParticleWeightMap.begin(), iterEnd = mcParticleWeightMap.end(); iter != iterEnd; ++iter)
    {
        const MCParticle *const pCurrentMCParticle = iter->first;
        const float currentWeight = iter->second;

        if (currentWeight > bestWeight)
        {
            pBestMCParticle = pCurrentMCParticle;
            bestWeight = currentWeight;
        }
    }

    if (NULL == pBestMCParticle)
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);

    return pBestMCParticle;
}

} // namespace pandora
