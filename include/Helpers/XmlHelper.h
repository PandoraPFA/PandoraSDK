/**
 *  @file   PandoraSDK/include/Helpers/XmlHelper.h
 * 
 *  @brief  Header file for the xml helper class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_XML_HELPER_H
#define PANDORA_XML_HELPER_H 1

#include "Objects/CartesianVector.h"
#include "Objects/TrackState.h"

#include "Pandora/PandoraInternal.h"
#include "Pandora/StatusCodes.h"

#include "Xml/tinyxml.h"

namespace pandora
{

/**
 *  @brief  XmlHelper class
 */
class XmlHelper
{
public:
    /**
     *  @brief  Read a value from an xml element
     * 
     *  @param  xmlHandle the relevant xml handle
     *  @param  xmlElementName the name of the xml element to examine
     *  @param  t to receive the value
     */
    template <typename T>
    static StatusCode ReadValue(const TiXmlHandle &xmlHandle, const std::string &xmlElementName, T &t);

    /**
     *  @brief  Read a vector of values from a (space separated) list in an xml element
     * 
     *  @param  xmlHandle the relevant xml handle
     *  @param  xmlElementName the name of the xml element to examine
     *  @param  vector to receive the vector of values
     */
    template <typename T>
    static StatusCode ReadVectorOfValues(const TiXmlHandle &xmlHandle, const std::string &xmlElementName, std::vector<T> &vector);

    /**
     *  @brief  Read a two-dimensional array of values into a vector of vectors. Each row of values must be contained
     *          within <rowname></rowname> xml tags, whilst the values in the row must be space separated
     * 
     *  @param  xmlHandle the relevant xml handle
     *  @param  xmlElementName the name of the xml element to examine
     *  @param  rowName the row name
     *  @param  vector to receive the 2d vector of values
     */
    template <typename T>
    static StatusCode Read2DVectorOfValues(const TiXmlHandle &xmlHandle, const std::string &xmlElementName, const std::string &rowName,
        std::vector< std::vector<T> > &vector);

    /**
     *  @brief  Process an algorithm described in an xml element with a matching "description = ..." attribute
     * 
     *  @param  algorithm the parent algorithm calling this function
     *  @param  xmlHandle the relevant xml handle
     *  @param  description the description attribute of the algorithm xml element
     *  @param  algorithmName to receive the name of the algorithm instance
     */
    static StatusCode ProcessAlgorithm(const Algorithm &algorithm, const TiXmlHandle &xmlHandle, const std::string &description,
        std::string &algorithmName);

    /**
     *  @brief  Process a single algorithm described in an xml file (the first found by the xml handle)
     * 
     *  @param  algorithm the parent algorithm calling this function
     *  @param  xmlHandle the relevant xml handle
     *  @param  algorithmName to receive the name of the algorithm instance
     */
    static StatusCode ProcessFirstAlgorithm(const Algorithm &algorithm, const TiXmlHandle &xmlHandle, std::string &algorithmName);

    /**
     *  @brief  Process a list of daughter algorithms in an xml file
     * 
     *  @param  algorithm the parent algorithm calling this function
     *  @param  xmlHandle the relevant xml handle
     *  @param  listName the name of the algorithm list
     *  @param  algorithmNames to receive the names of the algorithm instances
     */
    static StatusCode ProcessAlgorithmList(const Algorithm &algorithm, const TiXmlHandle &xmlHandle, const std::string &listName,
        StringVector &algorithmNames);

    /**
     *  @brief  Process an algorithm tool described in an xml element with a matching "description = ..." attribute
     * 
     *  @param  algorithm the parent algorithm calling this function
     *  @param  xmlHandle the relevant xml handle
     *  @param  description the description attribute of the algorithm tool xml element
     *  @param  pAlgorithmTool to receive the address of the algorithm tool instance
     */
    static StatusCode ProcessAlgorithmTool(const Algorithm &algorithm, const TiXmlHandle &xmlHandle, const std::string &description,
        AlgorithmTool *&pAlgorithmTool);

    /**
     *  @brief  Process a single algorithm tool described in an xml file (the first found by the xml handle)
     * 
     *  @param  algorithm the parent algorithm calling this function
     *  @param  xmlHandle the relevant xml handle
     *  @param  pAlgorithmTool to receive the address of the algorithm tool instance
     */
    static StatusCode ProcessFirstAlgorithmTool(const Algorithm &algorithm, const TiXmlHandle &xmlHandle, AlgorithmTool *&pAlgorithmTool);

    /**
     *  @brief  Process a list of algorithms tools in an xml file
     * 
     *  @param  algorithm the parent algorithm calling this function
     *  @param  xmlHandle the relevant xml handle
     *  @param  listName the name of the algorithm tool list
     *  @param  algorithmToolList to receive the list of addresses of the algorithm tool instances
     */
    static StatusCode ProcessAlgorithmToolList(const Algorithm &algorithm, const TiXmlHandle &xmlHandle, const std::string &listName,
        AlgorithmToolList &algorithmToolList);

    /**
     *  @brief  Tokenize a string
     * 
     *  @param  inputString the input string
     *  @param  tokens to receive the resulting tokens
     *  @param  delimiter the specified delimeter
     */
    static void TokenizeString(const std::string &inputString, StringVector &tokens, const std::string &delimiter = " ");
};

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline StatusCode XmlHelper::ReadValue(const TiXmlHandle &xmlHandle, const std::string &xmlElementName, T &t)
{
    const TiXmlElement *const pXmlElement = xmlHandle.FirstChild(xmlElementName).Element();

    if (NULL == pXmlElement)
        return STATUS_CODE_NOT_FOUND;

    if (!StringToType(pXmlElement->GetText(), t))
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

template <>
inline StatusCode XmlHelper::ReadValue<CartesianVector>(const TiXmlHandle &xmlHandle, const std::string &xmlElementName, CartesianVector &t)
{
    const TiXmlElement *const pXmlElement = xmlHandle.FirstChild(xmlElementName).Element();

    if (NULL == pXmlElement)
        return STATUS_CODE_NOT_FOUND;

    StringVector tokens;
    TokenizeString(pXmlElement->GetText(), tokens);

    if (tokens.size() != 3)
        return STATUS_CODE_FAILURE;

    float x(0.f), y(0.f), z(0.f);

    if (!StringToType(tokens[0], x) || !StringToType(tokens[1], y) || !StringToType(tokens[2], z))
        return STATUS_CODE_FAILURE;

    t = CartesianVector(x, y, z);

    return STATUS_CODE_SUCCESS;
}

template <>
inline StatusCode XmlHelper::ReadValue<TrackState>(const TiXmlHandle &xmlHandle, const std::string &xmlElementName, TrackState &t)
{
    const TiXmlElement *const pXmlElement = xmlHandle.FirstChild(xmlElementName).Element();

    if (NULL == pXmlElement)
        return STATUS_CODE_NOT_FOUND;

    StringVector tokens;
    TokenizeString(pXmlElement->GetText(), tokens);

    if (tokens.size() != 6)
        return STATUS_CODE_FAILURE;

    float x(0.f), y(0.f), z(0.f), px(0.f), py(0.f), pz(0.f);

    if (!StringToType(tokens[0], x) || !StringToType(tokens[1], y) || !StringToType(tokens[2], z) ||
        !StringToType(tokens[3], px) || !StringToType(tokens[4], py) || !StringToType(tokens[5], pz))
    {
        return STATUS_CODE_FAILURE;
    }

    t = TrackState(x, y, z, px, py, pz);

    return STATUS_CODE_SUCCESS;
}

template <>
inline StatusCode XmlHelper::ReadValue<bool>(const TiXmlHandle &xmlHandle, const std::string &xmlElementName, bool &t)
{
    const TiXmlElement *const pXmlElement = xmlHandle.FirstChild(xmlElementName).Element();

    if (NULL == pXmlElement)
        return STATUS_CODE_NOT_FOUND;

    const std::string xmlElementString = pXmlElement->GetText();

    if ((xmlElementString == "1") || (xmlElementString == "true"))
    {
        t = true;
    }
    else if ((xmlElementString == "0") || (xmlElementString == "false"))
    {
        t = false;
    }
    else
    {
        return STATUS_CODE_FAILURE;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline StatusCode XmlHelper::ReadVectorOfValues(const TiXmlHandle &xmlHandle, const std::string &xmlElementName, std::vector<T> &vector)
{
    const TiXmlElement *const pXmlElement = xmlHandle.FirstChild(xmlElementName).Element();

    if (NULL == pXmlElement)
        return STATUS_CODE_NOT_FOUND;

    StringVector tokens;
    TokenizeString(pXmlElement->GetText(), tokens);

    for (StringVector::const_iterator iter = tokens.begin(), iterEnd = tokens.end(); iter != iterEnd; ++iter)
    {
        T t;

        if (!StringToType(*iter, t))
            return STATUS_CODE_FAILURE;

        vector.push_back(t);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline StatusCode XmlHelper::Read2DVectorOfValues(const TiXmlHandle &xmlHandle, const std::string &xmlElementName, const std::string &rowName,
    std::vector< std::vector<T> > &vector)
{
    TiXmlElement *const pXmlElement = xmlHandle.FirstChild(xmlElementName).Element();

    if (NULL == pXmlElement)
        return STATUS_CODE_NOT_FOUND;

    TiXmlElement *pXmlRowElement = TiXmlHandle(pXmlElement).FirstChild(rowName).Element();

    if (NULL == pXmlRowElement)
        return STATUS_CODE_NOT_FOUND;

    for ( ; NULL != pXmlRowElement; pXmlRowElement = pXmlRowElement->NextSiblingElement(rowName))
    {
        std::vector<T> rowVector;

        StringVector tokens;
        TokenizeString(pXmlRowElement->GetText(), tokens);

        for (StringVector::const_iterator iter = tokens.begin(), iterEnd = tokens.end(); iter != iterEnd; ++iter)
        {
            T t;

            if (!StringToType(*iter, t))
                return STATUS_CODE_FAILURE;

            rowVector.push_back(t);
        }

        vector.push_back(rowVector);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode XmlHelper::ProcessFirstAlgorithm(const Algorithm &algorithm, const TiXmlHandle &xmlHandle, std::string &algorithmName)
{
    const std::string emptyDescription;
    return XmlHelper::ProcessAlgorithm(algorithm, xmlHandle, emptyDescription, algorithmName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode XmlHelper::ProcessFirstAlgorithmTool(const Algorithm &algorithm, const TiXmlHandle &xmlHandle, AlgorithmTool *&pAlgorithmTool)
{
    const std::string emptyDescription;
    return XmlHelper::ProcessAlgorithmTool(algorithm, xmlHandle, emptyDescription, pAlgorithmTool);
}

} // namespace pandora

#endif // #ifndef PANDORA_XML_HELPER_H
