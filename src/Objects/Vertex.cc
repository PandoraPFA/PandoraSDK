/**
 *  @file PandoraPFANew/Framework/src/Objects/Vertex.cc
 * 
 *  @brief Implementation of the vertex class.
 * 
 *  $Log: $
 */

#include "Objects/Vertex.h"

#include <iostream>

namespace pandora
{

Vertex::Vertex(const CartesianVector &vertexPosition) :
    m_position(vertexPosition),
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
