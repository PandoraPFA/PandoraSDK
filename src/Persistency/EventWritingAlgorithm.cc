/**
 *  @file   PandoraPFANew/Framework/src/Persistency/EventWritingAlgorithm.cc
 * 
 *  @brief  Implementation of the event writing algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "Persistency/EventWritingAlgorithm.h"
#include "Persistency/FileWriter.h"

using namespace pandora;

EventWritingAlgorithm::EventWritingAlgorithm() :
    m_pEventFileWriter(NULL)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

EventWritingAlgorithm::~EventWritingAlgorithm()
{
    delete m_pEventFileWriter;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EventWritingAlgorithm::Initialize()
{
    if (m_shouldWriteGeometry)
    {
        if (!GeometryHelper::IsInitialized())
            return STATUS_CODE_NOT_INITIALIZED;

        const FileMode fileMode(m_shouldOverwriteGeometryFile ? OVERWRITE : APPEND);
        FileWriter geometryFileWriter(*(this->GetPandora()), m_geometryFileName, fileMode);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, geometryFileWriter.WriteGeometry());
    }

    if (m_shouldWriteEvents)
    {
        const FileMode fileMode(m_shouldOverwriteEventFile ? OVERWRITE : APPEND);
        m_pEventFileWriter = new FileWriter(*(this->GetPandora()), m_eventFileName, fileMode);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EventWritingAlgorithm::Run()
{
    if ((NULL != m_pEventFileWriter) && m_shouldWriteEvents)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pEventFileWriter->WriteEvent(m_shouldWriteMCRelationships,
            m_shouldWriteTrackRelationships));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EventWritingAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_shouldWriteGeometry = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldWriteGeometry", m_shouldWriteGeometry));

    if (m_shouldWriteGeometry)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
            "GeometryFileName", m_geometryFileName));
    }

    m_shouldWriteEvents = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldWriteEvents", m_shouldWriteEvents));

    if (m_shouldWriteEvents)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
            "EventFileName", m_eventFileName));
    }

    m_shouldOverwriteEventFile = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldOverwriteEventFile", m_shouldOverwriteEventFile));

    m_shouldOverwriteGeometryFile = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldOverwriteGeometryFile", m_shouldOverwriteGeometryFile));

    m_shouldWriteMCRelationships = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldWriteMCRelationships", m_shouldWriteMCRelationships));

    m_shouldWriteTrackRelationships = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldWriteTrackRelationships", m_shouldWriteTrackRelationships));

    return STATUS_CODE_SUCCESS;
}
