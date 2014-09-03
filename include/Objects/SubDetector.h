/**
 *  @file   PandoraSDK/include/Objects/SubDetector.h
 * 
 *  @brief  Header file for the sub detector class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_SUB_DETECTOR_H
#define PANDORA_SUB_DETECTOR_H 1

#include "Api/PandoraApi.h"

#include "Pandora/PandoraInternal.h"

#include <string>

namespace pandora
{

/**
 *  @brief  SubDetector class
 */
class SubDetector
{
public:
    /**
     *  @brief  SubDetectorLayer class
     */
    class SubDetectorLayer
    {
    public:
        float       m_closestDistanceToIp;              ///< Closest distance of the layer from the interaction point, units mm
        float       m_nRadiationLengths;                ///< Absorber material in front of layer, units radiation lengths
        float       m_nInteractionLengths;              ///< Absorber material in front of layer, units interaction lengths
    };

    typedef std::vector<SubDetectorLayer> SubDetectorLayerList;

    /**
     *  @brief  Get the sub detector name, uniquely specifying the sub detector
     * 
     *  @return The sub detector name
     */
    const std::string &GetSubDetectorName() const;

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
     *  @brief  Get the order of symmetry of the outermost edge of sub detector
     * 
     *  @return The order of symmetry of the outermost edge of sub detector
     */
    unsigned int GetOuterSymmetryOrder() const;

    /**
     *  @brief  Whether a second sub detector exists, equivalent to a reflection in z=0 plane
     * 
     *  @return boolean
     */
    bool IsMirroredInZ() const;

    /**
     *  @brief  Get the number of layers in the sub detector
     * 
     *  @return The number of layers in the sub detector
     */
    unsigned int GetNLayers() const;

    /**
     *  @brief  Get the list of layer parameters for the sub detector
     * 
     *  @return The list of layer parameters for the sub detector
     */
    const SubDetectorLayerList &GetSubDetectorLayerList() const;

private:
    /**
     *  @brief  Constructor
     * 
     *  @param  inputParameters the input sub detector parameters
     */
    SubDetector(const PandoraApi::Geometry::SubDetector::Parameters &inputParameters);

    /**
     *  @brief  Destructor
     */
    ~SubDetector();

    std::string             m_subDetectorName;      ///< The sub detector name, must uniquely specify a single sub detector
    float                   m_innerRCoordinate;     ///< Inner cylindrical polar r coordinate, origin interaction point, units mm
    float                   m_innerZCoordinate;     ///< Inner cylindrical polar z coordinate, origin interaction point, units mm
    float                   m_innerPhiCoordinate;   ///< Inner cylindrical polar phi coordinate (angle wrt cartesian x axis)
    unsigned int            m_innerSymmetryOrder;   ///< Order of symmetry of the innermost edge of sub detector
    float                   m_outerRCoordinate;     ///< Outer cylindrical polar r coordinate, origin interaction point, units mm
    float                   m_outerZCoordinate;     ///< Outer cylindrical polar z coordinate, origin interaction point, units mm
    float                   m_outerPhiCoordinate;   ///< Outer cylindrical polar phi coordinate (angle wrt cartesian x axis)
    unsigned int            m_outerSymmetryOrder;   ///< Order of symmetry of the outermost edge of sub detector
    bool                    m_isMirroredInZ;        ///< Whether a second sub detector exists, equivalent to a reflection in z=0 plane
    unsigned int            m_nLayers;              ///< The number of layers in the sub detector section
    SubDetectorLayerList    m_subDetectorLayerList; ///< The list of layer parameters for the sub detector section

    friend class GeometryManager;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline const std::string &SubDetector::GetSubDetectorName() const
{
    return m_subDetectorName;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float SubDetector::GetInnerRCoordinate() const
{
    return m_innerRCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float SubDetector::GetInnerZCoordinate() const
{
    return m_innerZCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float SubDetector::GetInnerPhiCoordinate() const
{
    return m_innerPhiCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int SubDetector::GetInnerSymmetryOrder() const
{
    return m_innerSymmetryOrder;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float SubDetector::GetOuterRCoordinate() const
{
    return m_outerRCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float SubDetector::GetOuterZCoordinate() const
{
    return m_outerZCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float SubDetector::GetOuterPhiCoordinate() const
{
    return m_outerPhiCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int SubDetector::GetOuterSymmetryOrder() const
{
    return m_outerSymmetryOrder;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool SubDetector::IsMirroredInZ() const
{
    return m_isMirroredInZ;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int SubDetector::GetNLayers() const
{
    return m_nLayers;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const SubDetector::SubDetectorLayerList &SubDetector::GetSubDetectorLayerList() const
{
    return m_subDetectorLayerList;
}

} // namespace pandora

#endif // #ifndef PANDORA_SUB_DETECTOR_H
