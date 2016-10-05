/**
 *  @file   PandoraSDK/include/Managers/VertexManager.h
 * 
 *  @brief  Header file for the vertex manager class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_VERTEX_MANAGER_H
#define PANDORA_VERTEX_MANAGER_H 1

#include "Managers/AlgorithmObjectManager.h"

#include "Pandora/ObjectCreation.h"
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
     *  @param  factory the factory that performs the object allocation
     */
    StatusCode Create(const object_creation::Vertex::Parameters &parameters, const Vertex *&pVertex,
        const ObjectFactory<object_creation::Vertex::Parameters, object_creation::Vertex::Object> &factory);

    /**
     *  @brief  Alter the metadata information stored in a vertex
     * 
     *  @param  pVertex address of the vertex to modify
     *  @param  metaData the metadata (only populated metadata fields will be propagated to the object)
     */
    StatusCode AlterMetadata(const Vertex *const pVertex, const object_creation::Vertex::Metadata &metadata) const;

    /**
     *  @brief  Is a vertex, or a list of vertices, available to add to a particle flow object
     * 
     *  @param  pT address of the object or object list
     * 
     *  @return boolean
     */
    template <typename T>
    bool IsAvailable(const T *const pT) const;

    /**
     *  @brief  Set availability of a vertex, or a list of vertices, to be added to a particle flow object
     * 
     *  @param  pT the address of the object or object list
     *  @param  isAvailable the availability
     */
    template <typename T>
    void SetAvailability(const T *const pT, bool isAvailable) const;

    friend class PandoraContentApiImpl;
    friend class PandoraImpl;
};

} // namespace pandora

#endif // #ifndef PANDORA_VERTEX_MANAGER_H
