/**
 *  @file   PandoraPFANew/Framework/include/Objects/DetectorGap.h
 * 
 *  @brief  Header file for the detector gap class.
 * 
 *  $Log: $
 */
#ifndef DETECTOR_GAP_H
#define DETECTOR_GAP_H 1

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
     * 
     *  @return boolean
     */
    virtual bool IsInGap(const CartesianVector &positionVector) const = 0;
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  BoxGap class
 */
class BoxGap : public DetectorGap
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  gapParameters the gap parameters
     */
    BoxGap(const PandoraApi::BoxGap::Parameters &gapParameters);

    bool IsInGap(const CartesianVector &positionVector) const;

    const CartesianVector   m_vertex;               ///< Cartesian coordinates of a gap vertex, units mm
    const CartesianVector   m_side1;                ///< Cartesian vector describing first side meeting vertex, units mm
    const CartesianVector   m_side2;                ///< Cartesian vector describing second side meeting vertex, units mm
    const CartesianVector   m_side3;                ///< Cartesian vector describing third side meeting vertex, units mm
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  ConcentricGap class
 */
class ConcentricGap : public DetectorGap
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  gapParameters the gap parameters
     */
    ConcentricGap(const PandoraApi::ConcentricGap::Parameters &gapParameters);

    bool IsInGap(const CartesianVector &positionVector) const;

    const float             m_minZCoordinate;       ///< Min cylindrical polar z coordinate, origin interaction point, units mm
    const float             m_maxZCoordinate;       ///< Max cylindrical polar z coordinate, origin interaction point, units mm
    const float             m_innerRCoordinate;     ///< Inner cylindrical polar r coordinate, origin interaction point, units mm
    const float             m_innerPhiCoordinate;   ///< Inner cylindrical polar phi coordinate (angle wrt cartesian x axis)
    const unsigned int      m_innerSymmetryOrder;   ///< Order of symmetry of the innermost edge of gap
    const float             m_outerRCoordinate;     ///< Outer cylindrical polar r coordinate, origin interaction point, units mm
    const float             m_outerPhiCoordinate;   ///< Outer cylindrical polar phi coordinate (angle wrt cartesian x axis)
    const unsigned int      m_outerSymmetryOrder;   ///< Order of symmetry of the outermost edge of gap

private:
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

    VertexPointList         m_innerVertexPointList; ///< The vertex points of the inner polygon
    VertexPointList         m_outerVertexPointList; ///< The vertex points of the outer polygon
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline DetectorGap::~DetectorGap()
{
}

} // namespace pandora

#endif // #ifndef DETECTOR_GAP_H
