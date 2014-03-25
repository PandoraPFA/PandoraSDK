/**
 *  @file   PandoraPFANew/Framework/include/Managers/VertexManager.h
 * 
 *  @brief  Header file for the vertex manager class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_VERTEX_MANAGER_H
#define PANDORA_VERTEX_MANAGER_H 1

#include "Managers/AlgorithmObjectManager.h"

#include "Pandora/PandoraInternal.h"

namespace pandora
{

/**
 *    @brief VertexManager class
 */
class VertexManager : public AlgorithmObjectManager<Vertex>
{
public:
    /**
     *  @brief  Default constructor
     */
    VertexManager();

    /**
     *  @brief  Destructor
     */
    ~VertexManager();

private:
    /**
     *  @brief  Create a vertex
     * 
     *  @param  vertexPosition the vertex position
     *  @param  pVertex to receive the address of the vertex created
     */
    StatusCode CreateVertex(const CartesianVector &vertexPosition, Vertex *&pVertex);

    friend class PandoraContentApiImpl;
    friend class PandoraImpl;
};

} // namespace pandora

#endif // #ifndef PANDORA_VERTEX_MANAGER_H
