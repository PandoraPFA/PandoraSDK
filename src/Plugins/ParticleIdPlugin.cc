/**
 *  @file   PandoraSDK/src/Plugins/ParticleIdPlugin.cc
 * 
 *  @brief  Implementation of the particle id plugin class.
 * 
 *  $Log: $
 */

#include "Helpers/XmlHelper.h"

#include "Objects/Cluster.h"

#include "Plugins/ParticleIdPlugin.h"

namespace pandora
{

bool ParticleId::IsEmShower(const Cluster *const pCluster) const
{
    if (NULL == m_pEmShowerPlugin)
        return false;

    return m_pEmShowerPlugin->IsMatch(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleId::IsPhoton(const Cluster *const pCluster) const
{
    if (pCluster->IsFixedPhoton())
        return true;

    if (NULL == m_pPhotonPlugin)
        return false;

    return m_pPhotonPlugin->IsMatch(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleId::IsElectron(const Cluster *const pCluster) const
{
    if (pCluster->IsFixedElectron())
        return true;

    if (NULL == m_pElectronPlugin)
        return false;

    return m_pElectronPlugin->IsMatch(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleId::IsMuon(const Cluster *const pCluster) const
{
    if (pCluster->IsFixedMuon())
        return true;

    if (NULL == m_pMuonPlugin)
        return false;

    return m_pMuonPlugin->IsMatch(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

ParticleId::ParticleId(const Pandora *const pPandora) :
    m_pPandora(pPandora),
    m_pEmShowerPlugin(NULL),
    m_pPhotonPlugin(NULL),
    m_pElectronPlugin(NULL),
    m_pMuonPlugin(NULL)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

ParticleId::~ParticleId()
{
    for (ParticleIdPluginMap::const_iterator iter = m_particleIdPluginMap.begin(), iterEnd = m_particleIdPluginMap.end(); iter != iterEnd; ++iter)
    {
        delete iter->second;
    }

    m_particleIdPluginMap.clear();
    m_pEmShowerPlugin = NULL;
    m_pPhotonPlugin = NULL;
    m_pElectronPlugin = NULL;
    m_pMuonPlugin = NULL;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleId::RegisterPlugin(const std::string &name, ParticleIdPlugin *pParticleIdPlugin)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pParticleIdPlugin->RegisterDetails(m_pPandora, name));

    if (!m_particleIdPluginMap.insert(ParticleIdPluginMap::value_type(name, pParticleIdPlugin)).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleId::InitializePlugins(const TiXmlHandle *const pXmlHandle)
{
    for (ParticleIdPluginMap::const_iterator iter = m_particleIdPluginMap.begin(), iterEnd = m_particleIdPluginMap.end(); iter != iterEnd; ++iter)
    {
        TiXmlElement *pXmlElement(pXmlHandle->FirstChild(iter->first).Element());

        if (NULL != pXmlElement)
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, iter->second->ReadSettings(TiXmlHandle(pXmlElement)));

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, iter->second->Initialize());
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
    if (NULL != pParticleIdPlugin)
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

} // namespace pandora
