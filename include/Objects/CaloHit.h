/**
 *  @file   PandoraSDK/include/Objects/CaloHit.h
 * 
 *  @brief  Header file for the calo hit class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_CALO_HIT_H
#define PANDORA_CALO_HIT_H 1

#include "Pandora/ObjectCreation.h"
#include "Pandora/StatusCodes.h"

namespace pandora
{

template<typename T> class InputObjectManager;
template<typename T, typename S> class PandoraObjectFactory;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  CaloHit class
 */
class CaloHit
{
public:
    /**
     *  @brief  Get the position vector of center of calorimeter cell, units mm
     * 
     *  @return the position vector
     */
    const CartesianVector &GetPositionVector() const;

    /**
     *  @brief  For LArTPC usage, the x-coordinate shift associated with a drift time t0 shift, units mm
     * 
     *  @return the x-coordinate shift
     */
    float GetX0() const;

    /**
     *  @brief  Get the unit vector in direction of expected hit propagation
     * 
     *  @return the expected direction
     */
    const CartesianVector &GetExpectedDirection() const;

    /**
     *  @brief  Get the unit vector normal to the sampling layer, pointing outwards from the origin
     * 
     *  @return the normal vector
     */
    const CartesianVector &GetCellNormalVector() const;

    /**
     *  @brief  Get the cell geometry
     * 
     *  @return the cell geometry
     */
    CellGeometry GetCellGeometry() const;

    /**
     *  @brief  Get the cell size 0 [pointing: eta, rectangular: up in ENDCAP, along beam in BARREL, units mm]
     * 
     *  @return the cell size 0
     */
    float GetCellSize0() const;

    /**
     *  @brief  Get the cell size 1 [pointing: phi, rectangular: perpendicular to size 0 and thickness, units mm]
     * 
     *  @return the cell size 1
     */
    float GetCellSize1() const;

    /**
     *  @brief  Get the thickness of cell, units mm
     * 
     *  @return the thickness of cell
     */
    float GetCellThickness() const;

    /**
     *  @brief  Get the absorber material in front of cell, units radiation lengths
     * 
     *  @return the absorber material in front of cell in radiation lengths
     */
    float GetNCellRadiationLengths() const;

    /**
     *  @brief  Get the absorber material in front of cell, units interaction lengths
     * 
     *  @return the absorber material in front of cell in interaction lengths
     */
    float GetNCellInteractionLengths() const;

    /**
     *  @brief  Get the corrected energy of the calorimeter cell, units GeV, as supplied by the user
     * 
     *  @return the corrected energy of the calorimeter cell
     */
    float GetInputEnergy() const;

    /**
     *  @brief  Get the time of (earliest) energy deposition in this cell, units ns
     * 
     *  @return the time of (earliest) energy deposition in this cell
     */
    float GetTime() const;

    /**
     *  @brief  Whether cell should be treated as digital
     * 
     *  @return boolean
     */
    bool IsDigital() const;

    /**
     *  @brief  Get the calorimeter hit type
     * 
     *  @return the calorimeter hit type
     */
    HitType GetHitType() const;

    /**
     *  @brief  Get the region of the detector in which the calo hit is located
     * 
     *  @return the detector region
     */
    HitRegion GetHitRegion() const;

    /**
     *  @brief  Get the subdetector readout layer number
     * 
     *  @return the subdetector readout layer number
     */
    unsigned int GetLayer() const;

    /**
     *  @brief  Get pseudo layer for the calo hit
     * 
     *  @return the pseudo layer
     */
    unsigned int GetPseudoLayer() const;

    /**
     *  @brief  Whether cell is in one of the outermost detector sampling layers
     * 
     *  @return boolean
     */
    bool IsInOuterSamplingLayer() const;

    /**
     *  @brief  Get the calibrated mip equivalent energy
     * 
     *  @return the calibrated mip equivalent energy
     */
    float GetMipEquivalentEnergy() const;

    /**
     *  @brief  Get the calibrated electromagnetic energy measure
     * 
     *  @return the calibrated electromagnetic energy
     */
    float GetElectromagneticEnergy() const;

    /**
     *  @brief  Get the calibrated hadronic energy measure
     * 
     *  @return the calibrated hadronic energy
     */
    float GetHadronicEnergy() const;

    /**
     *  @brief  Get the typical length scale of cell, units mm
     * 
     *  @return the typical length scale of cell
     */
    float GetCellLengthScale() const;

    /**
     *  @brief  Whether the calo hit is flagged as a possible mip hit
     * 
     *  @return boolean
     */
    bool IsPossibleMip() const;

    /**
     *  @brief  Whether the calo hit is flagged as isolated
     * 
     *  @return boolean
     */
    bool IsIsolated() const;

    /**
     *  @brief  Get the calo hit weight, which may not be unity if the hit has been fragmented
     * 
     *  @return the calo hit weight
     */
    float GetWeight() const;

    /**
     *  @brief  Get mc particle weight map for the calo hit
     * 
     *  @return the mc particle weight map
     */
    const MCParticleWeightMap &GetMCParticleWeightMap() const;

    /**
     *  @brief  Get the address of the parent calo hit in the user framework
     */
    const void *GetParentAddress() const;

    /**
     *  @brief  Get the list of cartesian coordinates for the cell corners
     * 
     *  @param  cartesianPointVector to receive the cartesian coordinates of the cell corners
     */
    void GetCellCorners(CartesianPointVector &cartesianPointVector) const;

    /**
     *  @brief  operator< sorting by position, then energy
     * 
     *  @param  rhs the object for comparison
     * 
     *  @return boolean
     */
    bool operator< (const CaloHit &rhs) const;

    /**
     *  @brief  Get the map from registered property name to floating point property value
     * 
     *  @return The properties map
     */
    const PropertiesMap& GetPropertiesMap () const;

protected:
    /**
     *  @brief  Constructor
     * 
     *  @param  parameters the calo hit parameters
     */
    CaloHit(const object_creation::CaloHit::Parameters &parameters);

    /**
     *  @brief  Weighted copy constructor
     * 
     *  @param  parameters the calo hit fragmentation parameters
     */
    CaloHit(const object_creation::CaloHitFragment::Parameters &parameters);

    /**
     *  @brief  Destructor
     */
    virtual ~CaloHit();

    /**
     *  @brief  Alter the metadata information stored in a calo hit
     * 
     *  @param  metaData the metadata (only populated metadata fields will be propagated to the object)
     */
    StatusCode AlterMetadata(const object_creation::CaloHit::Metadata &metadata);

    /**
     *  @brief  Set the mc pseudo layer for the calo hit
     * 
     *  @param  pseudoLayer the pseudo layer
     */
    StatusCode SetPseudoLayer(const unsigned int pseudoLayer);

    /**
     *  @brief  Set the mc particles associated with the calo hit
     * 
     *  @param  mcParticleWeightMap the mc particle weight map
     */
    void SetMCParticleWeightMap(const MCParticleWeightMap &mcParticleWeightMap);

    /**
     *  @brief  Remove the mc particles associated with the calo hit
     */
    void RemoveMCParticles();

    /**
     *  @brief  Calculate the typical length scale of the cell, units mm
     * 
     *  @return the typical length scale of cell
     */
    float CalculateCellLengthScale() const;

    /**
     *  @brief  Get the list of cartesian coordinates for rectangular cell corners
     * 
     *  @param  cartesianPointVector to receive the cartesian coordinates of the cell corners
     */
    void GetRectangularCellCorners(CartesianPointVector &cartesianPointVector) const;

    /**
     *  @brief  Get the list of cartesian coordinates for pointing cell corners
     * 
     *  @param  cartesianPointVector to receive the cartesian coordinates of the cell corners
     */
    void GetPointingCellCorners(CartesianPointVector &cartesianPointVector) const;

    /**
     *  @brief  Whether the calo hit is available to be added to a cluster (access this function via PandoraContentAPI)
     * 
     *  @return boolean
     */
    bool IsAvailable() const;

    /**
     *  @brief  Set availability of calo hit to be added to a cluster
     * 
     *  @param  isAvailable the calo hit availability
     */
    void SetAvailability(bool isAvailable);

    /**
     *  @brief  Update the properties map
     * 
     *  @param  metadata The new CaloHit metadata
     */
    StatusCode UpdatePropertiesMap(const object_creation::CaloHit::Metadata &metadata);

    CartesianVector         m_positionVector;           ///< Position vector of center of calorimeter cell, units mm
    float                   m_x0;                       ///< For LArTPC usage, the x-coordinate shift associated with a drift time t0 shift, units mm
    const CartesianVector   m_expectedDirection;        ///< Unit vector in direction of expected hit propagation
    const CartesianVector   m_cellNormalVector;         ///< Unit normal to the sampling layer, pointing outwards from the origin
    const CellGeometry      m_cellGeometry;             ///< The cell geometry type, pointing or rectangular
    const float             m_cellSize0;                ///< Cell size 0 [pointing: pseudo rapidity, eta, rectangular: up in ENDCAP, along beam in BARREL, units mm]
    const float             m_cellSize1;                ///< Cell size 1 [pointing: azimuthal angle, phi, rectangular: perpendicular to size 0 and thickness, units mm]
    const float             m_cellThickness;            ///< Thickness of cell, units mm
    const float             m_nCellRadiationLengths;    ///< Absorber material in front of cell, units radiation lengths
    const float             m_nCellInteractionLengths;  ///< Absorber material in front of cell, units interaction lengths
    const float             m_time;                     ///< Time of (earliest) energy deposition in this cell, units ns
    const float             m_inputEnergy;              ///< Corrected energy of calorimeter cell in user framework, units GeV
    const float             m_mipEquivalentEnergy;      ///< The calibrated mip equivalent energy, units mip
    const float             m_electromagneticEnergy;    ///< The calibrated electromagnetic energy measure, units GeV
    const float             m_hadronicEnergy;           ///< The calibrated hadronic energy measure, units GeV
    const bool              m_isDigital;                ///< Whether cell should be treated as digital (implies constant cell energy)
    const HitType           m_hitType;                  ///< The type of calorimeter hit
    const HitRegion         m_hitRegion;                ///< Region of the detector in which the calo hit is located
    const unsigned int      m_layer;                    ///< The subdetector readout layer number
    InputUInt               m_pseudoLayer;              ///< The pseudo layer to which the calo hit has been assigned
    const bool              m_isInOuterSamplingLayer;   ///< Whether cell is in one of the outermost detector sampling layers
    float                   m_cellLengthScale;          ///< Typical length scale [pointing: measured at cell mid-point, rectangular: std::sqrt(cellSize0 * cellSize1), units mm ]
    bool                    m_isPossibleMip;            ///< Whether the calo hit is a possible mip hit
    bool                    m_isIsolated;               ///< Whether the calo hit is isolated
    bool                    m_isAvailable;              ///< Whether the calo hit is available to be added to a cluster
    float                   m_weight;                   ///< The calo hit weight, which may not be unity if the hit has been fragmented
    MCParticleWeightMap     m_mcParticleWeightMap;      ///< The mc particle weight map
    const void             *m_pParentAddress;           ///< The address of the parent calo hit in the user framework
    PropertiesMap           m_propertiesMap;            ///< The map from registered property name to flaoting point property value

    friend class CaloHitMetadata;
    friend class CaloHitManager;
    friend class InputObjectManager<CaloHit>;
    friend class PandoraObjectFactory<object_creation::CaloHit::Parameters, object_creation::CaloHit::Object>;
    friend class PandoraObjectFactory<object_creation::CaloHitFragment::Parameters, object_creation::CaloHitFragment::Object>;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &CaloHit::GetPositionVector() const
{
    return m_positionVector;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CaloHit::GetX0() const
{
    return m_x0;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &CaloHit::GetExpectedDirection() const
{
    return m_expectedDirection;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &CaloHit::GetCellNormalVector() const
{
    return m_cellNormalVector;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline CellGeometry CaloHit::GetCellGeometry() const
{
    return m_cellGeometry;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CaloHit::GetCellSize0() const
{
    return m_cellSize0;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CaloHit::GetCellSize1() const
{
    return m_cellSize1;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CaloHit::GetCellThickness() const
{
    return m_cellThickness;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CaloHit::GetNCellRadiationLengths() const
{
    return m_nCellRadiationLengths;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CaloHit::GetNCellInteractionLengths() const
{
    return m_nCellInteractionLengths;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CaloHit::GetInputEnergy() const
{
    return m_inputEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CaloHit::GetTime() const
{
    return m_time;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool CaloHit::IsDigital() const
{
    return m_isDigital;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline HitType CaloHit::GetHitType() const
{
    return m_hitType;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline HitRegion CaloHit::GetHitRegion() const
{
    return m_hitRegion;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int CaloHit::GetLayer() const
{
    return m_layer;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int CaloHit::GetPseudoLayer() const
{
    return m_pseudoLayer.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool CaloHit::IsInOuterSamplingLayer() const
{
    return m_isInOuterSamplingLayer;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CaloHit::GetMipEquivalentEnergy() const
{
    return m_mipEquivalentEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CaloHit::GetElectromagneticEnergy() const
{
    return m_electromagneticEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CaloHit::GetHadronicEnergy() const
{
    return m_hadronicEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CaloHit::GetCellLengthScale() const
{
    return m_cellLengthScale;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool CaloHit::IsPossibleMip() const
{
    return m_isPossibleMip;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CaloHit::GetWeight() const
{
    return m_weight;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool CaloHit::IsIsolated() const
{
    return m_isIsolated;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const MCParticleWeightMap &CaloHit::GetMCParticleWeightMap() const
{
    return m_mcParticleWeightMap;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const void *CaloHit::GetParentAddress() const
{
    return m_pParentAddress;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool CaloHit::IsAvailable() const
{
    return m_isAvailable;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void CaloHit::SetAvailability(bool isAvailable)
{
    m_isAvailable = isAvailable;
}

inline const PropertiesMap& CaloHit::GetPropertiesMap() const
{
    return m_propertiesMap;
}

} // namespace pandora

#endif // #ifndef PANDORA_CALO_HIT_H
