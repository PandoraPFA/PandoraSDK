/**
 *  @file   PandoraSDK/src/Managers/VertexManager.cc
 * 
 *  @brief  Implementation of the vertex manager class.
 * 
 *  $Log: $
 */

#include "Managers/VertexManager.h"

#include "Objects/Vertex.h"

namespace pandora
{

VertexManager::VertexManager(const Pandora *const pPandora) :
    AlgorithmObjectManager<Vertex>(pPandora)
{
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateInitialLists());
}

//------------------------------------------------------------------------------------------------------------------------------------------

VertexManager::~VertexManager()
{
    (void) this->EraseAllContent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode VertexManager::CreateVertex(const PandoraContentApi::Vertex::Parameters &parameters, Vertex *&pVertex)
{
    pVertex = NULL;

    try
    {
        if (!m_canMakeNewObjects)
            throw StatusCodeException(STATUS_CODE_NOT_ALLOWED);

        NameToListMap::iterator iter = m_nameToListMap.find(m_currentListName);

        if (m_nameToListMap.end() == iter)
             throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

        pVertex = new Vertex(parameters);

        if (NULL == pVertex)
             throw StatusCodeException(STATUS_CODE_FAILURE);

        if (!iter->second->insert(pVertex).second)
             throw StatusCodeException(STATUS_CODE_FAILURE);

        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create vertex: " << statusCodeException.ToString() << std::endl;
        delete pVertex;
        pVertex = NULL;
        return statusCodeException.GetStatusCode();
    }
}

} // namespace pandora
