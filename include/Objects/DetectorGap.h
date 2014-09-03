/**
 *  @file   PandoraSDK/include/Objects/DetectorGap.h
 * 
 *  @brief  Header file for the detector gap class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_DETECTOR_GAP_H
#define PANDORA_DETECTOR_GAP_H 1

#include "Api/PandoraApi.h"

namespace pandora
{

typedef std::vector<CartesianVector> VertexPointList;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  DetectorGap class
 */
class DetectorGap
{
public:
    /**
     *  @brief  Destructor
     */
    virtual ~DetectorGap();

    /**
     *  @brief  Whether a specified position lies within the gap
     * 
     *  @param  positionVector the position vector
     *  @param  gapTolerance tolerance allowed when declaring a point to be "in" a gap region, units mm
     * 
     *  @return boolean
     */
    virtual bool IsInGap(const CartesianVector &positionVector, const float gapTolerance = 0.f) const = 0;

    friend class GeometryManager;
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  BoxGap class
 */
class BoxGap : public DetectorGap
{
public:
    bool IsInGap(const CartesianVector &positionVector, const float gapTolerance) const;

    /**
     *  @brief  Get the gap vertex
     * 
     *  @param  the gap vertex
     */
    const CartesianVector &GetVertex() const;

    /**
     *  @brief  Get the vector describing first side meeting vertex
     * 
     *  @param  the vector describing first side meeting vertex
     */
    const CartesianVector &GetSide1() const;

    /**
     *  @brief  Get the vector describing second side meeting vertex
     * 
     *  @param  the vector describing second side meeting vertex
     */
    const CartesianVector &GetSide2() const;

    /**
     *  @brief  Get the vector describing third side meeting vertex
     * 
     *  @param  the vector describing third side meeting vertex
     */
    const CartesianVector &GetSide3() const;

private:
    /**
     *  @brief  Constructor
     * 
     *  @param  parameters the gap parameters
     */
    BoxGap(const PandoraApi::Geometry::BoxGap::Parameters &parameters);

    const CartesianVector   m_vertex;               ///< Cartesian coordinates of a gap vertex, units mm
    const CartesianVector   m_side1;                ///< Cartesian vector describing first side meeting vertex, units mm
    const CartesianVector   m_side2;                ///< Cartesian vector describing second side meeting vertex, units mm
    const CartesianVector   m_side3;                ///< Cartesian vector describing third side meeting vertex, units mm

    friend class GeometryManager;
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  ConcentricGap class
 */
class ConcentricGap : public DetectorGap
{
public:
    bool IsInGap(const CartesianVector &positionVector, const float gapTolerance) const;

    /**
     *  @brief  Get the min cylindrical polar z coordinate, origin interaction point
     * 
     *  @param  the min cylindrical polar z coordinate
     */
    float GetMinZCoordinate() const;

    /**
     *  @brief  Get the max cylindrical polar z coordinate, origin interaction point
     * 
     *  @param  the max cylindrical polar z coordinate
     */
    float GetMaxZCoordinate() const;

    /**
     *  @brief  Get the inner cylindrical polar r coordinate, origin interaction point
     * 
     *  @param  the inner cylindrical polar r coordinate
     */
    float GetInnerRCoordinate() const;

    /**
     *  @brief  Get the inner cylindrical polar phi coordinate
     * 
     *  @param  the inner cylindrical polar phi coordinate
     */
    float GetInnerPhiCoordinate() const;

    /**
     *  @brief  Get the order of symmetry of the innermost edge of gap
     * 
     *  @param  the order of symmetry of the innermost edge of gap
     */
    unsigned int GetInnerSymmetryOrder() const;

    /**
     *  @brief  Get the outer cylindrical polar r coordinate, origin interaction point
     * 
     *  @param  the outer cylindrical polar r coordinate
     */
    float GetOuterRCoordinate() const;

    /**
     *  @brief  Get the outer cylindrical polar phi coordinate
     * 
     *  @param  the outer cylindrical polar phi coordinate
     */
    float GetOuterPhiCoordinate() const;

    /**
     *  @brief  Get the order of symmetry of the outermost edge of gap
     * 
     *  @param  the order of symmetry of the outermost edge of gap
     */
    unsigned int GetOuterSymmetryOrder() const;

private:
    /**
     *  @brief  Constructor
     * 
     *  @param  parameters the gap parameters
     */
    ConcentricGap(const PandoraApi::Geometry::ConcentricGap::Parameters &parameters);

    /**
     *  @brief  Populate list of polygon vertices, assuming regular polygon in XY plane at constant z coordinate
     * 
     *  @brief  rCoordinate polygon r coordinate
     *  @brief  zCoordinate polygon z coordinate
     *  @brief  phiCoordinate polygon phi coordinate
     *  @brief  symmetryOrder polygon symmetry order
     *  @param  vertexPointList to receive the vertex point list, with vertexPointList[symmetryOrder] = vertexPointList[0]
     */
    void GetPolygonVertices(const float rCoordinate, const float zCoordinate, const float phiCoordinate,
        const unsigned int symmetryOrder, VertexPointList &vertexPointList) const;

    /**
     *  @brief  Winding number test for a point in a 2D polygon in the XY plane (z coordinates are ignored)
     * 
     *  @param  point the test point
     *  @param  vertexPointList vertex points of a polygon, with vertexPointList[symmetryOrder] = vertexPointList[0]
     *  @param  symmetryOrder order of symmetry of polygon
     * 
     *  @return whether point is inside polygon
     */
    bool IsIn2DPolygon(const CartesianVector &point, const VertexPointList &vertexPointList, const unsigned int symmetryOrder) const;

    const float             m_minZCoordinate;       ///< Min cylindrical polar z coordinate, origin interaction point, units mm
    const float             m_maxZCoordinate;       ///< Max cylindrical polar z coordinate, origin interaction point, units mm
    const float             m_innerRCoordinate;     ///< Inner cylindrical polar r coordinate, origin interaction point, units mm
    const float             m_innerPhiCoordinate;   ///< Inner cylindrical polar phi coordinate (angle wrt cartesian x axis)
    const unsigned int      m_innerSymmetryOrder;   ///< Order of symmetry of the innermost edge of gap
    const float             m_outerRCoordinate;     ///< Outer cylindrical polar r coordinate, origin interaction point, units mm
    const float             m_outerPhiCoordinate;   ///< Outer cylindrical polar phi coordinate (angle wrt cartesian x axis)
    const unsigned int      m_outerSymmetryOrder;   ///< Order of symmetry of the outermost edge of gap

    VertexPointList         m_innerVertexPointList; ///< The vertex points of the inner polygon
    VertexPointList         m_outerVertexPointList; ///< The vertex points of the outer polygon

    friend class GeometryManager;
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline DetectorGap::~DetectorGap()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &BoxGap::GetVertex() const
{
    return m_vertex;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &BoxGap::GetSide1() const
{
    return m_side1;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &BoxGap::GetSide2() const
{
    return m_side2;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &BoxGap::GetSide3() const
{
    return m_side3;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline float ConcentricGap::GetMinZCoordinate() const
{
    return m_minZCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ConcentricGap::GetMaxZCoordinate() const
{
    return m_maxZCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ConcentricGap::GetInnerRCoordinate() const
{
    return m_innerRCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ConcentricGap::GetInnerPhiCoordinate() const
{
    return m_innerPhiCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int ConcentricGap::GetInnerSymmetryOrder() const
{
    return m_innerSymmetryOrder;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ConcentricGap::GetOuterRCoordinate() const
{
    return m_outerRCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ConcentricGap::GetOuterPhiCoordinate() const
{
    return m_outerPhiCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int ConcentricGap::GetOuterSymmetryOrder() const
{
    return m_outerSymmetryOrder;
}

} // namespace pandora

#endif // #ifndef PANDORA_DETECTOR_GAP_H
