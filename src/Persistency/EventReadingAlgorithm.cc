/**
 *  @file   PandoraPFANew/Framework/src/Persistency/EventReadingAlgorithm.cc
 * 
 *  @brief  Implementation of the event reading algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "Persistency/EventReadingAlgorithm.h"
#include "Persistency/FileReader.h"

using namespace pandora;

EventReadingAlgorithm::EventReadingAlgorithm() :
    m_pEventFileReader(NULL)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

EventReadingAlgorithm::~EventReadingAlgorithm()
{
    delete m_pEventFileReader;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EventReadingAlgorithm::Initialize()
{
    if (m_shouldReadGeometry)
    {
        if (GeometryHelper::IsInitialized())
            return STATUS_CODE_ALREADY_INITIALIZED;

        pandora::FileReader fileReader(*(this->GetPandora()), m_geometryFileName);
        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, fileReader.ReadGeometry());
    }

    if (m_shouldReadEvents)
    {
        m_pEventFileReader = new FileReader(*(this->GetPandora()), m_eventFileName);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pEventFileReader->GoToEvent(m_skipToEvent));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EventReadingAlgorithm::Run()
{
    if ((NULL != m_pEventFileReader) && m_shouldReadEvents)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pEventFileReader->ReadEvent());
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RepeatEventPreparation(*this));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EventReadingAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_shouldReadGeometry = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldReadGeometry", m_shouldReadGeometry));

    if (m_shouldReadGeometry)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
            "GeometryFileName", m_geometryFileName));
    }

    m_shouldReadEvents = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldReadEvents", m_shouldReadEvents));

    if (m_shouldReadEvents)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
            "EventFileName", m_eventFileName));
    }

    m_skipToEvent = 0;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "SkipToEvent", m_skipToEvent));

    return STATUS_CODE_SUCCESS;
}
