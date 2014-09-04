/**
 *  @file   PandoraSDK/include/Objects/CaloHit.h
 * 
 *  @brief  Header file for the calo hit class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_CALO_HIT_H
#define PANDORA_CALO_HIT_H 1

#include "Api/PandoraApi.h"

#include "Pandora/PandoraInternal.h"

namespace pandora
{

template<typename T> class InputObjectManager;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  The calo cell type enum
 */
enum CellGeometry
{
    RECTANGULAR,
    POINTING,
    UNKNOWN_CELL_GEOMETRY
};

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
     *  @brief  Get the cell geometry
     * 
     *  @return the cell geometry
     */
    CellGeometry GetCellGeometry() const;

    /**
     *  @brief  Get mc particle weight map for the calo hit
     * 
     *  @return the mc particle weight map
     */
    const MCParticleWeightMap &GetMCParticleWeightMap() const;

    /**
     *  @brief  Get the address of the parent calo hit in the user framework
     */
    const void *GetParentCaloHitAddress() const;

    /**
     *  @brief  Get the typical length scale of cell, units mm
     * 
     *  @return the typical length scale of cell
     */
    virtual float GetCellLengthScale() const = 0;

    /**
     *  @brief  Get the list of cartesian coordinates for the cell corners
     * 
     *  @param  cartesianPointList to receive the cartesian coordinates of the cell corners
     */
    virtual void GetCellCorners(CartesianPointList &cartesianPointList) const = 0;

    /**
     *  @brief  Set the isolated hit flag
     * 
     *  @param  isolatedFlag the isolated hit flag
     */
    void SetIsolatedFlag(const bool isolatedFlag);

    /**
     *  @brief  Set the possible mip flag
     * 
     *  @param  possibleMipFlag the possible mip flag
     */
    void SetPossibleMipFlag(const bool possibleMipFlag);

protected:
    /**
     *  @brief  Constructor
     * 
     *  @param  parameters the calo hit parameters
     */
    CaloHit(const PandoraApi::CaloHitBaseParameters &parameters);

    /**
     *  @brief  Weighted copy constructor
     * 
     *  @param  pCaloHit address of the calo hit to copy
     *  @param  weight the weight to apply to energy depositions
     */
    CaloHit(CaloHit *pCaloHit, const float weight);

    /**
     *  @brief  Destructor
     */
    virtual ~CaloHit();

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

    const CartesianVector   m_positionVector;           ///< Position vector of center of calorimeter cell, units mm
    const CartesianVector   m_expectedDirection;        ///< Unit vector in direction of expected hit propagation
    const CartesianVector   m_cellNormalVector;         ///< Unit normal to the sampling layer, pointing outwards from the origin
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

    bool                    m_isPossibleMip;            ///< Whether the calo hit is a possible mip hit
    bool                    m_isIsolated;               ///< Whether the calo hit is isolated
    bool                    m_isAvailable;              ///< Whether the calo hit is available to be added to a cluster
    float                   m_weight;                   ///< The calo hit weight, which may not be unity if the hit has been fragmented

    CellGeometry            m_cellGeometry;             ///< The cell geometry

    MCParticleWeightMap     m_mcParticleWeightMap;      ///< The mc particle weight map
    const void             *m_pParentAddress;           ///< The address of the parent calo hit in the user framework

    friend class CaloHitHelper;
    friend class CaloHitMetadata;
    friend class CaloHitManager;
    friend class InputObjectManager<CaloHit>;
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  RectangularCaloHit class
 */
class RectangularCaloHit : public CaloHit
{
public:
    /**
     *  @brief  Get the u dimension of cell (up in ENDCAP, along beam in BARREL), units mm
     * 
     *  @return the u dimension of cell
     */
    float GetCellSizeU() const;

    /**
     *  @brief  Get the v dimension of cell (perpendicular to u and thickness), units mm
     * 
     *  @return the v dimension of cell
     */
    float GetCellSizeV() const;

    float GetCellLengthScale() const;
    void GetCellCorners(CartesianPointList &cartesianPointList) const;

private:
    /**
     *  @brief  Constructor
     * 
     *  @param  parameters the calo hit parameters
     */
    RectangularCaloHit(const PandoraApi::RectangularCaloHit::Parameters &parameters);

    /**
     *  @brief  Weighted copy constructor
     * 
     *  @param  pCaloHit address of the calo hit to copy
     *  @param  weight the weight to apply to energy depositions
     */
    RectangularCaloHit(RectangularCaloHit *pCaloHit, const float weight = 1.f);

    const float             m_cellSizeU;                ///< Dimension of cell (up in ENDCAP, along beam in BARREL), units mm
    const float             m_cellSizeV;                ///< Dimension of cell (perpendicular to u and thickness), units mm
    const float             m_cellLengthScale;          ///< Typical length scale of cell, std::sqrt(CellSizeU * CellSizeV), units mm

    friend class CaloHitManager;
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  PointingCaloHit class
 */
class PointingCaloHit : public CaloHit
{
public:
    /**
     *  @brief  Get the dimension of cell, as measured by change in pseudo rapidity, eta
     * 
     *  @return the eta dimension of cell
     */
    float GetCellSizeEta() const;

    /**
     *  @brief  Get the dimension of cell, as measured by change in azimuthal angle, phi
     * 
     *  @return the phi dimension of cell
     */
    float GetCellSizePhi() const;

    float GetCellLengthScale() const;
    void GetCellCorners(CartesianPointList &cartesianPointList) const;

private:
    /**
     *  @brief  Constructor
     * 
     *  @param  parameters the calo hit parameters
     */
    PointingCaloHit(const PandoraApi::PointingCaloHit::Parameters &parameters);

    /**
     *  @brief  Weighted copy constructor
     * 
     *  @param  pCaloHit address of the calo hit to copy
     *  @param  weight the weight to apply to energy depositions
     */
    PointingCaloHit(PointingCaloHit *pCaloHit, const float weight = 1.f);

    /**
     *  @brief  Calculate the typical length scale of cell, measured at cell mid-point, units mm
     * 
     *  @return the typical length scale of cell
     */
    float CalculateCellLengthScale() const;

    const float             m_cellSizeEta;              ///< Dimension of cell, as measured by change in pseudo rapidity, eta
    const float             m_cellSizePhi;              ///< Dimension of cell, as measured by change in azimuthal angle, phi
    const float             m_cellLengthScale;          ///< Typical length scale of cell, measured at cell mid-point, units mm

    friend class CaloHitManager;
};

/**
 *  @brief  Operator to dump calo hit properties to an ostream
 *
 *  @param  stream the target ostream
 *  @param  caloHit the calo hit
 */
std::ostream &operator<<(std::ostream &stream, const CaloHit &caloHit);

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &CaloHit::GetPositionVector() const
{
    return m_positionVector;
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

inline CellGeometry CaloHit::GetCellGeometry() const
{
    return m_cellGeometry;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const MCParticleWeightMap &CaloHit::GetMCParticleWeightMap() const
{
    return m_mcParticleWeightMap;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const void *CaloHit::GetParentCaloHitAddress() const
{
    return m_pParentAddress;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline float RectangularCaloHit::GetCellSizeU() const
{
    return m_cellSizeU;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float RectangularCaloHit::GetCellSizeV() const
{
    return m_cellSizeV;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float RectangularCaloHit::GetCellLengthScale() const
{
    return m_cellLengthScale;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline float PointingCaloHit::GetCellSizeEta() const
{
    return m_cellSizeEta;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PointingCaloHit::GetCellSizePhi() const
{
    return m_cellSizePhi;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PointingCaloHit::GetCellLengthScale() const
{
    return m_cellLengthScale;
}

} // namespace pandora

#endif // #ifndef PANDORA_CALO_HIT_H
