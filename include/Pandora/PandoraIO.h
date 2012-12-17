/**
 *  @file   PandoraPFANew/Framework/include/Pandora/PandoraIO.h
 * 
 *  @brief  Header file for pandora io structures.
 * 
 *  $Log: $
 */
#ifndef PANDORA_IO_H
#define PANDORA_IO_H 1

namespace pandora
{

const std::string PANDORA_FILE_HASH("pandora"); ///< Look for hash each event to check integrity

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  The container identification enum
 */
enum ContainerId
{
    EVENT,
    GEOMETRY,
    UNKNOWN_CONTAINER
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  The component identification enum
 */
enum ComponentId
{
    CALO_HIT,
    TRACK,
    MC_PARTICLE,
    RELATIONSHIP,
    EVENT_END,
    SUB_DETECTOR,
    BOX_GAP,
    CONCENTRIC_GAP,
    GEOMETRY_END,
    UNKNOWN_COMPONENT
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  The relationship identification enum
 */
enum RelationshipId
{
    CALO_HIT_TO_MC,
    TRACK_TO_MC,
    MC_PARENT_DAUGHTER,
    TRACK_PARENT_DAUGHTER,
    TRACK_SIBLING,
    UNKNOWN_RELATIONSHIP
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

} // namespace pandora

#endif // #ifndef PANDORA_IO_H
