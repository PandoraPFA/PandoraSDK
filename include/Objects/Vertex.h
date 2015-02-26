/**
 *  @file   PandoraSDK/include/Objects/Vertex.h
 * 
 *  @brief  Header file for the vertex class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_VERTEX_H
#define PANDORA_VERTEX_H 1

#include "Api/PandoraContentApi.h"

#include "Pandora/PandoraInternal.h"

namespace pandora
{

template<typename T> class AlgorithmObjectManager;

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

private:
    /**
     *  @brief  Constructor
     * 
     *  @param  parameters the vertex parameters
     */
    Vertex(const PandoraContentApi::Vertex::Parameters &parameters);

    /**
     *  @brief  Destructor
     */
    ~Vertex();

    /**
     *  @brief  Alter the metadata information stored in a vertex
     * 
     *  @param  metaData the metadata (only populated metadata fields will be propagated to the object)
     */
    StatusCode AlterMetadata(const PandoraContentApi::Vertex::Metadata &metadata);

    /**
     *  @brief  Set availability of vertex to be added to a particle flow object
     * 
     *  @param  isAvailable the vertex availability
     */
    void SetAvailability(bool isAvailable);

    CartesianVector         m_position;                 ///< The vertex position
    VertexLabel             m_vertexLabel;              ///< The vertex label (interaction, start, end, etc.)
    VertexType              m_vertexType;               ///< The vertex type (3d, view u, v, w, etc.)
    bool                    m_isAvailable;              ///< Whether the track is available to be added to a particle flow object

    friend class AlgorithmObjectManager<Vertex>;
    friend class VertexManager;
};

/**
 *  @brief  Operator to dump vertex properties to an ostream
 *
 *  @param  stream the target ostream
 *  @param  vertex the vertex
 */
std::ostream &operator<<(std::ostream &stream, const Vertex &vertex);

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &Vertex::GetPosition() const
{
    return m_position;
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
