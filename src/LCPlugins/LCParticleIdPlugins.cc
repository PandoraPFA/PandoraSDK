/**
 *  @file   LCContent/src/LCPlugins/LCParticleIdPlugins.cc
 * 
 *  @brief  Implementation of the lc particle id plugins class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCHelpers/ClusterHelper.h"

#include "LCPlugins/LCParticleIdPlugins.h"

using namespace pandora;

namespace lc_content
{

LCParticleIdPlugins::LCEmShowerId::LCEmShowerId() :
    m_mipCut_0(0.9f),
    m_mipCutEnergy_1(15.f),
    m_mipCut_1(0.3f),
    m_mipCutEnergy_2(7.5f),
    m_mipCut_2(0.4f),
    m_mipCutEnergy_3(3.f),
    m_mipCut_3(0.6f),
    m_mipCutEnergy_4(1.5f),
    m_mipCut_4(0.7f),
    m_dCosRCutEnergy(1.5f),
    m_dCosRLowECut(0.94f),
    m_dCosRHighECut(0.95f),
    m_rmsCutEnergy(40.f),
    m_rmsLowECut(40.f),
    m_rmsHighECut(90.f),
    m_minCosAngle(0.3f),
    m_maxInnerLayerRadLengths(10.f),
    m_minLayer90RadLengths(4.f),
    m_maxLayer90RadLengths(30.f),
    m_minShowerMaxRadLengths(0.f),
    m_maxShowerMaxRadLengths(25.f),
    m_highRadLengths(40.f),
    m_maxHighRadLengthEnergyFraction(0.04f),
    m_maxRadial90(40.f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool LCParticleIdPlugins::LCEmShowerId::IsMatch(const Cluster *const pCluster) const
{
    // Reject clusters starting outside inner fine granularity detectors
    if (this->GetPandora().GetGeometry()->GetHitTypeGranularity(pCluster->GetInnerLayerHitType()) > FINE)
        return false;

    // Cut on cluster mip fraction
    const float totalElectromagneticEnergy(pCluster->GetElectromagneticEnergy() - pCluster->GetIsolatedElectromagneticEnergy());

    float mipCut(m_mipCut_0);

    if (totalElectromagneticEnergy > m_mipCutEnergy_1)
    {
        mipCut = m_mipCut_1;
    }
    else if (totalElectromagneticEnergy > m_mipCutEnergy_2)
    {
        mipCut = m_mipCut_2;
    }
    else if (totalElectromagneticEnergy > m_mipCutEnergy_3)
    {
        mipCut = m_mipCut_3;
    }
    else if (totalElectromagneticEnergy > m_mipCutEnergy_4)
    {
        mipCut = m_mipCut_4;
    }

    if (pCluster->GetMipFraction() > mipCut)
        return false;

    // Cut on results of fit to all hits in cluster
    float dCosR(0.f);
    float clusterRms(0.f);

    const ClusterFitResult &clusterFitResult(pCluster->GetFitToAllHitsResult());

    if (clusterFitResult.IsFitSuccessful())
    {
        dCosR = clusterFitResult.GetRadialDirectionCosine();
        clusterRms = clusterFitResult.GetRms();
    }

    const float dCosRCut(totalElectromagneticEnergy < m_dCosRCutEnergy ? m_dCosRLowECut : m_dCosRHighECut);

    if (dCosR < dCosRCut)
        return false;

    const float rmsCut(totalElectromagneticEnergy < m_rmsCutEnergy ? m_rmsLowECut : m_rmsHighECut);

    if (clusterRms > rmsCut)
        return false;

    const CartesianVector &clusterDirection(pCluster->GetFitToAllHitsResult().IsFitSuccessful() ?
        pCluster->GetFitToAllHitsResult().GetDirection() : pCluster->GetInitialDirection());

    const CartesianVector &clusterIntercept(pCluster->GetFitToAllHitsResult().IsFitSuccessful() ?
        pCluster->GetFitToAllHitsResult().GetIntercept() : CartesianVector(0.f, 0.f, 0.f));

    // Calculate properties of longitudinal shower profile: layer90 and shower max layer
    bool foundLayer90(false);
    float layer90EnergySum(0.f), showerMaxRadLengths(0.f), energyAboveHighRadLengths(0.f);
    float nRadiationLengths(0.f), nRadiationLengthsInLastLayer(0.f), maxEnergyInlayer(0.f);
    HitEnergyDistanceVector hitEnergyDistanceVector;

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());
    const unsigned int innerPseudoLayer(pCluster->GetInnerPseudoLayer());
    const unsigned int firstPseudoLayer(this->GetPandora().GetPlugins()->GetPseudoLayerPlugin()->GetPseudoLayerAtIp());

    for (unsigned int iLayer = innerPseudoLayer, outerPseudoLayer = pCluster->GetOuterPseudoLayer(); iLayer <= outerPseudoLayer; ++iLayer)
    {
        OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(iLayer);

        if ((orderedCaloHitList.end() == iter) || (iter->second->empty()))
        {
            nRadiationLengths += nRadiationLengthsInLastLayer;
            continue;
        }

        // Extract information from the calo hits
        float energyInLayer(0.f);
        float nRadiationLengthsInLayer(0.f);

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            float cosOpeningAngle(std::fabs((*hitIter)->GetCellNormalVector().GetCosOpeningAngle(clusterDirection)));
            cosOpeningAngle = std::max(cosOpeningAngle, m_minCosAngle);

            const float hitEnergy((*hitIter)->GetElectromagneticEnergy());
            energyInLayer += hitEnergy;
            nRadiationLengthsInLayer += (*hitIter)->GetNCellRadiationLengths() / cosOpeningAngle;

            const float radialDistance(((*hitIter)->GetPositionVector() - clusterIntercept).GetCrossProduct(clusterDirection).GetMagnitude());
            hitEnergyDistanceVector.push_back(HitEnergyDistance(hitEnergy, radialDistance));
        }

        layer90EnergySum += energyInLayer;
        nRadiationLengthsInLayer /= static_cast<float>(iter->second->size());
        nRadiationLengthsInLastLayer = nRadiationLengthsInLayer;
        nRadiationLengths += nRadiationLengthsInLayer;

        // Cut on number of radiation lengths before cluster start
        if (innerPseudoLayer == iLayer)
        {
            nRadiationLengths *= static_cast<float>(innerPseudoLayer + 1 - firstPseudoLayer);

            if (nRadiationLengths > m_maxInnerLayerRadLengths)
                return false;
        }

        // Cut on number of radiation lengths before longitudinal layer90
        if (!foundLayer90 && (layer90EnergySum > 0.9f * totalElectromagneticEnergy))
        {
            foundLayer90 = true;

            if ((nRadiationLengths < m_minLayer90RadLengths) || (nRadiationLengths > m_maxLayer90RadLengths))
                return false;
        }

        // Identify number of radiation lengths before shower max layer
        if (energyInLayer > maxEnergyInlayer)
        {
            showerMaxRadLengths = nRadiationLengths;
            maxEnergyInlayer = energyInLayer;
        }

        // Count energy above specified "high" number of radiation lengths
        if (nRadiationLengths > m_highRadLengths)
        {
            energyAboveHighRadLengths += energyInLayer;
        }
    }

    // Cut on longitudinal shower profile properties
    if ((showerMaxRadLengths < m_minShowerMaxRadLengths) || (showerMaxRadLengths > m_maxShowerMaxRadLengths))
        return false;

    if (energyAboveHighRadLengths > m_maxHighRadLengthEnergyFraction * totalElectromagneticEnergy)
        return false;

    // Cut on transverse shower profile properties
    std::sort(hitEnergyDistanceVector.begin(), hitEnergyDistanceVector.end(), LCEmShowerId::SortHitsByDistance);
    float radial90EnergySum(0.f), radial90(std::numeric_limits<float>::max());

    for (HitEnergyDistanceVector::const_iterator iter = hitEnergyDistanceVector.begin(), iterEnd = hitEnergyDistanceVector.end(); iter != iterEnd; ++iter)
    {
        radial90EnergySum += iter->first;

        if (radial90EnergySum > 0.9f * totalElectromagneticEnergy)
        {
            radial90 = iter->second;
            break;
        }
    }

    if (radial90 > m_maxRadial90)
        return false;

    // Anything remaining at this point is classed as an electromagnetic shower
    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode LCParticleIdPlugins::LCEmShowerId::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MipCut_0", m_mipCut_0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MipCutEnergy_1", m_mipCutEnergy_1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MipCut_1", m_mipCut_1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MipCutEnergy_2", m_mipCutEnergy_2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MipCut_2", m_mipCut_2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MipCutEnergy_3", m_mipCutEnergy_3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MipCut_3", m_mipCut_3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MipCutEnergy_4", m_mipCutEnergy_4));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MipCut_4", m_mipCut_4));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DCosRCutEnergy", m_dCosRCutEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DCosRLowECut", m_dCosRLowECut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DCosRHighECut", m_dCosRHighECut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "RmsCutEnergy", m_rmsCutEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "RmsLowECut", m_rmsLowECut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "RmsHighECut", m_rmsHighECut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCosAngle", m_minCosAngle));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxInnerLayerRadLengths", m_maxInnerLayerRadLengths));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinLayer90RadLengths", m_minLayer90RadLengths));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxLayer90RadLengths", m_maxLayer90RadLengths));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinShowerMaxRadLengths", m_minShowerMaxRadLengths));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxShowerMaxRadLengths", m_maxShowerMaxRadLengths));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HighRadLengths", m_highRadLengths));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxHighRadLengthEnergyFraction", m_maxHighRadLengthEnergyFraction));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxRadial90", m_maxRadial90));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

LCParticleIdPlugins::LCPhotonId::LCPhotonId()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool LCParticleIdPlugins::LCPhotonId::IsMatch(const Cluster *const pCluster) const
{
    // Cluster with associated tracks is not a photon
    if (!pCluster->GetAssociatedTrackList().empty())
        return false;

    return this->GetPandora().GetPlugins()->GetParticleId()->IsEmShower(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode LCParticleIdPlugins::LCPhotonId::ReadSettings(const TiXmlHandle /*xmlHandle*/)
{
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

LCParticleIdPlugins::LCElectronId::LCElectronId() :
    m_maxInnerLayer(4),
    m_maxEnergy(5.f),
    m_maxProfileStart(4.5f),
    m_maxProfileDiscrepancy(0.6f),
    m_profileDiscrepancyForAutoId(0.5f),
    m_maxResidualEOverP(0.2f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool LCParticleIdPlugins::LCElectronId::IsMatch(const Cluster *const pCluster) const
{
    const TrackList &associatedTrackList(pCluster->GetAssociatedTrackList());

    if (associatedTrackList.empty())
        return false;

    const float electromagneticEnergy(pCluster->GetElectromagneticEnergy());

    if (!this->GetPandora().GetPlugins()->GetParticleId()->IsEmShower(pCluster) &&
        ((pCluster->GetInnerPseudoLayer() > m_maxInnerLayer) || (electromagneticEnergy > m_maxEnergy)))
    {
        return false;
    }

    const float showerProfileStart(pCluster->GetShowerProfileStart(this->GetPandora()));
    const float showerProfileDiscrepancy(pCluster->GetShowerProfileDiscrepancy(this->GetPandora()));

    if ((showerProfileStart > m_maxProfileStart) || (showerProfileDiscrepancy > m_maxProfileDiscrepancy))
        return false;

    if (showerProfileDiscrepancy < m_profileDiscrepancyForAutoId)
        return true;

    for (TrackList::const_iterator iter = associatedTrackList.begin(), iterEnd = associatedTrackList.end(); iter != iterEnd; ++iter)
    {
        const float momentumAtDca((*iter)->GetMomentumAtDca().GetMagnitude());

        if (momentumAtDca < std::numeric_limits<float>::epsilon())
            throw StatusCodeException(STATUS_CODE_FAILURE);

        const float eOverP(electromagneticEnergy / momentumAtDca);

        if (std::fabs(eOverP - 1.f) < m_maxResidualEOverP)
            return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode LCParticleIdPlugins::LCElectronId::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxInnerLayer", m_maxInnerLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxEnergy", m_maxEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxProfileStart", m_maxProfileStart));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxProfileDiscrepancy", m_maxProfileDiscrepancy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ProfileDiscrepancyForAutoId", m_profileDiscrepancyForAutoId));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxResidualEOverP", m_maxResidualEOverP));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

LCParticleIdPlugins::LCMuonId::LCMuonId() :
    m_maxInnerLayer(10),
    m_minTrackEnergy(2.5f),
    m_maxHCalHitEnergy(1.f),
    m_minECalLayers(20),
    m_minHCalLayers(20),
    m_shouldPerformGapCheck(true),
    m_minHCalLayersForGapCheck(4),
    m_minMuonHitsForGapCheck(3),
    m_eCalEnergyCut0(1.f),
    m_eCalEnergyCut1(0.05f),
    m_hCalEnergyCut0(5.f),
    m_hCalEnergyCut1(0.15f),
    m_minECalLayersForFit(10),
    m_minHCalLayersForFit(10),
    m_minMuonLayersForFit(4),
    m_eCalFitInnerLayer(1),
    m_eCalFitOuterLayer(30),
    m_hCalFitInnerLayer(31),
    m_hCalFitOuterLayer(79),
    m_eCalRmsCut0(20.f),
    m_eCalRmsCut1(0.2f),
    m_eCalMaxRmsCut(50.f),
    m_hCalRmsCut0(40.f),
    m_hCalRmsCut1(0.1f),
    m_hCalMaxRmsCut(80.f),
    m_eCalMipFractionCut0(0.8f),
    m_eCalMipFractionCut1(0.05f),
    m_eCalMaxMipFractionCut(0.6f),
    m_hCalMipFractionCut0(0.8f),
    m_hCalMipFractionCut1(0.02f),
    m_hCalMaxMipFractionCut(0.75f),
    m_eCalHitsPerLayerCut0(2.f),
    m_eCalHitsPerLayerCut1(0.02f),
    m_eCalMaxHitsPerLayerCut(5.f),
    m_hCalHitsPerLayerCut0(1.5f),
    m_hCalHitsPerLayerCut1(0.05f),
    m_hCalMaxHitsPerLayerCut(8.f),
    m_curlingTrackEnergy(4.f),
    m_inBarrelHitFraction(0.05f),
    m_tightMipFractionCut(0.85f),
    m_tightMipFractionECalCut(0.8f),
    m_tightMipFractionHCalCut(0.875f),
    m_minMuonHitsCut(2),
    m_minMuonTrackSegmentHitsCut(8),
    m_muonRmsCut(25.f),
    m_maxMuonHitsCut0(0.f),
    m_maxMuonHitsCut1(0.2f),
    m_maxMuonHitsCutMinValue(30.f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool LCParticleIdPlugins::LCMuonId::IsMatch(const Cluster *const pCluster) const
{
    // Simple pre-selection cuts
    if (pCluster->GetInnerPseudoLayer() > m_maxInnerLayer)
        return false;

    const TrackList &trackList(pCluster->GetAssociatedTrackList());

    if (trackList.size() != 1)
        return false;

    // For now only try to identify "high" energy muons
    const Track *const pTrack = *(trackList.begin());

    if (pTrack->GetEnergyAtDca() < m_minTrackEnergy)
        return false;

    // Calculate cut variables
    unsigned int nECalHits(0), nHCalHits(0), nMuonHits(0), nECalMipHits(0), nHCalMipHits(0), nHCalEndCapHits(0), nHCalBarrelHits(0);
    float energyECal(0.), energyHCal(0.), directionCosine(0.);

    typedef std::set<unsigned int> LayerList;
    LayerList pseudoLayersECal, pseudoLayersHCal, pseudoLayersMuon, layersECal, layersHCal;

    const CartesianVector &momentum(pTrack->GetTrackStateAtCalorimeter().GetMomentum());
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const CaloHit *const pCaloHit = *hitIter;

            const unsigned int pseudoLayer(pCaloHit->GetPseudoLayer());
            const unsigned int layer(pCaloHit->GetLayer());

            if (pCaloHit->GetHitType() == ECAL)
            {
                nECalHits++;

                if (pCaloHit->IsPossibleMip())
                    nECalMipHits++;

                pseudoLayersECal.insert(pseudoLayer);
                layersECal.insert(layer);

                energyECal += pCaloHit->GetHadronicEnergy();
                directionCosine += momentum.GetCosOpeningAngle(pCaloHit->GetCellNormalVector());
            }

            else if (pCaloHit->GetHitType() == HCAL)
            {
                nHCalHits++;

                if(pCaloHit->IsPossibleMip())
                    nHCalMipHits++;

                if (pCaloHit->GetHitRegion() == BARREL)
                    nHCalBarrelHits++;

                if (pCaloHit->GetHitRegion() == ENDCAP)
                    nHCalEndCapHits++;

                pseudoLayersHCal.insert(pseudoLayer);
                layersHCal.insert(layer);

                energyHCal += std::min(m_maxHCalHitEnergy, pCaloHit->GetHadronicEnergy());
            }

            else if (pCaloHit->GetHitType() == MUON)
            {
                nMuonHits++;
                pseudoLayersMuon.insert(pseudoLayer);
            }
        }
    }

    // Simple cuts on layer counts
    const unsigned int nPseudoLayersECal(pseudoLayersECal.size());
    const unsigned int nPseudoLayersHCal(pseudoLayersHCal.size());

    if ((nPseudoLayersECal < m_minECalLayers) && (layersECal.size() < m_minECalLayers))
        return false;

    if ((nPseudoLayersHCal < m_minHCalLayers) && (layersHCal.size() < m_minHCalLayers))
    {
        if (!m_shouldPerformGapCheck || (nPseudoLayersHCal < m_minHCalLayersForGapCheck) || (nMuonHits < m_minMuonHitsForGapCheck))
            return false;

        if (!ClusterHelper::DoesClusterCrossGapRegion(this->GetPandora(), pCluster, *(pseudoLayersHCal.begin()), *(pseudoLayersHCal.rbegin())))
            return false;
    }

    // Calculate energies per layer
    float energyECalDCos(0.), nHitsPerLayerECal(0.), nHitsPerLayerHCal(0.), mipFractionECal(0.), mipFractionHCal(0.);

    if ((nECalHits > 0) && (nPseudoLayersECal > 0))
    {
        directionCosine /= static_cast<float>(nECalHits);
        energyECalDCos = energyECal * directionCosine;

        mipFractionECal = static_cast<float>(nECalMipHits) / static_cast<float>(nECalHits);
        nHitsPerLayerECal = static_cast<float>(nECalHits) / static_cast<float>(nPseudoLayersECal);
    }

    if ((nHCalHits > 0) && (nPseudoLayersHCal > 0))
    {
        mipFractionHCal = static_cast<float>(nHCalMipHits) / static_cast<float>(nHCalHits);
        nHitsPerLayerHCal = static_cast<float>(nHCalHits) / static_cast<float>(nPseudoLayersHCal);
    }

    // Loose energy cuts
    const float trackEnergy(pTrack->GetEnergyAtDca());
    const float eCalEnergyCut(m_eCalEnergyCut0 + (m_eCalEnergyCut1 * trackEnergy));
    const float hCalEnergyCut(m_hCalEnergyCut0 + (m_hCalEnergyCut1 * trackEnergy));

    if ((energyECalDCos > eCalEnergyCut) || (energyHCal > hCalEnergyCut))
        return false;

    // Calculate event shape variables for ecal
    float eCalRms(std::numeric_limits<float>::max());
    int nECalCutsFailed(0);

    if (nPseudoLayersECal > m_minECalLayersForFit)
    {
        ClusterFitResult newFitResult;
        ClusterFitHelper::FitLayers(pCluster, m_eCalFitInnerLayer, m_eCalFitOuterLayer, newFitResult);

        if (newFitResult.IsFitSuccessful())
            eCalRms = newFitResult.GetRms();
    }

    const float rmsECalCut(std::min(m_eCalRmsCut0 + (trackEnergy * m_eCalRmsCut1), m_eCalMaxRmsCut));

    if (eCalRms > rmsECalCut)
        nECalCutsFailed++;

    const float mipFractionECalCut(std::min(m_eCalMipFractionCut0 - (trackEnergy * m_eCalMipFractionCut1), m_eCalMaxMipFractionCut));

    if (mipFractionECal < mipFractionECalCut)
        nECalCutsFailed++;

    const float nHitsPerLayerECalCut(std::min(m_eCalHitsPerLayerCut0 + (trackEnergy * m_eCalHitsPerLayerCut1), m_eCalMaxHitsPerLayerCut));

    if (nHitsPerLayerECal > nHitsPerLayerECalCut)
        nECalCutsFailed++;

    // Calculate event shape variables for hcal
    // ATTN rms cut should be made wrt Kalman filter fit: cut makes no sense for tracks which loop back in hcal barrel
    float hCalRms(std::numeric_limits<float>::max());
    int nHCalCutsFailed(0);

    if (nPseudoLayersHCal > m_minHCalLayersForFit)
    {
        ClusterFitResult newFitResult;
        ClusterFitHelper::FitLayers(pCluster, m_hCalFitInnerLayer, m_hCalFitOuterLayer, newFitResult);

        if (newFitResult.IsFitSuccessful())
            hCalRms = newFitResult.GetRms();
    }

    const bool inBarrel((nHCalEndCapHits == 0) ||
        (static_cast<float>(nHCalBarrelHits) / static_cast<float>(nHCalBarrelHits + nHCalEndCapHits) >= m_inBarrelHitFraction));

    float rmsHCalCut(std::min(m_hCalRmsCut0 + (trackEnergy * m_hCalRmsCut1), m_hCalMaxRmsCut));

    if ((trackEnergy < m_curlingTrackEnergy) && inBarrel)
        rmsHCalCut = m_hCalMaxRmsCut;

    if (hCalRms > rmsHCalCut)
    {
        // Impose tight cuts on MipFraction
        if ((trackEnergy > m_curlingTrackEnergy) || !inBarrel)
        {
            nHCalCutsFailed++;
        }
        else if ((pCluster->GetMipFraction() < m_tightMipFractionCut) || (mipFractionECal < m_tightMipFractionECalCut) ||
                (mipFractionHCal < m_tightMipFractionHCalCut))
        {
            nHCalCutsFailed++;
        }
    }

    const float mipFractionHCalCut(std::min(m_hCalMipFractionCut0 - (trackEnergy * m_hCalMipFractionCut1), m_hCalMaxMipFractionCut));

    if (mipFractionHCal < mipFractionHCalCut)
        nHCalCutsFailed++;

    const float nHitsPerlayerHCalCut(std::min(m_hCalHitsPerLayerCut0 + (trackEnergy * m_hCalHitsPerLayerCut1), m_hCalMaxHitsPerLayerCut));

    if (nHitsPerLayerHCal > nHitsPerlayerHCalCut)
        nHCalCutsFailed++;

    // Calculate event shape variables for muon
    float muonRms(std::numeric_limits<float>::max());
    int nMuonCutsPassed(0);

    if (pseudoLayersMuon.size() > m_minMuonLayersForFit)
    { 
        ClusterFitResult newFitResult;
        ClusterFitHelper::FitLayers(pCluster, *pseudoLayersMuon.begin(), *pseudoLayersMuon.rbegin(), newFitResult);

        if (newFitResult.IsFitSuccessful())
            muonRms = newFitResult.GetRms();
    }

    const float maxMuonHitsCut(std::max(m_maxMuonHitsCut0 + (m_maxMuonHitsCut1 * trackEnergy), m_maxMuonHitsCutMinValue));

    if ((nMuonHits > m_minMuonHitsCut) && (nMuonHits < maxMuonHitsCut))
        nMuonCutsPassed++;

    if ((nMuonHits > m_minMuonTrackSegmentHitsCut) && (muonRms < m_muonRmsCut))
        nMuonCutsPassed++;

    // Make final decision
    const int nCutsFailed(nECalCutsFailed + nHCalCutsFailed - nMuonCutsPassed);

    return (nCutsFailed <= 0);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode LCParticleIdPlugins::LCMuonId::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxInnerLayer", m_maxInnerLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinTrackEnergy", m_minTrackEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxHCalHitEnergy", m_maxHCalHitEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinECalLayers", m_minECalLayers));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHCalLayers", m_minHCalLayers));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldPerformGapCheck", m_shouldPerformGapCheck));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHCalLayersForGapCheck", m_minHCalLayersForGapCheck));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinMuonHitsForGapCheck", m_minMuonHitsForGapCheck));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ECalEnergyCut0", m_eCalEnergyCut0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ECalEnergyCut1", m_eCalEnergyCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HCalEnergyCut0", m_hCalEnergyCut0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HCalEnergyCut1", m_hCalEnergyCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinECalLayersForFit", m_minECalLayersForFit));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHCalLayersForFit", m_minHCalLayersForFit));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinMuonLayersForFit", m_minMuonLayersForFit));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ECalFitInnerLayer", m_eCalFitInnerLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ECalFitOuterLayer", m_eCalFitOuterLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HCalFitInnerLayer", m_hCalFitInnerLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HCalFitOuterLayer", m_hCalFitOuterLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ECalRmsCut0", m_eCalRmsCut0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ECalRmsCut1", m_eCalRmsCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ECalMaxRmsCut", m_eCalMaxRmsCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HCalRmsCut0", m_hCalRmsCut0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HCalRmsCut1", m_hCalRmsCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HCalMaxRmsCut", m_hCalMaxRmsCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ECalMipFractionCut0", m_eCalMipFractionCut0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ECalMipFractionCut1", m_eCalMipFractionCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ECalMaxMipFractionCut", m_eCalMaxMipFractionCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HCalMipFractionCut0", m_hCalMipFractionCut0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HCalMipFractionCut1", m_hCalMipFractionCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HCalMaxMipFractionCut", m_hCalMaxMipFractionCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ECalHitsPerLayerCut0", m_eCalHitsPerLayerCut0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ECalHitsPerLayerCut1", m_eCalHitsPerLayerCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ECalMaxHitsPerLayerCut", m_eCalMaxHitsPerLayerCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HCalHitsPerLayerCut0", m_hCalHitsPerLayerCut0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HCalHitsPerLayerCut1", m_hCalHitsPerLayerCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HCalMaxHitsPerLayerCut", m_hCalMaxHitsPerLayerCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CurlingTrackEnergy", m_curlingTrackEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "InBarrelHitFraction", m_inBarrelHitFraction));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TightMipFractionCut", m_tightMipFractionCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TightMipFractionECalCut", m_tightMipFractionECalCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TightMipFractionHCalCut", m_tightMipFractionHCalCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinMuonHitsCut", m_minMuonHitsCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinMuonTrackSegmentHitsCut", m_minMuonTrackSegmentHitsCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonRmsCut", m_muonRmsCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxMuonHitsCut0", m_maxMuonHitsCut0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxMuonHitsCut1", m_maxMuonHitsCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxMuonHitsCutMinValue", m_maxMuonHitsCutMinValue));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
