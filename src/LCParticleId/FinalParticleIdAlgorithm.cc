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
        const ParticleFlowObject *const pPfo = *iter;

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

        const Cluster *const pCluster = *(clusterList.begin());
        const ParticleId *const pParticleId(PandoraContentApi::GetPlugins(*this)->GetParticleId());

        // Run electron id, followed by muon id
        PandoraContentApi::ParticleFlowObject::Metadata metadata;

        if (pParticleId->IsElectron(pCluster))
        {
            metadata.m_particleId = (charge < 0) ? E_MINUS : E_PLUS;
        }
        else if (pParticleId->IsMuon(pCluster))
        {
            metadata.m_particleId = (charge < 0) ? MU_MINUS : MU_PLUS;
        }

        if (metadata.m_particleId.IsInitialized())
        {
            metadata.m_mass = PdgTable::GetParticleMass(metadata.m_particleId.Get());
            metadata.m_energy = std::sqrt(metadata.m_mass.Get() * metadata.m_mass.Get() + pPfo->GetMomentum().GetMagnitudeSquared());
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AlterMetadata(*this, pPfo, metadata));
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
