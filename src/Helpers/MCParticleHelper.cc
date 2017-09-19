/**
 *  @file   PandoraSDK/src/Helpers/MCParticleHelper.cc
 * 
 *  @brief  Implementation of the mc particle helper class.
 * 
 *  $Log: $
 */

#include "Helpers/MCParticleHelper.h"

#include "Objects/CaloHit.h"
#include "Objects/Cluster.h"
#include "Objects/MCParticle.h"
#include "Objects/Track.h"

#include "Pandora/PandoraInternal.h"

#include <algorithm>

namespace pandora
{

template <typename T>
const MCParticle *MCParticleHelper::GetMainMCParticle(const T *const pT)
{
    float bestWeight(0.f);
    const MCParticle *pBestMCParticle(nullptr);
    const MCParticleWeightMap &hitMCParticleWeightMap(pT->GetMCParticleWeightMap());

    MCParticleVector mcParticleVector;
    for (const MCParticleWeightMap::value_type &mapEntry : hitMCParticleWeightMap) mcParticleVector.push_back(mapEntry.first);
    std::sort(mcParticleVector.begin(), mcParticleVector.end(), PointerLessThan<MCParticle>());

    for (const MCParticle *const pMCParticle : mcParticleVector)
    {
        const float weight(hitMCParticleWeightMap.at(pMCParticle));

        if (weight > bestWeight)
        {
            bestWeight = weight;
            pBestMCParticle = pMCParticle;
        }
    }

    if (!pBestMCParticle)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return pBestMCParticle;
}

template <>
const MCParticle *MCParticleHelper::GetMainMCParticle(const CaloHitList *const pCaloHitList)
{
    MCParticleWeightMap mcParticleWeightMap;

    for (const CaloHit *const pCaloHit : *pCaloHitList)
    {
        const MCParticleWeightMap &hitMCParticleWeightMap(pCaloHit->GetMCParticleWeightMap());

        MCParticleVector mcParticleVector;
        for (const MCParticleWeightMap::value_type &mapEntry : hitMCParticleWeightMap) mcParticleVector.push_back(mapEntry.first);
        std::sort(mcParticleVector.begin(), mcParticleVector.end(), PointerLessThan<MCParticle>());

        for (const MCParticle *const pMCParticle : mcParticleVector)
        {
            const float weight(hitMCParticleWeightMap.at(pMCParticle));
            mcParticleWeightMap[pMCParticle] += weight;
        }
    }

    float bestWeight(0.f);
    const MCParticle *pBestMCParticle(nullptr);

    MCParticleVector mcParticleVector;
    for (const MCParticleWeightMap::value_type &mapEntry : mcParticleWeightMap) mcParticleVector.push_back(mapEntry.first);
    std::sort(mcParticleVector.begin(), mcParticleVector.end(), PointerLessThan<MCParticle>());

    for (const MCParticle *const pCurrentMCParticle : mcParticleVector)
    {
        const float currentWeight(mcParticleWeightMap.at(pCurrentMCParticle));

        if (currentWeight > bestWeight)
        {
            pBestMCParticle = pCurrentMCParticle;
            bestWeight = currentWeight;
        }
    }

    if (!pBestMCParticle)
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);

    return pBestMCParticle;
}

template <>
const MCParticle *MCParticleHelper::GetMainMCParticle(const Cluster *const pCluster)
{
    CaloHitList caloHitList;
    pCluster->GetOrderedCaloHitList().FillCaloHitList(caloHitList);

    return MCParticleHelper::GetMainMCParticle(&caloHitList);
}

template <>
const MCParticle *MCParticleHelper::GetMainMCParticle(const ClusterList *const pClusterList)
{
    CaloHitList caloHitList;

    for (const Cluster *const pCluster : *pClusterList)
        pCluster->GetOrderedCaloHitList().FillCaloHitList(caloHitList);

    return MCParticleHelper::GetMainMCParticle(&caloHitList);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template const MCParticle *MCParticleHelper::GetMainMCParticle(const CaloHit *const);
template const MCParticle *MCParticleHelper::GetMainMCParticle(const Track *const);

} // namespace pandora
