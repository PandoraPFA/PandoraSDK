/**
 *  @file   LCContent/src/LCCheating/PerfectParticleFlowAlgorithm.cc
 * 
 *  @brief  Implementation of the perfect particle flow algorithm class
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCCheating/PerfectParticleFlowAlgorithm.h"

using namespace pandora;

namespace lc_content
{

PerfectParticleFlowAlgorithm::PerfectParticleFlowAlgorithm() :
    m_simpleCaloHitCollection(true),
    m_minWeightFraction(0.01f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PerfectParticleFlowAlgorithm::Run()
{
    const MCParticleList *pMCParticleList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pMCParticleList));

    if (pMCParticleList->empty())
        return STATUS_CODE_SUCCESS;

    const ClusterList *pClusterList = NULL; std::string clusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::CreateTemporaryListAndSetCurrent(*this, pClusterList, clusterListName));

    const PfoList *pPfoList = NULL; std::string pfoListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::CreateTemporaryListAndSetCurrent(*this, pPfoList, pfoListName));

    for (MCParticleList::const_iterator iterMC = pMCParticleList->begin(), iterMCEnd = pMCParticleList->end(); iterMC != iterMCEnd; ++iterMC)
    {
        try
        {
            const MCParticle *const pPfoTarget = *iterMC;
            PfoParameters pfoParameters;

            this->CaloHitCollection(pPfoTarget, pfoParameters);
            this->TrackCollection(pPfoTarget, pfoParameters);

            int nTracksUsed(0);
            this->SetPfoParametersFromTracks(pPfoTarget, nTracksUsed, pfoParameters);
            this->SetPfoParametersFromClusters(pPfoTarget, nTracksUsed, pfoParameters);
            this->PfoParameterDebugInformation(pPfoTarget, nTracksUsed, pfoParameters);

            const ParticleFlowObject *pPfo = NULL;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ParticleFlowObject::Create(*this, pfoParameters, pPfo));
        }
        catch (StatusCodeException &)
        {
        }
    }

    if (!pClusterList->empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList<Cluster>(*this, m_outputClusterListName));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<Cluster>(*this, m_outputClusterListName));
    }

    if (!pPfoList->empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList<Pfo>(*this, m_outputPfoListName));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<Pfo>(*this, m_outputPfoListName));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PerfectParticleFlowAlgorithm::CaloHitCollection(const MCParticle *const pPfoTarget, PfoParameters &pfoParameters) const
{
    const CaloHitList *pCaloHitList = NULL;
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pCaloHitList));

    const Cluster *pCluster = NULL;
    const CaloHitList localCaloHitList(pCaloHitList->begin(), pCaloHitList->end());

    for (CaloHitList::const_iterator iter = localCaloHitList.begin(), iterEnd = localCaloHitList.end(); iter != iterEnd; ++iter)
    {
        try
        {
            const CaloHit *const pCaloHit = *iter;

            if (!PandoraContentApi::IsAvailable(*this, pCaloHit))
                continue;

            PandoraContentApi::Cluster::Parameters parameters;
            CaloHitList &caloHitList(parameters.m_caloHitList);

            if (m_simpleCaloHitCollection)
            {
                this->SimpleCaloHitCollection(pPfoTarget, pCaloHit, caloHitList);
            }
            else
            {
                this->FullCaloHitCollection(pPfoTarget, pCaloHit, caloHitList);
            }

            if (caloHitList.empty())
                continue;

            if (NULL == pCluster)
            {
                PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, parameters, pCluster));
                pfoParameters.m_clusterList.insert(pCluster);
            }
            else
            {
                for (CaloHitList::const_iterator hitIter = caloHitList.begin(), hitIterEnd = caloHitList.end(); hitIter != hitIterEnd; ++hitIter)
                    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddToCluster(*this, pCluster, *hitIter));
            }
        }
        catch (StatusCodeException &)
        {
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PerfectParticleFlowAlgorithm::SimpleCaloHitCollection(const MCParticle *const pPfoTarget, const CaloHit *const pCaloHit, CaloHitList &caloHitList) const
{
    const MCParticle *const pHitMCParticle(MCParticleHelper::GetMainMCParticle(pCaloHit));
    const MCParticle *const pHitPfoTarget(pHitMCParticle->GetPfoTarget());

    if (pHitPfoTarget != pPfoTarget)
        return;

    caloHitList.insert(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PerfectParticleFlowAlgorithm::FullCaloHitCollection(const MCParticle *const pPfoTarget, const CaloHit *const pCaloHit, CaloHitList &caloHitList) const
{
    const MCParticleWeightMap mcParticleWeightMap(pCaloHit->GetMCParticleWeightMap());

    if (mcParticleWeightMap.size() < 2)
        return this->SimpleCaloHitCollection(pPfoTarget, pCaloHit, caloHitList);

    float mcParticleWeightSum(0.f);

    for (MCParticleWeightMap::const_iterator wtIter = mcParticleWeightMap.begin(), wtIterEnd = mcParticleWeightMap.end(); wtIter != wtIterEnd; ++wtIter)
        mcParticleWeightSum += wtIter->second;

    if (mcParticleWeightSum < std::numeric_limits<float>::epsilon())
        throw StatusCodeException(STATUS_CODE_FAILURE);

    const CaloHit *pLocalCaloHit = pCaloHit;

    for (MCParticleWeightMap::const_iterator wtIter = mcParticleWeightMap.begin(), wtIterEnd = mcParticleWeightMap.end(); wtIter != wtIterEnd; ++wtIter)
    {
        const MCParticle *const pHitMCParticle(wtIter->first);
        const MCParticle *const pHitPfoTarget(pHitMCParticle->GetPfoTarget());
        const float weight(wtIter->second);

        if (pHitPfoTarget != pPfoTarget)
            continue;

        const CaloHit *pCaloHitToAdd = pLocalCaloHit;

        if (pCaloHitToAdd->GetWeight() < std::numeric_limits<float>::epsilon())
            throw StatusCodeException(STATUS_CODE_FAILURE);

        const float weightFraction(weight / (mcParticleWeightSum * pCaloHitToAdd->GetWeight()));
        const bool isBelowThreshold((weightFraction - m_minWeightFraction) < std::numeric_limits<float>::epsilon());

        if (isBelowThreshold)
            continue;

        const bool shouldFragment(weightFraction + m_minWeightFraction - 1.f < std::numeric_limits<float>::epsilon());

        if (shouldFragment)
            PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Fragment(*this, pLocalCaloHit, weightFraction, pCaloHitToAdd, pLocalCaloHit));

        caloHitList.insert(pCaloHitToAdd);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PerfectParticleFlowAlgorithm::TrackCollection(const MCParticle *const pPfoTarget, PfoParameters &pfoParameters) const
{
    const TrackList *pTrackList = NULL;
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pTrackList));

    for (TrackList::const_iterator iter = pTrackList->begin(), iterEnd = pTrackList->end(); iter != iterEnd; ++iter)
    {
        try
        {
            const Track *const pTrack = *iter;
            const MCParticle *const pTrkMCParticle(pTrack->GetMainMCParticle());
            const MCParticle *const pTrkPfoTarget(pTrkMCParticle->GetPfoTarget());

            if (pTrkPfoTarget != pPfoTarget)
                continue;

            pfoParameters.m_trackList.insert(pTrack);
        }
        catch (StatusCodeException &)
        {
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PerfectParticleFlowAlgorithm::SetPfoParametersFromTracks(const MCParticle *const pPfoTarget, int &nTracksUsed, PfoParameters &pfoParameters) const
{
    if (!pfoParameters.m_trackList.empty())
    {
        int charge(0);
        float energyWithPionMass(0.f), energyWithElectronMass(0.f);
        CartesianVector momentum(0.f, 0.f, 0.f);

        for (TrackList::const_iterator iter = pfoParameters.m_trackList.begin(), iterEnd = pfoParameters.m_trackList.end(); iter != iterEnd; ++iter)
        {
            const Track *const pTrack = *iter;

            if (!pTrack->CanFormPfo() && !pTrack->CanFormClusterlessPfo())
            {
                std::cout << pPfoTarget << " Drop track, E: " << pTrack->GetEnergyAtDca() << " cfp: " << pTrack->CanFormPfo() << " cfcp: " << pTrack->CanFormClusterlessPfo() << std::endl;
                continue;
            }

            if (!pTrack->GetParentTrackList().empty())
            {
                std::cout << pPfoTarget << " Drop track, E: " << pTrack->GetEnergyAtDca() << " nParents: " << pTrack->GetParentTrackList().size() << std::endl;
                continue;
            }

            ++nTracksUsed;

            // ATTN Assume neutral track-based pfos represent pair-production
            const float electronMass(PdgTable::GetParticleMass(E_MINUS));

            charge += pTrack->GetCharge();
            momentum += pTrack->GetMomentumAtDca();
            energyWithPionMass += pTrack->GetEnergyAtDca();
            energyWithElectronMass += std::sqrt(electronMass * electronMass + pTrack->GetMomentumAtDca().GetMagnitudeSquared());
        }

        if (0 == nTracksUsed)
            return;

        pfoParameters.m_charge = charge;
        pfoParameters.m_momentum = momentum;
        pfoParameters.m_particleId = (pfoParameters.m_charge.Get() == 0) ? PHOTON : (pfoParameters.m_charge.Get() < 0) ? PI_MINUS : PI_PLUS;
        pfoParameters.m_energy = (pfoParameters.m_particleId.Get() == PHOTON) ? energyWithElectronMass : energyWithPionMass;
        pfoParameters.m_mass = std::sqrt(std::max(pfoParameters.m_energy.Get() * pfoParameters.m_energy.Get() - pfoParameters.m_momentum.Get().GetDotProduct(pfoParameters.m_momentum.Get()), 0.f));
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PerfectParticleFlowAlgorithm::SetPfoParametersFromClusters(const MCParticle *const pPfoTarget, const int nTracksUsed, PfoParameters &pfoParameters) const
{
    const Cluster *pCluster = NULL;

    if (!pfoParameters.m_clusterList.empty())
    {
        if (1 != pfoParameters.m_clusterList.size())
            throw StatusCodeException(STATUS_CODE_FAILURE);

        pCluster = *(pfoParameters.m_clusterList.begin());
    }

    if ((0 == nTracksUsed) && !pfoParameters.m_clusterList.empty())
    {
        const bool isPhoton(PHOTON == pPfoTarget->GetParticleId());

        const float clusterEnergy(isPhoton ? pCluster->GetCorrectedElectromagneticEnergy(this->GetPandora()) :
            pCluster->GetCorrectedHadronicEnergy(this->GetPandora()));
        const CartesianVector positionVector(!isPhoton ? pCluster->GetCentroid(pCluster->GetInnerPseudoLayer()) :
            this->GetEnergyWeightedCentroid(pCluster, pCluster->GetInnerPseudoLayer(), pCluster->GetOuterPseudoLayer()));

        pfoParameters.m_particleId = (isPhoton ? PHOTON : NEUTRON);
        pfoParameters.m_charge = 0;
        pfoParameters.m_mass = (isPhoton ? PdgTable::GetParticleMass(PHOTON) : PdgTable::GetParticleMass(NEUTRON));
        pfoParameters.m_energy = clusterEnergy;
        pfoParameters.m_momentum = positionVector.GetUnitVector() * clusterEnergy;
    }

    // Track-cluster associations hack
    if (NULL != pCluster)
    {
        for (TrackList::const_iterator iterTrk = pfoParameters.m_trackList.begin(), iterTrkEnd = pfoParameters.m_trackList.end(); iterTrk != iterTrkEnd; ++iterTrk)
            PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, *iterTrk, pCluster));
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

const CartesianVector PerfectParticleFlowAlgorithm::GetEnergyWeightedCentroid(const Cluster *const pCluster, const unsigned int innerPseudoLayer,
    const unsigned int outerPseudoLayer) const
{
    float energySum(0.f);
    CartesianVector energyPositionSum(0.f, 0.f, 0.f);
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        if (iter->first > outerPseudoLayer)
            break;

        if (iter->first < innerPseudoLayer)
            continue;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const float electromagneticEnergy((*hitIter)->GetElectromagneticEnergy());
            energySum += electromagneticEnergy;
            energyPositionSum += ((*hitIter)->GetPositionVector() * electromagneticEnergy);
        }
    }

    if (energySum < std::numeric_limits<float>::epsilon())
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return (energyPositionSum * (1.f / energySum));
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PerfectParticleFlowAlgorithm::PfoParameterDebugInformation(const MCParticle *const pPfoTarget, const int nTracksUsed, PfoParameters &pfoParameters) const
{
    if ((0 == nTracksUsed) && pfoParameters.m_clusterList.empty())
    {
        std::cout << pPfoTarget << " No energy deposits for pfo target " << pPfoTarget->GetParticleId() << ", E: " << pPfoTarget->GetEnergy() << std::endl;
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }

    if (std::fabs(pfoParameters.m_energy.Get() - pPfoTarget->GetEnergy()) > 2.f)
    {
        std::cout << pPfoTarget << " Did not match pfo target energy, target: " << pPfoTarget->GetParticleId() << ", E: " << pPfoTarget->GetEnergy()
                  << " (reco: " << pfoParameters.m_particleId.Get() << ", E: " << pfoParameters.m_energy.Get() << ", nTrk: " << pfoParameters.m_trackList.size()
                  << ", nTrkUsed: " << nTracksUsed << ", nCls: " << pfoParameters.m_clusterList.size() << ")" << std::endl;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PerfectParticleFlowAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "OutputPfoListName", m_outputPfoListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "OutputClusterListName", m_outputClusterListName));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "SimpleCaloHitCollection", m_simpleCaloHitCollection));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinWeightFraction", m_minWeightFraction));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
