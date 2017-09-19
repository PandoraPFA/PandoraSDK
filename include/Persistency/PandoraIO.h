/**
 *  @file   PandoraSDK/include/Pandora/PandoraIO.h
 * 
 *  @brief  Header file for pandora io structures.
 * 
 *  $Log: $
 */
#ifndef PANDORA_IO_H
#define PANDORA_IO_H 1

#include <string>

namespace pandora
{

const std::string PANDORA_FILE_HASH("pandora"); ///< Look for hash each event to check integrity

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  The container identification enum
 */
enum ContainerId
{
    EVENT_CONTAINER,
    GEOMETRY_CONTAINER,
    UNKNOWN_CONTAINER
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  The component identification enum
 */
enum ComponentId
{
    CALO_HIT_COMPONENT,
    TRACK_COMPONENT,
    MC_PARTICLE_COMPONENT,
    RELATIONSHIP_COMPONENT,
    EVENT_END_COMPONENT,
    SUB_DETECTOR_COMPONENT,
    LINE_GAP_COMPONENT,
    BOX_GAP_COMPONENT,
    CONCENTRIC_GAP_COMPONENT,
    GEOMETRY_END_COMPONENT,
    LAR_TPC_COMPONENT,
    UNKNOWN_COMPONENT
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  The relationship identification enum
 */
enum RelationshipId
{
    CALO_HIT_TO_MC_RELATIONSHIP,
    TRACK_TO_MC_RELATIONSHIP,
    MC_PARENT_DAUGHTER_RELATIONSHIP,
    TRACK_PARENT_DAUGHTER_RELATIONSHIP,
    TRACK_SIBLING_RELATIONSHIP,
    UNKNOWN_RELATIONSHIP
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  The file type enum
 */
enum FileType
{
    BINARY,
    XML,
    UNKNOWN_FILE_TYPE
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  The file mode enum
 */
enum FileMode
{
    APPEND,
    OVERWRITE,
    UNKNOWN_MODE
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Stop processing exception class
 */
class StopProcessingException
{
public:
    /**
     *  @brief  Constructor
     *
     *  @param  description the description of the context under which exception was raised
     */
    StopProcessingException(const std::string &description);

    /**
     *  @brief  Copy constructor
     *
     *  @param  rhs the instance to copy
     */
    StopProcessingException(const StopProcessingException &rhs);

    /**
     *  @brief  Destructor
     */
    ~StopProcessingException();

    /**
     *  @brief  Get the description of the context under which exception was raised
     *
     *  @return the description
     */
    const std::string &GetDescription() const;

private:
    const std::string   m_description;      ///< Description of the context under which exception was raised
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline StopProcessingException::StopProcessingException(const std::string &description) :
    m_description(description)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StopProcessingException::StopProcessingException(const StopProcessingException &rhs) :
    m_description(rhs.GetDescription())
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StopProcessingException::~StopProcessingException()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const std::string &StopProcessingException::GetDescription() const
{
    return m_description;
}

} // namespace pandora

#endif // #ifndef PANDORA_IO_H
