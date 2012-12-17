/**
 *  @file   PandoraPFANew/Framework/include/Helpers/GeometryHelper.h
 * 
 *  @brief  Header file for the geometry helper class.
 * 
 *  $Log: $
 */
#ifndef GEOMETRY_HELPER_H
#define GEOMETRY_HELPER_H 1

#include "Api/PandoraApi.h"

namespace pandora
{

class DetectorGap;
class BFieldCalculator;
class PseudoLayerCalculator;
class TiXmlHandle;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  GeometryHelper class
 */
class GeometryHelper
{
public:
    /**
     *  @brief  LayerParameters class
     */
    class LayerParameters
    {
    public:
        float       m_closestDistanceToIp;              ///< Closest distance of the layer from the interaction point, units mm
        float       m_nRadiationLengths;                ///< Absorber material in front of layer, units radiation lengths
        float       m_nInteractionLengths;              ///< Absorber material in front of layer, units interaction lengths
    };

    typedef std::vector<LayerParameters> LayerParametersList;

    /**
     *  @brief  SubDetectorParameters class
     */
    class SubDetectorParameters
    {
    public:
        /**
         *  @brief  Default constructor
         */
        SubDetectorParameters();

        /**
         *  @brief  Initialize sub detector parameters
         * 
         *  @param  subDetectorName the sub detector name
         *  @param  inputParameters the input sub detector parameters
         */
        void Initialize(const std::string &subDetectorName, const PandoraApi::GeometryParameters::SubDetectorParameters &inputParameters);

        /**
         *  @brief  Whether the sub detector parameters have been initialized
         *
         *  @return boolean
         */
        bool IsInitialized() const;

        /**
         *  @brief  Get the inner cylindrical polar r coordinate, origin interaction point, units mm
         * 
         *  @return The inner cylindrical polar r coordinate
         */
        float GetInnerRCoordinate() const;

        /**
         *  @brief  Get the inner cylindrical polar z coordinate, origin interaction point, units mm
         * 
         *  @return The inner cylindrical polar z coordinate
         */
        float GetInnerZCoordinate() const;

        /**
         *  @brief  Get the inner cylindrical polar phi coordinate (angle wrt cartesian x axis)
         * 
         *  @return The inner cylindrical polar phi coordinate
         */
        float GetInnerPhiCoordinate() const;

        /**
         *  @brief  Get the order of symmetry of the innermost edge of subdetector
         * 
         *  @return The order of symmetry of the innermost edge of subdetector
         */
        unsigned int GetInnerSymmetryOrder() const;

        /**
         *  @brief  Get the outer cylindrical polar r coordinate, origin interaction point, units mm
         * 
         *  @return The outer cylindrical polar r coordinate
         */
        float GetOuterRCoordinate() const;

        /**
         *  @brief  Get the outer cylindrical polar z coordinate, origin interaction point, units mm
         * 
         *  @return The outer cylindrical polar z coordinate
         */
        float GetOuterZCoordinate() const;

        /**
         *  @brief  Get the outer cylindrical polar phi coordinate (angle wrt cartesian x axis)
         * 
         *  @return The outer cylindrical polar phi coordinate
         */
        float GetOuterPhiCoordinate() const;

        /**
         *  @brief  Get the order of symmetry of the outermost edge of subdetector
         * 
         *  @return The order of symmetry of the outermost edge of subdetector
         */
        unsigned int GetOuterSymmetryOrder() const;

        /**
         *  @brief  Whether a second subdetector exists, equivalent to a reflection in z=0 plane
         * 
         *  @return boolean
         */
        bool IsMirroredInZ() const;

        /**
         *  @brief  Get the number of layers in the detector section
         * 
         *  @return The number of layers in the detector section
         */
        unsigned int GetNLayers() const;

        /**
         *  @brief  Get the list of layer parameters for the detector section
         * 
         *  @return The list of layer parameters for the detector section
         */
        const LayerParametersList &GetLayerParametersList() const;

    private:
        bool                    m_isInitialized;        ///< Whether the sub detector parameters have been initialized
        float                   m_innerRCoordinate;     ///< Inner cylindrical polar r coordinate, origin interaction point, units mm
        float                   m_innerZCoordinate;     ///< Inner cylindrical polar z coordinate, origin interaction point, units mm
        float                   m_innerPhiCoordinate;   ///< Inner cylindrical polar phi coordinate (angle wrt cartesian x axis)
        unsigned int            m_innerSymmetryOrder;   ///< Order of symmetry of the innermost edge of subdetector
        float                   m_outerRCoordinate;     ///< Outer cylindrical polar r coordinate, origin interaction point, units mm
        float                   m_outerZCoordinate;     ///< Outer cylindrical polar z coordinate, origin interaction point, units mm
        float                   m_outerPhiCoordinate;   ///< Outer cylindrical polar phi coordinate (angle wrt cartesian x axis)
        unsigned int            m_outerSymmetryOrder;   ///< Order of symmetry of the outermost edge of subdetector
        bool                    m_isMirroredInZ;        ///< Whether a second subdetector exists, equivalent to a reflection in z=0 plane
        unsigned int            m_nLayers;              ///< The number of layers in the detector section
        LayerParametersList     m_layerParametersList;  ///< The list of layer parameters for the detector section
    };

    typedef std::map<std::string, SubDetectorParameters> SubDetectorParametersMap;
    typedef std::vector<DetectorGap *> DetectorGapList;

    /**
     *  @brief  Whether the geometry helper is initialized
     * 
     *  @return boolean
     */
    static bool IsInitialized();

    /**
     *  @brief  Get the bfield value for a specified position vector
     * 
     *  @param  positionVector the specified position
     * 
     *  @return the bfield, units Tesla
     */
    static float GetBField(const CartesianVector &positionVector);

    /**
     *  @brief  Get the appropriate pseudolayer for a specified position vector
     * 
     *  @param  positionVector the specified position
     * 
     *  @return the appropriate pseudolayer
     */
    static PseudoLayer GetPseudoLayer(const CartesianVector &positionVector);

    /**
     *  @brief  Get the pseudolayer assigned to a point at the ip, i.e. the initial offset for pseudolayer values
     *          and the start of the pseudolayer scale
     * 
     *  @return the pseudolayer assigned to a point at the ip
     */
    static PseudoLayer GetPseudoLayerAtIp();

    /**
     *  @brief  Get the inner detector barrel parameters
     * 
     *  @return The inner detector barrel parameters
     */
    static const SubDetectorParameters &GetInDetBarrelParameters();

    /**
     *  @brief  Get the inner detector end cap parameters
     * 
     *  @return The inner detector end cap parameters
     */
    static const SubDetectorParameters &GetInDetEndCapParameters();

    /**
     *  @brief  Get the ecal barrel parameters
     * 
     *  @return The ecal barrel parameters
     */
    static const SubDetectorParameters &GetECalBarrelParameters();

    /**
     *  @brief  Get the ecal end cap parameters
     * 
     *  @return The ecal end cap parameters
     */
    static const SubDetectorParameters &GetECalEndCapParameters();

    /**
     *  @brief  Get the hcal barrel parameters
     * 
     *  @return The hcal barrel parameters
     */
    static const SubDetectorParameters &GetHCalBarrelParameters();

    /**
     *  @brief  Get the hcal end cap parameters
     * 
     *  @return The hcal end cap parameters
     */
    static const SubDetectorParameters &GetHCalEndCapParameters();

    /**
     *  @brief  Get the muon detector barrel parameters
     * 
     *  @return The muon detector barrel parameters
     */
    static const SubDetectorParameters &GetMuonBarrelParameters();

    /**
     *  @brief  Get the muon detector end cap parameters
     * 
     *  @return The muon detector end cap parameters
     */
    static const SubDetectorParameters &GetMuonEndCapParameters();

    /**
     *  @brief  Get the main tracker inner radius, units mm
     * 
     *  @return The main tracker inner radius
     */
    static float GetMainTrackerInnerRadius();

    /**
     *  @brief  Get the main tracker outer radius, units mm
     * 
     *  @return The main tracker outer radius
     */
    static float GetMainTrackerOuterRadius();

    /**
     *  @brief  Get the main tracker z extent, units mm
     * 
     *  @return The main tracker z extent
     */
    static float GetMainTrackerZExtent();

    /**
     *  @brief  Get the coil inner radius, units mm
     * 
     *  @return The coil inner radius
     */
    static float GetCoilInnerRadius();

    /**
     *  @brief  Get the coil outer radius, units mm
     * 
     *  @return The coil outer radius
     */
    static float GetCoilOuterRadius();

    /**
     *  @brief  Get the coil z extent, units mm
     * 
     *  @return The coil z extent
     */
    static float GetCoilZExtent();

    /**
     *  @brief  Get the map from name to parameters for any additional sub detectors
     * 
     *  @return The map from name to parameters
     */
    static const SubDetectorParametersMap &GetAdditionalSubDetectors();

    /**
     *  @brief  Get the list of gaps in the active detector volume
     * 
     *  @return The list of gaps in the active detector volume
     */
    static const DetectorGapList &GetDetectorGapList();

    /**
     *  @brief  Whether a specified position is in a detector gap region
     * 
     *  @param  position the specified position
     * 
     *  @return boolean
     */
    static bool IsInDetectorGapRegion(const CartesianVector &position);

    /**
     *  @brief  Get the granularity level specified for a given calorimeter hit type
     * 
     *  @param  hitType the calorimeter hit type
     * 
     *  @return the granularity
     */
    static Granularity GetHitTypeGranularity(const HitType hitType);

    /**
     *  @brief  Get the maximum polygon radius
     * 
     *  @param  symmetryOrder the polygon symmetry order
     *  @param  phi0 the polygon cylindrical polar phi coordinate
     *  @param  x the cartesian x coordinate
     *  @param  y the cartesian y coordinate
     * 
     *  @return the maximum radius
     */
    static float GetMaximumRadius(const unsigned int symmetryOrder, const float phi0, const float x, const float y);

    typedef std::vector< std::pair<float, float> > AngleVector;

    /**
     *  @brief  Get the maximum polygon radius, with reference to cached sine/cosine values for relevant polygon angles
     * 
     *  @param  angleVector vector containing cached sine/cosine values
     *  @param  x the cartesian x coordinate
     *  @param  y the cartesian y coordinate
     * 
     *  @return the maximum radius
     */
    static float GetMaximumRadius(const AngleVector &angleVector, const float x, const float y);

    /**
     *  @brief  Fill a vector with sine/cosine values for relevant polygon angles
     * 
     *  @param  symmetryOrder the polygon symmetry order
     *  @param  phi0 the polygon cylindrical polar phi coordinate
     *  @param  angleVector the vector to fill with sine/cosine values for relevant polygon angles
     */
    static void FillAngleVector(const unsigned int symmetryOrder, const float phi0, AngleVector &angleVector);

    /**
     *  @brief  Get the tolerance allowed when declaring a point to be "in" a gap region, units mm
     * 
     *  @return The gap tolerance
     */
    static float GetGapTolerance();

private:
    /**
     *  @brief  Initialize the geometry helper
     * 
     *  @param  geometryParameters the geometry parameters
     */
    static StatusCode Initialize(const PandoraApi::GeometryParameters &geometryParameters);

    /**
     *  @brief  Create box gap
     * 
     *  @param  gapParameters the gap parameters
     */
    static StatusCode CreateBoxGap(const PandoraApi::BoxGap::Parameters &gapParameters);

    /**
     *  @brief  Create concentric gap
     * 
     *  @param  gapParameters the gap parameters
     */
    static StatusCode CreateConcentricGap(const PandoraApi::ConcentricGap::Parameters &gapParameters);

    /**
     *  @brief  Set the bfield calculator
     * 
     *  @param  pBFieldCalculator address of the bfield calculator
     */
    static StatusCode SetBFieldCalculator(BFieldCalculator *pBFieldCalculator);

    /**
     *  @brief  Set the pseudo layer calculator
     * 
     *  @param  pPseudoLayerCalculator address of the pseudo layer calculator
     */
    static StatusCode SetPseudoLayerCalculator(PseudoLayerCalculator *pPseudoLayerCalculator);

    typedef std::map<HitType, Granularity> HitTypeToGranularityMap;

    /**
     *  @brief  Get the default hit type to granularity map
     * 
     *  @return the default hit type to granularity map
     */
    static HitTypeToGranularityMap GetDefaultHitTypeToGranularityMap();

    /**
     *  @brief  Set the granularity level to be associated with a specified hit type
     * 
     *  @param  hitType the specified hit type
     *  @param  granularity the specified granularity
     */
    static StatusCode SetHitTypeGranularity(const HitType hitType, const Granularity granularity);

    /**
     *  @brief  Read the cluster helper settings
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    static StatusCode ReadSettings(const TiXmlHandle *const pXmlHandle);

    static bool                     m_isInitialized;            ///< Whether the geometry helper is initialized
    static BFieldCalculator        *m_pBFieldCalculator;        ///< Address of the bfield calculator
    static PseudoLayerCalculator   *m_pPseudoLayerCalculator;   ///< Address of the pseudolayer calculator

    static SubDetectorParameters    m_inDetBarrelParameters;    ///< The inner detector barrel parameters
    static SubDetectorParameters    m_inDetEndCapParameters;    ///< The inner detector end cap parameters
    static SubDetectorParameters    m_eCalBarrelParameters;     ///< The ecal barrel parameters
    static SubDetectorParameters    m_eCalEndCapParameters;     ///< The ecal end cap parameters
    static SubDetectorParameters    m_hCalBarrelParameters;     ///< The hcal barrel parameters
    static SubDetectorParameters    m_hCalEndCapParameters;     ///< The hcal end cap parameters
    static SubDetectorParameters    m_muonBarrelParameters;     ///< The muon detector barrel parameters
    static SubDetectorParameters    m_muonEndCapParameters;     ///< The muon detector end cap parameters

    static InputFloat               m_mainTrackerInnerRadius;   ///< The main tracker inner radius, units mm
    static InputFloat               m_mainTrackerOuterRadius;   ///< The main tracker outer radius, units mm
    static InputFloat               m_mainTrackerZExtent;       ///< The main tracker z extent, units mm
    static InputFloat               m_coilInnerRadius;          ///< The coil inner radius, units mm
    static InputFloat               m_coilOuterRadius;          ///< The coil outer radius, units mm
    static InputFloat               m_coilZExtent;              ///< The coil z extent, units mm

    static SubDetectorParametersMap m_additionalSubDetectors;   ///< Map from name to parameters for any additional subdetectors
    static DetectorGapList          m_detectorGapList;          ///< List of gaps in the active detector volume

    static HitTypeToGranularityMap  m_hitTypeToGranularityMap;  ///< The hit type to granularity map
    static float                    m_gapTolerance;             ///< Tolerance allowed when declaring a point to be "in" a gap region, units mm

    friend class PandoraApiImpl;
    friend class PandoraSettings;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool GeometryHelper::IsInitialized()
{
    return m_isInitialized;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetInDetBarrelParameters()
{
    return m_inDetBarrelParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetInDetEndCapParameters()
{
    return m_inDetEndCapParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetECalBarrelParameters()
{
    return m_eCalBarrelParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetECalEndCapParameters()
{
    return m_eCalEndCapParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetHCalBarrelParameters()
{
    return m_hCalBarrelParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetHCalEndCapParameters()
{
    return m_hCalEndCapParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetMuonBarrelParameters()
{
    return m_muonBarrelParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetMuonEndCapParameters()
{
    return m_muonEndCapParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetMainTrackerInnerRadius()
{
    return m_mainTrackerInnerRadius.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetMainTrackerOuterRadius()
{
    return m_mainTrackerOuterRadius.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetMainTrackerZExtent()
{
    return m_mainTrackerZExtent.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetCoilInnerRadius()
{
    return m_coilInnerRadius.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetCoilOuterRadius()
{
    return m_coilOuterRadius.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetCoilZExtent()
{
    return m_coilZExtent.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParametersMap &GeometryHelper::GetAdditionalSubDetectors()
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_additionalSubDetectors;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::DetectorGapList &GeometryHelper::GetDetectorGapList()
{
    return m_detectorGapList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetGapTolerance()
{
    return m_gapTolerance;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline bool GeometryHelper::SubDetectorParameters::IsInitialized() const
{
    return m_isInitialized;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::SubDetectorParameters::GetInnerRCoordinate() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_innerRCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::SubDetectorParameters::GetInnerZCoordinate() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_innerZCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::SubDetectorParameters::GetInnerPhiCoordinate() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_innerPhiCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int GeometryHelper::SubDetectorParameters::GetInnerSymmetryOrder() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_innerSymmetryOrder;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::SubDetectorParameters::GetOuterRCoordinate() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_outerRCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::SubDetectorParameters::GetOuterZCoordinate() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_outerZCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::SubDetectorParameters::GetOuterPhiCoordinate() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_outerPhiCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int GeometryHelper::SubDetectorParameters::GetOuterSymmetryOrder() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_outerSymmetryOrder;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool GeometryHelper::SubDetectorParameters::IsMirroredInZ() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_isMirroredInZ;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int GeometryHelper::SubDetectorParameters::GetNLayers() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_nLayers;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::LayerParametersList &GeometryHelper::SubDetectorParameters::GetLayerParametersList() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_layerParametersList;
}

} // namespace pandora

#endif // #ifndef GEOMETRY_HELPER_H
