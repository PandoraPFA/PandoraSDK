/**
 *  @file   PandoraSDK/include/Pandora/ObjectCreation.cc
 * 
 *  @brief  Implementation for functionality related to object parameters
 * 
 *  $Log: $
 */

#include "Api/PandoraApiImpl.h"
#include "Api/PandoraContentApiImpl.h"

#include "Pandora/Algorithm.h"
#include "Pandora/Pandora.h"
#include "Pandora/ObjectFactory.h"
#include "Pandora/ObjectCreation.h"

namespace object_creation
{

using namespace pandora;

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename PARAMETERS, typename METADATA, typename OBJECT>
StatusCode ObjectCreationHelper<PARAMETERS, METADATA, OBJECT>::Create(const Pandora &pandora, const Parameters &parameters,
    const ObjectFactory<PARAMETERS, OBJECT> &factory)
{
    return pandora.GetPandoraApiImpl()->Create(parameters, factory);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename PARAMETERS, typename METADATA, typename OBJECT>
StatusCode ObjectCreationHelper<PARAMETERS, METADATA, OBJECT>::Create(const Algorithm &algorithm, const PARAMETERS &parameters,
    const OBJECT *&pObject, const ObjectFactory<PARAMETERS, OBJECT> &factory)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->Create(parameters, pObject, factory);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename PARAMETERS, typename METADATA, typename OBJECT>
StatusCode ObjectCreationHelper<PARAMETERS, METADATA, OBJECT>::AlterMetadata(const Algorithm &algorithm, const OBJECT *const pObject,
    const METADATA &metadata)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->AlterMetadata(pObject, metadata);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template class ObjectCreationHelper<object_creation::CaloHit::Parameters, object_creation::CaloHit::Metadata, object_creation::CaloHit::Object>;
template class ObjectCreationHelper<object_creation::MCParticle::Parameters, object_creation::ObjectMetadata, object_creation::MCParticle::Object>;
template class ObjectCreationHelper<object_creation::Track::Parameters, object_creation::ObjectMetadata, object_creation::Track::Object>;
template class ObjectCreationHelper<object_creation::Geometry::SubDetector::Parameters, object_creation::ObjectMetadata, object_creation::Geometry::SubDetector::Object>;
template class ObjectCreationHelper<object_creation::Geometry::LArTPC::Parameters, object_creation::ObjectMetadata, object_creation::Geometry::LArTPC::Object>;
template class ObjectCreationHelper<object_creation::Geometry::LineGap::Parameters, object_creation::ObjectMetadata, object_creation::Geometry::LineGap::Object>;
template class ObjectCreationHelper<object_creation::Geometry::BoxGap::Parameters, object_creation::ObjectMetadata, object_creation::Geometry::BoxGap::Object>;
template class ObjectCreationHelper<object_creation::Geometry::ConcentricGap::Parameters, object_creation::ObjectMetadata, object_creation::Geometry::ConcentricGap::Object>;
template class ObjectCreationHelper<object_creation::Cluster::Parameters, object_creation::Cluster::Metadata, object_creation::Cluster::Object>;
template class ObjectCreationHelper<object_creation::ParticleFlowObject::Parameters, object_creation::ParticleFlowObject::Metadata, object_creation::ParticleFlowObject::Object>;
template class ObjectCreationHelper<object_creation::Vertex::Parameters, object_creation::Vertex::Metadata, object_creation::Vertex::Object>;

} // namespace object_creation
