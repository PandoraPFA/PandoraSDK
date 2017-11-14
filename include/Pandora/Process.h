/**
 *  @file   PandoraSDK/include/Pandora/Process.h
 * 
 *  @brief  Header file for the process class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_PROCESS_H
#define PANDORA_PROCESS_H 1

#include "Pandora/StatusCodes.h"

#include <string>

namespace pandora
{

class Pandora;
class TiXmlHandle;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Process class.
 */
class Process
{
public:
    /**
     *  @brief  Default constructor
     */
    Process();

    /**
     *  @brief  Get the type
     * 
     *  @return The type
     */
    const std::string &GetType() const;

    /**
     *  @brief  Get the instance name
     * 
     *  @return The instance name
     */
    const std::string &GetInstanceName() const;

    /**
     *  @brief  Get the associated pandora instance
     * 
     *  @return the associated pandora instance
     */
    const Pandora &GetPandora() const;

protected:
    /**
     *  @brief  Read the algorithm settings
     * 
     *  @param  xmlHandle the relevant xml handle
     */
    virtual StatusCode ReadSettings(const TiXmlHandle xmlHandle) = 0;

    /**
     *  @brief  Perform any operations that must occur after reading settings, but before running the process
     */
    virtual StatusCode Initialize();

    /**
     *  @brief  Perform any operations when pandora is reset, typically at the end of each event
     */
    virtual StatusCode Reset();

    /**
     *  @brief  Destructor
     */
    virtual ~Process();

    /**
     *  @brief  Register i) the pandora instance that will run the process and ii) the process type
     * 
     *  @param  pPandora address of the pandora object that will run the process
     *  @param  type the process type
     *  @param  instanceName the process instance name
     */
    StatusCode RegisterDetails(const Pandora *const pPandora, const std::string &type, const std::string &instanceName);

    const Pandora          *m_pPandora;             ///< The pandora object that will run the process
    std::string             m_type;                 ///< The process type
    std::string             m_instanceName;         ///< The process instance name
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline Process::Process() :
    m_pPandora(nullptr)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const std::string &Process::GetType() const
{
    return m_type;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const std::string &Process::GetInstanceName() const
{
    return m_instanceName;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const Pandora &Process::GetPandora() const
{
    if (!m_pPandora)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return *m_pPandora;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode Process::Initialize()
{
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode Process::Reset()
{
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Process::~Process()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode Process::RegisterDetails(const Pandora *const pPandora, const std::string &type, const std::string &instanceName)
{
    if (!pPandora || type.empty() || instanceName.empty())
        return STATUS_CODE_INVALID_PARAMETER;

    if (m_pPandora || !m_type.empty() || !m_instanceName.empty())
        return STATUS_CODE_ALREADY_PRESENT;

    m_pPandora = pPandora;
    m_type = type;
    m_instanceName = instanceName;

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

#endif // #ifndef PANDORA_ALGORITHM_TOOL_H
