/**
 *  @file   PandoraSDK/src/Managers/VertexManager.cc
 * 
 *  @brief  Implementation of the vertex manager class.
 * 
 *  $Log: $
 */

#include "Managers/VertexManager.h"

#include "Objects/Vertex.h"

#include "Pandora/ObjectFactory.h"

#include <algorithm>

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

StatusCode VertexManager::Create(const PandoraContentApi::Vertex::Parameters &parameters, const Vertex *&pVertex,
    const ObjectFactory<PandoraContentApi::Vertex::Parameters, Vertex> &factory)
{
    pVertex = NULL;

    try
    {
        if (!m_canMakeNewObjects)
            throw StatusCodeException(STATUS_CODE_NOT_ALLOWED);

        NameToListMap::iterator iter = m_nameToListMap.find(m_currentListName);

        if (m_nameToListMap.end() == iter)
             throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, factory.Create(parameters, pVertex));

        if (NULL == pVertex)
             throw StatusCodeException(STATUS_CODE_FAILURE);

        if (iter->second->end() != std::find(iter->second->begin(), iter->second->end(), pVertex))
            throw StatusCodeException(STATUS_CODE_ALREADY_PRESENT);

        iter->second->push_back(pVertex);
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

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode VertexManager::AlterMetadata(const Vertex *const pVertex, const PandoraContentApi::Vertex::Metadata &metadata) const
{
    return this->Modifiable(pVertex)->AlterMetadata(metadata);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
bool VertexManager::IsAvailable(const Vertex *const pVertex) const
{
    return pVertex->IsAvailable();
}

template <>
bool VertexManager::IsAvailable(const VertexList *const pVertexList) const
{
    bool isAvailable(true);

    for (VertexList::const_iterator iter = pVertexList->begin(), iterEnd = pVertexList->end(); iter != iterEnd; ++iter)
        isAvailable &= this->IsAvailable(*iter);

    return isAvailable;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
void VertexManager::SetAvailability(const Vertex *const pVertex, bool isAvailable) const
{
    this->Modifiable(pVertex)->SetAvailability(isAvailable);
}

template <>
void VertexManager::SetAvailability(const VertexList *const pVertexList, bool isAvailable) const
{
    for (VertexList::const_iterator iter = pVertexList->begin(), iterEnd = pVertexList->end(); iter != iterEnd; ++iter)
        this->SetAvailability(*iter, isAvailable);
}

} // namespace pandora
