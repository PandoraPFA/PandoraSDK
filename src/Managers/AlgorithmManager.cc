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
    for (AlgorithmMap::iterator iter = m_algorithmMap.begin(), iterEnd = m_algorithmMap.end(); iter != iterEnd; ++iter)
        delete iter->second;

    for (AlgorithmFactoryMap::iterator iter = m_algorithmFactoryMap.begin(), iterEnd = m_algorithmFactoryMap.end(); iter != iterEnd; ++iter)
        delete iter->second;

    for (AlgorithmToolList::iterator iter = m_algorithmToolList.begin(), iterEnd = m_algorithmToolList.end(); iter != iterEnd; ++iter)
        delete *iter;

    for (AlgorithmToolFactoryMap::iterator iter = m_algorithmToolFactoryMap.begin(), iterEnd = m_algorithmToolFactoryMap.end(); iter != iterEnd; ++iter)
        delete iter->second;

    m_algorithmMap.clear();
    m_algorithmFactoryMap.clear();
    m_algorithmToolList.clear();
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
    for (TiXmlElement *pXmlElement = pXmlHandle->FirstChild("algorithm").Element(); NULL != pXmlElement;
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
    const char *pAttribute(pXmlElement->Attribute("type"));

    if (NULL == pAttribute)
    {
        std::cout << "Algorithm encountered in xml without defined type." << std::endl;
        return STATUS_CODE_NOT_FOUND;
    }

    std::string instanceLabel;
    const StatusCode instanceStatusCode = FindSpecificAlgorithmInstance(pXmlElement, algorithmName, instanceLabel);

    if (STATUS_CODE_NOT_FOUND != instanceStatusCode)
        return instanceStatusCode;

    AlgorithmFactoryMap::const_iterator iter = m_algorithmFactoryMap.find(pXmlElement->Attribute("type"));

    if (m_algorithmFactoryMap.end() == iter)
    {
        std::cout << "Algorithm type '" << pXmlElement->Attribute("type") << "' not registered with pandora algorithm manager." << std::endl;
        return STATUS_CODE_NOT_FOUND;
    }

    Algorithm *pAlgorithm = NULL;
    pAlgorithm = iter->second->CreateAlgorithm();

    if (NULL == pAlgorithm)
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pAlgorithm->RegisterDetails(m_pPandora, iter->first));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pAlgorithm->ReadSettings(TiXmlHandle(pXmlElement)));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pAlgorithm->Initialize());

    algorithmName = TypeToString(pAlgorithm);

    if (!m_algorithmMap.insert(AlgorithmMap::value_type(algorithmName, pAlgorithm)).second)
        return STATUS_CODE_FAILURE;

    if (!instanceLabel.empty() && !m_specificAlgorithmInstanceMap.insert(SpecificAlgorithmInstanceMap::value_type(instanceLabel, algorithmName)).second)
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode AlgorithmManager::CreateAlgorithmTool(TiXmlElement *const pXmlElement, AlgorithmTool *&pAlgorithmTool)
{
    const char *pAttribute(pXmlElement->Attribute("type"));

    if (NULL == pAttribute)
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

    pAlgorithmTool = iter->second->CreateAlgorithmTool();

    if (NULL == pAlgorithmTool)
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pAlgorithmTool->RegisterDetails(m_pPandora, iter->first));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pAlgorithmTool->ReadSettings(TiXmlHandle(pXmlElement)));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pAlgorithmTool->Initialize());
    m_algorithmToolList.push_back(pAlgorithmTool);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode AlgorithmManager::FindSpecificAlgorithmInstance(TiXmlElement *const pXmlElement, std::string &algorithmName, std::string &instanceLabel) const
{
    try
    {
        const char *pAttribute(pXmlElement->Attribute("instance"));

        if (NULL == pAttribute)
            return STATUS_CODE_NOT_FOUND;

        instanceLabel = std::string(pAttribute);
        SpecificAlgorithmInstanceMap::const_iterator iter = m_specificAlgorithmInstanceMap.find(instanceLabel);

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

} // namespace pandora
