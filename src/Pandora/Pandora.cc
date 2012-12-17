/**
 *  @file   PandoraPFANew/Framework/src/Pandora/Pandora.cc
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
#include "Managers/MCManager.h"
#include "Managers/ParticleFlowObjectManager.h"
#include "Managers/PluginManager.h"
#include "Managers/TrackManager.h"

#include "Pandora/Pandora.h"
#include "Pandora/PandoraImpl.h"
#include "Pandora/PandoraSettings.h"

#include "Xml/tinyxml.h"

namespace pandora
{

Pandora::Pandora() :
    m_pAlgorithmManager(NULL),
    m_pCaloHitManager(NULL),
    m_pClusterManager(NULL),
    m_pMCManager(NULL),
    m_pPfoManager(NULL),
    m_pPluginManager(NULL),
    m_pTrackManager(NULL),
    m_pPandoraApiImpl(NULL),
    m_pPandoraContentApiImpl(NULL),
    m_pPandoraImpl(NULL)
{
    try
    {
        m_pAlgorithmManager = new AlgorithmManager(this);
        m_pCaloHitManager = new CaloHitManager;
        m_pClusterManager = new ClusterManager;
        m_pMCManager = new MCManager;
        m_pPfoManager = new ParticleFlowObjectManager;
        m_pPluginManager = new PluginManager;
        m_pTrackManager = new TrackManager;
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
    delete m_pMCManager;
    delete m_pPfoManager;
    delete m_pPluginManager;
    delete m_pTrackManager;
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

StatusCode Pandora::PrepareMCParticles()
{
    return m_pPandoraImpl->PrepareMCParticles();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Pandora::ProcessEvent()
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareEvent());

    // Loop over algorithms
    const StringVector *const pPandoraAlgorithms = m_pAlgorithmManager->GetPandoraAlgorithms();

    for (StringVector::const_iterator iter = pPandoraAlgorithms->begin(), iterEnd = pPandoraAlgorithms->end(); iter != iterEnd; ++iter)
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandoraImpl->RunAlgorithm(*iter));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Pandora::ResetEvent()
{
    return m_pPandoraImpl->ResetEvent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Pandora::RegisterResetFunction(ResetFunction *pResetFunction)
{
    return m_pPandoraImpl->RegisterResetFunction(pResetFunction);
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
        const TiXmlHandle xmlHandle = TiXmlHandle(xmlDocumentHandle.FirstChildElement().Element());

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraSettings::Initialize(&xmlHandle));
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

} // namespace pandora
