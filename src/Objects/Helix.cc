/**
 *  @file   PandoraPFANew/Framework/src/Objects/Helix.cc
 * 
 *  @brief  Implementation of the helix class, based on marlin util helix class.
 * 
 *  $Log: $
 */

#include "Objects/Helix.h"

#include <iostream>

namespace pandora
{

const float Helix::FCT = 2.99792458E-4f;
const float Helix::TWO_PI = static_cast<float>(2. * std::acos(-1.0));
const float Helix::HALF_PI = static_cast<float>(0.5 * std::acos(-1.0));

//------------------------------------------------------------------------------------------------------------------------------------------

Helix::Helix(const float phi0, const float d0, const float z0, const float omega, const float tanLambda, const float bField) :
    m_referencePoint(0.f, 0.f, 0.f),
    m_momentum(0.f, 0.f, 0.f),
    m_phi0(phi0),
    m_d0(d0),
    m_z0(z0),
    m_omega(omega),
    m_tanLambda(tanLambda)
{
    if ((0. >= bField) || (0. == omega))
    {
        std::cout << "Helix, invalid parameter: bField " << bField << ", omega " << omega << std::endl;
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }

    m_charge = omega / std::fabs(omega);
    m_radius = 1.f / std::fabs(omega);
    m_xAtPCA = -m_d0 * std::sin(m_phi0);
    m_yAtPCA = m_d0 * std::cos(m_phi0);
    m_referencePoint.SetValues(m_xAtPCA, m_yAtPCA, m_z0);
    m_pxy = FCT * bField * m_radius;
    m_momentum.SetValues(m_pxy * std::cos(m_phi0), m_pxy * std::sin(m_phi0), m_tanLambda * m_pxy);
    m_pxAtPCA = m_momentum.GetX();
    m_pyAtPCA = m_momentum.GetY();
    m_phiMomRefPoint = std::atan2(m_pyAtPCA, m_pxAtPCA);
    m_xCentre = m_referencePoint.GetX() + m_radius * std::cos(m_phi0 - HALF_PI * m_charge);
    m_yCentre = m_referencePoint.GetY() + m_radius * std::sin(m_phi0 - HALF_PI * m_charge);
    m_phiAtPCA = std::atan2(-m_yCentre, -m_xCentre);
    m_phiRefPoint = m_phiAtPCA;
}

//------------------------------------------------------------------------------------------------------------------------------------------

Helix::Helix(const CartesianVector &position, const CartesianVector &momentum, const float charge, const float bField) :
    m_referencePoint(position),
    m_momentum(momentum),
    m_charge(charge)
{
    const double px(momentum.GetX()), py(momentum.GetY());
    const double pxy(std::sqrt(px * px + py * py));

    if ((0. >= bField) || (0. == pxy))
    {
        std::cout << "Helix, invalid parameter: bField " << bField << ", pxy " << pxy << std::endl;
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }

    const double radius(pxy / (FCT * bField));
    m_pxy    = static_cast<float>(pxy);
    m_radius = static_cast<float>(radius);

    m_omega     = charge / m_radius;
    m_tanLambda = momentum.GetZ() / m_pxy;
    m_phiMomRefPoint = static_cast<float>(std::atan2(py, px));

    const double x(position.GetX()), y(position.GetY());
    const double xCentre(x + radius * static_cast<double>(std::cos(m_phiMomRefPoint - HALF_PI * charge)));
    const double yCentre(y + radius * static_cast<double>(std::sin(m_phiMomRefPoint - HALF_PI * charge)));
    m_xCentre = static_cast<float>(xCentre);
    m_yCentre = static_cast<float>(yCentre);

    double d0;
    if (charge > 0)
    {
        d0 = static_cast<double>(charge) * radius - static_cast<double>(std::sqrt(xCentre * xCentre + yCentre * yCentre));
    }
    else
    {
        d0 = static_cast<double>(charge) * radius + static_cast<double>(std::sqrt(xCentre * xCentre + yCentre * yCentre));
    }
    m_d0 = static_cast<float>(d0);

    m_phiRefPoint   =  static_cast<float>(std::atan2(y - yCentre, x - xCentre));
    m_phiAtPCA      =  static_cast<float>(std::atan2(-yCentre, -xCentre));
    m_phi0          = -HALF_PI * charge + m_phiAtPCA;

    while (m_phi0 < 0.)
        m_phi0 += TWO_PI;

    while (m_phi0 >= TWO_PI)
        m_phi0 -= TWO_PI;

    m_xAtPCA    = m_xCentre + m_radius * std::cos(m_phiAtPCA);
    m_yAtPCA    = m_yCentre + m_radius * std::sin(m_phiAtPCA);
    m_pxAtPCA   = m_pxy * std::cos(m_phi0);
    m_pyAtPCA   = m_pxy * std::sin(m_phi0);

    float deltaPhi = m_phiRefPoint - m_phiAtPCA;
    float xCircles = (-position.GetZ() * charge) / (TWO_PI * ((m_radius * m_tanLambda) - deltaPhi));

    int n1, n2;
    if (xCircles >= 0.)
    {
        n1 = static_cast<int>(xCircles);
        n2 = n1 + 1;
    }
    else
    {
        n1 = static_cast<int>(xCircles) - 1;
        n2 = n1 + 1;
    }

    const int nCircles((std::fabs(n1 - xCircles) < std::fabs(n2 - xCircles) ? n1 : n2));
    m_z0 = position.GetZ() + m_radius * m_tanLambda * charge * (deltaPhi + TWO_PI * nCircles);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Helix::GetPointInXY(const float x0, const float y0, const float ax, const float ay, const CartesianVector &referencePoint,
        CartesianVector &intersectionPoint, float &genericTime) const
{
    const float AA(std::sqrt(ax * ax + ay * ay));

    if (AA <= 0)
        return STATUS_CODE_FAILURE;

    const float BB((ax * (x0 - m_xCentre) + ay * (y0 - m_yCentre)) / AA);
    const float CC(((x0 - m_xCentre) * (x0 - m_xCentre) + (y0 - m_yCentre) * (y0 - m_yCentre) - m_radius * m_radius) / AA);

    const float DET(BB * BB - CC);

    if (DET < 0)
        return STATUS_CODE_NOT_FOUND;

    float tt1(-BB + std::sqrt(DET));
    float tt2(-BB - std::sqrt(DET));

    const float xx1(x0 + tt1 * ax);
    const float yy1(y0 + tt1 * ay);
    const float xx2(x0 + tt2 * ax);
    const float yy2(y0 + tt2 * ay);

    const float phi1(std::atan2(yy1 - m_yCentre, xx1 - m_xCentre));
    const float phi2(std::atan2(yy2 - m_yCentre, xx2 - m_xCentre));
    const float phi0(std::atan2(referencePoint.GetY() - m_yCentre, referencePoint.GetX() - m_xCentre));

    float dphi1(phi1 - phi0);
    float dphi2(phi2 - phi0);

    if (dphi1 < 0 && m_charge < 0)
    {
        dphi1 = dphi1 + TWO_PI;
    }
    else if (dphi1 > 0 && m_charge > 0)
    {
        dphi1 = dphi1 - TWO_PI;
    }

    if (dphi2 < 0 && m_charge < 0)
    {
        dphi2 = dphi2 + TWO_PI;
    }
    else if (dphi2 > 0 && m_charge > 0)
    {
        dphi2 = dphi2 - TWO_PI;
    }

    // Calculate generic time
    tt1 = -m_charge * dphi1 * m_radius / m_pxy;
    tt2 = -m_charge * dphi2 * m_radius / m_pxy;

    if ((tt1 < 0.) || (tt2 < 0.))
        std::cout << "Helix:: GetPointInXY, warning - negative generic time, tt1 " << tt1 << ", tt2 " << tt2 << std::endl;

    if (tt1 < tt2)
    {
        genericTime = tt1;
        intersectionPoint.SetValues(xx1, yy1, referencePoint.GetZ() + genericTime * m_momentum.GetZ());
    }
    else
    {
        genericTime = tt2;
        intersectionPoint.SetValues(xx2, yy2, referencePoint.GetZ() + genericTime * m_momentum.GetZ());
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Helix::GetPointInZ(const float zPlane, const CartesianVector &referencePoint, CartesianVector &intersectionPoint,
    float &genericTime) const
{
    if (m_momentum.GetZ() == 0.)
        return STATUS_CODE_NOT_FOUND;

    genericTime = (zPlane - referencePoint.GetZ()) / m_momentum.GetZ();

    const float phi0(std::atan2(referencePoint.GetY() - m_yCentre, referencePoint.GetX() - m_xCentre));
    const float phi(phi0 - m_charge * m_pxy * genericTime / m_radius);

    intersectionPoint.SetValues(m_xCentre + m_radius * std::cos(phi), m_yCentre + m_radius * std::sin(phi), zPlane);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Helix::GetPointOnCircle(const float radius, const CartesianVector &referencePoint, CartesianVector &intersectionPoint,
    float &genericTime) const
{
    const float distCenterToIP(std::sqrt(m_xCentre * m_xCentre + m_yCentre * m_yCentre));

    if ( ((distCenterToIP + m_radius) < radius) || ((m_radius + radius) < distCenterToIP) )
        return STATUS_CODE_NOT_FOUND;

    const float phiCentre(std::atan2(m_yCentre, m_xCentre));

    float phiStar(radius * radius + distCenterToIP * distCenterToIP - m_radius * m_radius);
    phiStar = 0.5f * phiStar / std::max(1.e-20f, radius * distCenterToIP);

    if (phiStar > 1.f)
        phiStar = 0.9999999f;

    if (phiStar < -1.f)
        phiStar = -0.9999999f;

    phiStar = std::acos(phiStar);

    const float xx1(radius * std::cos(phiCentre + phiStar));
    const float yy1(radius * std::sin(phiCentre + phiStar));

    const float xx2(radius * std::cos(phiCentre-phiStar));
    const float yy2(radius * std::sin(phiCentre-phiStar));

    const float phi1(std::atan2(yy1 - m_yCentre, xx1 - m_xCentre));
    const float phi2(std::atan2(yy2 - m_yCentre, xx2 - m_xCentre));
    const float phi0(std::atan2(referencePoint.GetY() - m_yCentre, referencePoint.GetX() - m_xCentre));

    float dphi1(phi1 - phi0);
    float dphi2(phi2 - phi0);

    if (dphi1 < 0 && m_charge < 0)
    {
        dphi1 = dphi1 + TWO_PI;
    }
    else if (dphi1 > 0 && m_charge > 0)
    {
        dphi1 = dphi1 - TWO_PI;
    }

    if (dphi2 < 0 && m_charge < 0)
    {
        dphi2 = dphi2 + TWO_PI;
    }
    else if (dphi2 > 0 && m_charge > 0)
    {
        dphi2 = dphi2 - TWO_PI;
    }

    // Calculate generic time
    const float tt1(-m_charge * dphi1 * m_radius / m_pxy);
    const float tt2(-m_charge * dphi2 * m_radius / m_pxy);

    if ((tt1 < 0.) || (tt2 < 0.))
        std::cout << "Helix:: GetPointOnCircle, warning - negative generic time, tt1 " << tt1 << ", tt2 " << tt2 << std::endl;

    // Previously returned both xx1, xx2, etc.
    if (tt1 < tt2)
    {
        genericTime = tt1;
        intersectionPoint.SetValues(xx1, yy1, referencePoint.GetZ() + genericTime * m_momentum.GetZ());
    }
    else
    {
        genericTime = tt2;
        intersectionPoint.SetValues(xx2, yy2, referencePoint.GetZ() + genericTime * m_momentum.GetZ());
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Helix::GetDistanceToPoint(const CartesianVector &point, CartesianVector &distance, float &genericTime) const
{
    const float phi(std::atan2(point.GetY() - m_yCentre, point.GetX() - m_xCentre));
    const float phi0(std::atan2(m_referencePoint.GetY() - m_yCentre, m_referencePoint.GetX() - m_xCentre));

    int nCircles = 0;
    if (std::fabs(m_tanLambda * m_radius) > 1.e-20)
    {
        const float xCircles((phi0 - phi - m_charge * (point.GetZ() - m_referencePoint.GetZ()) / (m_tanLambda * m_radius)) / TWO_PI);

        int n1, n2;
        if (xCircles >= 0.)
        {
            n1 = static_cast<int>(xCircles);
            n2 = n1 + 1;
        }
        else
        {
            n1 = static_cast<int>(xCircles) - 1;
            n2 = n1 + 1;
        }

        nCircles = ((std::fabs(n1 - xCircles) < std::fabs(n2 - xCircles) ? n1 : n2));
    }

    const float dPhi(TWO_PI * (static_cast<float>(nCircles)) + phi - phi0);
    const float zOnHelix(m_referencePoint.GetZ() - m_charge * m_radius * m_tanLambda * dPhi);

    const float distX(std::fabs(m_xCentre - point.GetX()));
    const float distY(std::fabs(m_yCentre - point.GetY()));
    const float distZ(std::fabs(zOnHelix - point.GetZ()));

    float distXY(std::sqrt(distX * distX + distY * distY));
    distXY = std::fabs(distXY - m_radius);

    distance.SetValues(distXY, distZ, std::sqrt(distXY * distXY + distZ * distZ));

    if (std::fabs(m_momentum.GetZ()) > 0)
    {
        genericTime = (zOnHelix - m_referencePoint.GetZ()) / m_momentum.GetZ();
    }
    else
    {
        genericTime = m_charge * m_radius * dPhi / m_pxy;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Helix::GetDistanceToHelix(const Helix *const pHelix, CartesianVector &positionOfClosestApproach, CartesianVector &v0momentum,
    float &helixDistance) const
{
    if (this == pHelix)
        return STATUS_CODE_INVALID_PARAMETER;

    const float x01(this->GetXCentre());
    const float y01(this->GetYCentre());

    const float x02(pHelix->GetXCentre());
    const float y02(pHelix->GetYCentre());

    const float rad1(this->GetRadius());
    const float rad2(pHelix->GetRadius());

    const float distance(std::sqrt((x01-x02) * (x01-x02) + (y01-y02) * (y01-y02)));

    bool singlePoint(true);
    float phi1(0.), phi2(0.);

    if (rad1 + rad2 < distance)
    {
        phi1 = std::atan2(y02 - y01, x02 - x01);
        phi2 = std::atan2(y01 - y02, x01 - x02);
    }
    else if (distance + rad2 < rad1)
    {
        phi1 = std::atan2(y02 - y01, x02 - x01);
        phi2 = phi1;
    }
    else if (distance + rad1 < rad2)
    {
        phi1 = std::atan2(y01 - y02, x01 - x02);
        phi2 = phi1;
    }
    else
    {
        if ((0. == distance) || (0. == rad2))
            return STATUS_CODE_FAILURE;

        singlePoint = false;
        float cosAlpha = 0.5f * (distance * distance + rad2 * rad2 - rad1 * rad1) / (distance * rad2);
        float alpha = std::acos(cosAlpha);
        float phi0 = std::atan2(y01 - y02, x01 - x02);
        phi1 = phi0 + alpha;
        phi2 = phi0 - alpha;
    }

    const CartesianVector &referencePoint1(m_referencePoint);
    const CartesianVector &referencePoint2(pHelix->GetReferencePoint());

    CartesianVector position1(0.f, 0.f, 0.f), position2(0.f, 0.f, 0.f);

    if (singlePoint)
    {
        const float xSect1(x01 + rad1 * std::cos(phi1));
        const float ySect1(y01 + rad1 * std::sin(phi1));
        const float xSect2(x02 + rad2 * std::cos(phi2));
        const float ySect2(y02 + rad2 * std::sin(phi2));
        const float r1(std::sqrt(xSect1 * xSect1 + ySect1 * ySect1));
        const float r2(std::sqrt(xSect2 * xSect2 + ySect2 * ySect2));

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetPointOnCircle(r1, referencePoint1, position1));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pHelix->GetPointOnCircle(r2, referencePoint2, position2));
    }
    else
    {
        const float xSect1(x02 + rad2*std::cos(phi1));
        const float ySect1(y02 + rad2*std::sin(phi1));
        const float xSect2(x02 + rad2*std::cos(phi2));
        const float ySect2(y02 + rad2*std::sin(phi2));

        const float phiI2(std::atan2(referencePoint2.GetY() - y02, referencePoint2.GetX() - x02));
        const float phiF21(std::atan2(ySect1 - y02, xSect1 - x02));
        const float phiF22(std::atan2(ySect2 - y02, xSect2 - x02));
        const float charge2(pHelix->GetCharge());

        float deltaPhi21(phiF21 - phiI2);
        float deltaPhi22(phiF22 - phiI2);

        if (deltaPhi21 < 0 && charge2 < 0)
        {
            deltaPhi21 += TWO_PI;
        }
        else if (deltaPhi21 > 0 && charge2 > 0)
        {
            deltaPhi21 -= TWO_PI;
        }

        if (deltaPhi22 < 0 && charge2 < 0)
        {
            deltaPhi22 += TWO_PI;
        }
        else if (deltaPhi22 > 0 && charge2 > 0)
        {
            deltaPhi22 -= TWO_PI;
        }

        const float pxy2(pHelix->GetPxy());
        const float genericTime21(-charge2 * deltaPhi21 * rad2 / pxy2);
        const float genericTime22(-charge2 * deltaPhi22 * rad2 / pxy2);

        const float pz2(pHelix->GetMomentum().GetZ());
        const float Z21(referencePoint2.GetZ() + genericTime21 * pz2);
        const float Z22(referencePoint2.GetZ() + genericTime22 * pz2);

        const CartesianVector temp21(xSect1, ySect1, Z21);
        const CartesianVector temp22(xSect2, ySect2, Z22);

        const float phiI1(std::atan2(referencePoint1.GetY() - y01, referencePoint1.GetX() - x01));
        const float phiF11(std::atan2(ySect1-y01,xSect1-x01));
        const float phiF12(std::atan2(ySect2-y01,xSect2-x01));
        const float charge1(m_charge);

        float deltaPhi11(phiF11 - phiI1);
        float deltaPhi12(phiF12 - phiI1);

        if (deltaPhi11 < 0 && charge1 < 0)
        {
            deltaPhi11 += TWO_PI;
        }
        else if (deltaPhi11 > 0 && charge1 > 0)
        {
            deltaPhi11 -= TWO_PI;
        }

        if (deltaPhi12 < 0 && charge1 < 0)
        {
            deltaPhi12 += TWO_PI;
        }
        else if (deltaPhi12 > 0 && charge1 > 0)
        {
            deltaPhi12 -= TWO_PI;
        }

        const float pxy1(m_pxy);
        const float genericTime11(-charge1 * deltaPhi11 * rad1 / pxy1);
        const float genericTime12(-charge1 * deltaPhi12 * rad1 / pxy1);

        const float pz1(m_momentum.GetZ());
        const float Z11(referencePoint1.GetZ() + genericTime11 * pz1);
        const float Z12(referencePoint1.GetZ() + genericTime12 * pz1);

        const CartesianVector temp11(xSect1, ySect1, Z11);
        const CartesianVector temp12(xSect2, ySect2, Z12);

        const float dist1((temp11 - temp21).GetMagnitudeSquared());
        const float dist2((temp12 - temp22).GetMagnitudeSquared());

        if (dist1 < dist2)
        {
            position1 = temp11;
            position2 = temp21;
        }
        else
        {
            position1 = temp12;
            position2 = temp22;
        }
    }

    const CartesianVector momentum1(this->GetExtrapolatedMomentum(position1));
    const CartesianVector momentum2(pHelix->GetExtrapolatedMomentum(position2));

    helixDistance = (position1 - position2).GetMagnitude();
    positionOfClosestApproach = (position1 + position2) * 0.5;
    v0momentum = momentum1 + momentum2;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

CartesianVector Helix::GetExtrapolatedMomentum(const CartesianVector &position) const
{
    float phi = std::atan2(position.GetY() - m_yCentre, position.GetX() - m_xCentre);

    if (phi < 0.)
        phi += TWO_PI;

    phi = phi - m_phiAtPCA + m_phi0;

    return CartesianVector(m_pxy * std::cos(phi), m_pxy * std::sin(phi), m_momentum.GetZ());
}

} // namespace pandora
