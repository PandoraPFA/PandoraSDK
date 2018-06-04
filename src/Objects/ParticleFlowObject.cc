/**
 *  @file   PandoraSDK/src/Objects/ParticleFlowObject.cc
 * 
 *  @brief  Implementation of the particle flow object class.
 * 
 *  $Log: $
 */

#include "Objects/Cluster.h"
#include "Objects/OrderedCaloHitList.h"
#include "Objects/ParticleFlowObject.h"
#include "Objects/Track.h"

#include <algorithm>

namespace pandora
{

ParticleFlowObject::ParticleFlowObject(const object_creation::ParticleFlowObject::Parameters &parameters) :
    m_particleId(parameters.m_particleId.Get()),
    m_charge(parameters.m_charge.Get()),
    m_mass(parameters.m_mass.Get()),
    m_energy(parameters.m_energy.Get()),
    m_momentum(parameters.m_momentum.Get()),
    m_trackList(parameters.m_trackList),
    m_clusterList(parameters.m_clusterList),
    m_vertexList(parameters.m_vertexList),
    m_propertiesMap(parameters.m_propertiesToAdd)
{
    if (!parameters.m_propertiesToRemove.empty())
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

ParticleFlowObject::~ParticleFlowObject()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObject::AlterMetadata(const object_creation::ParticleFlowObject::Metadata &metadata)
{
    if (!metadata.m_propertiesToAdd.empty() || !metadata.m_propertiesToRemove.empty())
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->UpdatePropertiesMap(metadata));

    if (metadata.m_particleId.IsInitialized())
        m_particleId = metadata.m_particleId.Get();

    if (metadata.m_charge.IsInitialized())
        m_charge = metadata.m_charge.Get();

    if (metadata.m_mass.IsInitialized())
        m_mass = metadata.m_mass.Get();

    if (metadata.m_energy.IsInitialized())
        m_energy = metadata.m_energy.Get();

    if (metadata.m_momentum.IsInitialized())
        m_momentum = metadata.m_momentum.Get();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

TrackAddressList ParticleFlowObject::GetTrackAddressList() const
{
    TrackAddressList trackAddressList;

    for (const Track *const pTrack : m_trackList)
    {
        trackAddressList.push_back(pTrack->GetParentAddress());
    }

    return trackAddressList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

ClusterAddressList ParticleFlowObject::GetClusterAddressList() const
{
    ClusterAddressList clusterAddressList;

    for (const Cluster *const pCluster : m_clusterList)
    {
        CaloHitAddressList caloHitAddressList;

        OrderedCaloHitList orderedCaloHitList(pCluster->GetOrderedCaloHitList());
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, orderedCaloHitList.Add(pCluster->GetIsolatedCaloHitList()));

        for (const OrderedCaloHitList::value_type &layerEntry : orderedCaloHitList)
        {
            for (const CaloHit *const pCaloHit : *layerEntry.second)
                caloHitAddressList.push_back(pCaloHit->GetParentAddress());
        }

        clusterAddressList.push_back(caloHitAddressList);
    }

    return clusterAddressList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode ParticleFlowObject::AddToPfo(const Cluster *const pCluster)
{
    if (m_clusterList.end() != std::find(m_clusterList.begin(), m_clusterList.end(), pCluster))
        return STATUS_CODE_ALREADY_PRESENT;

    m_clusterList.push_back(pCluster);
    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode ParticleFlowObject::AddToPfo(const Track *const pTrack)
{
    if (m_trackList.end() != std::find(m_trackList.begin(), m_trackList.end(), pTrack))
        return STATUS_CODE_ALREADY_PRESENT;

    m_trackList.push_back(pTrack);
    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode ParticleFlowObject::AddToPfo(const Vertex *const pVertex)
{
    if (m_vertexList.end() != std::find(m_vertexList.begin(), m_vertexList.end(), pVertex))
        return STATUS_CODE_ALREADY_PRESENT;

    m_vertexList.push_back(pVertex);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode ParticleFlowObject::RemoveFromPfo(const Cluster *const pCluster)
{
    ClusterList::iterator iter = std::find(m_clusterList.begin(), m_clusterList.end(), pCluster);

    if (m_clusterList.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    m_clusterList.erase(iter);
    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode ParticleFlowObject::RemoveFromPfo(const Track *const pTrack)
{
    TrackList::iterator iter = std::find(m_trackList.begin(), m_trackList.end(), pTrack);

    if (m_trackList.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    m_trackList.erase(iter);
    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode ParticleFlowObject::RemoveFromPfo(const Vertex *const pVertex)
{
    VertexList::iterator iter = std::find(m_vertexList.begin(), m_vertexList.end(), pVertex);

    if (m_vertexList.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    m_vertexList.erase(iter);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObject::AddParent(const ParticleFlowObject *const pPfo)
{
    if (!pPfo)
        return STATUS_CODE_INVALID_PARAMETER;

    if (m_parentPfoList.end() != std::find(m_parentPfoList.begin(), m_parentPfoList.end(), pPfo))
        return STATUS_CODE_ALREADY_PRESENT;

    m_parentPfoList.push_back(pPfo);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObject::AddDaughter(const ParticleFlowObject *const pPfo)
{
    if (!pPfo)
        return STATUS_CODE_INVALID_PARAMETER;

    if (m_daughterPfoList.end() != std::find(m_daughterPfoList.begin(), m_daughterPfoList.end(), pPfo))
        return STATUS_CODE_ALREADY_PRESENT;

    m_daughterPfoList.push_back(pPfo);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObject::RemoveParent(const ParticleFlowObject *const pPfo)
{
    PfoList::iterator iter = std::find(m_parentPfoList.begin(), m_parentPfoList.end(), pPfo);

    if (m_parentPfoList.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    m_parentPfoList.erase(iter);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObject::RemoveDaughter(const ParticleFlowObject *const pPfo)
{
    PfoList::iterator iter = std::find(m_daughterPfoList.begin(), m_daughterPfoList.end(), pPfo);

    if (m_daughterPfoList.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    m_daughterPfoList.erase(iter);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObject::UpdatePropertiesMap(const object_creation::ParticleFlowObject::Metadata &metadata)
{
    for (const std::string &propertyName : metadata.m_propertiesToRemove)
    {
        if (metadata.m_propertiesToAdd.count(propertyName))
            return STATUS_CODE_INVALID_PARAMETER;

        if (!m_propertiesMap.count(propertyName))
            return STATUS_CODE_NOT_FOUND;
    }

    for (const std::string &propertyName : metadata.m_propertiesToRemove)
        m_propertiesMap.erase(propertyName);

    for (const PropertiesMap::value_type &entryToAdd : metadata.m_propertiesToAdd)
        m_propertiesMap[entryToAdd.first] = entryToAdd.second;

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
