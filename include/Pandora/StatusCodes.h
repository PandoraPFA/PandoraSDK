/**
 *  @file   PandoraSDK/include/Pandora/StatusCodes.h
 *
 *  @brief  Header file defining status codes and relevant preprocessor macros
 *
 *  $Log: $
 */
#ifndef PANDORA_STATUS_CODES_H
#define PANDORA_STATUS_CODES_H 1

#include <exception>
#include <iostream>
#include <string>
#include <string_view>

#if defined(__GNUC__) && defined(BACKTRACE)
    #include <cstdlib>
    #include <execinfo.h>
#endif

namespace pandora
{
namespace deprecation_markers
{
    [[deprecated("Legacy macro.")]]
    inline void PANDORA_RETURN_is_deprecated() {}

    [[deprecated("Legacy macro. Use 'RETURN_IF' instead.")]]
    inline void PANDORA_RETURN_IF_is_deprecated() {}

    [[deprecated("Legacy macro. Use 'RETURN_ON_ERROR' instead.")]]
    inline void PANDORA_RETURN_RESULT_IF_is_deprecated() {}

    [[deprecated("Legacy macro. Use 'RETURN_ON_ERROR_EXCEPT' instead.")]]
    inline void PANDORA_RETURN_RESULT_IF_AND_IF_is_deprecated() {}

    [[deprecated("Legacy macro.")]]
    inline void PANDORA_THROW_is_deprecated() {}

    [[deprecated("Legacy macro. Use 'THROW_IF' instead.")]]
    inline void PANDORA_THROW_IF_is_deprecated() {}

    [[deprecated("Legacy macro. Use 'THROW_ON_ERROR' instead.")]]
    inline void PANDORA_THROW_RESULT_IF_is_deprecated() {}

    [[deprecated("Legacy macro. Use 'THROW_ON_ERROR_EXCEPT' instead.")]]
    inline void PANDORA_THROW_RESULT_IF_AND_IF_is_deprecated() {}
}
}

#if defined(__clang__)
    #define PANDORA_LEGACY_DEPRECATION_PUSH                                                               \
        _Pragma("clang diagnostic push")                                                                  \
        _Pragma("clang diagnostic warning \"-Wdeprecated-declarations\"")
    #define PANDORA_LEGACY_DEPRECATION_POP                                                                \
        _Pragma("clang diagnostic pop")
#elif defined(__GNUC__)
    #define PANDORA_LEGACY_DEPRECATION_PUSH                                                               \
        _Pragma("GCC diagnostic push")                                                                    \
        _Pragma("GCC diagnostic warning \"-Wdeprecated-declarations\"")
    #define PANDORA_LEGACY_DEPRECATION_POP                                                                \
        _Pragma("GCC diagnostic pop")
#else
    #define PANDORA_LEGACY_DEPRECATION_PUSH
    #define PANDORA_LEGACY_DEPRECATION_POP
#endif

#define PANDORA_LEGACY_DEPRECATION_MARK(CallExpr)                                                          \
{                                                                                                          \
    PANDORA_LEGACY_DEPRECATION_PUSH                                                                        \
    CallExpr;                                                                                              \
    PANDORA_LEGACY_DEPRECATION_POP                                                                         \
}

namespace pandora::detail
{
    inline void LogStatusAction(const std::string_view expression, const std::string_view action, const std::string_view status,
         const char *const function, const char *const file, const int line)
    {
        if (!expression.empty())
            std::cout << expression << " " << action << " " << status << std::endl;

        std::cout << "    in function: " << function << std::endl;
        std::cout << "    in file:     " << file << " line#: " << line << std::endl;
    }

    inline void LogLocation(const char *const function, const char *const file, const int line)
    {
        std::cout << "    in function: " << function << std::endl;
        std::cout << "    in file:     " << file << " line#: " << line << std::endl;
    }
}

#define PANDORA_RETURN(StatusCode)                                                                                                        \
{                                                                                                                                         \
    PANDORA_LEGACY_DEPRECATION_MARK(pandora::deprecation_markers::PANDORA_RETURN_is_deprecated())                                         \
    pandora::detail::LogLocation(__FUNCTION__, __FILE__, __LINE__);                                                                       \
    return StatusCode;                                                                                                                    \
}

#define PANDORA_RETURN_IF(StatusCode, Condition)                                                                                          \
{                                                                                                                                         \
    PANDORA_LEGACY_DEPRECATION_MARK(pandora::deprecation_markers::PANDORA_RETURN_IF_is_deprecated())                                      \
    if (Condition)                                                                                                                        \
    {                                                                                                                                     \
        pandora::detail::LogStatusAction(#Condition, "return", StatusCodeToString(StatusCode), __FUNCTION__, __FILE__, __LINE__);         \
        return StatusCode;                                                                                                                \
    }                                                                                                                                     \
}

#define PANDORA_RETURN_RESULT_IF(StatusCode1, Operator, Command)                                                                          \
{                                                                                                                                         \
    PANDORA_LEGACY_DEPRECATION_MARK(pandora::deprecation_markers::PANDORA_RETURN_RESULT_IF_is_deprecated())                               \
    const pandora::StatusCode statusCode(Command);                                                                                        \
    if (statusCode Operator StatusCode1)                                                                                                  \
    {                                                                                                                                     \
        pandora::detail::LogStatusAction(#Command, "return", StatusCodeToString(statusCode), __FUNCTION__, __FILE__, __LINE__);           \
        return statusCode;                                                                                                                \
    }                                                                                                                                     \
}

#define PANDORA_RETURN_RESULT_IF_AND_IF(StatusCode1, StatusCode2, Operator, Command)                                                      \
{                                                                                                                                         \
    PANDORA_LEGACY_DEPRECATION_MARK(pandora::deprecation_markers::PANDORA_RETURN_RESULT_IF_AND_IF_is_deprecated())                        \
    const pandora::StatusCode statusCode(Command);                                                                                        \
    if ((statusCode Operator StatusCode1) && (statusCode Operator StatusCode2))                                                           \
    {                                                                                                                                     \
        pandora::detail::LogStatusAction(#Command, "return", StatusCodeToString(statusCode), __FUNCTION__, __FILE__, __LINE__);           \
        return statusCode;                                                                                                                \
    }                                                                                                                                     \
}

#define PANDORA_THROW(StatusCode)                                                                                                         \
{                                                                                                                                         \
    PANDORA_LEGACY_DEPRECATION_MARK(pandora::deprecation_markers::PANDORA_THROW_is_deprecated())                                          \
    pandora::detail::LogLocation(__FUNCTION__, __FILE__, __LINE__);                                                                       \
    throw pandora::StatusCodeException(StatusCode);                                                                                       \
}

#define PANDORA_THROW_IF(StatusCode, Condition)                                                                                           \
{                                                                                                                                         \
    PANDORA_LEGACY_DEPRECATION_MARK(pandora::deprecation_markers::PANDORA_THROW_IF_is_deprecated())                                       \
    if (Condition)                                                                                                                        \
    {                                                                                                                                     \
        pandora::detail::LogStatusAction(#Condition, "throw", StatusCodeToString(StatusCode), __FUNCTION__, __FILE__, __LINE__);          \
        throw pandora::StatusCodeException(StatusCode);                                                                                   \
    }                                                                                                                                     \
}

#define PANDORA_THROW_RESULT_IF(StatusCode1, Operator, Command)                                                                           \
{                                                                                                                                         \
    PANDORA_LEGACY_DEPRECATION_MARK(pandora::deprecation_markers::PANDORA_THROW_RESULT_IF_is_deprecated())                                \
    const pandora::StatusCode statusCode(Command);                                                                                        \
    if (statusCode Operator StatusCode1)                                                                                                  \
    {                                                                                                                                     \
        pandora::detail::LogStatusAction(#Command, "throw", StatusCodeToString(statusCode), __FUNCTION__, __FILE__, __LINE__);            \
        throw pandora::StatusCodeException(statusCode);                                                                                   \
    }                                                                                                                                     \
}

#define PANDORA_THROW_RESULT_IF_AND_IF(StatusCode1, StatusCode2, Operator, Command)                                                       \
{                                                                                                                                         \
    PANDORA_LEGACY_DEPRECATION_MARK(pandora::deprecation_markers::PANDORA_THROW_RESULT_IF_AND_IF_is_deprecated())                         \
    const pandora::StatusCode statusCode(Command);                                                                                        \
    if ((statusCode Operator StatusCode1) && (statusCode Operator StatusCode2))                                                           \
    {                                                                                                                                     \
        pandora::detail::LogStatusAction(#Command, "throw", StatusCodeToString(statusCode), __FUNCTION__, __FILE__, __LINE__);            \
        throw pandora::StatusCodeException(statusCode);                                                                                   \
    }                                                                                                                                     \
}

#define RETURN_IF(StatusCode, Condition)                                                                                                      \
{                                                                                                                                                   \
    if (Condition)                                                                                                                                  \
    {                                                                                                                                               \
        const pandora::StatusCode _statusCode(StatusCode);                                                                                          \
        pandora::detail::LogStatusAction(#Condition, "return", StatusCodeToString(_statusCode), __FUNCTION__, __FILE__, __LINE__);                  \
        return _statusCode;                                                                                                                         \
    }                                                                                                                                               \
}

#define RETURN_ON_ERROR(Command)                                                                                                               \
{                                                                                                                                                   \
    if (const auto _status = (Command); _status != pandora::STATUS_CODE_SUCCESS)                                                                    \
    {                                                                                                                                               \
        pandora::detail::LogStatusAction(#Command, "return", StatusCodeToString(_status), __FUNCTION__, __FILE__, __LINE__);                        \
        return _status;                                                                                                                             \
    }                                                                                                                                               \
}

#define RETURN_ON_ERROR_EXCEPT(Command, AllowedCode)                                                                                            \
{                                                                                                                                                   \
    if (const auto _status = (Command); _status != pandora::STATUS_CODE_SUCCESS && _status != (AllowedCode))                                        \
    {                                                                                                                                               \
        pandora::detail::LogStatusAction(#Command, "return", StatusCodeToString(_status), __FUNCTION__, __FILE__, __LINE__);                        \
        return _status;                                                                                                                             \
    }                                                                                                                                               \
}

#define THROW_IF(StatusCode, Condition)                                                                                                       \
{                                                                                                                                                   \
    if (Condition)                                                                                                                                  \
    {                                                                                                                                               \
        const pandora::StatusCode _statusCode(StatusCode);                                                                                          \
        pandora::detail::LogStatusAction(#Condition, "throw", StatusCodeToString(_statusCode), __FUNCTION__, __FILE__, __LINE__);                   \
        throw pandora::StatusCodeException(_statusCode);                                                                                            \
    }                                                                                                                                               \
}

#define THROW_ON_ERROR(Command)                                                                                                                \
{                                                                                                                                                   \
    if (const auto _status = (Command); _status != pandora::STATUS_CODE_SUCCESS)                                                                    \
    {                                                                                                                                               \
        pandora::detail::LogStatusAction(#Command, "throw", StatusCodeToString(_status), __FUNCTION__, __FILE__, __LINE__);                         \
        throw pandora::StatusCodeException(_status);                                                                                                \
    }                                                                                                                                               \
}

#define THROW_ON_ERROR_EXCEPT(Command, AllowedCode)                                                                                             \
{                                                                                                                                                   \
    if (const auto _status = (Command); _status != pandora::STATUS_CODE_SUCCESS && _status != (AllowedCode))                                        \
    {                                                                                                                                               \
        pandora::detail::LogStatusAction(#Command, "throw", StatusCodeToString(_status), __FUNCTION__, __FILE__, __LINE__);                         \
        throw pandora::StatusCodeException(_status);                                                                                                \
    }                                                                                                                                               \
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
class StatusCodeException
{
public:
    /**
     *  @brief  Constructor
     *
     *  @param  statusCode the status code
     */
    StatusCodeException(const StatusCode statusCode);

    /**
     *  @brief  Destructor
     */
    ~StatusCodeException() noexcept = default;

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

    for (size_t i = 0; i < stackDepth; ++i)
    {
        m_backTrace += stackStrings[i];
        m_backTrace += "\n    ";
    }

    free(stackStrings); // malloc()ed by backtrace_symbols
#endif
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

inline std::string StatusCodeToString(const StatusCode statusCode)
{
    switch (statusCode)
    {
        STATUS_CODE_TABLE(GET_STATUS_CODE_NAME_SWITCH)
        default : return "UNKNOWN";
    }
}

} // namespace pandora

#endif // #ifndef PANDORA_STATUS_CODES_H
