/**
 *  @file   LCContent/src/LCParticleId/FinalParticleIdAlgorithm.cc
 * 
 *  @brief  Implementation of the final particle id algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCParticleId/FinalParticleIdAlgorithm.h"

using namespace pandora;

namespace lc_content
{

StatusCode FinalParticleIdAlgorithm::Run()
{
    const PfoList *pPfoList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pPfoList));

    for (PfoList::const_iterator iter = pPfoList->begin(), iterEnd = pPfoList->end();
        iter != iterEnd; ++iter)
    {
        ParticleFlowObject *pPfo = *iter;

        const TrackList &trackList(pPfo->GetTrackList());
        const ClusterList &clusterList(pPfo->GetClusterList());

        // Consider only pfos with a single cluster and no track sibling relationships
        if ((clusterList.size() != 1) || (trackList.empty()) || this->ContainsSiblingTrack(trackList))
            continue;

        const int charge(pPfo->GetCharge());

        if (0 == charge)
            return STATUS_CODE_FAILURE;

        // Ignore particle flow objects already tagged as electrons or muons
        if ((std::abs(pPfo->GetParticleId()) == E_MINUS) || (std::abs(pPfo->GetParticleId()) == MU_MINUS))
            continue;

        Cluster *pCluster = *(clusterList.begin());
        const ParticleId *pParticleId(PandoraContentApi::GetPlugins(*this)->GetParticleId());

        // Run electron id, followed by muon id
        if (pParticleId->IsElectron(pCluster))
        {
            pPfo->SetParticleId((charge < 0) ? E_MINUS : E_PLUS);
            pPfo->SetMass(PdgTable::GetParticleMass(pPfo->GetParticleId()));
            pPfo->SetEnergy(std::sqrt(pPfo->GetMass() * pPfo->GetMass() + pPfo->GetMomentum().GetMagnitudeSquared()));
        }

        if (pParticleId->IsMuon(pCluster))
        {
            pPfo->SetParticleId((charge < 0) ? MU_MINUS : MU_PLUS);
            pPfo->SetMass(PdgTable::GetParticleMass(pPfo->GetParticleId()));
            pPfo->SetEnergy(std::sqrt(pPfo->GetMass() * pPfo->GetMass() + pPfo->GetMomentum().GetMagnitudeSquared()));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool FinalParticleIdAlgorithm::ContainsSiblingTrack(const TrackList &trackList) const
{
    for (TrackList::const_iterator iter = trackList.begin(), iterEnd = trackList.end(); iter != iterEnd; ++iter)
    {
        if (!(*iter)->GetSiblingTrackList().empty())
        {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FinalParticleIdAlgorithm::ReadSettings(const TiXmlHandle /*xmlHandle*/)
{
    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
