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
    typedef std::map<const pandora::MCParticle*, float> MCParticleToEnergyMap;
    MCParticleToEnergyMap mcParticleToEnergyMap;

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            CaloHit *pCaloHit = *hitIter;

            const MCParticle *pMCParticle(NULL);
            (void) pCaloHit->GetMCParticle(pMCParticle);

            if (NULL == pMCParticle)
                continue;

            const float hitEnergy(pCaloHit->GetHadronicEnergy());

            MCParticleToEnergyMap::iterator itMCParticleToEnergy = mcParticleToEnergyMap.find(pMCParticle);

            if (itMCParticleToEnergy == mcParticleToEnergyMap.end())
            {
                if (!mcParticleToEnergyMap.insert(MCParticleToEnergyMap::value_type(pMCParticle, hitEnergy)).second)
                    throw StatusCodeException(STATUS_CODE_FAILURE);
            }
            else
            {
                itMCParticleToEnergy->second += hitEnergy;
            }
        }
    }

    const MCParticle *pMainMCParticle(NULL);
    float energyOfSelectedMCParticle(0.f);

    for (MCParticleToEnergyMap::const_iterator iter = mcParticleToEnergyMap.begin(), iterEnd = mcParticleToEnergyMap.end(); iter != iterEnd; ++iter)
    {
        const MCParticle *const pCurrentMCParticle = iter->first;
        const float currentEnergy = iter->second;

        if (currentEnergy > energyOfSelectedMCParticle)
        {
            pMainMCParticle = pCurrentMCParticle;
            energyOfSelectedMCParticle = currentEnergy;
        }
    }

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
