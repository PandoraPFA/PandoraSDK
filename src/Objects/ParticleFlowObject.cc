/**
 *  @file   PandoraPFANew/Framework/src/Objects/ParticleFlowObject.cc
 * 
 *  @brief  Implementation of the particle flow object class.
 * 
 *  $Log: $
 */

#include "Objects/Cluster.h"
#include "Objects/OrderedCaloHitList.h"
#include "Objects/ParticleFlowObject.h"
#include "Objects/Track.h"

namespace pandora
{

ParticleFlowObject::ParticleFlowObject(const PandoraContentApi::ParticleFlowObjectParameters &particleFlowObjectParameters) :
    m_particleId(particleFlowObjectParameters.m_particleId.Get()),
    m_charge(particleFlowObjectParameters.m_charge.Get()),
    m_mass(particleFlowObjectParameters.m_mass.Get()),
    m_energy(particleFlowObjectParameters.m_energy.Get()),
    m_momentum(particleFlowObjectParameters.m_momentum.Get()),
    m_vertex(particleFlowObjectParameters.m_vertex.Get()),
    m_trackList(particleFlowObjectParameters.m_trackList),
    m_clusterList(particleFlowObjectParameters.m_clusterList)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

TrackAddressList ParticleFlowObject::GetTrackAddressList() const
{
    TrackAddressList trackAddressList;

    for (TrackList::const_iterator iter = m_trackList.begin(), iterEnd = m_trackList.end(); iter != iterEnd; ++iter)
    {
        trackAddressList.push_back((*iter)->GetParentTrackAddress());
    }

    return trackAddressList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

ClusterAddressList ParticleFlowObject::GetClusterAddressList() const
{
    ClusterAddressList clusterAddressList;

    for (ClusterList::const_iterator iter = m_clusterList.begin(), iterEnd = m_clusterList.end(); iter != iterEnd; ++iter)
    {
        CaloHitAddressList caloHitAddressList;

        OrderedCaloHitList orderedCaloHitList((*iter)->GetOrderedCaloHitList());
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, orderedCaloHitList.Add((*iter)->GetIsolatedCaloHitList()));

        for (OrderedCaloHitList::const_iterator layerIter = orderedCaloHitList.begin(), layerIterEnd = orderedCaloHitList.end();
            layerIter != layerIterEnd; ++layerIter)
        {
            for (CaloHitList::const_iterator hitIter = layerIter->second->begin(), hitIterEnd = layerIter->second->end();
                hitIter != hitIterEnd; ++hitIter)
            {
                caloHitAddressList.push_back((*hitIter)->GetParentCaloHitAddress());
            }
        }

        clusterAddressList.push_back(caloHitAddressList);
    }

    return clusterAddressList;
}


//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObject::AddParent(ParticleFlowObject *const pPfo)
{
    if (NULL == pPfo)
        return STATUS_CODE_INVALID_PARAMETER;

    if (!m_parentPfoList.insert(pPfo).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObject::AddDaughter(ParticleFlowObject *const pPfo)
{
    if (NULL == pPfo)
        return STATUS_CODE_INVALID_PARAMETER;

    if (!m_daughterPfoList.insert(pPfo).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObject::RemoveParent(ParticleFlowObject *const pPfo)
{
    PfoList::const_iterator iter = m_parentPfoList.find(pPfo);

    if (m_parentPfoList.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    m_parentPfoList.erase(iter);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObject::RemoveDaughter(ParticleFlowObject *const pPfo)
{
    PfoList::const_iterator iter = m_daughterPfoList.find(pPfo);

    if (m_daughterPfoList.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    m_daughterPfoList.erase(iter);
    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
