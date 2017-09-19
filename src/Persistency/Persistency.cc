/**
 *  @file   PandoraSDK/src/Persistency/Persistency.cc
 * 
 *  @brief  Implementation of the persistency class
 * 
 *  $Log: $
 */

#include "Pandora/Pandora.h"

#include "Persistency/Persistency.h"

namespace pandora
{

Persistency::Persistency(const pandora::Pandora &pandora, const std::string &fileName) :
    m_pPandora(&pandora),
    m_fileName(fileName),
    m_fileType(UNKNOWN_FILE_TYPE),
    m_containerId(UNKNOWN_CONTAINER),
    m_pCaloHitFactory(new PandoraObjectFactory<object_creation::CaloHit::Parameters, object_creation::CaloHit::Object>()),
    m_pTrackFactory(new PandoraObjectFactory<object_creation::Track::Parameters, object_creation::Track::Object>()),
    m_pMCParticleFactory(new PandoraObjectFactory<object_creation::MCParticle::Parameters, object_creation::MCParticle::Object>()),
    m_pSubDetectorFactory(new PandoraObjectFactory<object_creation::Geometry::SubDetector::Parameters, object_creation::Geometry::SubDetector::Object>()),
    m_pLArTPCFactory(new PandoraObjectFactory<object_creation::Geometry::LArTPC::Parameters, object_creation::Geometry::LArTPC::Object>()),
    m_pLineGapFactory(new PandoraObjectFactory<object_creation::Geometry::LineGap::Parameters, object_creation::Geometry::LineGap::Object>()),
    m_pBoxGapFactory(new PandoraObjectFactory<object_creation::Geometry::BoxGap::Parameters, object_creation::Geometry::BoxGap::Object>()),
    m_pConcentricGapFactory(new PandoraObjectFactory<object_creation::Geometry::ConcentricGap::Parameters, object_creation::Geometry::ConcentricGap::Object>())
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

Persistency::~Persistency()
{
    delete m_pCaloHitFactory;
    delete m_pTrackFactory;
    delete m_pMCParticleFactory;
    delete m_pSubDetectorFactory;
    delete m_pLArTPCFactory;
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
void Persistency::ReplaceCurrentFactory(ObjectFactory<object_creation::CaloHit::Parameters, object_creation::CaloHit::Object> *const pFactory)
{
    delete m_pCaloHitFactory;
    m_pCaloHitFactory = pFactory;
}

template<>
void Persistency::ReplaceCurrentFactory(ObjectFactory<object_creation::Track::Parameters, object_creation::Track::Object> *const pFactory)
{
    delete m_pTrackFactory;
    m_pTrackFactory = pFactory;
}

template<>
void Persistency::ReplaceCurrentFactory(ObjectFactory<object_creation::MCParticle::Parameters, object_creation::MCParticle::Object> *const pFactory)
{
    delete m_pMCParticleFactory;
    m_pMCParticleFactory = pFactory;
}

template<>
void Persistency::ReplaceCurrentFactory(ObjectFactory<object_creation::Geometry::SubDetector::Parameters, object_creation::Geometry::SubDetector::Object> *const pFactory)
{
    delete m_pSubDetectorFactory;
    m_pSubDetectorFactory = pFactory;
}

template<>
void Persistency::ReplaceCurrentFactory(ObjectFactory<object_creation::Geometry::LArTPC::Parameters, object_creation::Geometry::LArTPC::Object> *const pFactory)
{
    delete m_pLArTPCFactory;
    m_pLArTPCFactory = pFactory;
}

template<>
void Persistency::ReplaceCurrentFactory(ObjectFactory<object_creation::Geometry::LineGap::Parameters, object_creation::Geometry::LineGap::Object> *const pFactory)
{
    delete m_pLineGapFactory;
    m_pLineGapFactory = pFactory;
}

template<>
void Persistency::ReplaceCurrentFactory(ObjectFactory<object_creation::Geometry::BoxGap::Parameters, object_creation::Geometry::BoxGap::Object> *const pFactory)
{
    delete m_pBoxGapFactory;
    m_pBoxGapFactory = pFactory;
}

template<>
void Persistency::ReplaceCurrentFactory(ObjectFactory<object_creation::Geometry::ConcentricGap::Parameters, object_creation::Geometry::ConcentricGap::Object> *const pFactory)
{
    delete m_pConcentricGapFactory;
    m_pConcentricGapFactory = pFactory;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template StatusCode Persistency::SetFactory(ObjectFactory<object_creation::CaloHit::Parameters, object_creation::CaloHit::Object> *const);
template StatusCode Persistency::SetFactory(ObjectFactory<object_creation::Track::Parameters, object_creation::Track::Object> *const);
template StatusCode Persistency::SetFactory(ObjectFactory<object_creation::MCParticle::Parameters, object_creation::MCParticle::Object> *const);
template StatusCode Persistency::SetFactory(ObjectFactory<object_creation::Geometry::SubDetector::Parameters, object_creation::Geometry::SubDetector::Object> *const);
template StatusCode Persistency::SetFactory(ObjectFactory<object_creation::Geometry::LArTPC::Parameters, object_creation::Geometry::LArTPC::Object> *const);
template StatusCode Persistency::SetFactory(ObjectFactory<object_creation::Geometry::LineGap::Parameters, object_creation::Geometry::LineGap::Object> *const);
template StatusCode Persistency::SetFactory(ObjectFactory<object_creation::Geometry::BoxGap::Parameters, object_creation::Geometry::BoxGap::Object> *const);
template StatusCode Persistency::SetFactory(ObjectFactory<object_creation::Geometry::ConcentricGap::Parameters, object_creation::Geometry::ConcentricGap::Object> *const);

} // namespace pandora
