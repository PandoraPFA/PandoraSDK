/**
 *  @file   LCContent/src/LCParticleId/PhotonRecoveryAlgorithm.cc
 * 
 *  @brief  Implementation of the photon recovery algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCParticleId/PhotonRecoveryAlgorithm.h"

using namespace pandora;

namespace lc_content
{

PhotonRecoveryAlgorithm::PhotonRecoveryAlgorithm() :
    m_minElectromagneticEnergy(1.5f),
    m_maxInnerLayer(14),
    m_profileStartCut1(4.1f),
    m_profileStartEnergyCut(5.f),
    m_profileStartCut2(5.1f),
    m_profileStartCut3(2.75f),
    m_profileDiscrepancyCut1(0.4f),
    m_profileDiscrepancyEnergyCut(2.5f),
    m_profileDiscrepancyCutParameter1(0.5f),
    m_profileDiscrepancyCutParameter2(0.02f),
    m_minProfileDiscrepancy(0.f),
    m_profileDiscrepancyCut2(0.5f),
    m_maxOverlapInnerLayer(10),
    m_maxOverlapMipFraction(0.5f),
    m_minOverlapRadialDirectionCosine(0.9f),
    m_maxBarrelEndCapSplit(0.9f),
    m_softPhotonMinCaloHits(0),
    m_softPhotonMaxCaloHits(25),
    m_softPhotonMaxEnergy(1.f),
    m_softPhotonMaxInnerLayer(15),
    m_softPhotonMaxDCosR(0.9f),
    m_softPhotonLowEnergyCut(0.5f),
    m_softPhotonLowEnergyMaxDCosR(0.8f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonRecoveryAlgorithm::Run()
{
    // Begin by recalculating track-cluster associations
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackClusterAssociationAlgName));

    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

    this->FindPhotonsIdentifiedAsHadrons(pClusterList);
    this->PerformSoftPhotonId(pClusterList);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PhotonRecoveryAlgorithm::FindPhotonsIdentifiedAsHadrons(const ClusterList *const pClusterList) const
{
    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        // Consider only plausible photon candidates, currently identified as hadrons
        if (!pCluster->GetAssociatedTrackList().empty())
            continue;

        if (pCluster->IsPhotonFast(this->GetPandora()))
            continue;

        // Apply simple initial cuts
        const float electroMagneticEnergy(pCluster->GetElectromagneticEnergy());

        if (electroMagneticEnergy < m_minElectromagneticEnergy)
            continue;

        const unsigned int innerPseudoLayer(pCluster->GetInnerPseudoLayer());

        if ((innerPseudoLayer >= m_maxInnerLayer) || (PandoraContentApi::GetGeometry(*this)->GetHitTypeGranularity(pCluster->GetInnerLayerHitType()) > FINE))
            continue;

        // Cut on cluster shower profile properties
        bool isPhoton(false);
        const float showerProfileStart(pCluster->GetShowerProfileStart(this->GetPandora()));
        const float showerProfileDiscrepancy(pCluster->GetShowerProfileDiscrepancy(this->GetPandora()));

        float profileStartCut(m_profileStartCut1);

        if (electroMagneticEnergy > m_profileStartEnergyCut)
        {
            profileStartCut = m_profileStartCut2;
        }

        float profileDiscrepancyCut(m_profileDiscrepancyCut1);

        if (electroMagneticEnergy > m_profileDiscrepancyEnergyCut)
        {
            profileDiscrepancyCut = m_profileDiscrepancyCutParameter1 - m_profileDiscrepancyCutParameter2 * showerProfileStart;
        }

        if ((showerProfileStart < profileStartCut) && (showerProfileDiscrepancy < profileDiscrepancyCut) &&
            (showerProfileDiscrepancy > m_minProfileDiscrepancy))
        {
            isPhoton = true;
        }
        else if ((showerProfileDiscrepancy > m_minProfileDiscrepancy) && (showerProfileDiscrepancy < m_profileDiscrepancyCut2) &&
            (showerProfileStart < m_profileStartCut3))
        {
            isPhoton = true;
        }

        // Check barrel-endcap overlap
        if (!isPhoton)
        {
            const ClusterFitResult &clusterFitResult(pCluster->GetFitToAllHitsResult());

            if ((innerPseudoLayer < m_maxOverlapInnerLayer) &&
                (pCluster->GetMipFraction() - m_maxOverlapMipFraction < std::numeric_limits<float>::epsilon()) &&
                (clusterFitResult.IsFitSuccessful()) && (clusterFitResult.GetRadialDirectionCosine() > m_minOverlapRadialDirectionCosine) &&
                (this->GetBarrelEndCapEnergySplit(pCluster) < m_maxBarrelEndCapSplit))
            {
                isPhoton = true;
            }
        }

        // Tag the cluster as a fixed photon
        if (isPhoton)
        {
            pCluster->SetIsFixedPhotonFlag(true);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

float PhotonRecoveryAlgorithm::GetBarrelEndCapEnergySplit(const Cluster *const pCluster) const
{
    float barrelEnergy(0.f), endCapEnergy(0.f);
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const HitRegion hitRegion((*hitIter)->GetHitRegion());

            if (hitRegion == BARREL)
            {
                barrelEnergy += (*hitIter)->GetElectromagneticEnergy();
            }
            else if (hitRegion == ENDCAP)
            {
                endCapEnergy += (*hitIter)->GetElectromagneticEnergy();
            }
        }
    }

    const float totalEnergy(barrelEnergy + endCapEnergy);

    if (totalEnergy > 0.f)
    {
        return std::max(barrelEnergy / totalEnergy, endCapEnergy / totalEnergy);
    }

    throw StatusCodeException(STATUS_CODE_NOT_FOUND);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PhotonRecoveryAlgorithm::PerformSoftPhotonId(const ClusterList *const pClusterList) const
{
    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        if (this->IsSoftPhoton(pCluster))
            pCluster->SetIsFixedPhotonFlag(true);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonRecoveryAlgorithm::IsSoftPhoton(Cluster *const pCluster) const
{
    const unsigned int nCaloHits(pCluster->GetNCaloHits());

    if ((nCaloHits <= m_softPhotonMinCaloHits) || (nCaloHits >= m_softPhotonMaxCaloHits))
        return false;

    const float electromagneticEnergy(pCluster->GetElectromagneticEnergy());

    if (electromagneticEnergy > m_softPhotonMaxEnergy)
        return false;

    if (pCluster->GetInnerPseudoLayer() > m_softPhotonMaxInnerLayer)
        return false;

    const ClusterFitResult &clusterFitResult(pCluster->GetFitToAllHitsResult());

    if (clusterFitResult.IsFitSuccessful())
    {
        const float radialDirectionCosine(clusterFitResult.GetRadialDirectionCosine());

        if (radialDirectionCosine > m_softPhotonMaxDCosR)
            return true;

        if ((electromagneticEnergy < m_softPhotonLowEnergyCut) && (radialDirectionCosine > m_softPhotonLowEnergyMaxDCosR))
            return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonRecoveryAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessFirstAlgorithm(*this, xmlHandle, m_trackClusterAssociationAlgName));

    // Photons identified as hadrons
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinElectromagneticEnergy", m_minElectromagneticEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxInnerLayer", m_maxInnerLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ProfileStartCut1", m_profileStartCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ProfileStartEnergyCut", m_profileStartEnergyCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ProfileStartCut2", m_profileStartCut2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ProfileStartCut3", m_profileStartCut3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ProfileDiscrepancyCut1", m_profileDiscrepancyCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ProfileDiscrepancyEnergyCut", m_profileDiscrepancyEnergyCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ProfileDiscrepancyCutParameter1", m_profileDiscrepancyCutParameter1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ProfileDiscrepancyCutParameter2", m_profileDiscrepancyCutParameter2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinProfileDiscrepancy", m_minProfileDiscrepancy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ProfileDiscrepancyCut2", m_profileDiscrepancyCut2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxOverlapInnerLayer", m_maxOverlapInnerLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxOverlapMipFraction", m_maxOverlapMipFraction));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinOverlapRadialDirectionCosine", m_minOverlapRadialDirectionCosine));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxBarrelEndCapSplit", m_maxBarrelEndCapSplit));

    // Soft photon id
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "SoftPhotonMinCaloHits", m_softPhotonMinCaloHits));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "SoftPhotonMaxCaloHits", m_softPhotonMaxCaloHits));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "SoftPhotonMaxEnergy", m_softPhotonMaxEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "SoftPhotonMaxInnerLayer", m_softPhotonMaxInnerLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "SoftPhotonMaxDCosR", m_softPhotonMaxDCosR));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "SoftPhotonLowEnergyCut", m_softPhotonLowEnergyCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "SoftPhotonLowEnergyMaxDCosR", m_softPhotonLowEnergyMaxDCosR));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
