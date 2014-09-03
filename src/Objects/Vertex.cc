/**
 *  @file PandoraSDK/src/Objects/Vertex.cc
 * 
 *  @brief Implementation of the vertex class.
 * 
 *  $Log: $
 */

#include "Objects/Vertex.h"

#include <iostream>

namespace pandora
{

Vertex::Vertex(const PandoraContentApi::Vertex::Parameters &parameters) :
    m_position(parameters.m_position.Get()),
    m_isAvailable(true)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

Vertex::~Vertex()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &stream, const Vertex &vertex)
{
    stream  << " Vertex: " << std::endl << " position " << vertex.GetPosition() << std::endl;
    return stream;
}

} // namespace pandora
