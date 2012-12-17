/**
 *  @file   PandoraPFANew/Framework/src/Helpers/ParticleIdHelper.cc
 * 
 *  @brief  Implementation of the particle id helper class.
 * 
 *  $Log: $
 */

#include "Helpers/GeometryHelper.h"
#include "Helpers/ParticleIdHelper.h"
#include "Helpers/XmlHelper.h"

#include "Objects/Cluster.h"
#include "Objects/Track.h"

#include "Utilities/ShowerProfileCalculator.h"

#include <cmath>
#include <limits>

namespace pandora
{

ParticleIdFunction *ParticleIdHelper::m_pEmShowerFastFunction = NULL;
ParticleIdFunction *ParticleIdHelper::m_pEmShowerFullFunction = NULL;
ParticleIdFunction *ParticleIdHelper::m_pPhotonFastFunction = NULL;
ParticleIdFunction *ParticleIdHelper::m_pPhotonFullFunction = NULL;
ParticleIdFunction *ParticleIdHelper::m_pElectronFastFunction = NULL;
ParticleIdFunction *ParticleIdHelper::m_pElectronFullFunction = NULL;
ParticleIdFunction *ParticleIdHelper::m_pMuonFastFunction = NULL;
ParticleIdFunction *ParticleIdHelper::m_pMuonFullFunction = NULL;

ShowerProfileCalculator *ParticleIdHelper::m_pShowerProfileCalculator = NULL;

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleIdHelper::IsEmShowerFast(const Cluster *const pCluster)
{
    if (NULL == m_pEmShowerFastFunction)
        return false;

    return (*m_pEmShowerFastFunction)(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleIdHelper::IsEmShowerFull(const Cluster *const pCluster)
{
    if (NULL == m_pEmShowerFullFunction)
        return false;

    return (*m_pEmShowerFullFunction)(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleIdHelper::IsPhotonFast(const Cluster *const pCluster)
{
    if (pCluster->IsFixedPhoton())
        return true;

    if (NULL == m_pPhotonFastFunction)
        return false;

    return (*m_pPhotonFastFunction)(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleIdHelper::IsPhotonFull(const Cluster *const pCluster)
{
    if (pCluster->IsFixedPhoton())
        return true;

    if (NULL == m_pPhotonFullFunction)
        return false;

    return (*m_pPhotonFullFunction)(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleIdHelper::IsElectronFast(const Cluster *const pCluster)
{
    if (pCluster->IsFixedElectron())
        return true;

    if (NULL == m_pElectronFastFunction)
        return false;

    return (*m_pElectronFastFunction)(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleIdHelper::IsElectronFull(const Cluster *const pCluster)
{
    if (pCluster->IsFixedElectron())
        return true;

    if (NULL == m_pElectronFullFunction)
        return false;

    return (*m_pElectronFullFunction)(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleIdHelper::IsMuonFast(const Cluster *const pCluster)
{
    if (pCluster->IsFixedMuon())
        return true;

    if (NULL == m_pMuonFastFunction)
        return false;

    return (*m_pMuonFastFunction)(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleIdHelper::IsMuonFull(const Cluster *const pCluster)
{
    if (pCluster->IsFixedMuon())
        return true;

   if (NULL == m_pMuonFullFunction)
        return false;

    return (*m_pMuonFullFunction)(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void ParticleIdHelper::CalculateLongitudinalProfile(const Cluster *const pCluster, float &profileStart, float &profileDiscrepancy)
{
    profileStart = std::numeric_limits<float>::max();
    profileDiscrepancy = std::numeric_limits<float>::max();

    if (NULL == m_pShowerProfileCalculator)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    m_pShowerProfileCalculator->CalculateLongitudinalProfile(pCluster, profileStart, profileDiscrepancy);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void ParticleIdHelper::CalculateTransverseProfile(const Cluster *const pCluster, const PseudoLayer maxPseudoLayer, ShowerPeakList &showerPeakList)
{
    showerPeakList.clear();

    if (NULL == m_pShowerProfileCalculator)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    m_pShowerProfileCalculator->CalculateTransverseProfile(pCluster, maxPseudoLayer, showerPeakList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleIdHelper::SetShowerProfileCalculator(ShowerProfileCalculator *pShowerProfileCalculator)
{
    delete m_pShowerProfileCalculator;
    m_pShowerProfileCalculator = pShowerProfileCalculator;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleIdHelper::ReadSettings(const TiXmlHandle *const pXmlHandle)
{
    // Read main particle id helper settings
    /*TiXmlElement *pXmlElement(pXmlHandle->FirstChild("ParticleIdHelper").Element());

    if (NULL != pXmlElement)
    {
        const TiXmlHandle xmlHandle(pXmlElement);
    }*/

    // Read shower profile calculator settings
    TiXmlElement *pShowerProfileXmlElement(pXmlHandle->FirstChild("ShowerProfileCalculator").Element());

    if ((NULL != pShowerProfileXmlElement) && (NULL != m_pShowerProfileCalculator))
    {
        const TiXmlHandle xmlHandle(pShowerProfileXmlElement);
        m_pShowerProfileCalculator->ReadSettings(&xmlHandle);
    }

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
