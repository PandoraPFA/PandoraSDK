/**
 *  @file   PandoraSDK/src/Managers/PluginManager.cc
 * 
 *  @brief  Implementation of the pandora plugin manager class.
 * 
 *  $Log: $
 */

#include "Helpers/XmlHelper.h"

#include "Managers/PluginManager.h"

#include "Plugins/BFieldPlugin.h"
#include "Plugins/EnergyCorrectionsPlugin.h"
#include "Plugins/ParticleIdPlugin.h"
#include "Plugins/PseudoLayerPlugin.h"
#include "Plugins/ShowerProfilePlugin.h"

namespace pandora
{

PluginManager::PluginManager(const Pandora *const pPandora) :
    m_pBFieldPlugin(NULL),
    m_pPseudoLayerPlugin(NULL),
    m_pShowerProfilePlugin(NULL),
    m_pEnergyCorrections(NULL),
    m_pParticleId(NULL),
    m_pPandora(pPandora)
{
    try
    {
        m_pEnergyCorrections = new EnergyCorrections(m_pPandora);
        m_pParticleId = new ParticleId(m_pPandora);
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create pandora plugin manager instance " << statusCodeException.ToString() << std::endl;
        delete this;
        throw statusCodeException;
    }
    catch (...)
    {
        std::cout << "Failed to create pandora plugin manager instance " << std::endl;
        delete this;
        throw;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

PluginManager::~PluginManager()
{
    delete m_pBFieldPlugin;
    delete m_pPseudoLayerPlugin;
    delete m_pShowerProfilePlugin;

    delete m_pEnergyCorrections;
    delete m_pParticleId;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const BFieldPlugin *PluginManager::GetBFieldPlugin() const
{
    if (NULL == m_pBFieldPlugin)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_pBFieldPlugin;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const PseudoLayerPlugin *PluginManager::GetPseudoLayerPlugin() const
{
    if (NULL == m_pPseudoLayerPlugin)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_pPseudoLayerPlugin;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const ShowerProfilePlugin *PluginManager::GetShowerProfilePlugin() const
{
    if (NULL == m_pShowerProfilePlugin)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_pShowerProfilePlugin;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const EnergyCorrections *PluginManager::GetEnergyCorrections() const
{
    if (NULL == m_pEnergyCorrections)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_pEnergyCorrections;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const ParticleId *PluginManager::GetParticleId() const
{
    if (NULL == m_pParticleId)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_pParticleId;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::SetBFieldPlugin(BFieldPlugin *pBFieldPlugin)
{
    if (NULL != m_pBFieldPlugin)
        return STATUS_CODE_ALREADY_INITIALIZED;

    m_pBFieldPlugin = pBFieldPlugin;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::SetPseudoLayerPlugin(PseudoLayerPlugin *pPseudoLayerPlugin)
{
    if (NULL != m_pPseudoLayerPlugin)
        return STATUS_CODE_ALREADY_INITIALIZED;

    m_pPseudoLayerPlugin = pPseudoLayerPlugin;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::SetShowerProfilePlugin(ShowerProfilePlugin *pShowerProfilePlugin)
{
    if (NULL != m_pShowerProfilePlugin)
        return STATUS_CODE_ALREADY_INITIALIZED;

    m_pShowerProfilePlugin = pShowerProfilePlugin;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::InitializePlugins(const TiXmlHandle *const pXmlHandle)
{
    if (NULL != m_pBFieldPlugin)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pBFieldPlugin->RegisterDetails(m_pPandora, "BFieldPlugin"));
        TiXmlElement *pBFieldXmlElement(pXmlHandle->FirstChild("BFieldPlugin").Element());

        if (NULL != pBFieldXmlElement)
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pBFieldPlugin->ReadSettings(TiXmlHandle(pBFieldXmlElement)));

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pBFieldPlugin->Initialize());
    }

    if (NULL != m_pPseudoLayerPlugin)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPseudoLayerPlugin->RegisterDetails(m_pPandora, "PseudoLayerPlugin"));
        TiXmlElement *pPseudoLayerXmlElement(pXmlHandle->FirstChild("PseudoLayerPlugin").Element());

        if (NULL != pPseudoLayerXmlElement)
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPseudoLayerPlugin->ReadSettings(TiXmlHandle(pPseudoLayerXmlElement)));

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPseudoLayerPlugin->Initialize());
    }

    if (NULL != m_pShowerProfilePlugin)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pShowerProfilePlugin->RegisterDetails(m_pPandora, "ShowerProfilePlugin"));
        TiXmlElement *pShowerProfileXmlElement(pXmlHandle->FirstChild("ShowerProfilePlugin").Element());

        if (NULL != pShowerProfileXmlElement)
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pShowerProfilePlugin->ReadSettings(TiXmlHandle(pShowerProfileXmlElement)));

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pShowerProfilePlugin->Initialize());
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pEnergyCorrections->InitializePlugins(pXmlHandle));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pParticleId->InitializePlugins(pXmlHandle));

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
