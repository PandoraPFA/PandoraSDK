/**
 *  @file   PandoraPFANew/Framework/include/Pandora/StatusCodes.h
 * 
 *  @brief  Header file defining status codes and relevant preprocessor macros
 * 
 *  $Log: $
 */
#ifndef STATUS_CODES_H
#define STATUS_CODES_H 1

#include <exception>
#include <string>

#if defined(__GNUC__) && defined(BACKTRACE)
    #include <cstdlib>
    #include <execinfo.h>
#endif

#define PANDORA_RETURN_RESULT_IF(StatusCode1, Operator, Command)                                        \
{                                                                                                       \
    const pandora::StatusCode statusCode(Command);                                                      \
    if (statusCode Operator StatusCode1)                                                                \
    {                                                                                                   \
        std::cout << #Command << " return " << StatusCodeToString(statusCode) << std::endl;             \
        std::cout << "    in function: " << __FUNCTION__ << std::endl;                                  \
        std::cout << "    in file:     " << __FILE__ << " line#: " << __LINE__ << std::endl;            \
        return statusCode;                                                                              \
    }                                                                                                   \
}

#define PANDORA_RETURN_RESULT_IF_AND_IF(StatusCode1, StatusCode2, Operator, Command)                    \
{                                                                                                       \
    const pandora::StatusCode statusCode(Command);                                                      \
    if ((statusCode Operator StatusCode1) && (statusCode Operator StatusCode2))                         \
    {                                                                                                   \
        std::cout << #Command << " return " << StatusCodeToString(statusCode) << std::endl;             \
        std::cout << "    in function: " << __FUNCTION__ << std::endl;                                  \
        std::cout << "    in file:     " << __FILE__ << " line#: " << __LINE__ << std::endl;            \
        return statusCode;                                                                              \
    }                                                                                                   \
}

#define PANDORA_THROW_RESULT_IF(StatusCode1, Operator, Command)                                         \
{                                                                                                       \
    const pandora::StatusCode statusCode(Command);                                                      \
    if (statusCode Operator StatusCode1)                                                                \
    {                                                                                                   \
        std::cout << #Command << " throw " << StatusCodeToString(statusCode) << std::endl;              \
        std::cout << "    in function: " << __FUNCTION__ << std::endl;                                  \
        std::cout << "    in file:     " << __FILE__ << " line#: " << __LINE__ << std::endl;            \
        throw pandora::StatusCodeException(statusCode);                                                 \
    }                                                                                                   \
}

#define PANDORA_THROW_RESULT_IF_AND_IF(StatusCode1, StatusCode2, Operator, Command)                     \
{                                                                                                       \
    const pandora::StatusCode statusCode(Command);                                                      \
    if ((statusCode Operator StatusCode1) && (statusCode Operator StatusCode2))                         \
    {                                                                                                   \
        std::cout << #Command << " throw " << StatusCodeToString(statusCode) << std::endl;              \
        std::cout << "    in function: " << __FUNCTION__ << std::endl;                                  \
        std::cout << "    in file:     " << __FILE__ << " line#: " << __LINE__ << std::endl;            \
        throw pandora::StatusCodeException(statusCode);                                                 \
    }                                                                                                   \
}

//------------------------------------------------------------------------------------------------------------------------------------------

namespace pandora
{

#define STATUS_CODE_TABLE(d)                                                                            \
    d(STATUS_CODE_SUCCESS,                  "STATUS_CODE_SUCCESS"                   )                   \
    d(STATUS_CODE_FAILURE,                  "STATUS_CODE_FAILURE"                   )                   \
    d(STATUS_CODE_NOT_FOUND,                "STATUS_CODE_NOT_FOUND"                 )                   \
    d(STATUS_CODE_NOT_INITIALIZED,          "STATUS_CODE_NOT_INITIALIZED"           )                   \
    d(STATUS_CODE_ALREADY_INITIALIZED,      "STATUS_CODE_ALREADY_INITIALIZED"       )                   \
    d(STATUS_CODE_ALREADY_PRESENT,          "STATUS_CODE_ALREADY_PRESENT"           )                   \
    d(STATUS_CODE_OUT_OF_RANGE,             "STATUS_CODE_OUT_OF_RANGE"              )                   \
    d(STATUS_CODE_NOT_ALLOWED,              "STATUS_CODE_NOT_ALLOWED"               )                   \
    d(STATUS_CODE_INVALID_PARAMETER,        "STATUS_CODE_INVALID_PARAMETER"         )                   \
    d(STATUS_CODE_UNCHANGED,                "STATUS_CODE_UNCHANGED"                 )

/**
 *  @brief  The status code enum entry macro
 */
#define GET_STATUS_CODE_ENUM_ENTRY(a, b)                                                                \
    a,

/**
 *  @brief  The status code name switch statement macro
 */
#define GET_STATUS_CODE_NAME_SWITCH(a, b)                                                               \
    case a : return b;

/**
 *  @brief  The StatusCode enum
 */
enum StatusCode
{
    STATUS_CODE_TABLE(GET_STATUS_CODE_ENUM_ENTRY)
    NUMBER_OF_STATUS_CODES
};

/**
 *  @brief  Get status code as a string
 * 
 *  @return The status code string
 */
std::string StatusCodeToString(const StatusCode statusCode);

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  StatusCodeException class
 */
class StatusCodeException : public std::exception
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  statusCode the status code
     */
    StatusCodeException(const StatusCode statusCode);

    /**
     *  @brief  Constructor
     */
    ~StatusCodeException() throw();

    /**
     *  @brief  Get status code
     * 
     *  @return the status code
     */
    StatusCode GetStatusCode() const;

    /**
     *  @brief  Get status code as a string
     * 
     *  @return The status code string
     */
    std::string ToString() const;

    /**
     *  @brief  Get back trace at point of exception construction (gcc only)
     * 
     *  @return The back trace
     */
    const std::string &GetBackTrace() const;

private:
    const StatusCode    m_statusCode;   ///< The status code
    std::string         m_backTrace;    ///< The back trace
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCodeException::StatusCodeException(const StatusCode statusCode) :
    m_statusCode(statusCode)
{
#if defined(__GNUC__) && defined(BACKTRACE)
    const size_t maxDepth = 100;
    void *stackAddresses[maxDepth];

    size_t stackDepth = backtrace(stackAddresses, maxDepth);
    char **stackStrings = backtrace_symbols(stackAddresses, stackDepth);

    m_backTrace = "\nBackTrace\n    ";

    for (size_t i = 1; i < stackDepth; ++i)
    {
        m_backTrace += stackStrings[i];
        m_backTrace += "\n    ";
    }

    free(stackStrings); // malloc()ed by backtrace_symbols
#endif
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCodeException::~StatusCodeException() throw()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode StatusCodeException::GetStatusCode() const
{
    return m_statusCode;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline std::string StatusCodeException::ToString() const
{
    return StatusCodeToString(m_statusCode);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const std::string &StatusCodeException::GetBackTrace() const
{
    return m_backTrace;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline std::string StatusCodeToString(const StatusCode statusCode)
{
    switch (statusCode)
    {
        STATUS_CODE_TABLE(GET_STATUS_CODE_NAME_SWITCH)
        default : throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }
}

} // namespace pandora

#endif // #ifndef STATUS_CODES_H
