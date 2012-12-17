/**
 *  @file   PandoraPFANew/Framework/src/Helpers/XmlHelper.cc
 * 
 *  @brief  Implementation of the xml helper class.
 * 
 *  $Log: $
 */

#include "Api/PandoraContentApi.h"

#include "Helpers/XmlHelper.h"

namespace pandora
{

StatusCode XmlHelper::ProcessAlgorithmList(const Algorithm &algorithm, const TiXmlHandle &xmlHandle, const std::string &listName,
    StringVector &algorithmNames)
{
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

StatusCode XmlHelper::ProcessFirstAlgorithm(const Algorithm &algorithm, const TiXmlHandle &xmlHandle, std::string &algorithmName)
{
    TiXmlElement *const pXmlElement = xmlHandle.FirstChild("algorithm").Element();

    if (NULL == pXmlElement)
        return STATUS_CODE_NOT_FOUND;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::CreateDaughterAlgorithm(algorithm, pXmlElement, algorithmName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlHelper::ProcessFirstAlgorithmInList(const Algorithm &algorithm, const TiXmlHandle &xmlHandle, const std::string &listName,
    std::string &algorithmName)
{
    const TiXmlHandle listXmlHandle = TiXmlHandle(xmlHandle.FirstChild(listName).Element());

    return ProcessFirstAlgorithm(algorithm, listXmlHandle, algorithmName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlHelper::ProcessAlgorithm(const Algorithm &algorithm, const TiXmlHandle &xmlHandle, const std::string &description,
    std::string &algorithmName)
{
    for (TiXmlElement *pXmlElement = xmlHandle.FirstChild("algorithm").Element(); NULL != pXmlElement;
        pXmlElement = pXmlElement->NextSiblingElement("algorithm"))
    {
        try
        {
            if (description == std::string(pXmlElement->Attribute("description")))
                return PandoraContentApi::CreateDaughterAlgorithm(algorithm, pXmlElement, algorithmName);
        }
        catch (...)
        {
        }
    }

    return STATUS_CODE_NOT_FOUND;
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
