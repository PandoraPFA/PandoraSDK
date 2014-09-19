/**
 *  @file   LCContent/src/LCParticleId/PhotonReconstructionAlgorithm.cc
 * 
 *  @brief  Implementation of the photon reconstruction algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCHelpers/ClusterHelper.h"
#include "LCHelpers/SortingHelper.h"

#include "LCParticleId/PhotonReconstructionAlgorithm.h"

using namespace pandora;

namespace lc_content
{

PhotonReconstructionAlgorithm::PhotonReconstructionAlgorithm() :
    m_replaceCurrentClusterList(false),
    m_shouldMakePdfHistograms(false),
    m_shouldDrawPdfHistograms(false),
    m_nEnergyBins(0),
    m_pSigPeakRms(NULL),
    m_pBkgPeakRms(NULL),
    m_pSigLongProfileStart(NULL),
    m_pBkgLongProfileStart(NULL),
    m_pSigLongProfileDiscrepancy(NULL),
    m_pBkgLongProfileDiscrepancy(NULL),
    m_pSigPeakEnergyFraction(NULL),
    m_pBkgPeakEnergyFraction(NULL),
    m_pSigMinDistanceToTrack(NULL),
    m_pBkgMinDistanceToTrack(NULL),
    m_pidCut(0.4f),
    m_minClusterEnergy(0.2f),
    m_transProfileMaxLayer(30),
    m_minPeakEnergy(0.2f),
    m_maxPeakRms(5.f),
    m_minPeakCaloHits(5),
    m_maxLongProfileStart(10.f),
    m_maxLongProfileDiscrepancy(1.f),
    m_maxSearchLayer(9),
    m_parallelDistanceCut(100.f),
    m_minTrackClusterCosAngle(0.f),
    m_minDistanceToTrackCut(3.f),
    m_oldClusterEnergyFraction0(0.95f),
    m_oldClusterEnergyFraction1(0.9f),
    m_oldClusterEnergyDifference1(2.f),
    m_oldClusterEnergyFraction2(0.8f),
    m_oldClusterEnergyDifference2(1.f),
    m_oldClusterEnergyFraction3(0.5f),
    m_oldClusterEnergyDifference3(0.5f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

PhotonReconstructionAlgorithm::~PhotonReconstructionAlgorithm()
{
    if (m_shouldMakePdfHistograms)
        this->NormalizeAndWriteHistograms();

    if (m_shouldDrawPdfHistograms)
        this->DrawHistograms();

    this->DeleteHistograms();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::Run()
{
    // Obtain current lists for later reference
    const ClusterList *pInputClusterList = NULL;
    std::string inputClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pInputClusterList, inputClusterListName));

    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pTrackList));

    TrackVector trackVector(pTrackList->begin(), pTrackList->end());
    std::sort(trackVector.begin(), trackVector.end(), SortingHelper::SortTracksByEnergy);

    // Create list of candidate photon clusters
    const ClusterList *pPhotonClusterList = NULL;
    std::string photonClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_photonClusteringAlgName,
        pPhotonClusterList, photonClusterListName));

    if (pPhotonClusterList->empty())
        return STATUS_CODE_SUCCESS;

    // Fragmentation can only proceed with reference to a saved cluster list, so save these temporary clusters
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList<Cluster>(*this, m_clusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<Cluster>(*this, m_clusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pPhotonClusterList));

    // Sort the clusters, then examine each cluster profile to obtain best-guess photon clusters. Delete non-photon clusters.
    ClusterVector clusterVector(pPhotonClusterList->begin(), pPhotonClusterList->end());
    std::sort(clusterVector.begin(), clusterVector.end(), SortingHelper::SortClustersByInnerLayer);

    const ShowerProfilePlugin *const pShowerProfilePlugin(PandoraContentApi::GetPlugins(*this)->GetShowerProfilePlugin());

    for (ClusterVector::iterator iter = clusterVector.begin(), iterEnd = clusterVector.end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;
        *iter = NULL;

        bool usedCluster(false);

        if ((pCluster->GetElectromagneticEnergy() > m_minClusterEnergy) && (PandoraContentApi::GetGeometry(*this)->GetHitTypeGranularity(pCluster->GetInnerLayerHitType()) <= FINE))
        {
            // Look for peaks in cluster transverse shower profile
            ShowerProfilePlugin::ShowerPeakList showerPeakList;
            pShowerProfilePlugin->CalculateTransverseProfile(pCluster, m_transProfileMaxLayer, showerPeakList);

            if (!showerPeakList.empty())
            {
                ClusterList clusterList;
                clusterList.insert(pCluster);

                std::string originalClusterListName, peakClusterListName;
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::InitializeFragmentation(*this, clusterList,
                    originalClusterListName, peakClusterListName));

                bool useOriginalCluster(false);

                for (unsigned int iPeak = 0, iPeakEnd = showerPeakList.size(); iPeak < iPeakEnd; ++iPeak)
                {
                    // Extract cluster corresponding to identified shower peak
                    const ShowerProfilePlugin::ShowerPeak &showerPeak(showerPeakList[iPeak]);

                    if (showerPeak.GetPeakEnergy() < m_minPeakEnergy)
                        continue;

                    if (showerPeak.GetPeakRms() > m_maxPeakRms)
                        continue;

                    if (showerPeak.GetPeakCaloHitList().size() < m_minPeakCaloHits)
                        continue;

                    Cluster *pPeakCluster = NULL;
                    PandoraContentApi::Cluster::Parameters parameters;
                    parameters.m_caloHitList = showerPeak.GetPeakCaloHitList();
                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, parameters, pPeakCluster));

                    // Decide whether to accept cluster from shower peak region as a photon candidate
                    bool acceptPhotonCandidate(false);
                    const float longProfileStart(pPeakCluster->GetShowerProfileStart(this->GetPandora()));
                    const float longProfileDiscrepancy(pPeakCluster->GetShowerProfileDiscrepancy(this->GetPandora()));

                    const float peakEnergyFraction(pPeakCluster->GetElectromagneticEnergy() / pCluster->GetElectromagneticEnergy());
                    const float minDistanceToTrack(this->GetMinDistanceToTrack(pPeakCluster, trackVector));

                    // Multivariate/pid analysis to identify photon candidate clusters
                    if ((longProfileStart < m_maxLongProfileStart) && (longProfileDiscrepancy < m_maxLongProfileDiscrepancy)
                        && (minDistanceToTrack > m_minDistanceToTrackCut))
                    {
                        if (!m_shouldMakePdfHistograms)
                        {
                            const float pid(this->GetPid(pPeakCluster, showerPeak.GetPeakRms(), longProfileStart, longProfileDiscrepancy,
                                peakEnergyFraction, minDistanceToTrack));

                            if (pid > m_pidCut)
                                acceptPhotonCandidate = true;
                        }
                        else
                        {
                            this->FillPdfHistograms(pPeakCluster, showerPeak.GetPeakRms(), longProfileStart, longProfileDiscrepancy,
                                peakEnergyFraction, minDistanceToTrack);
                        }
                    }

                    // If shower peak represents large fraction of original cluster, revert to original cluster
                    if (acceptPhotonCandidate && (0 == iPeak))
                    {
                        const float peakEnergyDifference(pCluster->GetElectromagneticEnergy() - pPeakCluster->GetElectromagneticEnergy());

                        if ((showerPeakList.size() == 1) || (peakEnergyFraction > m_oldClusterEnergyFraction0))
                        {
                            useOriginalCluster = true;
                        }
                        else if ((peakEnergyFraction > m_oldClusterEnergyFraction1) && (peakEnergyDifference < m_oldClusterEnergyDifference1))
                        {
                            useOriginalCluster = true;
                        }
                        else if ((peakEnergyFraction > m_oldClusterEnergyFraction2) && (peakEnergyDifference < m_oldClusterEnergyDifference2))
                        {
                            useOriginalCluster = true;
                        }
                        else if ((peakEnergyFraction > m_oldClusterEnergyFraction3) && (peakEnergyDifference < m_oldClusterEnergyDifference3))
                        {
                            useOriginalCluster = true;
                        }
                    }

                    // Decide whether or not to keep shower peak fragment
                    if (acceptPhotonCandidate)
                    {
                        usedCluster = true;
                        useOriginalCluster ? pCluster->SetIsFixedPhotonFlag(true) : pPeakCluster->SetIsFixedPhotonFlag(true);
                    }
                    else
                    {
                        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Delete(*this, pPeakCluster));
                    }

                    if (useOriginalCluster)
                        break;
                }

                const std::string clusterListToSaveName((usedCluster && !useOriginalCluster) ? peakClusterListName : originalClusterListName);
                const std::string clusterListToDeleteName((usedCluster && !useOriginalCluster) ? originalClusterListName : peakClusterListName);
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::EndFragmentation(*this, clusterListToSaveName,
                    clusterListToDeleteName));
            }
        }

        // Delete clusters from which no components have been used
        if (!usedCluster)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Delete(*this, pCluster));
        }
    }

    const std::string replacementListName(m_replaceCurrentClusterList ? m_clusterListName : inputClusterListName);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<Cluster>(*this, replacementListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float PhotonReconstructionAlgorithm::GetMinDistanceToTrack(const Cluster *const pPeakCluster, const TrackVector &trackVector) const
{
    float minDistance(std::numeric_limits<float>::max());
    float minEnergyDifference(std::numeric_limits<float>::max());

    for (TrackVector::const_iterator trackIter = trackVector.begin(), trackIterEnd = trackVector.end(); trackIter != trackIterEnd; ++trackIter)
    {
        Track *pTrack = *trackIter;
        float trackClusterDistance(std::numeric_limits<float>::max());

        if (STATUS_CODE_SUCCESS == ClusterHelper::GetTrackClusterDistance(pTrack, pPeakCluster, m_maxSearchLayer, m_parallelDistanceCut,
            m_minTrackClusterCosAngle, trackClusterDistance))
        {
            const float energyDifference(std::fabs(pPeakCluster->GetHadronicEnergy() - pTrack->GetEnergyAtDca()));

            if ((trackClusterDistance < minDistance) || ((trackClusterDistance == minDistance) && (energyDifference < minEnergyDifference)))
            {
                minDistance = trackClusterDistance;
                minEnergyDifference = energyDifference;
            }
        }
    }

    return minDistance;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float PhotonReconstructionAlgorithm::GetPid(const Cluster *const pPeakCluster, const float peakRms, const float longProfileStart,
    const float longProfileDiscrepancy, const float peakEnergyFraction, const float minDistanceToTrack) const
{
    double pid(0.);
    const unsigned int energyBin(this->GetEnergyBin(pPeakCluster->GetElectromagneticEnergy()));

    const double yes(static_cast<double>(this->GetHistogramContent(m_pSigPeakRms[energyBin], peakRms)) *
        static_cast<double>(this->GetHistogramContent(m_pSigLongProfileStart[energyBin], longProfileStart)) *
        static_cast<double>(this->GetHistogramContent(m_pSigLongProfileDiscrepancy[energyBin], longProfileDiscrepancy)) *
        static_cast<double>(this->GetHistogramContent(m_pSigPeakEnergyFraction[energyBin], peakEnergyFraction)) *
        static_cast<double>(this->GetHistogramContent(m_pSigMinDistanceToTrack[energyBin], minDistanceToTrack)));

    const double no(static_cast<double>(this->GetHistogramContent(m_pBkgPeakRms[energyBin], peakRms)) *
        static_cast<double>(this->GetHistogramContent(m_pBkgLongProfileStart[energyBin], longProfileStart)) *
        static_cast<double>(this->GetHistogramContent(m_pBkgLongProfileDiscrepancy[energyBin], longProfileDiscrepancy)) *
        static_cast<double>(this->GetHistogramContent(m_pBkgPeakEnergyFraction[energyBin], peakEnergyFraction)) *
        static_cast<double>(this->GetHistogramContent(m_pBkgMinDistanceToTrack[energyBin], minDistanceToTrack)));

    if ((yes + no) > 0.)
        pid = yes / (yes + no);

    return static_cast<float>(pid);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PhotonReconstructionAlgorithm::FillPdfHistograms(const Cluster *const pPeakCluster, const float peakRms, const float longProfileStart,
    const float longProfileDiscrepancy, const float peakEnergyFraction, const float minDistanceToTrack) const
{
    const MCParticle *pMCParticle(NULL);

    try
    {
        pMCParticle = MCParticleHelper::GetMainMCParticle(pPeakCluster);
    }
    catch (StatusCodeException &)
    {
    }

    const unsigned int energyBin(this->GetEnergyBin(pPeakCluster->GetElectromagneticEnergy()));

    if ((NULL != pMCParticle) && (PHOTON == pMCParticle->GetParticleId()))
    {
        m_pSigPeakRms[energyBin]->Fill(peakRms);
        m_pSigLongProfileStart[energyBin]->Fill(longProfileStart);
        m_pSigLongProfileDiscrepancy[energyBin]->Fill(longProfileDiscrepancy);
        m_pSigPeakEnergyFraction[energyBin]->Fill(peakEnergyFraction);
        m_pSigMinDistanceToTrack[energyBin]->Fill(minDistanceToTrack);
    }
    else
    {
        m_pBkgPeakRms[energyBin]->Fill(peakRms);
        m_pBkgLongProfileStart[energyBin]->Fill(longProfileStart);
        m_pBkgLongProfileDiscrepancy[energyBin]->Fill(longProfileDiscrepancy);
        m_pBkgPeakEnergyFraction[energyBin]->Fill(peakEnergyFraction);
        m_pBkgMinDistanceToTrack[energyBin]->Fill(minDistanceToTrack);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PhotonReconstructionAlgorithm::NormalizeAndWriteHistograms()
{
    std::string energyBinLowerEdgesString;

    for (unsigned int energyBin = 0; energyBin < m_nEnergyBins; ++energyBin)
    {
        this->NormalizeHistogram(m_pSigPeakRms[energyBin]);
        this->NormalizeHistogram(m_pBkgPeakRms[energyBin]);
        this->NormalizeHistogram(m_pSigLongProfileStart[energyBin]);
        this->NormalizeHistogram(m_pBkgLongProfileStart[energyBin]);
        this->NormalizeHistogram(m_pSigLongProfileDiscrepancy[energyBin]);
        this->NormalizeHistogram(m_pBkgLongProfileDiscrepancy[energyBin]);
        this->NormalizeHistogram(m_pSigPeakEnergyFraction[energyBin]);
        this->NormalizeHistogram(m_pBkgPeakEnergyFraction[energyBin]);
        this->NormalizeHistogram(m_pSigMinDistanceToTrack[energyBin]);
        this->NormalizeHistogram(m_pBkgMinDistanceToTrack[energyBin]);
        energyBinLowerEdgesString += TypeToString(m_energyBinLowerEdges[energyBin]) + " ";
    }

    TiXmlDocument xmlDocument;
    TiXmlElement *pEnergyBinLowerEdgesElement = new TiXmlElement("EnergyBinLowerEdges");
    pEnergyBinLowerEdgesElement->LinkEndChild(new TiXmlText(energyBinLowerEdgesString));
    xmlDocument.LinkEndChild(pEnergyBinLowerEdgesElement);

    for (unsigned int energyBin = 0; energyBin < m_nEnergyBins; ++energyBin)
    {
        m_pSigPeakRms[energyBin]->WriteToXml(&xmlDocument, "PhotonSigPeakRms_" + TypeToString(energyBin));
        m_pBkgPeakRms[energyBin]->WriteToXml(&xmlDocument, "PhotonBkgPeakRms_" + TypeToString(energyBin));
        m_pSigLongProfileStart[energyBin]->WriteToXml(&xmlDocument, "PhotonSigLongProfileStart_" + TypeToString(energyBin));
        m_pBkgLongProfileStart[energyBin]->WriteToXml(&xmlDocument, "PhotonBkgLongProfileStart_" + TypeToString(energyBin));
        m_pSigLongProfileDiscrepancy[energyBin]->WriteToXml(&xmlDocument, "PhotonSigLongProfileDiscrepancy_" + TypeToString(energyBin));
        m_pBkgLongProfileDiscrepancy[energyBin]->WriteToXml(&xmlDocument, "PhotonBkgLongProfileDiscrepancy_" + TypeToString(energyBin));
        m_pSigPeakEnergyFraction[energyBin]->WriteToXml(&xmlDocument, "PhotonSigPeakEnergyFraction_" + TypeToString(energyBin));
        m_pBkgPeakEnergyFraction[energyBin]->WriteToXml(&xmlDocument, "PhotonBkgPeakEnergyFraction_" + TypeToString(energyBin));
        m_pSigMinDistanceToTrack[energyBin]->WriteToXml(&xmlDocument, "PhotonSigMinDistanceToTrack_" + TypeToString(energyBin));
        m_pBkgMinDistanceToTrack[energyBin]->WriteToXml(&xmlDocument, "PhotonBkgMinDistanceToTrack_" + TypeToString(energyBin));
    }

    xmlDocument.SaveFile(m_histogramFile);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PhotonReconstructionAlgorithm::DrawHistograms() const
{
    for (unsigned int energyBin = 0; energyBin < m_nEnergyBins; ++energyBin)
    {
        std::cout << "PDF EnergyBin " << energyBin << std::endl << "PeakRms, Signal, Background " << std::endl;
        PANDORA_MONITORING_API(DrawPandoraHistogram(this->GetPandora(), *m_pSigPeakRms[energyBin]));
        PANDORA_MONITORING_API(DrawPandoraHistogram(this->GetPandora(), *m_pBkgPeakRms[energyBin]));
        std::cout << "LongProfileStart, Signal, Background " << std::endl;
        PANDORA_MONITORING_API(DrawPandoraHistogram(this->GetPandora(), *m_pSigLongProfileStart[energyBin]));
        PANDORA_MONITORING_API(DrawPandoraHistogram(this->GetPandora(), *m_pBkgLongProfileStart[energyBin]));
        std::cout << "LongProfileDiscrepancy, Signal, Background " << std::endl;
        PANDORA_MONITORING_API(DrawPandoraHistogram(this->GetPandora(), *m_pSigLongProfileDiscrepancy[energyBin]));
        PANDORA_MONITORING_API(DrawPandoraHistogram(this->GetPandora(), *m_pBkgLongProfileDiscrepancy[energyBin]));
        std::cout << "PeakEnergyFraction, Signal, Background " << std::endl;
        PANDORA_MONITORING_API(DrawPandoraHistogram(this->GetPandora(), *m_pSigPeakEnergyFraction[energyBin]));
        PANDORA_MONITORING_API(DrawPandoraHistogram(this->GetPandora(), *m_pBkgPeakEnergyFraction[energyBin]));
        std::cout << "MinDistanceToTrack, Signal, Background " << std::endl;
        PANDORA_MONITORING_API(DrawPandoraHistogram(this->GetPandora(), *m_pSigMinDistanceToTrack[energyBin]));
        PANDORA_MONITORING_API(DrawPandoraHistogram(this->GetPandora(), *m_pBkgMinDistanceToTrack[energyBin]));
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PhotonReconstructionAlgorithm::DeleteHistograms()
{
    for (unsigned int energyBin = 0; energyBin < m_nEnergyBins; ++energyBin)
    {
        delete m_pSigPeakRms[energyBin];
        delete m_pBkgPeakRms[energyBin];
        delete m_pSigLongProfileStart[energyBin];
        delete m_pBkgLongProfileStart[energyBin];
        delete m_pSigLongProfileDiscrepancy[energyBin];
        delete m_pBkgLongProfileDiscrepancy[energyBin];
        delete m_pSigPeakEnergyFraction[energyBin];
        delete m_pBkgPeakEnergyFraction[energyBin];
        delete m_pSigMinDistanceToTrack[energyBin];
        delete m_pBkgMinDistanceToTrack[energyBin];
    }

    delete [] m_pSigPeakRms;
    delete [] m_pBkgPeakRms;
    delete [] m_pSigLongProfileStart;
    delete [] m_pBkgLongProfileStart;
    delete [] m_pSigLongProfileDiscrepancy;
    delete [] m_pBkgLongProfileDiscrepancy;
    delete [] m_pSigPeakEnergyFraction;
    delete [] m_pBkgPeakEnergyFraction;
    delete [] m_pSigMinDistanceToTrack;
    delete [] m_pBkgMinDistanceToTrack;
}

//------------------------------------------------------------------------------------------------------------------------------------------

unsigned int PhotonReconstructionAlgorithm::GetEnergyBin(const float energy) const
{
    FloatVector::const_iterator binEdgesIter = std::upper_bound(m_energyBinLowerEdges.begin(), m_energyBinLowerEdges.end(), energy);
    const unsigned int edgeElementNumber(std::distance(m_energyBinLowerEdges.begin(), binEdgesIter));

    if (0 == edgeElementNumber)
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);

    return static_cast<unsigned int>(edgeElementNumber - 1);
}

//------------------------------------------------------------------------------------------------------------------------------------------

float PhotonReconstructionAlgorithm::GetHistogramContent(const Histogram *const pHistogram, const float value) const
{
    const float binWidth(pHistogram->GetXBinWidth());

    if (binWidth < std::numeric_limits<float>::epsilon())
    {
        std::cout << "PhotonReconstructionAlgorithm: Cannot extract value from histogram, invalid bin width " << std::endl;
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }

    const int binNumber(std::max(0, std::min(pHistogram->GetNBinsX() - 1, static_cast<int>(value / binWidth))));
    return pHistogram->GetBinContent(binNumber);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PhotonReconstructionAlgorithm::NormalizeHistogram(Histogram *const pHistogram) const
{
    const float cumulativeSum(pHistogram->GetCumulativeSum());

    if (std::fabs(cumulativeSum) < std::numeric_limits<float>::epsilon())
    {
        std::cout << "PhotonReconstructionAlgorithm: Cannot normalize empty histogram " << std::endl;
        return;
    }

    pHistogram->Scale(1.f / cumulativeSum);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle,
        "PhotonClusterFormation", m_photonClusteringAlgName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "ReplaceCurrentClusterList", m_replaceCurrentClusterList));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "ClusterListName", m_clusterListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "HistogramFile", m_histogramFile));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "NEnergyBins", m_nEnergyBins));

    if (0 == m_nEnergyBins)
    {
        std::cout << "PhotonReconstructionAlgorithm::ReadSettings - Invalid number PDF energy bins specified." << std::endl;
        return STATUS_CODE_INVALID_PARAMETER;
    }

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldMakePdfHistograms", m_shouldMakePdfHistograms));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldDrawPdfHistograms", m_shouldDrawPdfHistograms));

    if (m_shouldMakePdfHistograms)
    {
        int peakRmsNBins = 50;
        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "PeakRmsNBins", peakRmsNBins));

        float peakRmsLowValue = 0.f;
        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "PeakRmsLowValue", peakRmsLowValue));

        float peakRmsHighValue = 5.f;
        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "PeakRmsHighValue", peakRmsHighValue));

        int longProfileStartNBins = 11;
        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "LongProfileStartNBins", longProfileStartNBins));

        float longProfileStartLowValue = -0.5f;
        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "LongProfileStartLowValue", longProfileStartLowValue));

        float longProfileStartHighValue = 10.5f;
        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "LongProfileStartHighValue", longProfileStartHighValue));

        int longProfileDiscrepancyNBins = 52;
        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "LongProfileDiscrepancyNBins", longProfileDiscrepancyNBins));

        float longProfileDiscrepancyLowValue = -0.02f;
        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "LongProfileDiscrepancyLowValue", longProfileDiscrepancyLowValue));

        float longProfileDiscrepancyHighValue = 1.02f;
        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "LongProfileDiscrepancyHighValue", longProfileDiscrepancyHighValue));

        int peakEnergyFractionNBins = 52;
        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "PeakEnergyFractionNBins", peakEnergyFractionNBins));

        float peakEnergyFractionLowValue = -0.02f;
        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "PeakEnergyFractionLowValue", peakEnergyFractionLowValue));

        float peakEnergyFractionHighValue = 1.02f;
        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "PeakEnergyFractionHighValue", peakEnergyFractionHighValue));

        int minDistanceToTrackNBins = 50;
        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "MinDistanceToTrackNBins", minDistanceToTrackNBins));

        float minDistanceToTrackLowValue = 0.f;
        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "MinDistanceToTrackLowValue", minDistanceToTrackLowValue));

        float minDistanceToTrackHighValue = 500.f;
        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "MinDistanceToTrackHighValue", minDistanceToTrackHighValue));

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
            "EnergyBinLowerEdges", m_energyBinLowerEdges));

        std::sort(m_energyBinLowerEdges.begin(), m_energyBinLowerEdges.end());
        FloatVector::const_iterator binEdgesIter = std::unique(m_energyBinLowerEdges.begin(), m_energyBinLowerEdges.end());

        if ((m_energyBinLowerEdges.size() != m_nEnergyBins) || (m_energyBinLowerEdges.end() != binEdgesIter))
        {
            std::cout << "PhotonReconstructionAlgorithm::ReadSettings - PDF energy binning issue." << std::endl;
            return STATUS_CODE_INVALID_PARAMETER;
        }

        m_pSigPeakRms = new Histogram*[m_nEnergyBins];
        m_pBkgPeakRms = new Histogram*[m_nEnergyBins];
        m_pSigLongProfileStart = new Histogram*[m_nEnergyBins];
        m_pBkgLongProfileStart = new Histogram*[m_nEnergyBins];
        m_pSigLongProfileDiscrepancy = new Histogram*[m_nEnergyBins];
        m_pBkgLongProfileDiscrepancy = new Histogram*[m_nEnergyBins];
        m_pSigPeakEnergyFraction = new Histogram*[m_nEnergyBins];
        m_pBkgPeakEnergyFraction = new Histogram*[m_nEnergyBins];
        m_pSigMinDistanceToTrack = new Histogram*[m_nEnergyBins];
        m_pBkgMinDistanceToTrack = new Histogram*[m_nEnergyBins];

        for (unsigned int energyBin = 0; energyBin < m_nEnergyBins; ++energyBin)
        {
            m_pSigPeakRms[energyBin] = new Histogram(peakRmsNBins, peakRmsLowValue, peakRmsHighValue);
            m_pBkgPeakRms[energyBin] = new Histogram(peakRmsNBins, peakRmsLowValue, peakRmsHighValue);
            m_pSigLongProfileStart[energyBin] = new Histogram(longProfileStartNBins, longProfileStartLowValue, longProfileStartHighValue);
            m_pBkgLongProfileStart[energyBin] = new Histogram(longProfileStartNBins, longProfileStartLowValue, longProfileStartHighValue);
            m_pSigLongProfileDiscrepancy[energyBin] = new Histogram(longProfileDiscrepancyNBins, longProfileDiscrepancyLowValue, longProfileDiscrepancyHighValue);
            m_pBkgLongProfileDiscrepancy[energyBin] = new Histogram(longProfileDiscrepancyNBins, longProfileDiscrepancyLowValue, longProfileDiscrepancyHighValue);
            m_pSigPeakEnergyFraction[energyBin] = new Histogram(peakEnergyFractionNBins, peakEnergyFractionLowValue, peakEnergyFractionHighValue);
            m_pBkgPeakEnergyFraction[energyBin] = new Histogram(peakEnergyFractionNBins, peakEnergyFractionLowValue, peakEnergyFractionHighValue);
            m_pSigMinDistanceToTrack[energyBin] = new Histogram(minDistanceToTrackNBins, minDistanceToTrackLowValue, minDistanceToTrackHighValue);
            m_pBkgMinDistanceToTrack[energyBin] = new Histogram(minDistanceToTrackNBins, minDistanceToTrackLowValue, minDistanceToTrackHighValue);
        }
    }
    else
    {
        TiXmlDocument pdfXmlDocument(m_histogramFile);

        if (!pdfXmlDocument.LoadFile())
        {
            std::cout << "PhotonReconstructionAlgorithm::ReadSettings - Invalid xml file specified for pdf histograms." << std::endl;
            return STATUS_CODE_INVALID_PARAMETER;
        }

        const TiXmlHandle pdfXmlHandle(&pdfXmlDocument);

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadVectorOfValues(pdfXmlHandle,
            "EnergyBinLowerEdges", m_energyBinLowerEdges));

        std::sort(m_energyBinLowerEdges.begin(), m_energyBinLowerEdges.end());
        FloatVector::const_iterator binEdgesIter = std::unique(m_energyBinLowerEdges.begin(), m_energyBinLowerEdges.end());

        if ((m_energyBinLowerEdges.size() != m_nEnergyBins) || (m_energyBinLowerEdges.end() != binEdgesIter))
        {
            std::cout << "PhotonReconstructionAlgorithm::ReadSettings - PDF energy binning issue." << std::endl;
            return STATUS_CODE_INVALID_PARAMETER;
        }

        m_pSigPeakRms = new Histogram*[m_nEnergyBins];
        m_pBkgPeakRms = new Histogram*[m_nEnergyBins];
        m_pSigLongProfileStart = new Histogram*[m_nEnergyBins];
        m_pBkgLongProfileStart = new Histogram*[m_nEnergyBins];
        m_pSigLongProfileDiscrepancy = new Histogram*[m_nEnergyBins];
        m_pBkgLongProfileDiscrepancy = new Histogram*[m_nEnergyBins];
        m_pSigPeakEnergyFraction = new Histogram*[m_nEnergyBins];
        m_pBkgPeakEnergyFraction = new Histogram*[m_nEnergyBins];
        m_pSigMinDistanceToTrack = new Histogram*[m_nEnergyBins];
        m_pBkgMinDistanceToTrack = new Histogram*[m_nEnergyBins];

        for (unsigned int energyBin = 0; energyBin < m_nEnergyBins; ++energyBin)
        {
            m_pSigPeakRms[energyBin] = new Histogram(&pdfXmlHandle, "PhotonSigPeakRms_" + TypeToString(energyBin));
            m_pBkgPeakRms[energyBin] = new Histogram(&pdfXmlHandle, "PhotonBkgPeakRms_" + TypeToString(energyBin));
            m_pSigLongProfileStart[energyBin] = new Histogram(&pdfXmlHandle, "PhotonSigLongProfileStart_" + TypeToString(energyBin));
            m_pBkgLongProfileStart[energyBin] = new Histogram(&pdfXmlHandle, "PhotonBkgLongProfileStart_" + TypeToString(energyBin));
            m_pSigLongProfileDiscrepancy[energyBin] = new Histogram(&pdfXmlHandle, "PhotonSigLongProfileDiscrepancy_" + TypeToString(energyBin));
            m_pBkgLongProfileDiscrepancy[energyBin] = new Histogram(&pdfXmlHandle, "PhotonBkgLongProfileDiscrepancy_" + TypeToString(energyBin));
            m_pSigPeakEnergyFraction[energyBin] = new Histogram(&pdfXmlHandle, "PhotonSigPeakEnergyFraction_" + TypeToString(energyBin));
            m_pBkgPeakEnergyFraction[energyBin] = new Histogram(&pdfXmlHandle, "PhotonBkgPeakEnergyFraction_" + TypeToString(energyBin));
            m_pSigMinDistanceToTrack[energyBin] = new Histogram(&pdfXmlHandle, "PhotonSigMinDistanceToTrack_" + TypeToString(energyBin));
            m_pBkgMinDistanceToTrack[energyBin] = new Histogram(&pdfXmlHandle, "PhotonBkgMinDistanceToTrack_" + TypeToString(energyBin));
        }
    }

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PidCut", m_pidCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinClusterEnergy", m_minClusterEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TransProfileMaxLayer", m_transProfileMaxLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinPeakEnergy", m_minPeakEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxPeakRms", m_maxPeakRms));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinPeakCaloHits", m_minPeakCaloHits));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxLongProfileStart", m_maxLongProfileStart));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxLongProfileDiscrepancy", m_maxLongProfileDiscrepancy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxSearchLayer", m_maxSearchLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ParallelDistanceCut", m_parallelDistanceCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinTrackClusterCosAngle", m_minTrackClusterCosAngle));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinDistanceToTrackCut", m_minDistanceToTrackCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OldClusterEnergyFraction0", m_oldClusterEnergyFraction0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OldClusterEnergyFraction1", m_oldClusterEnergyFraction1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OldClusterEnergyDifference1", m_oldClusterEnergyDifference1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OldClusterEnergyFraction2", m_oldClusterEnergyFraction2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OldClusterEnergyDifference2", m_oldClusterEnergyDifference2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OldClusterEnergyFraction3", m_oldClusterEnergyFraction3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OldClusterEnergyDifference3", m_oldClusterEnergyDifference3));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
