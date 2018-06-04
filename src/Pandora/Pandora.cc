/**
 *  @file   PandoraSDK/src/Pandora/Pandora.cc
 * 
 *  @brief  Implementation of the pandora class.
 * 
 *  $Log: $
 */

#include "Api/PandoraApi.h"
#include "Api/PandoraApiImpl.h"
#include "Api/PandoraContentApi.h"
#include "Api/PandoraContentApiImpl.h"

#include "Managers/AlgorithmManager.h"
#include "Managers/CaloHitManager.h"
#include "Managers/ClusterManager.h"
#include "Managers/GeometryManager.h"
#include "Managers/MCManager.h"
#include "Managers/ParticleFlowObjectManager.h"
#include "Managers/PluginManager.h"
#include "Managers/TrackManager.h"
#include "Managers/VertexManager.h"

#include "Pandora/Pandora.h"
#include "Pandora/PandoraImpl.h"
#include "Pandora/PandoraSettings.h"

#include "Xml/tinyxml.h"

namespace pandora
{

Pandora::Pandora(const std::string &name) :
    m_pAlgorithmManager(nullptr),
    m_pCaloHitManager(nullptr),
    m_pClusterManager(nullptr),
    m_pGeometryManager(nullptr),
    m_pMCManager(nullptr),
    m_pPfoManager(nullptr),
    m_pPluginManager(nullptr),
    m_pTrackManager(nullptr),
    m_pVertexManager(nullptr),
    m_pPandoraSettings(nullptr),
    m_pPandoraApiImpl(nullptr),
    m_pPandoraContentApiImpl(nullptr),
    m_pPandoraImpl(nullptr),
    m_name(name)
{
    try
    {
        m_pAlgorithmManager = new AlgorithmManager(this);
        m_pCaloHitManager = new CaloHitManager(this);
        m_pClusterManager = new ClusterManager(this);
        m_pGeometryManager = new GeometryManager(this);
        m_pMCManager = new MCManager(this);
        m_pPfoManager = new ParticleFlowObjectManager(this);
        m_pPluginManager = new PluginManager(this);
        m_pTrackManager = new TrackManager(this);
        m_pVertexManager = new VertexManager(this);
        m_pPandoraSettings = new PandoraSettings(this);
        m_pPandoraApiImpl = new PandoraApiImpl(this);
        m_pPandoraContentApiImpl = new PandoraContentApiImpl(this);
        m_pPandoraImpl = new PandoraImpl(this);
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create pandora instance " << statusCodeException.ToString() << std::endl;
        delete this;
        throw statusCodeException;
    }
    catch (...)
    {
        std::cout << "Failed to create pandora instance " << std::endl;
        delete this;
        throw;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

Pandora::~Pandora()
{
    delete m_pAlgorithmManager;
    delete m_pCaloHitManager;
    delete m_pClusterManager;
    delete m_pGeometryManager;
    delete m_pMCManager;
    delete m_pPfoManager;
    delete m_pPluginManager;
    delete m_pTrackManager;
    delete m_pVertexManager;
    delete m_pPandoraSettings;
    delete m_pPandoraApiImpl;
    delete m_pPandoraContentApiImpl;
    delete m_pPandoraImpl;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Pandora::PrepareEvent()
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandoraImpl->PrepareMCParticles());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandoraImpl->PrepareCaloHits());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandoraImpl->PrepareTracks());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Pandora::ProcessEvent()
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareEvent());

    // Loop over algorithms
    const StringVector &pandoraAlgorithms(m_pPandoraImpl->GetPandoraAlgorithms());

    for (const std::string &algorithmName : pandoraAlgorithms)
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandoraImpl->RunAlgorithm(algorithmName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Pandora::ResetEvent()
{
    return m_pPandoraImpl->ResetEvent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Pandora::ReadSettings(const std::string &xmlFileName)
{
    try
    {
        TiXmlDocument xmlDocument(xmlFileName);

        if (!xmlDocument.LoadFile())
        {
            std::cout << "Pandora::ReadSettings - Invalid xml file." << std::endl;
            throw StatusCodeException(STATUS_CODE_FAILURE);
        }

        const TiXmlHandle xmlDocumentHandle(&xmlDocument);
        const TiXmlHandle xmlHandle(TiXmlHandle(xmlDocumentHandle.FirstChildElement().Element()));

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandoraImpl->InitializeSettings(&xmlHandle));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandoraImpl->InitializeAlgorithms(&xmlHandle));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandoraImpl->InitializePlugins(&xmlHandle));
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failure in reading pandora settings, " << statusCodeException.ToString() << std::endl;
        return STATUS_CODE_FAILURE;
    }
    catch (...)
    {
        std::cout << "Failure in reading pandora settings, unrecognized exception" << std::endl;
        return STATUS_CODE_FAILURE;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const PandoraApiImpl *Pandora::GetPandoraApiImpl() const
{
    return m_pPandoraApiImpl;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const PandoraContentApiImpl *Pandora::GetPandoraContentApiImpl() const
{
    return m_pPandoraContentApiImpl;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const PandoraSettings *Pandora::GetSettings() const
{
    return m_pPandoraSettings;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const GeometryManager *Pandora::GetGeometry() const
{
    return m_pGeometryManager;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const PluginManager *Pandora::GetPlugins() const
{
    return m_pPluginManager;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const std::string &Pandora::GetName() const
{
    return m_name;
}

} // namespace pandora
