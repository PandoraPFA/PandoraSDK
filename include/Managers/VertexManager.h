/**
 *  @file   PandoraSDK/include/Managers/VertexManager.h
 * 
 *  @brief  Header file for the vertex manager class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_VERTEX_MANAGER_H
#define PANDORA_VERTEX_MANAGER_H 1

#include "Api/PandoraContentApi.h"

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
     *  @brief  Constructor
     * 
     *  @param  pPandora address of the associated pandora object
     */
    VertexManager(const Pandora *const pPandora);

    /**
     *  @brief  Destructor
     */
    ~VertexManager();

private:
    /**
     *  @brief  Create a vertex
     * 
     *  @param  parameters the vertex parameters
     *  @param  pVertex to receive the address of the vertex created
     */
    StatusCode CreateVertex(const PandoraContentApi::Vertex::Parameters &parameters, Vertex *&pVertex);

    friend class PandoraContentApiImpl;
    friend class PandoraImpl;
};

} // namespace pandora

#endif // #ifndef PANDORA_VERTEX_MANAGER_H
