/**
 *  @file   PandoraPFANew/Framework/src/Helpers/MCParticleHelper.cc
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

const MCParticle *MCParticleHelper::GetMainMCParticle(const Cluster *const pCluster)
{
    MCParticleWeightMap mcParticleWeightMap;
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            CaloHit *pCaloHit = *hitIter;
            const MCParticleWeightMap &hitMCParticleWeightMap(pCaloHit->GetMCParticleWeightMap());

            for (MCParticleWeightMap::const_iterator weightIter = hitMCParticleWeightMap.begin(), weightIterEnd = hitMCParticleWeightMap.end();
                weightIter != weightIterEnd; ++weightIter)
            {
                const float weight(weightIter->second);
                mcParticleWeightMap[weightIter->first] += weight;
            }
        }
    }

    const MCParticle *pMainMCParticle(NULL);
    float energyOfSelectedMCParticle(0.f);

    for (MCParticleWeightMap::const_iterator iter = mcParticleWeightMap.begin(), iterEnd = mcParticleWeightMap.end(); iter != iterEnd; ++iter)
    {
        const MCParticle *const pCurrentMCParticle = iter->first;
        const float currentEnergy = iter->second;

        if (currentEnergy > energyOfSelectedMCParticle)
        {
            pMainMCParticle = pCurrentMCParticle;
            energyOfSelectedMCParticle = currentEnergy;
        }
    }

    if (NULL == pMainMCParticle)
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);

    return pMainMCParticle;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCParticleHelper::ReadSettings(const TiXmlHandle *const /*pXmlHandle*/)
{
    /*TiXmlElement *pXmlElement(pXmlHandle->FirstChild("MCParticleHelper").Element());

    if (NULL != pXmlElement)
    {
        const TiXmlHandle xmlHandle(pXmlElement);
    }*/

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
