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
#include "Plugins/LArTransformationPlugin.h"
#include "Plugins/ParticleIdPlugin.h"
#include "Plugins/PseudoLayerPlugin.h"
#include "Plugins/ShowerProfilePlugin.h"

namespace pandora
{

PluginManager::PluginManager(const Pandora *const pPandora) :
    m_pBFieldPlugin(nullptr),
    m_pLArTransformationPlugin(nullptr),
    m_pPseudoLayerPlugin(nullptr),
    m_pShowerProfilePlugin(nullptr),
    m_pEnergyCorrections(nullptr),
    m_pParticleId(nullptr),
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
    delete m_pLArTransformationPlugin;
    delete m_pPseudoLayerPlugin;
    delete m_pShowerProfilePlugin;

    delete m_pEnergyCorrections;
    delete m_pParticleId;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PluginManager::HasBFieldPlugin() const
{
    return (nullptr != m_pBFieldPlugin);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PluginManager::HasLArTransformationPlugin() const
{
    return (nullptr != m_pLArTransformationPlugin);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PluginManager::HasPseudoLayerPlugin() const
{
    return (nullptr != m_pPseudoLayerPlugin);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PluginManager::HasShowerProfilePlugin() const
{
    return (nullptr != m_pShowerProfilePlugin);
}

//------------------------------------------------------------------------------------------------------------------------------------------

const BFieldPlugin *PluginManager::GetBFieldPlugin() const
{
    if (!m_pBFieldPlugin)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_pBFieldPlugin;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const LArTransformationPlugin *PluginManager::GetLArTransformationPlugin() const
{
    if (!m_pLArTransformationPlugin)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_pLArTransformationPlugin;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const PseudoLayerPlugin *PluginManager::GetPseudoLayerPlugin() const
{
    if (!m_pPseudoLayerPlugin)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_pPseudoLayerPlugin;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const ShowerProfilePlugin *PluginManager::GetShowerProfilePlugin() const
{
    if (!m_pShowerProfilePlugin)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_pShowerProfilePlugin;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const EnergyCorrections *PluginManager::GetEnergyCorrections() const
{
    if (!m_pEnergyCorrections)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_pEnergyCorrections;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const ParticleId *PluginManager::GetParticleId() const
{
    if (!m_pParticleId)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_pParticleId;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::SetBFieldPlugin(BFieldPlugin *const pBFieldPlugin)
{
    if (nullptr != m_pBFieldPlugin)
        return STATUS_CODE_ALREADY_INITIALIZED;

    m_pBFieldPlugin = pBFieldPlugin;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::SetLArTransformationPlugin(LArTransformationPlugin *const pLArTransformationPlugin)
{
    if (nullptr != m_pLArTransformationPlugin)
        return STATUS_CODE_ALREADY_INITIALIZED;

    m_pLArTransformationPlugin = pLArTransformationPlugin;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::SetPseudoLayerPlugin(PseudoLayerPlugin *const pPseudoLayerPlugin)
{
    if (nullptr != m_pPseudoLayerPlugin)
        return STATUS_CODE_ALREADY_INITIALIZED;

    m_pPseudoLayerPlugin = pPseudoLayerPlugin;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::SetShowerProfilePlugin(ShowerProfilePlugin *const pShowerProfilePlugin)
{
    if (nullptr != m_pShowerProfilePlugin)
        return STATUS_CODE_ALREADY_INITIALIZED;

    m_pShowerProfilePlugin = pShowerProfilePlugin;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::InitializePlugins(const TiXmlHandle *const pXmlHandle)
{
    if (nullptr != m_pBFieldPlugin)
    {
        RETURN_ON_ERROR(m_pBFieldPlugin->RegisterDetails(m_pPandora, "BFieldPlugin", "BFieldPlugin"));
        TiXmlElement *const pBFieldXmlElement(pXmlHandle->FirstChild("BFieldPlugin").Element());

        if (nullptr != pBFieldXmlElement)
            RETURN_ON_ERROR(m_pBFieldPlugin->ReadSettings(TiXmlHandle(pBFieldXmlElement)));

        RETURN_ON_ERROR(m_pBFieldPlugin->Initialize());
    }

    if (nullptr != m_pLArTransformationPlugin)
    {
        RETURN_ON_ERROR(m_pLArTransformationPlugin->RegisterDetails(m_pPandora, "LArTransformationPlugin", "LArTransformationPlugin"));
        TiXmlElement *const pLArTransformationXmlElement(pXmlHandle->FirstChild("LArTransformationPlugin").Element());

        if (nullptr != pLArTransformationXmlElement)
            RETURN_ON_ERROR(m_pLArTransformationPlugin->ReadSettings(TiXmlHandle(pLArTransformationXmlElement)));

        RETURN_ON_ERROR(m_pLArTransformationPlugin->Initialize());
    }

    if (nullptr != m_pPseudoLayerPlugin)
    {
        RETURN_ON_ERROR(m_pPseudoLayerPlugin->RegisterDetails(m_pPandora, "PseudoLayerPlugin", "PseudoLayerPlugin"));
        TiXmlElement *const pPseudoLayerXmlElement(pXmlHandle->FirstChild("PseudoLayerPlugin").Element());

        if (nullptr != pPseudoLayerXmlElement)
            RETURN_ON_ERROR(m_pPseudoLayerPlugin->ReadSettings(TiXmlHandle(pPseudoLayerXmlElement)));

        RETURN_ON_ERROR(m_pPseudoLayerPlugin->Initialize());
    }

    if (nullptr != m_pShowerProfilePlugin)
    {
        RETURN_ON_ERROR(m_pShowerProfilePlugin->RegisterDetails(m_pPandora, "ShowerProfilePlugin", "ShowerProfilePlugin"));
        TiXmlElement *const pShowerProfileXmlElement(pXmlHandle->FirstChild("ShowerProfilePlugin").Element());

        if (nullptr != pShowerProfileXmlElement)
            RETURN_ON_ERROR(m_pShowerProfilePlugin->ReadSettings(TiXmlHandle(pShowerProfileXmlElement)));

        RETURN_ON_ERROR(m_pShowerProfilePlugin->Initialize());
    }

    RETURN_ON_ERROR(m_pEnergyCorrections->InitializePlugins(pXmlHandle));
    RETURN_ON_ERROR(m_pParticleId->InitializePlugins(pXmlHandle));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::ResetForNextEvent()
{
    if (m_pBFieldPlugin)
        RETURN_ON_ERROR(m_pBFieldPlugin->Reset());

    if (m_pLArTransformationPlugin)
        RETURN_ON_ERROR(m_pLArTransformationPlugin->Reset());

    if (m_pPseudoLayerPlugin)
        RETURN_ON_ERROR(m_pPseudoLayerPlugin->Reset());

    if (m_pShowerProfilePlugin)
        RETURN_ON_ERROR(m_pShowerProfilePlugin->Reset());

    RETURN_ON_ERROR(m_pEnergyCorrections->ResetForNextEvent());
    RETURN_ON_ERROR(m_pParticleId->ResetForNextEvent());

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
