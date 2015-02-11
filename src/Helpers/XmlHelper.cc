/**
 *  @file   PandoraSDK/src/Helpers/XmlHelper.cc
 * 
 *  @brief  Implementation of the xml helper class.
 * 
 *  $Log: $
 */

#include "Api/PandoraContentApi.h"

#include "Helpers/XmlHelper.h"

namespace pandora
{

StatusCode XmlHelper::ProcessAlgorithm(const Algorithm &algorithm, const TiXmlHandle &xmlHandle, const std::string &description,
    std::string &algorithmName)
{
    if ("algorithm" != xmlHandle.ToNode()->ValueStr())
        return STATUS_CODE_NOT_ALLOWED;

    for (TiXmlElement *pXmlElement = xmlHandle.FirstChild("algorithm").Element(); NULL != pXmlElement;
        pXmlElement = pXmlElement->NextSiblingElement("algorithm"))
    {
        if (description.empty())
            return PandoraContentApi::CreateDaughterAlgorithm(algorithm, pXmlElement, algorithmName);

        try
        {
            const char *const pAttribute(pXmlElement->Attribute("description"));

            if (NULL == pAttribute)
                return STATUS_CODE_NOT_FOUND;

            if (description == std::string(pAttribute))
                return PandoraContentApi::CreateDaughterAlgorithm(algorithm, pXmlElement, algorithmName);
        }
        catch (...)
        {
        }
    }

    return STATUS_CODE_NOT_FOUND;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlHelper::ProcessAlgorithmList(const Algorithm &algorithm, const TiXmlHandle &xmlHandle, const std::string &listName,
    StringVector &algorithmNames)
{
    if ("algorithm" != xmlHandle.ToNode()->ValueStr())
        return STATUS_CODE_NOT_ALLOWED;

    const TiXmlHandle algorithmListHandle = TiXmlHandle(xmlHandle.FirstChild(listName).Element());

    for (TiXmlElement *pXmlElement = algorithmListHandle.FirstChild("algorithm").Element(); NULL != pXmlElement;
        pXmlElement = pXmlElement->NextSiblingElement("algorithm"))
    {
        std::string algorithmName;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::CreateDaughterAlgorithm(algorithm, pXmlElement, algorithmName));
        algorithmNames.push_back(algorithmName);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlHelper::ProcessAlgorithmTool(const Algorithm &algorithm, const TiXmlHandle &xmlHandle, const std::string &description,
    AlgorithmTool *&pAlgorithmTool)
{
    if ("algorithm" != xmlHandle.ToNode()->ValueStr())
        return STATUS_CODE_NOT_ALLOWED;

    for (TiXmlElement *pXmlElement = xmlHandle.FirstChild("tool").Element(); NULL != pXmlElement;
        pXmlElement = pXmlElement->NextSiblingElement("tool"))
    {
        if (description.empty())
            return PandoraContentApi::CreateAlgorithmTool(algorithm, pXmlElement, pAlgorithmTool);

        try
        {
            const char *const pAttribute(pXmlElement->Attribute("description"));

            if (NULL == pAttribute)
                return STATUS_CODE_NOT_FOUND;

            if (description == std::string(pAttribute))
                return PandoraContentApi::CreateAlgorithmTool(algorithm, pXmlElement, pAlgorithmTool);
        }
        catch (...)
        {
        }
    }

    return STATUS_CODE_NOT_FOUND;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlHelper::ProcessAlgorithmToolList(const Algorithm &algorithm, const TiXmlHandle &xmlHandle, const std::string &listName,
    AlgorithmToolList &algorithmToolList)
{
    if ("algorithm" != xmlHandle.ToNode()->ValueStr())
        return STATUS_CODE_NOT_ALLOWED;

    const TiXmlHandle algorithmListHandle = TiXmlHandle(xmlHandle.FirstChild(listName).Element());

    for (TiXmlElement *pXmlElement = algorithmListHandle.FirstChild("tool").Element(); NULL != pXmlElement;
        pXmlElement = pXmlElement->NextSiblingElement("tool"))
    {
        AlgorithmTool *pAlgorithmTool = NULL;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::CreateAlgorithmTool(algorithm, pXmlElement, pAlgorithmTool));
        algorithmToolList.push_back(pAlgorithmTool);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void XmlHelper::TokenizeString(const std::string &inputString, StringVector &tokens, const std::string &delimiter)
{
    std::string::size_type lastPos = inputString.find_first_not_of(delimiter, 0);
    std::string::size_type pos     = inputString.find_first_of(delimiter, lastPos);

    while ((std::string::npos != pos) || (std::string::npos != lastPos))
    {
        tokens.push_back(inputString.substr(lastPos, pos - lastPos));
        lastPos = inputString.find_first_not_of(delimiter, pos);
        pos = inputString.find_first_of(delimiter, lastPos);
    }
}

} // namespace pandora
