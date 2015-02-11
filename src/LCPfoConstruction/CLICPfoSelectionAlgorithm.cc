/**
 *  @file   LCContent/src/LCPfoConstruction/CLICPfoSelectionAlgorithm.cc
 * 
 *  @brief  Implementation of an algorithm to select PFOs for CLIC physics studies
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCPfoConstruction/CLICPfoSelectionAlgorithm.h"

using namespace pandora;

namespace lc_content
{

CLICPfoSelectionAlgorithm::CLICPfoSelectionAlgorithm() :
    m_monitoring(false),
    m_displaySelectedPfos(false),
    m_displayRejectedPfos(true),
    m_monitoringPfoEnergyToDisplay(1.f),
    m_farForwardCosTheta(0.975f),
    m_ptCutForTightTiming(0.75f),
    m_photonPtCut(0.f),
    m_photonPtCutForLooseTiming(4.f),
    m_photonLooseTimingCut(2.f),
    m_photonTightTimingCut(1.f),
    m_chargedPfoPtCut(0.f),
    m_chargedPfoPtCutForLooseTiming(4.f),
    m_chargedPfoLooseTimingCut(3.f),
    m_chargedPfoTightTimingCut(1.5f),
    m_chargedPfoNegativeLooseTimingCut(-2.f),
    m_chargedPfoNegativeTightTimingCut(-2.f),
    m_neutralHadronPtCut(0.f),
    m_neutralHadronPtCutForLooseTiming(8.f),
    m_neutralHadronLooseTimingCut(2.5f),
    m_neutralHadronTightTimingCut(1.5f),
    m_neutralFarForwardLooseTimingCut(2.f),
    m_neutralFarForwardTightTimingCut(1.f),
    m_hCalBarrelLooseTimingCut(20.f),
    m_hCalBarrelTightTimingCut(10.f),
    m_hCalEndCapTimingFactor(1.f),
    m_neutralHadronBarrelPtCutForLooseTiming(3.5f),
    m_minECalHitsForTiming(5),
    m_minHCalEndCapHitsForTiming(5),
    m_useClusterLessPfos(true),
    m_minMomentumForClusterLessPfos(0.5f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CLICPfoSelectionAlgorithm::Run()
{
    const PfoList *pPfoList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pPfoList));

    for (PfoList::const_iterator iter = pPfoList->begin(); iter != pPfoList->end();)
    {
        const ParticleFlowObject *const pPfo = *iter;
        iter++;

        const CartesianVector &pfoMomentum(pPfo->GetMomentum());
        const float pfoP(pfoMomentum.GetMagnitude());

        if (pfoP < std::numeric_limits<float>::epsilon())
            return STATUS_CODE_FAILURE;

        // Select appropriate values for pt and timing cuts
        float ptCut(m_neutralHadronPtCut);
        float ptCutForLooseTiming(m_neutralHadronPtCutForLooseTiming);

        float timingCutLow(0.);
        float timingCutHigh(m_neutralHadronLooseTimingCut);
        float hCalBarrelTimingCut(m_hCalBarrelLooseTimingCut);

        const float cosTheta(std::fabs(pfoMomentum.GetZ()) / pfoP);

        if (cosTheta > m_farForwardCosTheta)
            timingCutHigh = m_neutralFarForwardLooseTimingCut;

        const float pfoPt(std::sqrt(pfoMomentum.GetX() * pfoMomentum.GetX() + pfoMomentum.GetY() * pfoMomentum.GetY()));

        // Neutral hadron cuts
        if (pfoPt <= m_ptCutForTightTiming)
        {
            timingCutHigh = m_neutralHadronTightTimingCut;
            hCalBarrelTimingCut = m_hCalBarrelTightTimingCut;

            if (cosTheta > m_farForwardCosTheta)
                timingCutHigh = m_neutralFarForwardTightTimingCut;
        }

        // Photon cuts
        if (pPfo->GetParticleId() == PHOTON)
        {
            ptCut = m_photonPtCut;
            ptCutForLooseTiming = m_photonPtCutForLooseTiming;
            timingCutHigh = m_photonLooseTimingCut;

            if (pfoPt <= m_ptCutForTightTiming)
                timingCutHigh = m_photonTightTimingCut;
        }

        // Charged hadron cuts
        const TrackList &pfoTrackList(pPfo->GetTrackList());

        if (!pfoTrackList.empty())
        {
            ptCut = m_chargedPfoPtCut;
            ptCutForLooseTiming = m_chargedPfoPtCutForLooseTiming;
            timingCutLow = m_chargedPfoNegativeLooseTimingCut;
            timingCutHigh = m_chargedPfoLooseTimingCut;

            if (pfoPt <= m_ptCutForTightTiming)
            {
                timingCutLow = m_chargedPfoNegativeTightTimingCut;
                timingCutHigh = m_chargedPfoTightTimingCut;
            }
        }

        // Reject low pt pfos (default is to set ptcut to zero)
        if (pfoPt < ptCut)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Delete(*this, pPfo))
            continue;
        }

        // Only apply cuts to low pt pfos
        if (pfoPt > ptCutForLooseTiming)
            continue;

        // Examine any associated clusters for additional timing information
        bool selectPfo(false);
        const ClusterList &pfoClusterList(pPfo->GetClusterList());
        const float pfoEnergyToDisplay(m_monitoring ? m_monitoringPfoEnergyToDisplay : std::numeric_limits<float>::max());

        // Require any cluster to be "in time" to select pfo
        if (!pfoClusterList.empty())
        {
            for (ClusterList::const_iterator clusterIter = pfoClusterList.begin(), clusterIterEnd = pfoClusterList.end();
                clusterIter != clusterIterEnd; ++clusterIter)
            {
                const Cluster *const pCluster = *clusterIter;

                float meanTime(0.f);
                float meanTimeECal(0.f);
                unsigned int nECalHits(0);
                unsigned int nHCalEndCapHits(0);
                float meanTimeHCalEndCap(0.f);

                // Extract information to be used in selection process
                this->GetClusterTimes(pCluster, meanTime, meanTimeECal, nECalHits, meanTimeHCalEndCap, nHCalEndCapHits);
                const TrackList &trackList(pCluster->GetAssociatedTrackList());

                // if there is a track correct for track propagation time
                if (!trackList.empty())
                {
                    const Track *const pTrack(*(trackList.begin()));

                    const float tof(pTrack->GetTrackStateAtCalorimeter().GetPosition().GetMagnitude() / 300.f);
                    const float tTrack(pTrack->GetTimeAtCalorimeter() - tof);

                    meanTime -= tTrack;
                    meanTimeECal -= tTrack;
                    meanTimeHCalEndCap -= tTrack;
                }

                // Make the selection decisions
                if (nECalHits > m_minECalHitsForTiming)
                {
                    if ((meanTimeECal >= timingCutLow) && (meanTimeECal<= timingCutHigh))
                        selectPfo = true;
                }
                else if (pPfo->GetParticleId() == PHOTON)
                {
                    if ((meanTime >= timingCutLow) && (meanTime<= timingCutHigh))
                        selectPfo = true;
                }
                else if (nHCalEndCapHits > m_minHCalEndCapHitsForTiming)
                {
                    if ((meanTimeHCalEndCap >= timingCutLow) && (meanTimeHCalEndCap <= (m_hCalEndCapTimingFactor * timingCutHigh)))
                        selectPfo = true;
                }
                else
                {
                    if (meanTime < hCalBarrelTimingCut)
                        selectPfo = true;

                    if (trackList.empty() && (pfoPt > m_neutralHadronBarrelPtCutForLooseTiming))
                        selectPfo = true;
                }

                // Print monitoring information
                if (selectPfo && (pPfo->GetEnergy() > pfoEnergyToDisplay) && m_displaySelectedPfos)
                {
                    std::cout << " SELECTED PFO " << pPfo->GetParticleId() << " tracks = " << trackList.size() << " e = " << pPfo->GetEnergy()
                              << " pt = " << pfoPt << " nc = " << pfoClusterList.size() << " t = " << meanTime << " ne = " << nECalHits
                              << " te = " << meanTimeECal << " nhe = " << nHCalEndCapHits << " the = " << meanTimeHCalEndCap << std::endl;
                }

                if (!selectPfo && (pPfo->GetEnergy() > pfoEnergyToDisplay) && m_displayRejectedPfos)
                {
                    std::cout << " REJECTED PFO " << pPfo->GetParticleId() << " tracks = " << trackList.size() << " e = " << pPfo->GetEnergy()
                              << " pt = " << pfoPt << " nc = " << pfoClusterList.size() << " t = " << meanTime << " ne = " << nECalHits
                              << " te = " << meanTimeECal << " nhe = " << nHCalEndCapHits << " the = " << meanTimeHCalEndCap << std::endl;
                }
            }
        }
        else
        {
            // No clusters form part of this pfo - no additional timing information
            if (pfoP > m_minMomentumForClusterLessPfos)
                selectPfo = m_useClusterLessPfos;

            // Print monitoring information
            if (selectPfo && (pPfo->GetEnergy() > pfoEnergyToDisplay) && m_displaySelectedPfos)
            {
                std::cout << "SELECTED PFO " << pPfo->GetParticleId() << " e = " << pPfo->GetEnergy() << " pt = " << pfoPt
                          << " nc = 0 " << std::endl;
            }

            if (!selectPfo && (pPfo->GetEnergy() > pfoEnergyToDisplay) && m_displayRejectedPfos)
            {
                std::cout << " REJECTED PFO " << pPfo->GetParticleId() << " e = " << pPfo->GetEnergy() << " pt = " << pfoPt
                          << " nc = 0 " << std::endl;
            }
        }

        // Modify the pfo list
        if (!selectPfo)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Delete(*this, pPfo));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CLICPfoSelectionAlgorithm::GetClusterTimes(const Cluster *const pCluster, float &meanTime, float &meanTimeECal, unsigned int &nECalHits,
    float &meanTimeHCalEndCap, unsigned int &nHCalEndCapHits) const
{
    meanTime = std::numeric_limits<float>::max();
    meanTimeECal = std::numeric_limits<float>::max();
    meanTimeHCalEndCap = std::numeric_limits<float>::max();
    nECalHits = 0;
    nHCalEndCapHits = 0;

    float sumEnergy(0.f);
    float sumTimeEnergy(0.f);
    float sumEnergyECal(0.f);
    float sumTimeEnergyECal(0.f);
    float sumEnergyHCalEndCap(0.f);
    float sumTimeEnergyHCalEndCap(0.f);

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const CaloHit *const pCaloHit = *hitIter;

            sumEnergy += pCaloHit->GetHadronicEnergy();
            sumTimeEnergy += pCaloHit->GetHadronicEnergy() * pCaloHit->GetTime();

            if (pCaloHit->GetHitType() == ECAL)
            {
                nECalHits++;
                sumEnergyECal += pCaloHit->GetHadronicEnergy();
                sumTimeEnergyECal += pCaloHit->GetHadronicEnergy() * pCaloHit->GetTime();
            }
            else if (pCaloHit->GetHitRegion() == ENDCAP)
            {
                nHCalEndCapHits++;
                sumEnergyHCalEndCap += pCaloHit->GetHadronicEnergy();
                sumTimeEnergyHCalEndCap += pCaloHit->GetHadronicEnergy() * pCaloHit->GetTime();
            }
        }
    }

    if (sumEnergy > 0.f)
        meanTime = sumTimeEnergy / sumEnergy;

    if (sumEnergyECal > 0.f)
        meanTimeECal = sumTimeEnergyECal / sumEnergyECal;

    if (sumEnergyHCalEndCap > 0.f)
        meanTimeHCalEndCap = sumTimeEnergyHCalEndCap / sumEnergyHCalEndCap;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CLICPfoSelectionAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Monitoring", m_monitoring));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DisplaySelectedPfos", m_displaySelectedPfos));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DisplayRejectedPfos", m_displayRejectedPfos));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MonitoringPfoEnergyToDisplay", m_monitoringPfoEnergyToDisplay));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FarForwardCosTheta", m_farForwardCosTheta));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PtCutForTightTiming", m_ptCutForTightTiming));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonPtCut", m_photonPtCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonPtCutForLooseTiming", m_photonPtCutForLooseTiming));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonLooseTimingCut", m_photonLooseTimingCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonTightTimingCut", m_photonTightTimingCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ChargedPfoPtCut", m_chargedPfoPtCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ChargedPfoPtCutForLooseTiming", m_chargedPfoPtCutForLooseTiming));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ChargedPfoLooseTimingCut", m_chargedPfoLooseTimingCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ChargedPfoTightTimingCut", m_chargedPfoTightTimingCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ChargedPfoNegativeLooseTimingCut", m_chargedPfoNegativeLooseTimingCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ChargedPfoNegativeTightTimingCut", m_chargedPfoNegativeTightTimingCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NeutralHadronPtCut", m_neutralHadronPtCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NeutralHadronPtCutForLooseTiming", m_neutralHadronPtCutForLooseTiming));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NeutralHadronLooseTimingCut", m_neutralHadronLooseTimingCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NeutralHadronTightTimingCut", m_neutralHadronTightTimingCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NeutralFarForwardLooseTimingCut",  m_neutralFarForwardLooseTimingCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NeutralFarForwardTightTimingCut",  m_neutralFarForwardTightTimingCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HCalBarrelLooseTimingCut", m_hCalBarrelLooseTimingCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HCalBarrelTightTimingCut", m_hCalBarrelTightTimingCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HCalEndCapTimingFactor", m_hCalBarrelLooseTimingCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NeutralHadronBarrelPtCutForLooseTiming", m_neutralHadronBarrelPtCutForLooseTiming));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinECalHitsForTiming", m_minECalHitsForTiming));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHCalEndCapHitsForTiming", m_minHCalEndCapHitsForTiming));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "UseClusterLessPfos", m_useClusterLessPfos));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinMomentumForClusterLessPfos", m_minMomentumForClusterLessPfos));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
