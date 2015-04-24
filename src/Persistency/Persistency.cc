/**
 *  @file   PandoraSDK/src/Persistency/Persistency.cc
 * 
 *  @brief  Implementation of the persistency class
 * 
 *  $Log: $
 */

#include "Pandora/Pandora.h"
#include "Pandora/PandoraObjectFactories.h"

#include "Persistency/Persistency.h"

namespace pandora
{

Persistency::Persistency(const pandora::Pandora &pandora, const std::string &fileName) :
    m_pPandora(&pandora),
    m_fileName(fileName),
    m_fileType(UNKNOWN_FILE_TYPE),
    m_containerId(UNKNOWN_CONTAINER),
    m_pCaloHitFactory(new PandoraObjectFactory<PandoraApi::CaloHit::Parameters, CaloHit>()),
    m_pTrackFactory(new PandoraObjectFactory<PandoraApi::Track::Parameters, Track>()),
    m_pMCParticleFactory(new PandoraObjectFactory<PandoraApi::MCParticle::Parameters, MCParticle>()),
    m_pSubDetectorFactory(new PandoraObjectFactory<PandoraApi::Geometry::SubDetector::Parameters, SubDetector>()),
    m_pBoxGapFactory(new PandoraObjectFactory<PandoraApi::Geometry::BoxGap::Parameters, BoxGap>()),
    m_pConcentricGapFactory(new PandoraObjectFactory<PandoraApi::Geometry::ConcentricGap::Parameters, ConcentricGap>())
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

Persistency::~Persistency()
{
    delete m_pCaloHitFactory;
    delete m_pTrackFactory;
    delete m_pMCParticleFactory;
    delete m_pSubDetectorFactory;
    delete m_pBoxGapFactory;
    delete m_pConcentricGapFactory;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Persistency::SetCaloHitFactory(ObjectFactory<PandoraApi::CaloHit::Parameters, CaloHit> *const pFactory)
{
    if (!pFactory)
        return STATUS_CODE_INVALID_PARAMETER;

    delete m_pCaloHitFactory;
    m_pCaloHitFactory = pFactory;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Persistency::SetTrackFactory(ObjectFactory<PandoraApi::Track::Parameters, Track> *const pFactory)
{
    if (!pFactory)
        return STATUS_CODE_INVALID_PARAMETER;

    delete m_pTrackFactory;
    m_pTrackFactory = pFactory;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Persistency::SetMCParticleFactory(ObjectFactory<PandoraApi::MCParticle::Parameters, MCParticle> *const pFactory)
{
    if (!pFactory)
        return STATUS_CODE_INVALID_PARAMETER;

    delete m_pMCParticleFactory;
    m_pMCParticleFactory = pFactory;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Persistency::SetSubDetectorFactory(ObjectFactory<PandoraApi::Geometry::SubDetector::Parameters, SubDetector> *const pFactory)
{
    if (!pFactory)
        return STATUS_CODE_INVALID_PARAMETER;

    delete m_pSubDetectorFactory;
    m_pSubDetectorFactory = pFactory;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Persistency::SetBoxGapFactory(ObjectFactory<PandoraApi::Geometry::BoxGap::Parameters, BoxGap> *const pFactory)
{
    if (!pFactory)
        return STATUS_CODE_INVALID_PARAMETER;

    delete m_pBoxGapFactory;
    m_pBoxGapFactory = pFactory;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Persistency::SetConcentricGapFactory(ObjectFactory<PandoraApi::Geometry::ConcentricGap::Parameters, ConcentricGap> *const pFactory)
{
    if (!pFactory)
        return STATUS_CODE_INVALID_PARAMETER;

    delete m_pConcentricGapFactory;
    m_pConcentricGapFactory = pFactory;
    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
