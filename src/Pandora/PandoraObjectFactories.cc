/**
 *  @file   PandoraSDK/src/Pandora/PandoraObjectFactories.cc
 * 
 *  @brief  Implementation of the pandora object factory classes
 * 
 *  $Log: $
 */

#include "Pandora/ObjectCreation.h"
#include "Pandora/PandoraObjectFactories.h"

#include "Geometry/DetectorGap.h"
#include "Geometry/LArTPC.h"
#include "Geometry/SubDetector.h"

#include "Objects/CaloHit.h"
#include "Objects/Cluster.h"
#include "Objects/MCParticle.h"
#include "Objects/ParticleFlowObject.h"
#include "Objects/Track.h"
#include "Objects/Vertex.h"

namespace pandora
{

template <typename PARAMETERS, typename OBJECT>
typename PandoraObjectFactory<PARAMETERS, OBJECT>::Parameters *PandoraObjectFactory<PARAMETERS, OBJECT>::NewParameters() const
{
    return (new Parameters);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename PARAMETERS, typename OBJECT>
StatusCode PandoraObjectFactory<PARAMETERS, OBJECT>::Read(Parameters &/*parameters*/, FileReader &/*fileReader*/) const
{
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename PARAMETERS, typename OBJECT>
StatusCode PandoraObjectFactory<PARAMETERS, OBJECT>::Write(const Object *const /*pObject*/, FileWriter &/*fileWriter*/) const
{
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename PARAMETERS, typename OBJECT>
StatusCode PandoraObjectFactory<PARAMETERS, OBJECT>::Create(const PARAMETERS &parameters, const OBJECT *&pObject) const
{
    pObject = nullptr;

    try
    {
        pObject = new OBJECT(parameters);
    }
    catch (StatusCodeException &statusCodeException)
    {
        delete pObject;
        pObject = nullptr;

        std::cout << "StatusCodeException caught while instantiating pandora object :" << statusCodeException.ToString() << std::endl;
        return statusCodeException.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template class PandoraObjectFactory<object_creation::CaloHit::Parameters, object_creation::CaloHit::Object>;
template class PandoraObjectFactory<object_creation::CaloHitFragment::Parameters, object_creation::CaloHitFragment::Object>;
template class PandoraObjectFactory<object_creation::Track::Parameters, object_creation::Track::Object>;
template class PandoraObjectFactory<object_creation::MCParticle::Parameters, object_creation::MCParticle::Object>;
template class PandoraObjectFactory<object_creation::Cluster::Parameters, object_creation::Cluster::Object>;
template class PandoraObjectFactory<object_creation::ParticleFlowObject::Parameters, object_creation::ParticleFlowObject::Object>;
template class PandoraObjectFactory<object_creation::Vertex::Parameters, object_creation::Vertex::Object>;
template class PandoraObjectFactory<object_creation::Geometry::SubDetector::Parameters, object_creation::Geometry::SubDetector::Object>;
template class PandoraObjectFactory<object_creation::Geometry::LArTPC::Parameters, object_creation::Geometry::LArTPC::Object>;
template class PandoraObjectFactory<object_creation::Geometry::LineGap::Parameters, object_creation::Geometry::LineGap::Object>;
template class PandoraObjectFactory<object_creation::Geometry::BoxGap::Parameters, object_creation::Geometry::BoxGap::Object>;
template class PandoraObjectFactory<object_creation::Geometry::ConcentricGap::Parameters, object_creation::Geometry::ConcentricGap::Object>;

} // namespace pandora
