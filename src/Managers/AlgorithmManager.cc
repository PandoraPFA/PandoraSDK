/**
 *  @file   PandoraSDK/src/Managers/AlgorithmManager.cc
 * 
 *  @brief  Implementation of the algorithm manager class.
 * 
 *  $Log: $
 */

#include "Pandora/Algorithm.h"
#include "Pandora/AlgorithmTool.h"

#include "Persistency/EventReadingAlgorithm.h"
#include "Persistency/EventWritingAlgorithm.h"

#include "Managers/AlgorithmManager.h"

#include "Xml/tinyxml.h"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace pandora
{

AlgorithmManager::AlgorithmManager(const Pandora *const pPandora) :
    m_pPandora(pPandora)
{
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, RegisterAlgorithmFactory("EventReading", new EventReadingAlgorithm::Factory));
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, RegisterAlgorithmFactory("EventWriting", new EventWritingAlgorithm::Factory));
}

//------------------------------------------------------------------------------------------------------------------------------------------

AlgorithmManager::~AlgorithmManager()
{
    for (AlgorithmMap::value_type &mapEntry : m_algorithmMap)
        delete mapEntry.second;

    for (AlgorithmFactoryMap::value_type &mapEntry : m_algorithmFactoryMap)
        delete mapEntry.second;

    for (AlgorithmTool *const pAlgorithmTool : m_algorithmToolVector)
        delete pAlgorithmTool;

    for (AlgorithmToolFactoryMap::value_type &mapEntry : m_algorithmToolFactoryMap)
        delete mapEntry.second;

    m_algorithmMap.clear();
    m_algorithmFactoryMap.clear();
    m_algorithmToolVector.clear();
    m_algorithmToolFactoryMap.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode AlgorithmManager::RegisterAlgorithmFactory(const std::string &algorithmType, AlgorithmFactory *const pAlgorithmFactory)
{
    if (!m_algorithmFactoryMap.insert(AlgorithmFactoryMap::value_type(algorithmType, pAlgorithmFactory)).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode AlgorithmManager::RegisterAlgorithmToolFactory(const std::string &algorithmToolType, AlgorithmToolFactory *const pAlgorithmToolFactory)
{
    if (!m_algorithmToolFactoryMap.insert(AlgorithmToolFactoryMap::value_type(algorithmToolType, pAlgorithmToolFactory)).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode AlgorithmManager::InitializeAlgorithms(const TiXmlHandle *const pXmlHandle)
{
    for (TiXmlElement *pXmlElement = pXmlHandle->FirstChild("algorithm").Element(); nullptr != pXmlElement;
        pXmlElement = pXmlElement->NextSiblingElement("algorithm"))
    {
        std::string algorithmName;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CreateAlgorithm(pXmlElement, algorithmName));
        m_pandoraAlgorithms.push_back(algorithmName);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode AlgorithmManager::CreateAlgorithm(TiXmlElement *const pXmlElement, std::string &algorithmName)
{
    const char *const pAttribute(pXmlElement->Attribute("type"));

    if (!pAttribute)
    {
        std::cout << "Algorithm encountered in xml without defined type." << std::endl;
        return STATUS_CODE_NOT_FOUND;
    }

    std::string xmlInstanceLabel;
    const StatusCode instanceStatusCode(FindSpecificAlgorithmInstance(pXmlElement, algorithmName, xmlInstanceLabel));

    if (STATUS_CODE_NOT_FOUND != instanceStatusCode)
        return instanceStatusCode;

    AlgorithmFactoryMap::const_iterator iter = m_algorithmFactoryMap.find(pXmlElement->Attribute("type"));

    if (m_algorithmFactoryMap.end() == iter)
    {
        std::cout << "Algorithm type '" << pXmlElement->Attribute("type") << "' not registered with pandora algorithm manager." << std::endl;
        return STATUS_CODE_NOT_FOUND;
    }

    Algorithm *const pLocalAlgorithm = iter->second->CreateAlgorithm();

    try
    {
        std::stringstream ss;
        ss << std::setw(4) << std::setfill('0') << (1 + m_algorithmMap.size());
        algorithmName = "Alg" + ss.str();

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pLocalAlgorithm->RegisterDetails(m_pPandora, iter->first, algorithmName));

        if (!m_algorithmMap.insert(AlgorithmMap::value_type(algorithmName, pLocalAlgorithm)).second)
            throw StatusCodeException(STATUS_CODE_FAILURE);

        if (!xmlInstanceLabel.empty() && !m_specificAlgorithmInstanceMap.insert(SpecificAlgorithmInstanceMap::value_type(xmlInstanceLabel, algorithmName)).second)
        {
            std::cout << "Failure attempting to reuse algorithm with xml instance label " << xmlInstanceLabel << std::endl;
            throw StatusCodeException(STATUS_CODE_FAILURE);
        }

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pLocalAlgorithm->ReadSettings(TiXmlHandle(pXmlElement)));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pLocalAlgorithm->Initialize());
    }
    catch (StatusCodeException &statusCodeException)
    {
        m_algorithmMap.erase(algorithmName);
        delete pLocalAlgorithm;
        throw statusCodeException;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode AlgorithmManager::CreateAlgorithmTool(TiXmlElement *const pXmlElement, AlgorithmTool *&pAlgorithmTool)
{
    const char *const pAttribute(pXmlElement->Attribute("type"));

    if (!pAttribute)
    {
        std::cout << "Algorithm tool encountered in xml without defined type." << std::endl;
        return STATUS_CODE_NOT_FOUND;
    }

    AlgorithmToolFactoryMap::const_iterator iter = m_algorithmToolFactoryMap.find(pXmlElement->Attribute("type"));

    if (m_algorithmToolFactoryMap.end() == iter)
    {
        std::cout << "Algorithm tool type '" << pXmlElement->Attribute("type") << "' not registered with pandora algorithm manager." << std::endl;
        return STATUS_CODE_NOT_FOUND;
    }

    AlgorithmTool *const pLocalAlgorithmTool = iter->second->CreateAlgorithmTool();

    try
    {
        std::stringstream ss;
        ss << std::setw(4) << std::setfill('0') << (1 + m_algorithmToolVector.size());
        const std::string toolInstanceName("Tool" + ss.str());

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pLocalAlgorithmTool->RegisterDetails(m_pPandora, iter->first, toolInstanceName));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pLocalAlgorithmTool->ReadSettings(TiXmlHandle(pXmlElement)));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pLocalAlgorithmTool->Initialize());

        m_algorithmToolVector.push_back(pLocalAlgorithmTool);
        pAlgorithmTool = pLocalAlgorithmTool;
    }
    catch (StatusCodeException &statusCodeException)
    {
        delete pLocalAlgorithmTool;
        throw statusCodeException;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode AlgorithmManager::FindSpecificAlgorithmInstance(TiXmlElement *const pXmlElement, std::string &algorithmName, std::string &xmlInstanceLabel) const
{
    try
    {
        const char *const pAttribute(pXmlElement->Attribute("instance"));

        if (!pAttribute)
            return STATUS_CODE_NOT_FOUND;

        xmlInstanceLabel = std::string(pAttribute);
        SpecificAlgorithmInstanceMap::const_iterator iter = m_specificAlgorithmInstanceMap.find(xmlInstanceLabel);

        if (m_specificAlgorithmInstanceMap.end() == iter)
            return STATUS_CODE_NOT_FOUND;

        algorithmName = iter->second;
        AlgorithmMap::const_iterator targetIter = m_algorithmMap.find(algorithmName);

        if ((m_algorithmMap.end() == targetIter) || (targetIter->second->GetType() != std::string(pXmlElement->Attribute("type"))))
            return STATUS_CODE_FAILURE;

        return STATUS_CODE_SUCCESS;
    }
    catch (...)
    {
        return STATUS_CODE_NOT_FOUND;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode AlgorithmManager::ResetForNextEvent()
{
    for (AlgorithmMap::value_type &mapEntry : m_algorithmMap)
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, mapEntry.second->Reset());

    for (AlgorithmTool *const pAlgorithmTool : m_algorithmToolVector)
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pAlgorithmTool->Reset());

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
