/**
 *  @file   PandoraSDK/src/Objects/Cluster.cc
 * 
 *  @brief  Implementation of the cluster class.
 * 
 *  $Log: $
 */

#include "Managers/PluginManager.h"

#include "Objects/CaloHit.h"
#include "Objects/Cluster.h"
#include "Objects/Track.h"

#include "Pandora/Pandora.h"
#include "Pandora/PdgTable.h"

#include "Plugins/EnergyCorrectionsPlugin.h"
#include "Plugins/ParticleIdPlugin.h"
#include "Plugins/ShowerProfilePlugin.h"

#include <algorithm>

namespace pandora
{

Cluster::Cluster(const object_creation::Cluster::Parameters &parameters) :
    m_nCaloHits(0),
    m_nPossibleMipHits(0),
    m_nCaloHitsInOuterLayer(0),
    m_electromagneticEnergy(0),
    m_hadronicEnergy(0),
    m_isolatedElectromagneticEnergy(0),
    m_isolatedHadronicEnergy(0),
    m_particleId(UNKNOWN_PARTICLE_TYPE),
    m_pTrackSeed(parameters.m_pTrack.IsInitialized() ? parameters.m_pTrack.Get() : nullptr),
    m_initialDirection(0.f, 0.f, 0.f),
    m_isDirectionUpToDate(false),
    m_isFitUpToDate(false),
    m_isAvailable(true)
{
    if (parameters.m_caloHitList.empty() && parameters.m_isolatedCaloHitList.empty() && !parameters.m_pTrack.IsInitialized())
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    if (parameters.m_pTrack.IsInitialized())
    {
        m_initialDirection = parameters.m_pTrack.Get()->GetTrackStateAtCalorimeter().GetMomentum().GetUnitVector();
        m_isDirectionUpToDate = true;
    }

    for (const CaloHit *const pCaloHit : parameters.m_caloHitList)
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AddCaloHit(pCaloHit));
    }

    for (const CaloHit *const pCaloHit : parameters.m_isolatedCaloHitList)
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AddIsolatedCaloHit(pCaloHit));
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

Cluster::~Cluster()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::AlterMetadata(const object_creation::Cluster::Metadata &metadata)
{
    if (metadata.m_particleId.IsInitialized())
    {
        m_isPhotonFast.Reset();
        m_particleId = metadata.m_particleId.Get();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::AddCaloHit(const CaloHit *const pCaloHit)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_orderedCaloHitList.Add(pCaloHit));

    this->ResetOutdatedProperties();

    ++m_nCaloHits;

    if (pCaloHit->IsPossibleMip())
        ++m_nPossibleMipHits;

    if (pCaloHit->IsInOuterSamplingLayer()) 
        ++m_nCaloHitsInOuterLayer;

    const float x(pCaloHit->GetPositionVector().GetX());
    const float y(pCaloHit->GetPositionVector().GetY());
    const float z(pCaloHit->GetPositionVector().GetZ());

    m_electromagneticEnergy += pCaloHit->GetElectromagneticEnergy();
    m_hadronicEnergy += pCaloHit->GetHadronicEnergy();

    const unsigned int pseudoLayer(pCaloHit->GetPseudoLayer());
    OrderedCaloHitList::const_iterator iter = m_orderedCaloHitList.find(pseudoLayer);

    if ((m_orderedCaloHitList.end() != iter) && (iter->second->size() > 1))
    {
        SimplePoint &mypoint = m_sumXYZByPseudoLayer[pseudoLayer];
        mypoint.m_xyzPositionSums[0] += x;
        mypoint.m_xyzPositionSums[1] += y;
        mypoint.m_xyzPositionSums[2] += z;
        ++mypoint.m_nHits;
    }
    else
    {
        SimplePoint &mypoint = m_sumXYZByPseudoLayer[pseudoLayer];
        mypoint.m_xyzPositionSums[0] = x;
        mypoint.m_xyzPositionSums[1] = y;
        mypoint.m_xyzPositionSums[2] = z;
        mypoint.m_nHits = 1;
    }

    if (!m_innerPseudoLayer.IsInitialized() || (pseudoLayer < m_innerPseudoLayer.Get()))
        m_innerPseudoLayer = pseudoLayer;

    if (!m_outerPseudoLayer.IsInitialized() || (pseudoLayer > m_outerPseudoLayer.Get()))
        m_outerPseudoLayer = pseudoLayer;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::RemoveCaloHit(const CaloHit *const pCaloHit)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_orderedCaloHitList.Remove(pCaloHit));

    if (m_orderedCaloHitList.empty())
        return this->ResetProperties();

    this->ResetOutdatedProperties();

    --m_nCaloHits;

    if (pCaloHit->IsPossibleMip())
        --m_nPossibleMipHits;

    if (pCaloHit->IsInOuterSamplingLayer())
        --m_nCaloHitsInOuterLayer;

    const float x(pCaloHit->GetPositionVector().GetX());
    const float y(pCaloHit->GetPositionVector().GetY());
    const float z(pCaloHit->GetPositionVector().GetZ());

    m_electromagneticEnergy -= pCaloHit->GetElectromagneticEnergy();
    m_hadronicEnergy -= pCaloHit->GetHadronicEnergy();

    const unsigned int pseudoLayer(pCaloHit->GetPseudoLayer());

    if (m_orderedCaloHitList.end() != m_orderedCaloHitList.find(pseudoLayer))
    {
        SimplePoint &mypoint = m_sumXYZByPseudoLayer[pseudoLayer];
        mypoint.m_xyzPositionSums[0] -= x;
        mypoint.m_xyzPositionSums[1] -= y;
        mypoint.m_xyzPositionSums[2] -= z;
        --mypoint.m_nHits;
    }
    else
    {
        m_sumXYZByPseudoLayer.erase(pseudoLayer);
    }

    if (pseudoLayer <= m_innerPseudoLayer.Get())
        m_innerPseudoLayer = m_orderedCaloHitList.begin()->first;

    if (pseudoLayer >= m_outerPseudoLayer.Get())
        m_outerPseudoLayer = m_orderedCaloHitList.rbegin()->first;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::AddIsolatedCaloHit(const CaloHit *const pCaloHit)
{
    if (m_isolatedCaloHitList.end() != std::find(m_isolatedCaloHitList.begin(), m_isolatedCaloHitList.end(), pCaloHit))
        return STATUS_CODE_ALREADY_PRESENT;

    m_isolatedCaloHitList.push_back(pCaloHit);
    const float electromagneticEnergy(pCaloHit->GetElectromagneticEnergy());
    const float hadronicEnergy(pCaloHit->GetHadronicEnergy());

    m_electromagneticEnergy += electromagneticEnergy;
    m_hadronicEnergy += hadronicEnergy;
    m_isolatedElectromagneticEnergy += electromagneticEnergy;
    m_isolatedHadronicEnergy += hadronicEnergy;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::RemoveIsolatedCaloHit(const CaloHit *const pCaloHit)
{
    CaloHitList::iterator iter = std::find(m_isolatedCaloHitList.begin(), m_isolatedCaloHitList.end(), pCaloHit);

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

const CartesianVector Cluster::GetCentroid(const unsigned int pseudoLayer) const
{
    PointByPseudoLayerMap::const_iterator pointValueIter = m_sumXYZByPseudoLayer.find(pseudoLayer);

    if (m_sumXYZByPseudoLayer.end() == pointValueIter)
        throw StatusCodeException(STATUS_CODE_FAILURE);

    const SimplePoint &mypoint = pointValueIter->second;

    if (0 == mypoint.m_nHits)
        throw StatusCodeException(STATUS_CODE_FAILURE);

    return CartesianVector(static_cast<float>(mypoint.m_xyzPositionSums[0] / static_cast<float>(mypoint.m_nHits)),
        static_cast<float>(mypoint.m_xyzPositionSums[1] / static_cast<float>(mypoint.m_nHits)),
        static_cast<float>(mypoint.m_xyzPositionSums[2] / static_cast<float>(mypoint.m_nHits)));
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Cluster::CalculateFitToAllHitsResult() const
{
    (void) ClusterFitHelper::FitFullCluster(this, m_fitToAllHitsResult);
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
    CaloHitList *const pCaloHitList(m_orderedCaloHitList.begin()->second);

    for (const CaloHit *const pCaloHit : *pCaloHitList)
        initialDirection += pCaloHit->GetExpectedDirection();

    m_initialDirection = initialDirection.GetUnitVector();
    m_isDirectionUpToDate = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Cluster::CalculateLayerHitType(const unsigned int pseudoLayer, InputHitType &layerHitType) const
{
    OrderedCaloHitList::const_iterator listIter = m_orderedCaloHitList.find(pseudoLayer);

    if ((m_orderedCaloHitList.end() == listIter) || (listIter->second->empty()))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    HitTypeToEnergyMap hitTypeToEnergyMap;

    for (const CaloHit *const pCaloHit : *listIter->second)
    {
        HitTypeToEnergyMap::iterator mapIter = hitTypeToEnergyMap.find(pCaloHit->GetHitType());

        if (hitTypeToEnergyMap.end() != mapIter)
        {
            mapIter->second += pCaloHit->GetHadronicEnergy();
            continue;
        }

        if (!hitTypeToEnergyMap.insert(HitTypeToEnergyMap::value_type(pCaloHit->GetHitType(), pCaloHit->GetHadronicEnergy())).second)
            throw StatusCodeException(STATUS_CODE_FAILURE);
    }

    float highestEnergy(0.f);

    for (HitTypeToEnergyMap::value_type &mapEntry : hitTypeToEnergyMap)
    {
        if (mapEntry.second > highestEnergy)
        {
            layerHitType = mapEntry.first;
            highestEnergy = mapEntry.second;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Cluster::PerformEnergyCorrections(const Pandora &pandora) const
{
    const EnergyCorrections *const pEnergyCorrections(pandora.GetPlugins()->GetEnergyCorrections());
    const ParticleId *const pParticleId(pandora.GetPlugins()->GetParticleId());

    float correctedElectromagneticEnergy(0.f), correctedHadronicEnergy(0.f), trackComparisonEnergy(0.f);
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pEnergyCorrections->MakeEnergyCorrections(this, correctedElectromagneticEnergy,
        correctedHadronicEnergy));

    if (pParticleId->IsEmShower(this))
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

void Cluster::CalculateFastPhotonFlag(const Pandora &pandora) const
{
    const bool fastPhotonFlag(pandora.GetPlugins()->GetParticleId()->IsPhoton(this));

    if (!(m_isPhotonFast = fastPhotonFlag))
        throw StatusCodeException(STATUS_CODE_FAILURE);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Cluster::CalculateShowerStartLayer(const Pandora &pandora) const
{
    const ShowerProfilePlugin *const pShowerProfilePlugin(pandora.GetPlugins()->GetShowerProfilePlugin());

    unsigned int showerStartLayer(std::numeric_limits<unsigned int>::max());
    pShowerProfilePlugin->CalculateShowerStartLayer(this, showerStartLayer);

    if (!(m_showerStartLayer = showerStartLayer))
        throw StatusCodeException(STATUS_CODE_FAILURE);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Cluster::CalculateShowerProfile(const Pandora &pandora) const
{
    const ShowerProfilePlugin *const pShowerProfilePlugin(pandora.GetPlugins()->GetShowerProfilePlugin());

    float showerProfileStart(std::numeric_limits<float>::max()), showerProfileDiscrepancy(std::numeric_limits<float>::max());
    pShowerProfilePlugin->CalculateLongitudinalProfile(this, showerProfileStart, showerProfileDiscrepancy);

    if (!(m_showerProfileStart = showerProfileStart) || !(m_showerProfileDiscrepancy = showerProfileDiscrepancy))
        throw StatusCodeException(STATUS_CODE_FAILURE);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::ResetProperties()
{
    if (!m_orderedCaloHitList.empty())
        m_orderedCaloHitList.Reset();

    m_isolatedCaloHitList.clear();

    m_nCaloHits = 0;
    m_nPossibleMipHits = 0;
    m_nCaloHitsInOuterLayer = 0;

    m_sumXYZByPseudoLayer.clear();

    m_electromagneticEnergy = 0;
    m_hadronicEnergy = 0;

    m_innerPseudoLayer.Reset();
    m_outerPseudoLayer.Reset();

    m_particleId = UNKNOWN_PARTICLE_TYPE;

    this->ResetOutdatedProperties();
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Cluster::ResetOutdatedProperties()
{
    m_isFitUpToDate = false;
    m_isDirectionUpToDate = false;
    m_initialDirection.SetValues(0.f, 0.f, 0.f);
    m_fitToAllHitsResult.Reset();
    m_showerStartLayer.Reset();
    m_isPhotonFast.Reset();
    m_showerProfileStart.Reset();
    m_showerProfileDiscrepancy.Reset();
    m_correctedElectromagneticEnergy.Reset();
    m_correctedHadronicEnergy.Reset();
    m_trackComparisonEnergy.Reset();
    m_innerLayerHitType.Reset();
    m_outerLayerHitType.Reset();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::AddHitsFromSecondCluster(const Cluster *const pCluster)
{
    if (this == pCluster)
        return STATUS_CODE_NOT_ALLOWED;

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_orderedCaloHitList.Add(orderedCaloHitList));

    const CaloHitList &isolatedCaloHitList(pCluster->GetIsolatedCaloHitList());
    for (const CaloHit *const pCaloHit : isolatedCaloHitList)
    {
        if (m_isolatedCaloHitList.end() != std::find(m_isolatedCaloHitList.begin(), m_isolatedCaloHitList.end(), pCaloHit))
            return STATUS_CODE_ALREADY_PRESENT;

        m_isolatedCaloHitList.push_back(pCaloHit);
    }

    this->ResetOutdatedProperties();

    m_nCaloHits += pCluster->GetNCaloHits();
    m_nPossibleMipHits += pCluster->GetNPossibleMipHits();
    m_nCaloHitsInOuterLayer += pCluster->GetNHitsInOuterLayer();

    m_electromagneticEnergy += pCluster->GetElectromagneticEnergy();
    m_hadronicEnergy += pCluster->GetHadronicEnergy();

    // Loop over pseudo layers in second cluster
    for (const OrderedCaloHitList::value_type &layerEntry : orderedCaloHitList)
    {
        const unsigned int pseudoLayer(layerEntry.first);
        OrderedCaloHitList::const_iterator currentIter = m_orderedCaloHitList.find(pseudoLayer);

        SimplePoint &mypoint = m_sumXYZByPseudoLayer[pseudoLayer];
        const SimplePoint &theirpoint = pCluster->m_sumXYZByPseudoLayer.at(pseudoLayer);

        if ((m_orderedCaloHitList.end() != currentIter) && (currentIter->second->size() > 1))
        {
            mypoint.m_xyzPositionSums[0] += theirpoint.m_xyzPositionSums[0];
            mypoint.m_xyzPositionSums[1] += theirpoint.m_xyzPositionSums[1];
            mypoint.m_xyzPositionSums[2] += theirpoint.m_xyzPositionSums[2];
            mypoint.m_nHits += theirpoint.m_nHits;
        }
        else
        {
            mypoint.m_xyzPositionSums[0] = theirpoint.m_xyzPositionSums[0];
            mypoint.m_xyzPositionSums[1] = theirpoint.m_xyzPositionSums[1];
            mypoint.m_xyzPositionSums[2] = theirpoint.m_xyzPositionSums[2];
            mypoint.m_nHits = theirpoint.m_nHits;
        }
    }

    m_innerPseudoLayer = m_orderedCaloHitList.begin()->first;
    m_outerPseudoLayer = m_orderedCaloHitList.rbegin()->first;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::AddTrackAssociation(const Track *const pTrack)
{
    if (!pTrack)
        return STATUS_CODE_INVALID_PARAMETER;

    if (m_associatedTrackList.end() != std::find(m_associatedTrackList.begin(), m_associatedTrackList.end(), pTrack))
        return STATUS_CODE_ALREADY_PRESENT;

    m_associatedTrackList.push_back(pTrack);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::RemoveTrackAssociation(const Track *const pTrack)
{
    TrackList::iterator iter = std::find(m_associatedTrackList.begin(), m_associatedTrackList.end(), pTrack);

    if (m_associatedTrackList.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    m_associatedTrackList.erase(iter);
    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
