/**
 *  @file   PandoraSDK/src/Plugins/ParticleIdPlugin.cc
 * 
 *  @brief  Implementation of the particle id plugin class.
 * 
 *  $Log: $
 */

#include "Helpers/XmlHelper.h"

#include "Objects/Cluster.h"
#include "Objects/ParticleFlowObject.h"

#include "Pandora/PdgTable.h"

#include "Plugins/ParticleIdPlugin.h"

#include <cstdlib>

namespace pandora
{

template <typename T>
bool ParticleId::IsEmShower(const T *const pT) const
{
    if (!m_pEmShowerPlugin)
        return false;

    return m_pEmShowerPlugin->IsMatch(pT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
bool ParticleId::IsPhoton(const T *const pT) const
{
    if (!m_pPhotonPlugin)
        return false;

    return m_pPhotonPlugin->IsMatch(pT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
bool ParticleId::IsElectron(const T *const pT) const
{
    if (!m_pElectronPlugin)
        return false;

    return m_pElectronPlugin->IsMatch(pT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
bool ParticleId::IsMuon(const T *const pT) const
{
    if (!m_pMuonPlugin)
        return false;

    return m_pMuonPlugin->IsMatch(pT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

ParticleId::ParticleId(const Pandora *const pPandora) :
    m_pPandora(pPandora),
    m_pEmShowerPlugin(nullptr),
    m_pPhotonPlugin(nullptr),
    m_pElectronPlugin(nullptr),
    m_pMuonPlugin(nullptr)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

ParticleId::~ParticleId()
{
    for (const ParticleIdPluginMap::value_type &mapEntry : m_particleIdPluginMap)
        delete mapEntry.second;

    m_particleIdPluginMap.clear();
    m_pEmShowerPlugin = nullptr;
    m_pPhotonPlugin = nullptr;
    m_pElectronPlugin = nullptr;
    m_pMuonPlugin = nullptr;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleId::RegisterPlugin(const std::string &name, ParticleIdPlugin *const pParticleIdPlugin)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pParticleIdPlugin->RegisterDetails(m_pPandora, name, name));

    if (!m_particleIdPluginMap.insert(ParticleIdPluginMap::value_type(name, pParticleIdPlugin)).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleId::InitializePlugins(const TiXmlHandle *const pXmlHandle)
{
    for (const ParticleIdPluginMap::value_type &mapEntry : m_particleIdPluginMap)
    {
        TiXmlElement *pXmlElement(pXmlHandle->FirstChild(mapEntry.first).Element());

        if (nullptr != pXmlElement)
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, mapEntry.second->ReadSettings(TiXmlHandle(pXmlElement)));

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, mapEntry.second->Initialize());
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->InitializePlugin(pXmlHandle, "EmShowerPlugin", m_pEmShowerPlugin));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->InitializePlugin(pXmlHandle, "PhotonPlugin", m_pPhotonPlugin));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->InitializePlugin(pXmlHandle, "ElectronPlugin", m_pElectronPlugin));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->InitializePlugin(pXmlHandle, "MuonPlugin", m_pMuonPlugin));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleId::InitializePlugin(const TiXmlHandle *const pXmlHandle, const std::string &xmlTagName, ParticleIdPlugin *&pParticleIdPlugin)
{
    if (nullptr != pParticleIdPlugin)
        return STATUS_CODE_FAILURE;

    std::string requestedPluginName;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        xmlTagName, requestedPluginName));

    if (requestedPluginName.empty())
        return STATUS_CODE_SUCCESS;

    ParticleIdPluginMap::const_iterator mapIter = m_particleIdPluginMap.find(requestedPluginName);

    if (m_particleIdPluginMap.end() == mapIter)
        return STATUS_CODE_NOT_FOUND;

    pParticleIdPlugin = mapIter->second;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleId::ResetForNextEvent()
{
    for (const ParticleIdPluginMap::value_type &mapEntry : m_particleIdPluginMap)
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, mapEntry.second->Reset());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template bool ParticleId::IsEmShower(const Cluster *const ) const;
template bool ParticleId::IsPhoton(const Cluster *const ) const;
template bool ParticleId::IsElectron(const Cluster *const ) const;
template bool ParticleId::IsMuon(const Cluster *const ) const;

template bool ParticleId::IsEmShower(const ParticleFlowObject *const ) const;
template bool ParticleId::IsPhoton(const ParticleFlowObject *const ) const;
template bool ParticleId::IsElectron(const ParticleFlowObject *const ) const;
template bool ParticleId::IsMuon(const ParticleFlowObject *const ) const;

} // namespace pandora
