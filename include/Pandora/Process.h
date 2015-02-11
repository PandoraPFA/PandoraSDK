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
     *  @brief  Destructor
     */
    virtual ~Process();

    /**
     *  @brief  Register i) the pandora instance that will run the process and ii) the process type
     * 
     *  @param  pPandora address of the pandora object that will run the process
     *  @param  type the process type
     */
    StatusCode RegisterDetails(const Pandora *const pPandora, const std::string &type);

    const Pandora          *m_pPandora;             ///< The pandora object that will run the process
    std::string             m_type;                 ///< The process type
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline Process::Process() :
    m_pPandora(NULL)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const std::string &Process::GetType() const
{
    return m_type;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const Pandora &Process::GetPandora() const
{
    if (NULL == m_pPandora)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return *m_pPandora;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode Process::Initialize()
{
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Process::~Process()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode Process::RegisterDetails(const Pandora *const pPandora, const std::string &type)
{
    if ((NULL == pPandora) || (type.empty()))
        return STATUS_CODE_FAILURE;

    m_pPandora = pPandora;
    m_type = type;

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

#endif // #ifndef PANDORA_ALGORITHM_TOOL_H
