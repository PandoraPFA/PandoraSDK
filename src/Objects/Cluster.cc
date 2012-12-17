/**
 *  @file   PandoraPFANew/Framework/src/Objects/Cluster.cc
 * 
 *  @brief  Implementation of the cluster class.
 * 
 *  $Log: $
 */

#include "Helpers/EnergyCorrectionsHelper.h"
#include "Helpers/ParticleIdHelper.h"

#include "Objects/CaloHit.h"
#include "Objects/Cluster.h"
#include "Objects/Track.h"

namespace pandora
{

Cluster::Cluster(CaloHit *pCaloHit) :
    m_nCaloHits(0),
    m_nPossibleMipHits(0),
    m_electromagneticEnergy(0),
    m_hadronicEnergy(0),
    m_isolatedElectromagneticEnergy(0),
    m_isolatedHadronicEnergy(0),
    m_isFixedPhoton(false),
    m_isFixedElectron(false),
    m_isFixedMuon(false),
    m_isMipTrack(false),
    m_pTrackSeed(NULL),
    m_initialDirection(pCaloHit->GetExpectedDirection()),
    m_isDirectionUpToDate(true),
    m_isFitUpToDate(false),
    m_isAvailable(true)
{
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AddCaloHit(pCaloHit));
}

//------------------------------------------------------------------------------------------------------------------------------------------

Cluster::Cluster(CaloHitList *pCaloHitList) :
    m_nCaloHits(0),
    m_nPossibleMipHits(0),
    m_electromagneticEnergy(0),
    m_hadronicEnergy(0),
    m_isolatedElectromagneticEnergy(0),
    m_isolatedHadronicEnergy(0),
    m_isFixedPhoton(false),
    m_isFixedElectron(false),
    m_isFixedMuon(false),
    m_isMipTrack(false),
    m_pTrackSeed(NULL),
    m_initialDirection(0.f, 0.f, 0.f),
    m_isDirectionUpToDate(false),
    m_isFitUpToDate(false),
    m_isAvailable(true)
{
    if (pCaloHitList->empty())
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    for (CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AddCaloHit(*iter));

    this->CalculateInitialDirection();
}

//------------------------------------------------------------------------------------------------------------------------------------------

Cluster::Cluster(Track *pTrack) :
    m_nCaloHits(0),
    m_nPossibleMipHits(0),
    m_electromagneticEnergy(0),
    m_hadronicEnergy(0),
    m_isolatedElectromagneticEnergy(0),
    m_isolatedHadronicEnergy(0),
    m_isFixedPhoton(false),
    m_isFixedElectron(false),
    m_isFixedMuon(false),
    m_isMipTrack(true),
    m_pTrackSeed(pTrack),
    m_initialDirection(pTrack->GetTrackStateAtCalorimeter().GetMomentum().GetUnitVector()),
    m_isDirectionUpToDate(true),
    m_isFitUpToDate(false),
    m_isAvailable(true)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::AddCaloHit(CaloHit *const pCaloHit)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_orderedCaloHitList.Add(pCaloHit));

    this->ResetOutdatedProperties();

    m_nCaloHits++;

    if (pCaloHit->IsPossibleMip())
        m_nPossibleMipHits++;

    const float x(pCaloHit->GetPositionVector().GetX());
    const float y(pCaloHit->GetPositionVector().GetY());
    const float z(pCaloHit->GetPositionVector().GetZ());

    m_electromagneticEnergy += pCaloHit->GetElectromagneticEnergy();
    m_hadronicEnergy += pCaloHit->GetHadronicEnergy();

    const PseudoLayer pseudoLayer(pCaloHit->GetPseudoLayer());

    if (m_orderedCaloHitList[pseudoLayer]->size() > 1)
    {
        m_sumXByPseudoLayer[pseudoLayer] += x;
        m_sumYByPseudoLayer[pseudoLayer] += y;
        m_sumZByPseudoLayer[pseudoLayer] += z;
    }
    else
    {
        m_sumXByPseudoLayer[pseudoLayer] = x;
        m_sumYByPseudoLayer[pseudoLayer] = y;
        m_sumZByPseudoLayer[pseudoLayer] = z;
    }

    if (!m_innerPseudoLayer.IsInitialized() || (pseudoLayer < m_innerPseudoLayer.Get()))
        m_innerPseudoLayer = pseudoLayer;

    if (!m_outerPseudoLayer.IsInitialized() || (pseudoLayer > m_outerPseudoLayer.Get()))
        m_outerPseudoLayer = pseudoLayer;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::RemoveCaloHit(CaloHit *const pCaloHit)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_orderedCaloHitList.Remove(pCaloHit));

    if (m_orderedCaloHitList.empty())
        return this->ResetProperties();

    this->ResetOutdatedProperties();

    m_nCaloHits--;

    if (pCaloHit->IsPossibleMip())
        m_nPossibleMipHits--;

    const float x(pCaloHit->GetPositionVector().GetX());
    const float y(pCaloHit->GetPositionVector().GetY());
    const float z(pCaloHit->GetPositionVector().GetZ());

    m_electromagneticEnergy -= pCaloHit->GetElectromagneticEnergy();
    m_hadronicEnergy -= pCaloHit->GetHadronicEnergy();

    const PseudoLayer pseudoLayer(pCaloHit->GetPseudoLayer());

    if (m_orderedCaloHitList.end() != m_orderedCaloHitList.find(pseudoLayer))
    {
        m_sumXByPseudoLayer[pseudoLayer] -= x;
        m_sumYByPseudoLayer[pseudoLayer] -= y;
        m_sumZByPseudoLayer[pseudoLayer] -= z;
    }
    else
    {
        m_sumXByPseudoLayer.erase(pseudoLayer);
        m_sumYByPseudoLayer.erase(pseudoLayer);
        m_sumZByPseudoLayer.erase(pseudoLayer);
    }

    if (pseudoLayer <= m_innerPseudoLayer.Get())
        m_innerPseudoLayer = m_orderedCaloHitList.begin()->first;

    if (pseudoLayer >= m_outerPseudoLayer.Get())
        m_outerPseudoLayer = m_orderedCaloHitList.rbegin()->first;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::AddIsolatedCaloHit(CaloHit *const pCaloHit)
{
    if (!m_isolatedCaloHitList.insert(pCaloHit).second)
        return STATUS_CODE_ALREADY_PRESENT;

    const float electromagneticEnergy(pCaloHit->GetElectromagneticEnergy());
    const float hadronicEnergy(pCaloHit->GetHadronicEnergy());

    m_electromagneticEnergy += electromagneticEnergy;
    m_hadronicEnergy += hadronicEnergy;
    m_isolatedElectromagneticEnergy += electromagneticEnergy;
    m_isolatedHadronicEnergy += hadronicEnergy;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::RemoveIsolatedCaloHit(CaloHit *const pCaloHit)
{
    CaloHitList::iterator iter = m_isolatedCaloHitList.find(pCaloHit);

    if (m_isolatedCaloHitList.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    m_isolatedCaloHitList.erase(iter);

    const float electromagneticEnergy(pCaloHit->GetElectromagneticEnergy());
    const float hadronicEnergy(pCaloHit->GetHadronicEnergy());

    m_electromagneticEnergy -= electromagneticEnergy;
    m_hadronicEnergy -= hadronicEnergy;
    m_isolatedElectromagneticEnergy -= electromagneticEnergy;
    m_isolatedHadronicEnergy -= hadronicEnergy;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool Cluster::ContainsHitInOuterSamplingLayer() const
{
    for (OrderedCaloHitList::const_reverse_iterator iter = m_orderedCaloHitList.rbegin(), iterEnd = m_orderedCaloHitList.rend();
        iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd;
            ++hitIter)
        {
            if ((*hitIter)->IsInOuterSamplingLayer())
                return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool Cluster::ContainsHitType(const HitType hitType) const
{
    for (OrderedCaloHitList::const_reverse_iterator iter = m_orderedCaloHitList.rbegin(), iterEnd = m_orderedCaloHitList.rend();
        iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd;
            ++hitIter)
        {
            if (hitType == (*hitIter)->GetHitType())
                return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const CartesianVector Cluster::GetCentroid(const PseudoLayer pseudoLayer) const
{
    OrderedCaloHitList::const_iterator iter = m_orderedCaloHitList.find(pseudoLayer);

    if (m_orderedCaloHitList.end() == iter)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    const float nHitsInLayer(static_cast<float>(iter->second->size()));

    if (0 == nHitsInLayer)
        throw StatusCodeException(STATUS_CODE_FAILURE);

    ValueByPseudoLayerMap::const_iterator xValueIter = m_sumXByPseudoLayer.find(pseudoLayer);
    ValueByPseudoLayerMap::const_iterator yValueIter = m_sumYByPseudoLayer.find(pseudoLayer);
    ValueByPseudoLayerMap::const_iterator zValueIter = m_sumZByPseudoLayer.find(pseudoLayer);

    if ((m_sumXByPseudoLayer.end() == xValueIter) || (m_sumYByPseudoLayer.end() == yValueIter) || (m_sumZByPseudoLayer.end() == zValueIter))
        throw StatusCodeException(STATUS_CODE_FAILURE);

    return CartesianVector(static_cast<float>(xValueIter->second / nHitsInLayer), static_cast<float>(yValueIter->second / nHitsInLayer),
        static_cast<float>(zValueIter->second / nHitsInLayer));
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Cluster::PerformEnergyCorrections() const
{
    float correctedElectromagneticEnergy(0.f), correctedHadronicEnergy(0.f), trackComparisonEnergy(0.f);

    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, EnergyCorrectionsHelper::EnergyCorrection(this, correctedElectromagneticEnergy,
        correctedHadronicEnergy));

    if (ParticleIdHelper::IsEmShowerFast(this))
    {
        trackComparisonEnergy = correctedElectromagneticEnergy;
    }
    else
    {
        trackComparisonEnergy = correctedHadronicEnergy;
    }

    if (!(m_correctedElectromagneticEnergy = correctedElectromagneticEnergy) || !(m_correctedHadronicEnergy = correctedHadronicEnergy) ||
        !(m_trackComparisonEnergy = trackComparisonEnergy))
    {
        throw StatusCodeException(STATUS_CODE_FAILURE);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Cluster::CalculateFastPhotonFlag() const
{
    const bool fastPhotonFlag(ParticleIdHelper::IsPhotonFast(this));

    if (!(m_isPhotonFast = fastPhotonFlag))
        throw StatusCodeException(STATUS_CODE_FAILURE);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Cluster::CalculateShowerStartLayer() const
{
    const PseudoLayer showerStartLayer(ClusterHelper::GetShowerStartLayer(this));

    if (!(m_showerStartLayer = showerStartLayer))
        throw StatusCodeException(STATUS_CODE_FAILURE);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Cluster::CalculateShowerProfile() const
{
    float showerProfileStart(std::numeric_limits<float>::max()), showerProfileDiscrepancy(std::numeric_limits<float>::max());
    ParticleIdHelper::CalculateLongitudinalProfile(this, showerProfileStart, showerProfileDiscrepancy);

    if (!(m_showerProfileStart = showerProfileStart) || !(m_showerProfileDiscrepancy = showerProfileDiscrepancy))
        throw StatusCodeException(STATUS_CODE_FAILURE);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Cluster::CalculateFitToAllHitsResult() const
{
    (void) ClusterHelper::FitFullCluster(this, m_fitToAllHitsResult);
    m_isFitUpToDate = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Cluster::CalculateInitialDirection() const
{
    if (m_orderedCaloHitList.empty())
    {
        m_initialDirection.SetValues(0.f, 0.f, 0.f);
        m_isDirectionUpToDate = false;
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);
    }
    
    CartesianVector initialDirection(0.f, 0.f, 0.f);
    CaloHitList *pCaloHitList(m_orderedCaloHitList.begin()->second);

    for (CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
        initialDirection += (*iter)->GetExpectedDirection();

    m_initialDirection = initialDirection.GetUnitVector();
    m_isDirectionUpToDate = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Cluster::CalculateLayerHitType(const PseudoLayer pseudoLayer, InputHitType &layerHitType) const
{
    OrderedCaloHitList::const_iterator listIter = m_orderedCaloHitList.find(pseudoLayer);

    if ((m_orderedCaloHitList.end() == listIter) || (listIter->second->empty()))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    HitTypeToEnergyMap hitTypeToEnergyMap;

    for (CaloHitList::const_iterator iter = listIter->second->begin(), iterEnd = listIter->second->end(); iter != iterEnd; ++iter)
    {
        HitTypeToEnergyMap::iterator mapIter = hitTypeToEnergyMap.find((*iter)->GetHitType());

        if (hitTypeToEnergyMap.end() != mapIter)
        {
            mapIter->second += (*iter)->GetHadronicEnergy();
            continue;
        }

        if (!hitTypeToEnergyMap.insert(HitTypeToEnergyMap::value_type((*iter)->GetHitType(), (*iter)->GetHadronicEnergy())).second)
            throw StatusCodeException(STATUS_CODE_FAILURE);
    }

    float highestEnergy(0.f);

    for (HitTypeToEnergyMap::const_iterator iter = hitTypeToEnergyMap.begin(), iterEnd = hitTypeToEnergyMap.end(); iter != iterEnd; ++iter)
    {
        if (iter->second > highestEnergy)
        {
            layerHitType = iter->first;
            highestEnergy = iter->second;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::ResetProperties()
{
    if (!m_orderedCaloHitList.empty())
        m_orderedCaloHitList.Reset();

    m_isolatedCaloHitList.clear();

    m_nCaloHits = 0;
    m_nPossibleMipHits = 0;

    m_sumXByPseudoLayer.clear();
    m_sumYByPseudoLayer.clear();
    m_sumZByPseudoLayer.clear();

    m_electromagneticEnergy = 0;
    m_hadronicEnergy = 0;

    m_innerPseudoLayer.Reset();
    m_outerPseudoLayer.Reset();

    m_currentFitResult.Reset();

    m_isFixedPhoton = false;
    m_isFixedElectron = false;
    m_isFixedMuon = false;

    this->ResetOutdatedProperties();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::AddHitsFromSecondCluster(Cluster *const pCluster)
{
    if (this == pCluster)
        return STATUS_CODE_NOT_ALLOWED;

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_orderedCaloHitList.Add(orderedCaloHitList));

    const CaloHitList &isolatedCaloHitList(pCluster->GetIsolatedCaloHitList());
    for (CaloHitList::const_iterator iter = isolatedCaloHitList.begin(), iterEnd = isolatedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        if (!m_isolatedCaloHitList.insert(*iter).second)
            return STATUS_CODE_ALREADY_PRESENT;
    }

    this->ResetOutdatedProperties();

    m_nCaloHits += pCluster->GetNCaloHits();
    m_nPossibleMipHits += pCluster->GetNPossibleMipHits();

    m_electromagneticEnergy += pCluster->GetElectromagneticEnergy();
    m_hadronicEnergy += pCluster->GetHadronicEnergy();

    // Loop over pseudo layers in second cluster
    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        const PseudoLayer pseudoLayer(iter->first);

        if (m_orderedCaloHitList[pseudoLayer]->size() > 1)
        {
            m_sumXByPseudoLayer[pseudoLayer] += pCluster->m_sumXByPseudoLayer[pseudoLayer];
            m_sumYByPseudoLayer[pseudoLayer] += pCluster->m_sumYByPseudoLayer[pseudoLayer];
            m_sumZByPseudoLayer[pseudoLayer] += pCluster->m_sumZByPseudoLayer[pseudoLayer];
        }
        else
        {
            m_sumXByPseudoLayer[pseudoLayer] = pCluster->m_sumXByPseudoLayer[pseudoLayer];
            m_sumYByPseudoLayer[pseudoLayer] = pCluster->m_sumYByPseudoLayer[pseudoLayer];
            m_sumZByPseudoLayer[pseudoLayer] = pCluster->m_sumZByPseudoLayer[pseudoLayer];
        }
    }

    m_innerPseudoLayer = m_orderedCaloHitList.begin()->first;
    m_outerPseudoLayer = m_orderedCaloHitList.rbegin()->first;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::AddTrackAssociation(Track *const pTrack)
{
    if (NULL == pTrack)
        return STATUS_CODE_INVALID_PARAMETER;

    if (!m_associatedTrackList.insert(pTrack).second)
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::RemoveTrackAssociation(Track *const pTrack)
{
    TrackList::iterator iter = m_associatedTrackList.find(pTrack);

    if (m_associatedTrackList.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    m_associatedTrackList.erase(iter);

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
