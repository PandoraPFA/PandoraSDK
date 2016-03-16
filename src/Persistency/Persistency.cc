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
    m_pLineGapFactory(new PandoraObjectFactory<PandoraApi::Geometry::LineGap::Parameters, LineGap>()),
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
    delete m_pLineGapFactory;
    delete m_pBoxGapFactory;
    delete m_pConcentricGapFactory;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename PARAMETERS, typename OBJECT>
StatusCode Persistency::SetFactory(ObjectFactory<PARAMETERS, OBJECT> *const pFactory)
{
    if (!pFactory)
        return STATUS_CODE_INVALID_PARAMETER;

    this->ReplaceCurrentFactory(pFactory);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
void Persistency::ReplaceCurrentFactory(ObjectFactory<PandoraApi::CaloHit::Parameters, CaloHit> *const pFactory)
{
    delete m_pCaloHitFactory;
    m_pCaloHitFactory = pFactory;
}

template<>
void Persistency::ReplaceCurrentFactory(ObjectFactory<PandoraApi::Track::Parameters, Track> *const pFactory)
{
    delete m_pTrackFactory;
    m_pTrackFactory = pFactory;
}

template<>
void Persistency::ReplaceCurrentFactory(ObjectFactory<PandoraApi::MCParticle::Parameters, MCParticle> *const pFactory)
{
    delete m_pMCParticleFactory;
    m_pMCParticleFactory = pFactory;
}

template<>
void Persistency::ReplaceCurrentFactory(ObjectFactory<PandoraApi::Geometry::SubDetector::Parameters, SubDetector> *const pFactory)
{
    delete m_pSubDetectorFactory;
    m_pSubDetectorFactory = pFactory;
}

template<>
void Persistency::ReplaceCurrentFactory(ObjectFactory<PandoraApi::Geometry::LineGap::Parameters, LineGap> *const pFactory)
{
    delete m_pLineGapFactory;
    m_pLineGapFactory = pFactory;
}

template<>
void Persistency::ReplaceCurrentFactory(ObjectFactory<PandoraApi::Geometry::BoxGap::Parameters, BoxGap> *const pFactory)
{
    delete m_pBoxGapFactory;
    m_pBoxGapFactory = pFactory;
}

template<>
void Persistency::ReplaceCurrentFactory(ObjectFactory<PandoraApi::Geometry::ConcentricGap::Parameters, ConcentricGap> *const pFactory)
{
    delete m_pConcentricGapFactory;
    m_pConcentricGapFactory = pFactory;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template StatusCode Persistency::SetFactory(ObjectFactory<PandoraApi::CaloHit::Parameters, CaloHit> *const);
template StatusCode Persistency::SetFactory(ObjectFactory<PandoraApi::Track::Parameters, Track> *const);
template StatusCode Persistency::SetFactory(ObjectFactory<PandoraApi::MCParticle::Parameters, MCParticle> *const);
template StatusCode Persistency::SetFactory(ObjectFactory<PandoraApi::Geometry::SubDetector::Parameters, SubDetector> *const);
template StatusCode Persistency::SetFactory(ObjectFactory<PandoraApi::Geometry::LineGap::Parameters, LineGap> *const);
template StatusCode Persistency::SetFactory(ObjectFactory<PandoraApi::Geometry::BoxGap::Parameters, BoxGap> *const);
template StatusCode Persistency::SetFactory(ObjectFactory<PandoraApi::Geometry::ConcentricGap::Parameters, ConcentricGap> *const);

} // namespace pandora
