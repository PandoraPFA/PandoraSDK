/**
 *  @file   PandoraPFANew/Framework/src/Objects/DetectorGap.cc
 * 
 *  @brief  Implementation of the detector gap class.
 * 
 *  $Log: $
 */

#include "Helpers/GeometryHelper.h"

#include "Objects/DetectorGap.h"

namespace pandora
{

BoxGap::BoxGap(const PandoraApi::BoxGap::Parameters &gapParameters) :
    m_vertex(gapParameters.m_vertex.Get()),
    m_side1(gapParameters.m_side1.Get()),
    m_side2(gapParameters.m_side2.Get()),
    m_side3(gapParameters.m_side3.Get())
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool BoxGap::IsInGap(const CartesianVector &positionVector) const
{
    static const float gapTolerance(GeometryHelper::GetGapTolerance());
    const CartesianVector relativePosition(positionVector - m_vertex);

    const float projection1(relativePosition.GetDotProduct(m_side1.GetUnitVector()));

    if ((projection1 < -gapTolerance) || (projection1 > m_side1.GetMagnitude() + gapTolerance))
        return false;

    const float projection2(relativePosition.GetDotProduct(m_side2.GetUnitVector()));

    if ((projection2 < -gapTolerance) || (projection2 > m_side2.GetMagnitude() + gapTolerance))
        return false;

    const float projection3(relativePosition.GetDotProduct(m_side3.GetUnitVector()));

    if ((projection3 < -gapTolerance) || (projection3 > m_side3.GetMagnitude() + gapTolerance))
        return false;

    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

ConcentricGap::ConcentricGap(const PandoraApi::ConcentricGap::Parameters &gapParameters) :
    m_minZCoordinate(gapParameters.m_minZCoordinate.Get()),
    m_maxZCoordinate(gapParameters.m_maxZCoordinate.Get()),
    m_innerRCoordinate(gapParameters.m_innerRCoordinate.Get()),
    m_innerPhiCoordinate(gapParameters.m_innerPhiCoordinate.Get()),
    m_innerSymmetryOrder(gapParameters.m_innerSymmetryOrder.Get()),
    m_outerRCoordinate(gapParameters.m_outerRCoordinate.Get()),
    m_outerPhiCoordinate(gapParameters.m_outerPhiCoordinate.Get()),
    m_outerSymmetryOrder(gapParameters.m_outerSymmetryOrder.Get())
{
    if ((0 == m_innerSymmetryOrder) || (0 == m_outerSymmetryOrder))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    const float centralZCoordinate(0.5f * (m_maxZCoordinate + m_minZCoordinate));
    this->GetPolygonVertices(m_innerRCoordinate, centralZCoordinate, m_innerPhiCoordinate, m_innerSymmetryOrder, m_innerVertexPointList);
    this->GetPolygonVertices(m_outerRCoordinate, centralZCoordinate, m_outerPhiCoordinate, m_outerSymmetryOrder, m_outerVertexPointList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ConcentricGap::IsInGap(const CartesianVector &positionVector) const
{
    static const float gapTolerance(GeometryHelper::GetGapTolerance());
    const float z(positionVector.GetZ());

    if ((z < m_minZCoordinate - gapTolerance) || (z > m_maxZCoordinate + gapTolerance))
        return false;

    // ATTN: For concentric gaps, the gap tolerance is currently only used for the z position check
    const float x(positionVector.GetX()), y(positionVector.GetY());
    const float r(std::sqrt(x * x + y * y));

    if (r < m_innerRCoordinate)
        return false;

    static const float pi(std::acos(-1.f));

    if (r > m_outerRCoordinate / std::cos(pi / static_cast<float>(m_outerSymmetryOrder)))
        return false;

    if (!this->IsIn2DPolygon(positionVector, m_outerVertexPointList, m_outerSymmetryOrder))
        return false;

    if (this->IsIn2DPolygon(positionVector, m_innerVertexPointList, m_innerSymmetryOrder))
        return false;

    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void ConcentricGap::GetPolygonVertices(const float rCoordinate, const float zCoordinate, const float phiCoordinate,
    const unsigned int symmetryOrder, VertexPointList &vertexPointList) const
{
    if (0 == symmetryOrder)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    static const float pi(std::acos(-1.f));
    const float firstVertexAngle(pi / static_cast<float>(symmetryOrder));
    const float rMax(rCoordinate / std::cos(firstVertexAngle));

    for (unsigned int i = 0; i < symmetryOrder + 1; ++i)
    {
        const float phi = phiCoordinate + firstVertexAngle + (2.f * pi * static_cast<float>(i) / static_cast<float>(symmetryOrder));
        const float sinPhi(std::sin(phi));
        const float cosPhi(std::cos(phi));
        vertexPointList.push_back(CartesianVector(sinPhi * rMax, cosPhi * rMax, zCoordinate));
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ConcentricGap::IsIn2DPolygon(const CartesianVector &point, const VertexPointList &vertexPointList, const unsigned int symmetryOrder) const
{
    if (vertexPointList.size() != (symmetryOrder + 1))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    int windingNumber(0);

    for (unsigned int i = 0; i < symmetryOrder; ++i)
    {
        if (vertexPointList[i].GetY() <= point.GetY())
        {
            if (vertexPointList[i + 1].GetY() > point.GetY())
            {
                // If point is left of edge, identify an upward crossing
                if (((vertexPointList[i + 1].GetX() - vertexPointList[i].GetX()) * (point.GetY() - vertexPointList[i].GetY()) -
                     (point.GetX() - vertexPointList[i].GetX()) * (vertexPointList[i + 1].GetY() - vertexPointList[i].GetY()) ) > 0.f)
                {
                    ++windingNumber;
                }
            }
        }
        else
        {
            if (vertexPointList[i + 1].GetY() <= point.GetY())
            {
                // If point is right of edge, identify a downward crossing
                if (((vertexPointList[i + 1].GetX() - vertexPointList[i].GetX()) * (point.GetY() - vertexPointList[i].GetY()) -
                     (point.GetX() - vertexPointList[i].GetX()) * (vertexPointList[i + 1].GetY() - vertexPointList[i].GetY()) ) < 0.f)
                {
                    --windingNumber;
                }
            }
        }
    }

    return (0 != windingNumber);
}

} // namespace pandora
