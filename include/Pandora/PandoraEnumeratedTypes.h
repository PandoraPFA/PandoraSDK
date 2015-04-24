/**
 *  @file   PandoraSDK/include/Pandora/PandoraEnumeratedTypes.h
 * 
 *  @brief  Header file for pandora enumerated types
 * 
 *  $Log: $
 */
#ifndef PANDORA_ENUMERATED_TYPES_H
#define PANDORA_ENUMERATED_TYPES_H 1

namespace pandora
{

/**
 *  @brief  Energy correction type enum
 */
enum EnergyCorrectionType
{
    HADRONIC,
    ELECTROMAGNETIC
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Cell geometry enum
 */
enum CellGeometry
{
    RECTANGULAR,
    POINTING
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Calorimeter hit type enum
 */
enum HitType
{
    TRACKER,
    ECAL,
    HCAL,
    MUON,
    TPC_VIEW_U,
    TPC_VIEW_V,
    TPC_VIEW_W,
    TPC_3D,
    HIT_CUSTOM
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Calorimeter hit region enum
 */
enum HitRegion
{
    BARREL,
    ENDCAP,
    SINGLE_REGION
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  MC particle type enum
 */
enum MCParticleType
{
    MC_VIEW_U,
    MC_VIEW_V,
    MC_VIEW_W,
    MC_3D,
    MC_CUSTOM
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Subdetector type enum
 */
enum SubDetectorType
{
    INNER_TRACKER,
    ECAL_BARREL,
    ECAL_ENDCAP,
    HCAL_BARREL,
    HCAL_ENDCAP,
    COIL,
    MUON_BARREL,
    MUON_ENDCAP,
    LAR_TPC,
    SUB_DETECTOR_OTHER
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Vertex type enum
 */
enum VertexType
{
    VERTEX_U,
    VERTEX_V,
    VERTEX_W,
    VERTEX_3D,
    VERTEX_CUSTOM
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Vertex label enum
 */
enum VertexLabel
{
    VERTEX_INTERACTION,
    VERTEX_START,
    VERTEX_END,
    VERTEX_APEX,
    VERTEX_CORNER,
    VERTEX_FEATURE
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Granularity enum
 */
enum Granularity
{
    VERY_FINE,
    FINE,
    COARSE,
    VERY_COARSE
};

} // namespace pandora

#endif // #ifndef PANDORA_ENUMERATED_TYPES_H
