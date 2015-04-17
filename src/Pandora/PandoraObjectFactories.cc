/**
 *  @file   PandoraSDK/src/Pandora/PandoraObjectFactories.cc
 * 
 *  @brief  Implementation of the pandora object factory classes
 * 
 *  $Log: $
 */

#include "Pandora/PandoraObjectFactories.h"

#include "Api/PandoraApi.h"
#include "Api/PandoraContentApi.h"

#include "Objects/CaloHit.h"
#include "Objects/Cluster.h"
#include "Objects/MCParticle.h"
#include "Objects/Track.h"
#include "Objects/Vertex.h"
#include "Objects/ParticleFlowObject.h"
#include "Objects/DetectorGap.h"
#include "Objects/SubDetector.h"

namespace pandora
{

template <typename PARAMETERS, typename OBJECT>
StatusCode PandoraObjectFactory<PARAMETERS, OBJECT>::Create(const PARAMETERS &parameters, const OBJECT *&pObject) const
{
    pObject = NULL;

    try
    {
        pObject = new OBJECT(parameters);
    }
    catch (StatusCodeException &statusCodeException)
    {
        delete pObject;
        pObject = NULL;

        std::cout << "StatusCodeException caught while instantiating pandora object :" << statusCodeException.ToString() << std::endl;
        return statusCodeException.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

template class PandoraObjectFactory<PandoraApi::CaloHit::Parameters, CaloHit>;
template class PandoraObjectFactory<PandoraContentApi::CaloHitFragment::Parameters, CaloHit>;
template class PandoraObjectFactory<PandoraApi::MCParticle::Parameters, MCParticle>;
template class PandoraObjectFactory<PandoraApi::Track::Parameters, Track>;
template class PandoraObjectFactory<PandoraContentApi::Cluster::Parameters, Cluster>;
template class PandoraObjectFactory<PandoraContentApi::Vertex::Parameters, Vertex>;
template class PandoraObjectFactory<PandoraContentApi::ParticleFlowObject::Parameters, ParticleFlowObject>;
template class PandoraObjectFactory<PandoraApi::Geometry::SubDetector::Parameters, SubDetector>;
template class PandoraObjectFactory<PandoraApi::Geometry::BoxGap::Parameters, BoxGap>;
template class PandoraObjectFactory<PandoraApi::Geometry::ConcentricGap::Parameters, ConcentricGap>;

} // namespace pandora
