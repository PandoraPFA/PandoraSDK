/**
 *  @file   PandoraPFANew/Framework/include/Objects/Helix.h
 * 
 *  @brief  Header file for the helix class, based on marlin util helix class.
 * 
 *  $Log: $
 */
#ifndef HELIX_H
#define HELIX_H 1

#include "Objects/CartesianVector.h"

#include "Pandora/StatusCodes.h"

#include <cmath>

namespace pandora
{

/**
 *  @brief  Helix class
 */
class Helix
{
public:
    /**
     *  @brief  Constructor using canonical (LEP-wise) parameterisation
     *
     *  @param  phi0 phi angle of momentum vector at the point of closest approach to IP in R-Phi plane
     *  @param  d0 signed distance of closest approach in R-Phi plane
     *  @param  z0 z coordinate of the point of closest approach to IP in R-Phi plane
     *  @param  omega signed curvature
     *  @param  tanLambda tangent of dip angle
     *  @param  bField magnetic field (in Tesla)
     */
    Helix(const float phi0, const float d0, const float z0, const float omega, const float tanlambda, const float bField);

    /**
     *  @brief  Constructor
     * 
     *  @param  position position of the reference point
     *  @param  momentum momentum vector at the reference point
     *  @param  charge particle charge
     *  @param  bField magnetic field (in Tesla)
     */  
    Helix(const CartesianVector &position, const CartesianVector &momentum, const float charge, const float bField);

    /**
     *  @brief  Get helix intersection point with a plane parallel to z axis. The plane is defined by two coordinates in the
     *          plane (x0,y0) and a normal vector (ax,ay).
     * 
     *  @param  x0 x coordinate in the specified plane
     *  @param  y0 y coordinate in the specified plane
     *  @param  ax x component of vector normal to specified plane
     *  @param  ay y component of vector normal to specified plane
     *  @param  referencePoint the reference point of the helix
     *  @param  intersectionPoint to receive the coordinates of the intersection point
     */
    StatusCode GetPointInXY(const float x0, const float y0, const float ax, const float ay, const CartesianVector &referencePoint,
        CartesianVector &intersectionPoint) const;

    /**
     *  @brief  Get helix intersection point with a plane parallel to z axis. The plane is defined by two coordinates in the
     *          plane (x0,y0) and a normal vector (ax,ay).
     * 
     *  @param  x0 x coordinate in the specified plane
     *  @param  y0 y coordinate in the specified plane
     *  @param  ax x component of vector normal to specified plane
     *  @param  ay y component of vector normal to specified plane
     *  @param  referencePoint the reference point of the helix
     *  @param  intersectionPoint to receive the coordinates of the intersection point
     *  @param  genericTime to receive the generic time (helix length, from reference point to intersection, divided by particle momentum)
     */
    StatusCode GetPointInXY(const float x0, const float y0, const float ax, const float ay, const CartesianVector &referencePoint,
        CartesianVector &intersectionPoint, float &genericTime) const;

    /**
     *  @brief  Get helix intersection point with a plane perpendicular to z axis.
     * 
     *  @param  zPlane the z coordinate for the specified plane
     *  @param  referencePoint the reference point of the helix
     *  @param  intersectionPoint to receive the coordinates of the intersection point
     */
    StatusCode GetPointInZ(const float zPlane, const CartesianVector &referencePoint, CartesianVector &intersectionPoint) const;

    /**
     *  @brief  Get helix intersection point with a plane perpendicular to z axis.
     * 
     *  @param  zPlane the z coordinate for the specified plane
     *  @param  referencePoint the reference point of the helix
     *  @param  intersectionPoint to receive the coordinates of the intersection point
     *  @param  genericTime to receive the generic time (helix length, from reference point to intersection, divided by particle momentum)
     */
    StatusCode GetPointInZ(const float zPlane, const CartesianVector &referencePoint, CartesianVector &intersectionPoint,
        float &genericTime) const;

    /**
     *  @brief  Get coordinates of helix intersection with cylinder, aligned along z-axis
     * 
     *  @param  radius the radius of the cylinder
     *  @param  referencePoint the reference point of the helix
     *  @param  intersectionPoint to receive the coordinates of the intersection point
     */
    StatusCode GetPointOnCircle(const float radius, const CartesianVector &referencePoint, CartesianVector &intersectionPoint) const;

    /**
     *  @brief  Get coordinates of helix intersection with cylinder, aligned along z-axis
     * 
     *  @param  radius the radius of the cylinder
     *  @param  referencePoint the reference point of the helix
     *  @param  intersectionPoint to receive the coordinates of the intersection point
     *  @param  genericTime to receive the generic time (helix length, from reference point to intersection, divided by particle momentum)
     */
    StatusCode GetPointOnCircle(const float radius, const CartesianVector &referencePoint, CartesianVector &intersectionPoint,
        float &genericTime) const;

    /**
     *  @brief  Get distance of the closest approach of helix to an arbitrary point in space
     * 
     *  @param  point coordinates of the specified point
     *  @param  distance to receive a vector of distances from helix to point in the following projections:
     *          x component: distance in R-Phi plane
     *          y-component: distance along Z axis
     *          z-component: 3D distance magnitude
     */
    StatusCode GetDistanceToPoint(const CartesianVector &point, CartesianVector &distance) const;

    /**
     *  @brief  Get distance of the closest approach of helix to an arbitrary point in space
     * 
     *  @param  point coordinates of the specified point
     *  @param  distance to receive a vector of distances from helix to point in the following projections:
     *          x component: distance in R-Phi plane
     *          y-component: distance along Z axis
     *          z-component: 3D distance magnitude
     *  @param  genericTime to receive the generic time (helix length, from reference point to intersection, divided by particle momentum)
     */
    StatusCode GetDistanceToPoint(const CartesianVector &point, CartesianVector &distance, float &genericTime) const;

    /**
     *  @brief  Get distance between two helices
     * 
     *  @param  pHelix address of a second helix
     *  @param  positionOfClosestApproach to receive position of the point of closest approach
     *  @param  v0momentum to receive the v0 momentum
     *  @param  helixDistance to receive the distance between the two helices
     */
    StatusCode GetDistanceToHelix(const Helix *const pHelix, CartesianVector &positionOfClosestApproach, CartesianVector &v0momentum,
        float &helixDistance) const;

    /**
     *  @param  Get extrapolated momentum at a specified position
     * 
     *  @param  position the specified position
     * 
     *  @return the extrapolated momentum
     */
    CartesianVector GetExtrapolatedMomentum(const CartesianVector &position) const;

    /**
     *  @brief  Get momentum of particle at the point of closest approach to IP
     * 
     *  @return the momentum of particle
     */
    const CartesianVector &GetMomentum() const;

    /**
     *  @brief  Get reference point of track
     * 
     *  @return the reference point of track
     */
    const CartesianVector &GetReferencePoint() const;

    /**
     *  @brief  Get phi angle of the momentum vector at the point of closest approach to IP
     * 
     *  @return the phi angle of the momentum vector
     */
    float GetPhi0() const;

    /**
     *  @brief  Get z signed distance of closest approach to IP in the R-Phi plane
     * 
     *  @return the signed distance of closest approach
     */
    float GetD0() const;

    /**
     *  @brief  Get z coordinate of the point of closest approach to IP in the R-Phi plane
     * 
     *  @return the z coordinate of the point of closest approach
     */
    float GetZ0() const;

    /**
     *  @brief  Get signed curvature of the track
     * 
     *  @return the signed curvature of the track
     */
    float GetOmega() const;

    /**
     *  @brief  Get tangent of dip angle of the track
     * 
     *  @return the tangent of dip angle of the track
     */
    float GetTanLambda() const;

    /**
     *  @brief  Get transverse momentum of the track
     * 
     *  @return the transverse momentum of the track
     */
    float GetPxy() const;

    /**
     *  @brief  Get charge
     * 
     *  @return the charge
     */
    float GetCharge() const;

    /**
     *  @brief  Get x coordinate of circumference
     * 
     *  @return the x coordinate of circumference
     */
    float GetXCentre() const;

    /**
     *  @brief  Get y coordinate of circumference
     * 
     *  @return the y coordinate of circumference
     */
    float GetYCentre() const;

    /**
     *  @brief  Get radius of circumference
     * 
     *  @return the radius of circumference
     */
    float GetRadius() const;

private:
    static const float FCT;
    static const float TWO_PI;
    static const float HALF_PI;

    CartesianVector     m_referencePoint;       ///< The coordinates of the reference point
    CartesianVector     m_momentum;             ///< The momentum vector at reference point 

    float               m_phi0;                 ///< phi0 in canonical parameterization 
    float               m_d0;                   ///< d0 in canonical parameterisation
    float               m_z0;                   ///< z0 in canonical parameterisation
    float               m_omega;                ///< signed curvature in canonical parameterisation
    float               m_tanLambda;            ///< tanLambda 
    float               m_pxy;                  ///< The transverse momentum
    float               m_charge;               ///< The particle charge
    float               m_xCentre;              ///< The circle centre x coordinate
    float               m_yCentre;              ///< The circle centre y coordinate
    float               m_radius;               ///< The radius of circle in XY plane
    float               m_phiRefPoint;          ///< Phi w.r.t. (X0, Y0) of circle at reference point
    float               m_phiAtPCA;             ///< Phi w.r.t. (X0, Y0) of circle at point of closest approach
    float               m_xAtPCA;               ///< x coordinate at point of closest approach
    float               m_yAtPCA;               ///< y coordinate at point of closest approach
    float               m_pxAtPCA;              ///< Momentum x component at point of closest approach
    float               m_pyAtPCA;              ///< Momentum y component at point of closest approach
    float               m_phiMomRefPoint;       ///< Phi of Momentum vector at reference point
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode Helix::GetPointInXY(const float x0, const float y0, const float ax, const float ay, const CartesianVector &referencePoint,
    CartesianVector &intersectionPoint) const
{
    float genericTime;
    return this->GetPointInXY(x0, y0, ax, ay, referencePoint, intersectionPoint, genericTime);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode Helix::GetPointInZ(const float zPlane, const CartesianVector &referencePoint, CartesianVector &intersectionPoint) const
{
    float genericTime;
    return this->GetPointInZ(zPlane, referencePoint, intersectionPoint, genericTime);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode Helix::GetPointOnCircle(const float radius, const CartesianVector &referencePoint, CartesianVector &intersectionPoint) const
{
    float genericTime;
    return this->GetPointOnCircle(radius, referencePoint, intersectionPoint, genericTime);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode Helix::GetDistanceToPoint(const CartesianVector &point, CartesianVector &distance) const
{
    float genericTime;
    return this->GetDistanceToPoint(point, distance, genericTime);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &Helix::GetMomentum() const
{
    return m_momentum;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &Helix::GetReferencePoint() const
{
    return m_referencePoint;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Helix::GetPhi0() const
{
    if (m_phi0 < 0.)
        return m_phi0 + TWO_PI;

    return m_phi0;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Helix::GetD0() const
{
    return m_d0;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Helix::GetZ0() const
{
    return m_z0;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Helix::GetOmega() const
{
    return m_omega;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Helix::GetTanLambda() const
{
    return m_tanLambda;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Helix::GetPxy() const
{
    return m_pxy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Helix::GetCharge() const
{
    return m_charge;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Helix::GetXCentre() const
{
    return m_xCentre;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Helix::GetYCentre() const
{
    return m_yCentre;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Helix::GetRadius() const
{
    return m_radius;
}

} // namespace pandora

#endif // #ifndef HELIX_H
