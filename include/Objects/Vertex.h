/**
 *  @file   PandoraSDK/include/Objects/Vertex.h
 * 
 *  @brief  Header file for the vertex class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_VERTEX_H
#define PANDORA_VERTEX_H 1

#include "Pandora/ObjectCreation.h"
#include "Pandora/StatusCodes.h"

namespace pandora
{

template<typename T> class AlgorithmObjectManager;
template<typename T, typename S> class PandoraObjectFactory;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Vertex class
 */
class Vertex
{
public:
    /**
     *  @brief  Get the vertex position
     * 
     *  @return the vertex position
     */
    const CartesianVector &GetPosition() const;

    /**
     *  @brief  For LArTPC usage, the x-coordinate shift associated with a drift time t0 shift, units mm
     * 
     *  @return the x-coordinate shift
     */
    float GetX0() const;

    /**
     *  @brief  Get the vertex label
     * 
     *  @return the vertex label
     */
    VertexLabel GetVertexLabel() const;

    /**
     *  @brief  Get the vertex type
     * 
     *  @return the vertex type
     */
    VertexType GetVertexType() const;

    /**
     *  @brief  Whether the vertex is available to be added to a particle flow object
     * 
     *  @return boolean
     */
    bool IsAvailable() const;

protected:
    /**
     *  @brief  Constructor
     * 
     *  @param  parameters the vertex parameters
     */
    Vertex(const object_creation::Vertex::Parameters &parameters);

    /**
     *  @brief  Destructor
     */
    virtual ~Vertex();

    /**
     *  @brief  Alter the metadata information stored in a vertex
     * 
     *  @param  metaData the metadata (only populated metadata fields will be propagated to the object)
     */
    StatusCode AlterMetadata(const object_creation::Vertex::Metadata &metadata);

    /**
     *  @brief  Set availability of vertex to be added to a particle flow object
     * 
     *  @param  isAvailable the vertex availability
     */
    void SetAvailability(bool isAvailable);

    CartesianVector         m_position;                 ///< The vertex position
    float                   m_x0;                       ///< For LArTPC usage, the x-coordinate shift associated with a drift time t0 shift, units mm
    VertexLabel             m_vertexLabel;              ///< The vertex label (interaction, start, end, etc.)
    VertexType              m_vertexType;               ///< The vertex type (3d, view u, v, w, etc.)
    bool                    m_isAvailable;              ///< Whether the track is available to be added to a particle flow object

    friend class VertexManager;
    friend class AlgorithmObjectManager<Vertex>;
    friend class PandoraObjectFactory<object_creation::Vertex::Parameters, object_creation::Vertex::Object>;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &Vertex::GetPosition() const
{
    return m_position;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Vertex::GetX0() const
{
    return m_x0;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline VertexLabel Vertex::GetVertexLabel() const
{
    return m_vertexLabel;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline VertexType Vertex::GetVertexType() const
{
    return m_vertexType;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool Vertex::IsAvailable() const
{
    return m_isAvailable;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void Vertex::SetAvailability(bool isAvailable)
{
    m_isAvailable = isAvailable;
}

} // namespace pandora

#endif // #ifndef PANDORA_VERTEX_H
