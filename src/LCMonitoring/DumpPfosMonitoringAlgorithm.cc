/**
 *  @file   LCContent/src/LCMonitoring/DumpPfosMonitoringAlgorithm.cc
 * 
 *  @brief  Implementation of the dump pfos monitoring algorithm class
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCHelpers/ClusterHelper.h"
#include "LCHelpers/ReclusterHelper.h"
#include "LCHelpers/SortingHelper.h"

#include "LCMonitoring/DumpPfosMonitoringAlgorithm.h"

#include <iomanip>

using namespace pandora;

namespace lc_content
{

const int precision = 2;
const int widthFloat = 7;
const int widthBigFloat = 9;
const int widthSmallFloat = 5;
const int widthInt = 5;
const int widthInt4 = 4;
const int widthFlag = 2;

DumpPfosMonitoringAlgorithm::DumpPfosMonitoringAlgorithm() :
    m_trackRecoAsTrackEnergy(0.f),
    m_trackRecoAsPhotonEnergy(0.f),
    m_trackRecoAsNeutralEnergy(0.f),
    m_photonRecoAsTrackEnergy(0.f),
    m_photonRecoAsPhotonEnergy(0.f),
    m_photonRecoAsNeutralEnergy(0.f),
    m_neutralRecoAsTrackEnergy(0.f),
    m_neutralRecoAsPhotonEnergy(0.f),
    m_neutralRecoAsNeutralEnergy(0.f),
    m_count(0),
    m_photonOrNeutralRecoAsTrackEnergySum(0.f),
    m_photonOrNeutralRecoAsTrackEnergySum2(0.f),
    m_trackRecoAsPhotonOrNeutralEnergySum(0.f),
    m_trackRecoAsPhotonOrNeutralEnergySum2(0.f),
    m_confusionCorrelation(0.f),
    m_minPfoEnergyToDisplay(0.f),
    m_minAbsChiToDisplay(3.f),
    m_minConfusionEnergyToDisplay(5.f),
    m_minFragmentEnergyToDisplay(5.f),
    m_totalPfoEnergyDisplayLessThan(1000000.f),
    m_totalPfoEnergyDisplayGreaterThan(0.f),
    m_fragmentEnergyToDisplay(5.f),
    m_photonIdEnergyToDisplay(5.f),
    m_trackRecoAsTrackEnergySum(0.f),
    m_trackRecoAsPhotonEnergySum(0.f),
    m_trackRecoAsNeutralEnergySum(0.f),
    m_photonRecoAsTrackEnergySum(0.f),
    m_photonRecoAsPhotonEnergySum(0.f),
    m_photonRecoAsNeutralEnergySum(0.f),
    m_neutralRecoAsTrackEnergySum(0.f),
    m_neutralRecoAsPhotonEnergySum(0.f),
    m_neutralRecoAsNeutralEnergySum(0.f),
    m_goodTrackEnergy(0.f),
    m_goodPhotonEnergy(0.f),
    m_goodIdedPhotonEnergy(0.f),
    m_goodNeutralEnergy(0.f),
    m_goodIdedNeutralEnergy(0.f),
    m_badTrackEnergy(0.f),
    m_badPhotonEnergy(0.f),
    m_badNeutralEnergy(0.f),
    m_goodTrackEnergySum(0.f),
    m_goodPhotonEnergySum(0.f),
    m_goodIdedPhotonEnergySum(0.f),
    m_goodNeutralEnergySum(0.f),
    m_goodIdedNeutralEnergySum(0.f),
    m_badTrackEnergySum(0.f),
    m_badPhotonEnergySum(0.f),
    m_badNeutralEnergySum(0.f),
    m_goodFractionCut(0.9f),
    m_firstChargedPfoToPrint(true),
    m_firstNeutralPfoToPrint(true),
    m_firstPhotonPfoToPrint(true)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

DumpPfosMonitoringAlgorithm::~DumpPfosMonitoringAlgorithm()
{
    // Alter formatting options, caching original parameters
    const int originalPrecision(std::cout.precision(precision));
    const int originalWidth(std::cout.width());
    (void) std::cout.setf(std::ios::fixed, std::ios::floatfield);

    std::cout << " ------------------------------------------------------------ " << std::endl;
    std::cout << " ---------- DumpPfosMonitoringAlgorithm : Summary ----------- " << std::endl;
    std::cout << " ------------------------------------------------------------ " << std::endl;
    std::cout << std::endl;
    std::cout << " Confusion matrix summary " << std::endl;

    const float confmat[3][3] =
    {
        {m_trackRecoAsTrackEnergySum, m_photonRecoAsTrackEnergySum, m_neutralRecoAsTrackEnergySum},
        {m_trackRecoAsPhotonEnergySum, m_photonRecoAsPhotonEnergySum, m_neutralRecoAsPhotonEnergySum},
        {m_trackRecoAsNeutralEnergySum, m_photonRecoAsNeutralEnergySum, m_neutralRecoAsNeutralEnergySum}
    };
  
    std::cout << std::endl;
    FORMATTED_OUTPUT_CONFUSION(confmat[0][0], confmat[0][1], confmat[0][2], confmat[1][0], confmat[1][1], confmat[1][2], confmat[2][0],
        confmat[2][1], confmat[2][2]);

    const float sumCal(m_trackRecoAsTrackEnergySum + m_photonRecoAsTrackEnergySum + m_neutralRecoAsTrackEnergySum +
        m_trackRecoAsPhotonEnergySum + m_photonRecoAsPhotonEnergySum + m_neutralRecoAsPhotonEnergySum + m_trackRecoAsNeutralEnergySum +
        m_photonRecoAsNeutralEnergySum + m_neutralRecoAsNeutralEnergySum);

    std::cout << std::endl;

    if (m_count > 0)
    {
        const float confusionA = m_photonOrNeutralRecoAsTrackEnergySum / static_cast<float>(m_count);
        const float confusionB = m_trackRecoAsPhotonOrNeutralEnergySum / static_cast<float>(m_count);
        const float sigmaA = std::sqrt(m_photonOrNeutralRecoAsTrackEnergySum2 / static_cast<float>(m_count) - confusionA * confusionA);
        const float sigmaB = std::sqrt(m_trackRecoAsPhotonOrNeutralEnergySum2 / static_cast<float>(m_count) - confusionB * confusionB);
        const float covarianceAB = m_confusionCorrelation / static_cast<float>(m_count) - confusionA * confusionB;
        const float rhoAB = covarianceAB / sigmaA / sigmaB;

        std::cout << std::endl;
        std::cout << " Confusion covariance " << std::endl;
        std::cout << " photon/neutral reconstructed as charged mean : " << confusionA << std::endl;
        std::cout << "                                         rms  : " << sigmaA << std::endl;
        std::cout << " charged reconstructed as neutral/photon mean : " << confusionB << std::endl;
        std::cout << "                                         rms  : " << sigmaB << std::endl;
        std::cout << " Covariance                                   : " << covarianceAB << std::endl;
        std::cout << " Correlation coefficient                      : " << rhoAB << std::endl;
        std::cout << " Rms total confusion                          : " << std::sqrt(sigmaA * sigmaA + sigmaB * sigmaB + 2 * rhoAB * sigmaA * sigmaB) << std::endl;
        std::cout << " Rms net confusion                            : " << std::sqrt(sigmaA * sigmaA + sigmaB * sigmaB - 2 * rhoAB * sigmaA * sigmaB) << std::endl;
        std::cout << std::endl;

        const float nSumCal(sumCal / 100.f);

        if (nSumCal > std::numeric_limits<float>::epsilon())
        {
            const float econfmat[3][3] =
            {
                {m_trackRecoAsTrackEnergySum / nSumCal, m_photonRecoAsTrackEnergySum / nSumCal, m_neutralRecoAsTrackEnergySum / nSumCal},
                {m_trackRecoAsPhotonEnergySum / nSumCal, m_photonRecoAsPhotonEnergySum / nSumCal, m_neutralRecoAsPhotonEnergySum / nSumCal},
                {m_trackRecoAsNeutralEnergySum / nSumCal, m_photonRecoAsNeutralEnergySum / nSumCal, m_neutralRecoAsNeutralEnergySum / nSumCal}
            };

            FORMATTED_OUTPUT_CONFUSION(econfmat[0][0], econfmat[0][1], econfmat[0][2], econfmat[1][0], econfmat[1][1], econfmat[1][2], econfmat[2][0],
                econfmat[2][1], econfmat[2][2]);

            std::cout << std::endl;
            std::cout << " Well constructed PFOs summary " << std::endl;
            std::cout << std::endl;

            const float goodEnergy = m_goodTrackEnergySum + m_goodPhotonEnergySum + m_goodNeutralEnergySum;
            const float badEnergy = m_badTrackEnergySum  + m_badPhotonEnergySum  + m_badNeutralEnergySum;
            FORMATTED_OUTPUT_GOODENERGY(m_goodTrackEnergySum, m_goodPhotonEnergySum, m_goodNeutralEnergySum, goodEnergy);
            FORMATTED_OUTPUT_BADENERGY(m_badTrackEnergySum, m_badPhotonEnergySum, m_badNeutralEnergySum, badEnergy); 

            std::cout << std::endl;
            std::cout << " Particle ID Summary (Neutral/Photon Separation) " << std::endl;
            std::cout << std::endl;

            if (m_goodPhotonEnergySum > std::numeric_limits<float>::epsilon())
                std::cout <<  " Correctly IDed Photon  Energy Fraction : " << m_goodIdedPhotonEnergySum / m_goodPhotonEnergySum << std::endl;

            if (m_goodNeutralEnergySum > std::numeric_limits<float>::epsilon())
                std::cout << " Correctly IDed Neutral Energy Fraction : " << m_goodIdedNeutralEnergySum / m_goodNeutralEnergySum << std::endl;
        }
    }

    // Restore formatting options
    (void) std::cout.unsetf(std::ios::fixed);
    (void) std::cout.precision(originalPrecision);
    (void) std::cout.width(originalWidth);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode DumpPfosMonitoringAlgorithm::Run()
{
    m_trackMcPfoTargets.clear();
    m_trackToErrorTypeMap.clear();
    m_mcParticleToTrackMap.clear();
    m_trackRecoAsTrackEnergy = 0.f;
    m_trackRecoAsPhotonEnergy = 0.f;
    m_trackRecoAsNeutralEnergy = 0.f;
    m_photonRecoAsTrackEnergy = 0.f;
    m_photonRecoAsPhotonEnergy = 0.f;
    m_photonRecoAsNeutralEnergy = 0.f;
    m_neutralRecoAsTrackEnergy = 0.f;
    m_neutralRecoAsPhotonEnergy = 0.f;
    m_neutralRecoAsNeutralEnergy = 0.f;
    m_firstChargedPfoToPrint = true;
    m_firstNeutralPfoToPrint = true;
    m_firstPhotonPfoToPrint = true;
    m_goodTrackEnergy = 0.f;
    m_goodPhotonEnergy = 0.f;
    m_goodIdedPhotonEnergy = 0.f;
    m_goodNeutralEnergy = 0.f;
    m_goodIdedNeutralEnergy = 0.f;
    m_badTrackEnergy = 0.f;
    m_badPhotonEnergy = 0.f;
    m_badNeutralEnergy = 0.f;

    const PfoList *pPfoList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pPfoList));

    // Alter formatting options, caching original parameters
    const int originalPrecision(std::cout.precision(precision));
    const int originalWidth(std::cout.width());
    (void) std::cout.setf(std::ios::fixed, std::ios::floatfield);

    PfoList chargedPfos;
    PfoList photonPfos;
    PfoList neutralHadronPfos;

    float totalPfoEnergy(0.f);

    // First loop over pfos to make collections and save track mc pfo list
    for (PfoList::const_iterator pfoIter = pPfoList->begin(); pfoIter != pPfoList->end(); ++pfoIter)
    {
        ParticleFlowObject *pPfo = *pfoIter;
        totalPfoEnergy += pPfo->GetEnergy();

        const int pfoPid(pPfo->GetParticleId());
        const TrackList &trackList(pPfo->GetTrackList());

        if (trackList.size() > 0)
            chargedPfos.insert(pPfo);

        if (trackList.size() == 0)
            (pfoPid == PHOTON) ? photonPfos.insert(pPfo) : neutralHadronPfos.insert(pPfo);

        for (TrackList::const_iterator trackIter = trackList.begin(); trackIter != trackList.end(); ++trackIter)
        {
            try
            {
                const Track *pTrack = *trackIter;

                const MCParticle *pMCParticle(pTrack->GetMainMCParticle());
                m_trackMcPfoTargets.insert(pMCParticle);

                const TrackList &daughterTracks(pTrack->GetDaughterTrackList());

                if (!daughterTracks.empty())
                    continue;

                MCParticleToTrackMap::iterator it = m_mcParticleToTrackMap.find(pMCParticle);

                if (it == m_mcParticleToTrackMap.end())
                {
                    m_mcParticleToTrackMap.insert(MCParticleToTrackMap::value_type(pMCParticle,pTrack));
                }
                else
                {
                    const TrackList siblingTracks = pTrack->GetSiblingTrackList();

                    if (!siblingTracks.empty())
                        continue;

                    switch(pMCParticle->GetParticleId())
                    {
                    case PHOTON :
                        m_trackToErrorTypeMap.insert(TrackToErrorTypeMap::value_type(it->second, TRACK_STATUS_MISSED_CONVERSION));
                        m_trackToErrorTypeMap.insert(TrackToErrorTypeMap::value_type(pTrack, TRACK_STATUS_MISSED_CONVERSION));
                        std::cout << " Track appears twice in list - conversion " << pMCParticle->GetEnergy() << std::endl;
                        break;

                    case K_SHORT :
                        m_trackToErrorTypeMap.insert(TrackToErrorTypeMap::value_type(it->second, TRACK_STATUS_MISSED_KSHORT));
                        m_trackToErrorTypeMap.insert(TrackToErrorTypeMap::value_type(pTrack, TRACK_STATUS_MISSED_KSHORT));
                        std::cout << " Track appears twice in list - ks " << pMCParticle->GetEnergy() << std::endl;
                        break;

                    default:
                        m_trackToErrorTypeMap.insert(TrackToErrorTypeMap::value_type(it->second, TRACK_STATUS_SPLIT_TRACK));
                        m_trackToErrorTypeMap.insert(TrackToErrorTypeMap::value_type(pTrack, TRACK_STATUS_SPLIT_TRACK));
                        std::cout << " Track appears twice in list - split " << pMCParticle->GetEnergy() << std::endl;
                        break;
                    }
                }
            }
            catch (StatusCodeException &)
            {
            }
        }
    }

    if ((totalPfoEnergy > m_totalPfoEnergyDisplayGreaterThan) && (totalPfoEnergy < m_totalPfoEnergyDisplayLessThan))
    {
        std::cout << " <---------------------------------DumpPfosMonitoringAlgorithm----------------------------------------->" <<  std::endl;
        std::cout << " Total RECO PFO Energy = " << totalPfoEnergy << std::endl;

        // Now order the lists
        ParticleFlowObjectVector sortedChargedPfos(chargedPfos.begin(), chargedPfos.end());
        ParticleFlowObjectVector sortedPhotonPfos(photonPfos.begin(), photonPfos.end());
        ParticleFlowObjectVector sortedNeutralHadronPfos(neutralHadronPfos.begin(), neutralHadronPfos.end());
        std::sort(sortedChargedPfos.begin(), sortedChargedPfos.end(), SortingHelper::SortPfosByEnergy);
        std::sort(sortedPhotonPfos.begin(),  sortedPhotonPfos.end(), SortingHelper::SortPfosByEnergy);
        std::sort(sortedNeutralHadronPfos.begin(), sortedNeutralHadronPfos.end(), SortingHelper::SortPfosByEnergy);

        for (ParticleFlowObjectVector::const_iterator pfoIter = sortedChargedPfos.begin(); pfoIter != sortedChargedPfos.end(); ++pfoIter)
        {
            this->DumpChargedPfo(*pfoIter);
        }

        for (ParticleFlowObjectVector::const_iterator pfoIter = sortedPhotonPfos.begin(); pfoIter != sortedPhotonPfos.end(); ++pfoIter)
        {
            this->DumpPhotonPfo(*pfoIter);
        }

        for (ParticleFlowObjectVector::const_iterator pfoIter = sortedNeutralHadronPfos.begin(); pfoIter != sortedNeutralHadronPfos.end(); ++pfoIter)
        {
            this->DumpNeutralPfo(*pfoIter);
        }

        m_trackRecoAsTrackEnergySum     += m_trackRecoAsTrackEnergy;
        m_photonRecoAsTrackEnergySum    += m_photonRecoAsTrackEnergy;
        m_neutralRecoAsTrackEnergySum   += m_neutralRecoAsTrackEnergy;
        m_trackRecoAsPhotonEnergySum    += m_trackRecoAsPhotonEnergy;
        m_photonRecoAsPhotonEnergySum   += m_photonRecoAsPhotonEnergy; 
        m_neutralRecoAsPhotonEnergySum  += m_neutralRecoAsPhotonEnergy;
        m_trackRecoAsNeutralEnergySum   += m_trackRecoAsNeutralEnergy;
        m_photonRecoAsNeutralEnergySum  += m_photonRecoAsNeutralEnergy;
        m_neutralRecoAsNeutralEnergySum += m_neutralRecoAsNeutralEnergy;
        m_goodTrackEnergySum   += m_goodTrackEnergy;
        m_goodPhotonEnergySum  += m_goodPhotonEnergy;
        m_goodIdedPhotonEnergySum  += m_goodIdedPhotonEnergy;
        m_goodNeutralEnergySum += m_goodNeutralEnergy;
        m_goodIdedNeutralEnergySum += m_goodIdedNeutralEnergy;
        m_badTrackEnergySum    += m_badTrackEnergy;
        m_badPhotonEnergySum   += m_badPhotonEnergy;
        m_badNeutralEnergySum  += m_badNeutralEnergy;

        const float confmat[3][3] =
        {
            {m_trackRecoAsTrackEnergy, m_photonRecoAsTrackEnergy, m_neutralRecoAsTrackEnergy},
            {m_trackRecoAsPhotonEnergy, m_photonRecoAsPhotonEnergy, m_neutralRecoAsPhotonEnergy},
            {m_trackRecoAsNeutralEnergy, m_photonRecoAsNeutralEnergy, m_neutralRecoAsNeutralEnergy}
        };

        std::cout << std::endl;
        FORMATTED_OUTPUT_CONFUSION(confmat[0][0], confmat[0][1], confmat[0][2], confmat[1][0], confmat[1][1], confmat[1][2], confmat[2][0],
            confmat[2][1], confmat[2][2]);

        const float sumCal(m_trackRecoAsTrackEnergy + m_photonRecoAsTrackEnergy + m_neutralRecoAsTrackEnergy +
            m_trackRecoAsPhotonEnergy + m_photonRecoAsPhotonEnergy + m_neutralRecoAsPhotonEnergy + m_trackRecoAsNeutralEnergy +
            m_photonRecoAsNeutralEnergy + m_neutralRecoAsNeutralEnergy);

        const float nSumCal(sumCal / 100.f);

        if (nSumCal > 0.f)
        {
            m_count++;
            float confusionA = (m_photonRecoAsTrackEnergy + m_neutralRecoAsTrackEnergy) / nSumCal;
            float confusionB = (m_trackRecoAsPhotonEnergy + m_trackRecoAsNeutralEnergy) / nSumCal;

            m_photonOrNeutralRecoAsTrackEnergySum  += confusionA; 
            m_photonOrNeutralRecoAsTrackEnergySum2 += confusionA*confusionA;
            m_trackRecoAsPhotonOrNeutralEnergySum  += confusionB;
            m_trackRecoAsPhotonOrNeutralEnergySum2 += confusionB*confusionB;
            m_confusionCorrelation                 += confusionA*confusionB;

            const float econfmat[3][3] =
            {
                {m_trackRecoAsTrackEnergy / nSumCal, m_photonRecoAsTrackEnergy / nSumCal, m_neutralRecoAsTrackEnergy / nSumCal},
                {m_trackRecoAsPhotonEnergy / nSumCal, m_photonRecoAsPhotonEnergy / nSumCal, m_neutralRecoAsPhotonEnergy / nSumCal},
                {m_trackRecoAsNeutralEnergy / nSumCal, m_photonRecoAsNeutralEnergy / nSumCal, m_neutralRecoAsNeutralEnergy / nSumCal}
            };

            FORMATTED_OUTPUT_CONFUSION(econfmat[0][0], econfmat[0][1], econfmat[0][2], econfmat[1][0], econfmat[1][1], econfmat[1][2], econfmat[2][0],
                econfmat[2][1], econfmat[2][2]);

            float goodEnergy = m_goodTrackEnergy + m_goodPhotonEnergy + m_goodNeutralEnergy;
            float badEnergy  = m_badTrackEnergy  + m_badPhotonEnergy  + m_badNeutralEnergy;
            FORMATTED_OUTPUT_GOODENERGY(m_goodTrackEnergy, m_goodPhotonEnergy, m_goodNeutralEnergy, goodEnergy);
            FORMATTED_OUTPUT_BADENERGY(m_badTrackEnergy, m_badPhotonEnergy, m_badNeutralEnergy, badEnergy);
        }
    }

    // Restore formatting options
    (void) std::cout.unsetf(std::ios::fixed);
    (void) std::cout.precision(originalPrecision);
    (void) std::cout.width(originalWidth);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void DumpPfosMonitoringAlgorithm::DumpChargedPfo(const ParticleFlowObject *const pPfo)
{
    const TrackList &trackList(pPfo->GetTrackList());
    const int pfoPid(pPfo->GetParticleId());
    const float pfoEnergy(pPfo->GetEnergy());
    bool printedHeader(false);
    bool printThisPfo(false);

    for (TrackList::const_iterator trackIter = trackList.begin(); trackIter != trackList.end(); ++trackIter)
    {
        Track *pTrack = *trackIter;
        TrackErrorTypes trackStatus = TRACK_STATUS_OK;

        TrackToErrorTypeMap::const_iterator it = m_trackToErrorTypeMap.find(pTrack);

        if (it != m_trackToErrorTypeMap.end())
            trackStatus = it->second;

        const float trackEnergy(pTrack->GetEnergyAtDca());
        const int trackId(pTrack->GetParticleId());

        float clusterEnergy(0.f);
        float clusterTime(0.f);
        Cluster *pCluster(NULL);

        if (pTrack->HasAssociatedCluster())
        {
            pCluster = pTrack->GetAssociatedCluster();
            clusterEnergy += pCluster->GetTrackComparisonEnergy(this->GetPandora());
            clusterTime = this->ClusterTime(pCluster);
        }

        int mcId(0);
        float mcEnergy(0.);

        try
        {
            const MCParticle *pMCParticle(pTrack->GetMainMCParticle());
            mcId = pMCParticle->GetParticleId();
            mcEnergy = pMCParticle->GetEnergy();
        }
        catch (StatusCodeException &)
        {
        }

        std::string leaving(" ");
        const bool isLeaving(pCluster != NULL && ClusterHelper::IsClusterLeavingDetector(pCluster));

        if (isLeaving)
            leaving= "L";

        const float chi(ReclusterHelper::GetTrackClusterCompatibility(this->GetPandora(), clusterEnergy, trackEnergy));

        const TrackList &daughterTrackList(pTrack->GetDaughterTrackList());
        const bool isParent(!daughterTrackList.empty());
        const bool badChi((chi > m_minAbsChiToDisplay) || (chi < -m_minAbsChiToDisplay && !isLeaving && !isParent));

        float fCharged(0.f);
        float fPhoton(0.f);
        float fNeutral(0.f);
        const MCParticle *pBestMcMatch(NULL);

        if (pCluster != NULL)
            this->DumpPfosMonitoringAlgorithm::ClusterEnergyFractions(pCluster, fCharged, fPhoton, fNeutral, pBestMcMatch);

        // Fix for conversions (where both tracks are ided)
        if (trackStatus == TRACK_STATUS_MISSED_CONVERSION)
        {
            fCharged += fPhoton;
            fPhoton = 0;
        }

        m_trackRecoAsTrackEnergy += clusterEnergy * fCharged; 
        m_photonRecoAsTrackEnergy += clusterEnergy * fPhoton;
        m_neutralRecoAsTrackEnergy += clusterEnergy * fNeutral;

        (fCharged > m_goodFractionCut) ? m_goodTrackEnergy += clusterEnergy : m_badTrackEnergy+=clusterEnergy;

        const bool badConfusion(clusterEnergy * (fPhoton + fNeutral) > m_minConfusionEnergyToDisplay);

        // Decide whether to print
        if (pfoEnergy > m_minPfoEnergyToDisplay)
            printThisPfo = true;

        if (badConfusion || badChi)
            printThisPfo = true;

        if (printThisPfo)
        {
            if (m_firstChargedPfoToPrint)
            {
                // First loop to dump charged pfos
                std::cout << std::endl;
                FORMATTED_OUTPUT_PFO_HEADER_TITLE();
                FORMATTED_OUTPUT_TRACK_TITLE();
                m_firstChargedPfoToPrint = false;
            }

            if (!printedHeader)
            {
                FORMATTED_OUTPUT_PFO_HEADER(pfoPid, pfoEnergy);

                if (trackList.size() > 1)
                    std::cout << std::endl;

                printedHeader = true;
            }

            if (trackList.size() > 1)
            {
                FORMATTED_OUTPUT_PFO_HEADER("", "");
            }

            const pandora::CartesianVector ecalIntercept = (pTrack->GetTrackStateAtCalorimeter()).GetPosition();
            const float tof = ecalIntercept.GetMagnitude() / 300.;
            const float tTrack = pTrack->GetTimeAtCalorimeter() - tof;

            if (pCluster != NULL)
            {
                FORMATTED_OUTPUT_TRACK(trackId, mcId, pTrack->CanFormPfo(), pTrack->ReachesCalorimeter(), trackEnergy, mcEnergy,
                    clusterEnergy, clusterTime, chi, leaving, fCharged, fPhoton, fNeutral);
                std::cout << " tTrack = " << tTrack;
            }
            else
            {
                FORMATTED_OUTPUT_TRACK(trackId, mcId, pTrack->CanFormPfo(), pTrack->ReachesCalorimeter(), trackEnergy, mcEnergy,
                    " ", " ", chi, " ", " ", " ", " ");
                std::cout << " tTrack = " << tTrack;
            }

            if (badChi)
            {
                std::cout << " <-- Bad E-P consistency : " << chi << std::endl;
                continue;
            }

            if (badConfusion)
            {
                std::cout << " <-- confusion : " << clusterEnergy * (fPhoton + fNeutral) << " GeV " << std::endl;
                continue;
            }

            if (trackStatus == TRACK_STATUS_SPLIT_TRACK)
            {
                std::cout << " <-- split track" << std::endl;
                continue;
            }

            if (trackStatus == TRACK_STATUS_MISSED_CONVERSION)
            {
                std::cout << " <-- missed conversion" << std::endl;
                continue;
            }

            if (trackStatus == TRACK_STATUS_MISSED_KSHORT)
            {
                std::cout << " <-- missed kshort" << std::endl;
                continue;
            }
            std::cout << std::endl;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void DumpPfosMonitoringAlgorithm::DumpNeutralOrPhotonPfo(const ParticleFlowObject *const pPfo, bool isPhotonPfo)
{
    float fCharged(0.f);
    float fPhoton(0.f);
    float fNeutral(0.f);
    bool printThisPfo(false);
    bool printedHeader(false);

    const int pfoPid(pPfo->GetParticleId());
    const float pfoEnergy(pPfo->GetEnergy());
    const ClusterList &clusterList(pPfo->GetClusterList());

    for (ClusterList::const_iterator clusterIter = clusterList.begin(); clusterIter != clusterList.end(); ++clusterIter)
    {
        Cluster *pCluster = *clusterIter;
        const float clusterEnergy(pCluster->GetHadronicEnergy());
        const float clusterTime = this->ClusterTime(pCluster);

        const MCParticle *pBestMCMatch(NULL);
        this->DumpPfosMonitoringAlgorithm::ClusterEnergyFractions(pCluster, fCharged, fPhoton, fNeutral, pBestMCMatch);

        if (isPhotonPfo)
        {
            m_trackRecoAsPhotonEnergy += clusterEnergy * fCharged;
            m_photonRecoAsPhotonEnergy += clusterEnergy * fPhoton;
            m_neutralRecoAsPhotonEnergy += clusterEnergy * fNeutral;

            if ((fPhoton + fNeutral) > m_goodFractionCut)
            {
                m_goodPhotonEnergy += clusterEnergy;

                if (fPhoton > fNeutral)
                    m_goodIdedPhotonEnergy += clusterEnergy;
            }
            else
            {
                m_badPhotonEnergy += clusterEnergy;
            }
        }
        else
        {
            m_trackRecoAsNeutralEnergy += clusterEnergy * fCharged;
            m_photonRecoAsNeutralEnergy += clusterEnergy * fPhoton;
            m_neutralRecoAsNeutralEnergy += clusterEnergy * fNeutral;

            if ((fNeutral + fPhoton) > m_goodFractionCut)
            {
                m_goodNeutralEnergy += clusterEnergy;

                if (fNeutral > fPhoton)
                    m_goodIdedNeutralEnergy += clusterEnergy;
            }
            else
            {
                m_badNeutralEnergy+=clusterEnergy;
            }
        }

        const bool badConfusion((clusterEnergy * fCharged) > m_minConfusionEnergyToDisplay);
        const bool badFragment((fCharged > 0.8f) && ((clusterEnergy * fCharged) > m_fragmentEnergyToDisplay));
        bool badTrackMatch((fCharged > 0.95f) && ((clusterEnergy * fCharged) >m_fragmentEnergyToDisplay));

        if (badTrackMatch)
        {
            MCParticleToTrackMap::iterator it = m_mcParticleToTrackMap.find(pBestMCMatch);

            if (it != m_mcParticleToTrackMap.end())
            {
                if (it->second->HasAssociatedCluster())
                    badTrackMatch = false;
            }
        }

        const bool badPhotonId( (!isPhotonPfo && (fPhoton > 0.8f) && ((clusterEnergy * fPhoton)  > m_photonIdEnergyToDisplay))
            ||( isPhotonPfo && (fNeutral > 0.8f) && ((clusterEnergy * fNeutral)> m_photonIdEnergyToDisplay)) );

        if (pfoEnergy > m_minPfoEnergyToDisplay)
            printThisPfo = true;

        if (badConfusion || badFragment || badPhotonId)
            printThisPfo = true;

        if (printThisPfo)
        {
            if (m_firstNeutralPfoToPrint)
            {
                // First loop to dump charged pfos
                std::cout << std::endl;
                FORMATTED_OUTPUT_PFO_HEADER_TITLE();
                FORMATTED_OUTPUT_NEUTRAL_TITLE();
                m_firstNeutralPfoToPrint = false;
            }

            if (!printedHeader)
            {
                FORMATTED_OUTPUT_PFO_HEADER(pfoPid, pfoEnergy);
                printedHeader = true;
            }

            const float showerProfileStart(pCluster->GetShowerProfileStart(this->GetPandora()));
            const float showerProfileDiscrepancy(pCluster->GetShowerProfileDiscrepancy(this->GetPandora()));

            FORMATTED_OUTPUT_NEUTRAL(clusterEnergy, clusterTime, fCharged, fPhoton, fNeutral, pCluster->GetInnerPseudoLayer(), pCluster->GetOuterPseudoLayer(),
                ((showerProfileStart == std::numeric_limits<float>::max()) ? -99 : showerProfileStart),
                ((showerProfileDiscrepancy == std::numeric_limits<float>::max()) ? -99 : showerProfileDiscrepancy));

            if (badTrackMatch)
            {
                std::cout << " <-- bad track match  : " << clusterEnergy * fCharged << " GeV ";

                if (pBestMCMatch != NULL)
                    std::cout << "(" << pBestMCMatch->GetEnergy() << " ) ";

                std::cout << std::endl;
                continue;
            }

            if (badFragment)
            {
                std::cout << " <-- fragment  : " << clusterEnergy * fCharged << " GeV ";

                if (pBestMCMatch != NULL)
                    std::cout << "(" << pBestMCMatch->GetEnergy() << " ) ";

                std::cout << std::endl;
                continue;
            }

            if (badConfusion)
            {
                std::cout << " <-- confusion : " << clusterEnergy * fCharged << " GeV " << std::endl;
                continue;
            }

            if (badPhotonId)
            {
                if (isPhotonPfo)
                {
                    std::cout << " <-- neutral hadron : " << clusterEnergy * fNeutral << " GeV " << std::endl;
                }
                else
                {
                    std::cout << " <-- photon    : " << clusterEnergy * fPhoton << " GeV " << std::endl;
                }

                continue;
            }

            std::cout << std::endl;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void DumpPfosMonitoringAlgorithm::ClusterEnergyFractions(const Cluster *const pCluster, float &fCharged, float &fPhoton, float &fNeutral,
    const MCParticle *&pBestMatchedMcPfo) const
{
    pBestMatchedMcPfo = NULL;
    float totEnergy(0.f);
    float neutralEnergy(0.f);
    float photonEnergy(0.f);
    float chargedEnergy(0.f);

    MCParticleToFloatMap mcParticleContributions;

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            try
            {
                CaloHit *pCaloHit = *hitIter;
                const MCParticle *pMCParticle(MCParticleHelper::GetMainMCParticle(pCaloHit));
                const MCParticle *pMCPfoTarget(pMCParticle->GetPfoTarget());

                totEnergy += pCaloHit->GetHadronicEnergy();
                MCParticleToFloatMap::iterator it = mcParticleContributions.find(pMCPfoTarget);

                if (it != mcParticleContributions.end())
                {
                    it->second += pCaloHit->GetHadronicEnergy();
                }
                else
                {
                    mcParticleContributions.insert(MCParticleToFloatMap::value_type(pMCPfoTarget, pCaloHit->GetHadronicEnergy()));
                }

                const int pdgCode(pMCPfoTarget->GetParticleId());
                const int charge(PdgTable::GetParticleCharge(pdgCode));

                if ((charge != 0) || (std::abs(pdgCode) == LAMBDA) || (std::abs(pdgCode) == K_SHORT))
                {
                    if (m_trackMcPfoTargets.count(pMCParticle) == 0)
                    {
                        neutralEnergy += pCaloHit->GetHadronicEnergy();
                    }
                    else
                    {
                        chargedEnergy += pCaloHit->GetHadronicEnergy();
                    }
                }
                else
                {
                    (pMCParticle->GetParticleId() == PHOTON) ? photonEnergy += pCaloHit->GetHadronicEnergy() : neutralEnergy += pCaloHit->GetHadronicEnergy();
                }
            }
            catch (StatusCodeException &)
            {
            }
        }
    }

    if (totEnergy > 0.f)
    {
        fCharged = chargedEnergy/totEnergy;
        fPhoton  = photonEnergy/totEnergy;
        fNeutral = neutralEnergy/totEnergy;
    }

    // Find mc particle with largest associated energy
    float maximumEnergy(0.f);

    for (MCParticleToFloatMap::const_iterator iter = mcParticleContributions.begin(), iterEnd = mcParticleContributions.end(); iter != iterEnd; ++iter)
    {
        if (iter->second > maximumEnergy)
        {
            maximumEnergy = iter->second;
            pBestMatchedMcPfo = iter->first;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

float DumpPfosMonitoringAlgorithm::ClusterTime(const Cluster *const pCluster) const
{
    float sumTimeEnergy(0.f);
    float sumEnergy(0.f);
    float avTime(0.f);

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            CaloHit *pCaloHit = *hitIter;

            sumEnergy += pCaloHit->GetHadronicEnergy();
            sumTimeEnergy += pCaloHit->GetHadronicEnergy()*pCaloHit->GetTime();
        }
    }

    if (sumEnergy > 0.f)
        avTime = sumTimeEnergy / sumEnergy;

    return avTime;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode DumpPfosMonitoringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinPfoEnergyToDisplay", m_minPfoEnergyToDisplay));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinAbsChiToDisplay", m_minAbsChiToDisplay));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinConfusionEnergyToDisplay", m_minConfusionEnergyToDisplay));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinFragmentEnergyToDisplay", m_minFragmentEnergyToDisplay));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TotalPfoEnergyDisplayLessThan", m_totalPfoEnergyDisplayLessThan));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TotalPfoEnergyDisplayGreaterThan", m_totalPfoEnergyDisplayGreaterThan));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FragmentEnergyToDisplay", m_fragmentEnergyToDisplay));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdEnergyToDisplay", m_photonIdEnergyToDisplay));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "GoodFractionCut", m_goodFractionCut));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
