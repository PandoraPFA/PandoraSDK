/**
 *  @file PandoraSDK/src/Objects/Vertex.cc
 * 
 *  @brief Implementation of the vertex class.
 * 
 *  $Log: $
 */

#include "Objects/Vertex.h"

namespace pandora
{

Vertex::Vertex(const object_creation::Vertex::Parameters &parameters) :
    m_position(parameters.m_position.Get() + CartesianVector(parameters.m_x0.IsInitialized() ? parameters.m_x0.Get() : 0.f, 0.f, 0.f)),
    m_x0(parameters.m_x0.IsInitialized() ? parameters.m_x0.Get() : 0.f),
    m_vertexLabel(parameters.m_vertexLabel.Get()),
    m_vertexType(parameters.m_vertexType.Get()),
    m_isAvailable(true)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

Vertex::~Vertex()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Vertex::AlterMetadata(const object_creation::Vertex::Metadata &metadata)
{
    if (metadata.m_x0.IsInitialized())
    {
        const float oldX0(m_x0);
        m_x0 = metadata.m_x0.Get();
        m_position += CartesianVector(m_x0 - oldX0, 0.f, 0.f);
    }

    if (metadata.m_vertexLabel.IsInitialized())
        m_vertexLabel = metadata.m_vertexLabel.Get();

    if (metadata.m_vertexType.IsInitialized())
        m_vertexType = metadata.m_vertexType.Get();

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
