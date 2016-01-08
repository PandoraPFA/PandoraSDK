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
    m_shouldDeleteNonPhotonClusters(true),
    m_minClusterEnergy(0.f),
    m_minPeakEnergy(0.f),
    m_maxPeakRms(5.f),
    m_maxRmsRatio(3.f),
    m_maxLongProfileStart(10.f),
    m_maxLongProfileDiscrepancy(0.8f),
    m_maxSearchLayer(9),
    m_parallelDistanceCut(100.f),
    m_minTrackClusterCosAngle(0.f),
    m_minDistanceToTrackDivisionCut(3.f),
    m_transProfileEcalOnly(true),
    m_transProfileMaxLayer(0),
    m_minDistanceToTrackCutLow(2.f),
    m_minDistanceToTrackCutHigh(200.f),
    m_energyCutForPid1(0.2f),
    m_pidCut1(1.f),
    m_energyCutForPid2(0.5f),
    m_pidCut2(0.6f),
    m_pidCut3(0.4f),
    // histogram related initialisation
    m_shouldMakePdfHistograms(false),
    m_shouldDrawPdfHistograms(false),
    m_nEnergyBins(0)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

PhotonReconstructionAlgorithm::~PhotonReconstructionAlgorithm()
{
    if (m_shouldMakePdfHistograms)
        this->NormalizeAndWriteHistograms();

    if (m_shouldDrawPdfHistograms)
        this->DrawHistograms();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::Run()
{
    // ATTN Implicit assumption that individual physical layers in the ECAL will always correspond to individual pseudo layers
    // Also ECAL BARRAL has same layer as ECAL ENDCAP and ecal is the first inner detector
    if (m_transProfileEcalOnly && m_transProfileMaxLayer <= 0 )
        m_transProfileMaxLayer = PandoraContentApi::GetPlugins(*this)->GetPseudoLayerPlugin()->GetPseudoLayerAtIp() + 
            PandoraContentApi::GetGeometry(*this)->GetSubDetector(ECAL_BARREL).GetNLayers();
    
    std::string inputClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->InitialiseInputClusterListName(inputClusterListName));
    
    ClusterVector clusterVector;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateClustersOfInterest(clusterVector));
    TrackVector trackVector;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetTrackVectors(trackVector));
    for (ClusterVector::const_iterator iter = clusterVector.begin(), iterEnd = clusterVector.end(); iter != iterEnd; ++iter)
    {
        const Cluster *const pCluster = *iter;
        if (!this->PassClusterQualityPreCut(pCluster))
            continue;
            
        const Track *pMinTrack = NULL;
        float minDistance(std::numeric_limits<float>::max());
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetMinDistanceToTrack(pCluster, trackVector, minDistance, pMinTrack));

        ShowerProfilePlugin::ShowerPeakList showersPhoton, showersCharged;
        bool fromTrack(false);
        if (pMinTrack && minDistance < m_minDistanceToTrackDivisionCut)
        {
            // cluster close to track            
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetTrackClusterShowerList(pCluster, pMinTrack, trackVector, showersPhoton, showersCharged));
            fromTrack = true;
        }
        else
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetTracklessClusterShowerList(pCluster, showersPhoton));
        }

        if (m_shouldMakePdfHistograms)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreatePhotonsForTraining(pCluster, showersPhoton));
        }            
        else
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreatePhotons(pCluster, showersPhoton, fromTrack));
        }
    }
    
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RunNestedFragmentRemovalAlg());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReplaceInputClusterList(inputClusterListName));
    
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::InitialiseInputClusterListName(std::string &inputClusterListName) const
{
    const ClusterList *pInputClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pInputClusterList, inputClusterListName));
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::CreateClustersOfInterest(ClusterVector &clusterVector) const
{
    // Create list of clusters as region of interests, potential photon candidates
    const ClusterList *pPhotonClusterList = NULL;
    std::string photonClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_photonClusteringAlgName,
        pPhotonClusterList, photonClusterListName));
        
    if (pPhotonClusterList->empty())
    {
        std::cout << "PhotonReconstructionAlgorithm::CreateRegionsOfInterests no photon candidates avaiable, no regions of interests are created" << std::endl;
        return STATUS_CODE_INVALID_PARAMETER;
    }
    
    // Fragmentation can only proceed with reference to a saved cluster list, so save these temporary clusters
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList<Cluster>(*this, m_clusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<Cluster>(*this, m_clusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pPhotonClusterList));
    
    // Sort the clusters.
    clusterVector.assign(pPhotonClusterList->begin(), pPhotonClusterList->end());
    std::sort(clusterVector.begin(), clusterVector.end(), lc_content::SortingHelper::SortClustersByInnerLayer);
    
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::GetTrackVectors(TrackVector &trackVector) const
{
    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pTrackList));

    trackVector.assign(pTrackList->begin(), pTrackList->end());
    std::sort(trackVector.begin(), trackVector.end(), lc_content::SortingHelper::SortTracksByEnergy);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonReconstructionAlgorithm::PassClusterQualityPreCut(const Cluster *const pCluster) const
{
    return ((pCluster->GetElectromagneticEnergy() > m_minClusterEnergy) &&
            (PandoraContentApi::GetGeometry(*this)->GetHitTypeGranularity(pCluster->GetInnerLayerHitType()) <= FINE));
}


//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::GetTracklessClusterShowerList(const Cluster *const pCluster, ShowerProfilePlugin::ShowerPeakList &showersPhoton) const
{
    const ShowerProfilePlugin *const pShowerProfilePlugin(PandoraContentApi::GetPlugins(*this)->GetShowerProfilePlugin());
    pShowerProfilePlugin->CalculateTransverseProfile(pCluster, m_transProfileMaxLayer, showersPhoton, false);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::GetTrackClusterShowerList(const Cluster *const pCluster, const Track *const pMinTrack, 
    const TrackVector trackVector, ShowerProfilePlugin::ShowerPeakList &showersPhoton, ShowerProfilePlugin::ShowerPeakList &showersCharged) const
{
    const ShowerProfilePlugin *const pShowerProfilePlugin(PandoraContentApi::GetPlugins(*this)->GetShowerProfilePlugin());
    pShowerProfilePlugin->CalculateTrackBasedTransverseProfile(pCluster, m_transProfileMaxLayer, pMinTrack, trackVector, showersPhoton, showersCharged);
    return STATUS_CODE_SUCCESS;
}


//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::CreatePhotons(const Cluster *const pCluster, const ShowerProfilePlugin::ShowerPeakList &showersPhoton, const bool isFromTrack) const
{
    // ATTN showersCharged or showersPhoton could empty
    if (showersPhoton.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->DeleteCluster(pCluster));
        return STATUS_CODE_SUCCESS;
    }

    std::string originalClusterListName, peakClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->InitialiseFragmentation(pCluster, originalClusterListName, peakClusterListName));    
    bool usedCluster(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateClustersAndSetPhotonID(showersPhoton, pCluster->GetElectromagneticEnergy(), usedCluster, isFromTrack));    
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->EndFragmentation(usedCluster, originalClusterListName, peakClusterListName));
    if (!usedCluster)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->DeleteCluster(pCluster));
    }
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::InitialiseFragmentation(const Cluster *const pCluster, std::string &originalClusterListName, std::string &peakClusterListName) const
{
    ClusterList clusterList;
    clusterList.insert(pCluster);    
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::InitializeFragmentation(*this, clusterList,
        originalClusterListName, peakClusterListName));
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::EndFragmentation(const bool usedCluster, const std::string &originalClusterListName, const std::string &peakClusterListName) const
{
    const std::string &clusterListToSaveName((usedCluster) ? peakClusterListName : originalClusterListName);
    const std::string &clusterListToDeleteName((usedCluster) ? originalClusterListName : peakClusterListName);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::EndFragmentation(*this, clusterListToSaveName,clusterListToDeleteName));
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::CreateClustersAndSetPhotonID(const ShowerProfilePlugin::ShowerPeakList &showersPhoton, const float wholeClusuterEnergy, 
    bool &usedCluster, const bool isFromTrack) const
{
    for (unsigned int iPeak = 0, iPeakEnd = showersPhoton.size(); iPeak < iPeakEnd; ++iPeak)
    {
        const ShowerProfilePlugin::ShowerPeak &showerPeak(showersPhoton[iPeak]);
        const Cluster *pPeakCluster = NULL;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateCluster(showerPeak, pPeakCluster));
        bool isPhoton(false);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CheckAndSetPhotonID(showerPeak, pPeakCluster, wholeClusuterEnergy, isPhoton, isFromTrack));
        if (isPhoton)
        {
            usedCluster = true;
        }
        else
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->DeleteCluster(pPeakCluster));
        }
    }
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::CreateCluster(const ShowerProfilePlugin::ShowerPeak &showerPeak, const Cluster *&pPeakCluster) const
{
    PandoraContentApi::Cluster::Parameters parameters;
    parameters.m_caloHitList = showerPeak.GetPeakCaloHitList();
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, parameters, pPeakCluster));
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::CheckAndSetPhotonID(const ShowerProfilePlugin::ShowerPeak &showerPeak, const Cluster *const pPeakCluster
    , const float wholeClusuterEnergy, bool &isPhoton, const bool isFromTrack) const
{
    PDFVarFloatMap pdfVarFloatMap;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CalculateForPhotonID(showerPeak, pPeakCluster, wholeClusuterEnergy, pdfVarFloatMap));
    if (this->IsPhoton(pPeakCluster, pdfVarFloatMap, isFromTrack))
    {
        isPhoton = true;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SetPhotonID(pPeakCluster));
    }
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::CalculateForPhotonID(const ShowerProfilePlugin::ShowerPeak &showerPeak, const Cluster *const pPeakCluster, 
    const float wholeClusuterEnergy, PDFVarFloatMap &pdfVarFloatMap) const
{
    const float peakRMS(showerPeak.GetPeakRms());
    const float rmsRatio(showerPeak.GetRmsXYRatio());

    const ShowerProfilePlugin *const pShowerProfilePlugin(PandoraContentApi::GetPlugins(*this)->GetShowerProfilePlugin());
    float profileStart(0.f), profileDiscrepancy(0.f);
    pShowerProfilePlugin->CalculateLongitudinalProfile(pPeakCluster, profileStart, profileDiscrepancy);
    const float longProfileStart(pPeakCluster->GetShowerProfileStart(this->GetPandora()));
    const float longProfileDiscrepancy(pPeakCluster->GetShowerProfileDiscrepancy(this->GetPandora()));
    const float energyFraction(pPeakCluster->GetElectromagneticEnergy() / wholeClusuterEnergy);
    TrackVector trackVector;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetTrackVectors(trackVector));
    const Track *pMinTrack = NULL;
    float minDistance(std::numeric_limits<float>::max());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetMinDistanceToTrack(pPeakCluster, trackVector, minDistance, pMinTrack));

    if ((!pdfVarFloatMap.insert(PDFVarFloatMap::value_type(PEAKRMS, peakRMS)).second) ||
        (!pdfVarFloatMap.insert(PDFVarFloatMap::value_type(RMSRATIO, rmsRatio)).second) ||
        (!pdfVarFloatMap.insert(PDFVarFloatMap::value_type(LONGPROFILESTART, longProfileStart)).second) ||
        (!pdfVarFloatMap.insert(PDFVarFloatMap::value_type(LONGPROFILEDISCREPANCY, longProfileDiscrepancy)).second) ||
        (!pdfVarFloatMap.insert(PDFVarFloatMap::value_type(PEAKENERGYFRACTION, energyFraction)).second) ||
        (!pdfVarFloatMap.insert(PDFVarFloatMap::value_type(MINDISTANCETOTRACK, minDistance)).second))
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonReconstructionAlgorithm::IsPhoton(const pandora::Cluster *const pPeakCluster, const PDFVarFloatMap &pdfVarFloatMap, const bool isFromTrack) const
{
    const float clusterEnergy(pPeakCluster->GetElectromagneticEnergy());
    return (this->PassPhotonQualityCut(clusterEnergy, pdfVarFloatMap) &&
        this->PassPhotonPIDCut(this->GetPIDForPhotonID(clusterEnergy, pdfVarFloatMap), clusterEnergy, isFromTrack));
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::SetPhotonID(const Cluster *const pPeakCluster) const
{
    PandoraContentApi::Cluster::Metadata metadata;
    metadata.m_particleId = PHOTON;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AlterMetadata(*this, pPeakCluster, metadata));
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonReconstructionAlgorithm::PassPhotonQualityCut(const float clusterEnergy, const PDFVarFloatMap &pdfVarFloatMap) const
{
    return ( clusterEnergy > m_minPeakEnergy &&
        pdfVarFloatMap.find(PEAKRMS)->second < m_maxPeakRms && 
        pdfVarFloatMap.find(RMSRATIO)->second < m_maxRmsRatio && 
        pdfVarFloatMap.find(LONGPROFILESTART)->second < m_maxLongProfileStart && 
        pdfVarFloatMap.find(LONGPROFILEDISCREPANCY)->second < m_maxLongProfileDiscrepancy && 
        pdfVarFloatMap.find(MINDISTANCETOTRACK)->second > m_minDistanceToTrackCutLow &&
        pdfVarFloatMap.find(MINDISTANCETOTRACK)->second < m_minDistanceToTrackCutHigh);
}

//------------------------------------------------------------------------------------------------------------------------------------------

float PhotonReconstructionAlgorithm::GetPIDForPhotonID(const float clusterEnergy, const PDFVarFloatMap &pdfVarFloatMap) const
{
    double yes(1.f), no(1.f);
    const unsigned int energyBin(this->GetEnergyBin(clusterEnergy));
    
    for (PDFVarLikelihoodPDFMap::const_iterator iter = m_pdfVarLikelihoodPDFMap.begin(), iterEnd = m_pdfVarLikelihoodPDFMap.end(); iter != iterEnd; ++iter)
    {
        const LikelihoodPDFObject &likelihoodPDFObject((*iter).second);
        const PDFVar &pdfVar((*iter).first);
        
        yes *= this->GetHistogramContent(likelihoodPDFObject.m_pSignalPDF[energyBin], pdfVarFloatMap.find(pdfVar)->second);
        no  *= this->GetHistogramContent(likelihoodPDFObject.m_pBackgroundPDF[energyBin], pdfVarFloatMap.find(pdfVar)->second);
    }
    const double yesWeighted(yes * m_nSignalEvents[energyBin]), noWeighted(no * m_nBackgroundEvents[energyBin]);
    return ((yesWeighted + noWeighted) > std::numeric_limits<double>::epsilon() ? static_cast<float>(yesWeighted / (yesWeighted + noWeighted)) : 0.f);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonReconstructionAlgorithm::PassPhotonPIDCut(const float pid, const float clusterEnergy, const bool /*isFromTrack*/) const
{
    // ATTN isFromTrack is left for future if someone wants to have different cut for cluster close to track or not
    if (clusterEnergy < m_energyCutForPid1)
    {
        if (pid > m_pidCut1)
            return true;
    }else if (clusterEnergy < m_energyCutForPid2)
    {
        if (pid > m_pidCut2 )
            return true;
    }else
    {
        if (pid > m_pidCut3 )
            return true;
    }
    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::DeleteCluster(const Cluster *const pCluster) const
{
    if (m_shouldDeleteNonPhotonClusters)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Delete(*this, pCluster));
    }
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::RunNestedFragmentRemovalAlg() const
{
    // Run the daughter fragment merging algorithm, if provided
    if (!m_fragmentMergingAlgName.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_fragmentMergingAlgName));
    }
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::ReplaceInputClusterList(const std::string  &inputClusterListName) const
{
    const std::string replacementListName(m_replaceCurrentClusterList ? m_clusterListName : inputClusterListName);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<Cluster>(*this, replacementListName));
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::GetMinDistanceToTrack(const pandora::Cluster *const pCluster, const pandora::TrackVector &trackVector,
    float &minDistance, const pandora::Track *&pMinTrack) const
{
    minDistance = std::numeric_limits<float>::max();
    float minEnergyDifference(std::numeric_limits<float>::max());
    
    for (TrackVector::const_iterator trackIter = trackVector.begin(), trackIterEnd = trackVector.end(); trackIter != trackIterEnd; ++trackIter)
    {
        const Track *const pTrack = *trackIter;
        float trackClusterDistance(std::numeric_limits<float>::max());

        if (STATUS_CODE_SUCCESS == lc_content::ClusterHelper::GetTrackClusterDistance(pTrack, pCluster, m_maxSearchLayer, m_parallelDistanceCut,
            m_minTrackClusterCosAngle, trackClusterDistance))
        {
            const float energyDifference(std::fabs(pCluster->GetHadronicEnergy() - pTrack->GetEnergyAtDca()));

            if ((trackClusterDistance < minDistance) || ((trackClusterDistance == minDistance) && (energyDifference < minEnergyDifference)))
            {
                minDistance = trackClusterDistance;
                minEnergyDifference = energyDifference;
                pMinTrack=*trackIter;
            }
        }
    }
    
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle,
        "PhotonClusterFormation", m_photonClusteringAlgName));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle,
        "PhotonFragmentMerging", m_fragmentMergingAlgName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "ClusterListName", m_clusterListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "ReplaceCurrentClusterList", m_replaceCurrentClusterList));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldDeleteNonPhotonClusters", m_shouldDeleteNonPhotonClusters));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinClusterEnergy", m_minClusterEnergy));
        
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinPeakEnergy", m_minPeakEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxPeakRms", m_maxPeakRms));
        
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
        "MinDistanceToTrackDivisionCut", m_minDistanceToTrackDivisionCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TransProfileEcalOnly", m_transProfileEcalOnly));
        
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TransProfileMaxLayer", m_transProfileMaxLayer));
        
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinDistanceToTrackCutLow", m_minDistanceToTrackCutLow));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinDistanceToTrackCutHigh", m_minDistanceToTrackCutHigh));
        
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyCutForPid1", m_energyCutForPid1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PidCut1", m_pidCut1));
        
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyCutForPid2", m_energyCutForPid2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PidCut2", m_pidCut2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PidCut3", m_pidCut3));
        
    return this->ReadHistogramSettings(xmlHandle);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::ReadHistogramSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "HistogramFile", m_histogramFile));
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "ShouldMakePdfHistograms", m_shouldMakePdfHistograms));
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "ShouldDrawPdfHistograms", m_shouldDrawPdfHistograms));        
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "NEnergyBins", m_nEnergyBins));

    if (0 == m_nEnergyBins)
    {
        std::cout << "PhotonReconstructionAlgorithm::ReadHistogramSettings - Invalid number PDF energy bins specified." << std::endl;
        return STATUS_CODE_INVALID_PARAMETER;
    }
    
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->InitialisePDFVarLikelihoodPDFObjectMap());
    
    if (m_shouldMakePdfHistograms)
    {
        return this->InitialiseHistogramWriting(xmlHandle);
    }
    else
    {
        return this->InitialiseHistogramReading();
    }
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::InitialiseHistogramWriting(const pandora::TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetEnergyBinLowerEdges(xmlHandle, "EnergyBinLowerEdges"));
    
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FillPDFVarLikelihoodPDFMapParameters(xmlHandle, PEAKRMS, "PeakRmsNBins", 50, 
        "PeakRmsLowValue", 0.f, "PeakRmsHighValue", 5.f));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FillPDFVarLikelihoodPDFMapParameters(xmlHandle, RMSRATIO, "RmsRatioNBins", 30, 
        "RmsRatioLowValue", 1.f, "RmsRatioHighValue", 3.f));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FillPDFVarLikelihoodPDFMapParameters(xmlHandle, LONGPROFILESTART, "LongProfileStartNBins", 11, 
        "LongProfileStartLowValue", -0.5f, "LongProfileStartHighValue", 10.5f));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FillPDFVarLikelihoodPDFMapParameters(xmlHandle, LONGPROFILEDISCREPANCY, "LongProfileDiscrepancyNBins", 42, 
        "LongProfileDiscrepancyLowValue", -0.02f, "LongProfileDiscrepancyHighValue", 0.82f));//52 -0.02 1.02
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FillPDFVarLikelihoodPDFMapParameters(xmlHandle, PEAKENERGYFRACTION, "PeakEnergyFractionNBins", 52, 
        "PeakEnergyFractionLowValue", -0.02f, "PeakEnergyFractionHighValue", 1.02f));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FillPDFVarLikelihoodPDFMapParameters(xmlHandle, MINDISTANCETOTRACK, "MinDistanceToTrackNBins", 40, 
        "MinDistanceToTrackLowValue", 0.f, "MinDistanceToTrackHighValue", 20.f));

    for (PDFVarLikelihoodPDFMap::iterator iter = m_pdfVarLikelihoodPDFMap.begin(), iterEnd = m_pdfVarLikelihoodPDFMap.end(); iter != iterEnd; ++iter)
    {
        LikelihoodPDFObject &likelihoodPDFObject((*iter).second);
        
        likelihoodPDFObject.m_pSignalPDF = new Histogram*[m_nEnergyBins];
        likelihoodPDFObject.m_pBackgroundPDF = new Histogram*[m_nEnergyBins];
        
        for (unsigned int energyBin = 0; energyBin < m_nEnergyBins; ++energyBin)
        {
            likelihoodPDFObject.m_pSignalPDF[energyBin] = new Histogram(likelihoodPDFObject.m_nBins, likelihoodPDFObject.m_lowValue, likelihoodPDFObject.m_highValue);
            likelihoodPDFObject.m_pBackgroundPDF[energyBin] = new Histogram(likelihoodPDFObject.m_nBins, likelihoodPDFObject.m_lowValue, likelihoodPDFObject.m_highValue);
        }
    }
    
    for (unsigned int energyBin = 0; energyBin < m_nEnergyBins; ++energyBin)
    {
        m_nSignalEvents.push_back(0);
        m_nBackgroundEvents.push_back(0);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::InitialiseHistogramReading()
{
    TiXmlDocument pdfXmlDocument(m_histogramFile);
    if (!pdfXmlDocument.LoadFile())
    {
        std::cout << "PhotonReconstructionAlgorithm::InitialiseHistogramReading - Invalid xml file specified for pdf histograms." << std::endl;
        return STATUS_CODE_INVALID_PARAMETER;
    }
    const TiXmlHandle pdfXmlHandle(&pdfXmlDocument);
    
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetEnergyBinLowerEdges(pdfXmlHandle, "EnergyBinLowerEdges"));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetNSglBkgEvts(pdfXmlHandle, "NSignalEvents", "NBackgroundEvents"));
    
    for (PDFVarLikelihoodPDFMap::iterator iter = m_pdfVarLikelihoodPDFMap.begin(), iterEnd = m_pdfVarLikelihoodPDFMap.end(); iter != iterEnd; ++iter)
    {
        LikelihoodPDFObject &likelihoodPDFObject((*iter).second);
        
        likelihoodPDFObject.m_pSignalPDF = new Histogram*[m_nEnergyBins];
        likelihoodPDFObject.m_pBackgroundPDF = new Histogram*[m_nEnergyBins];
        
        for (unsigned int energyBin = 0; energyBin < m_nEnergyBins; ++energyBin)
        {
            likelihoodPDFObject.m_pSignalPDF[energyBin] = new Histogram(&pdfXmlHandle, "PhotonSig" + likelihoodPDFObject.m_pdfVarName + "_" + TypeToString(energyBin));
            likelihoodPDFObject.m_pBackgroundPDF[energyBin] = new Histogram(&pdfXmlHandle, "PhotonBkg" + likelihoodPDFObject.m_pdfVarName + "_" + TypeToString(energyBin));
        }
    }
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::InitialisePDFVarLikelihoodPDFObjectMap()
{
    if (false == m_pdfVarLikelihoodPDFMap.insert(PDFVarLikelihoodPDFMap::value_type(PEAKRMS, LikelihoodPDFObject("PeakRms"))).second)
        return STATUS_CODE_FAILURE;
    if (false == m_pdfVarLikelihoodPDFMap.insert(PDFVarLikelihoodPDFMap::value_type(RMSRATIO, LikelihoodPDFObject("RmsRatio"))).second)
        return STATUS_CODE_FAILURE;
    if (false == m_pdfVarLikelihoodPDFMap.insert(PDFVarLikelihoodPDFMap::value_type(LONGPROFILESTART, LikelihoodPDFObject("LongProfileStart"))).second)
        return STATUS_CODE_FAILURE;
    if (false == m_pdfVarLikelihoodPDFMap.insert(PDFVarLikelihoodPDFMap::value_type(LONGPROFILEDISCREPANCY, LikelihoodPDFObject("LongProfileDiscrepancy"))).second)
        return STATUS_CODE_FAILURE;
    if (false == m_pdfVarLikelihoodPDFMap.insert(PDFVarLikelihoodPDFMap::value_type(PEAKENERGYFRACTION, LikelihoodPDFObject("PeakEnergyFraction"))).second)
        return STATUS_CODE_FAILURE;
    if (false == m_pdfVarLikelihoodPDFMap.insert(PDFVarLikelihoodPDFMap::value_type(MINDISTANCETOTRACK, LikelihoodPDFObject("MinDistanceToTrack"))).second)
        return STATUS_CODE_FAILURE;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::GetEnergyBinLowerEdges(const TiXmlHandle xmlHandle, const std::string &energyBinLowerEdgesStr)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadVectorOfValues(xmlHandle, energyBinLowerEdgesStr, m_energyBinLowerEdges));
    std::sort(m_energyBinLowerEdges.begin(), m_energyBinLowerEdges.end());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ParameterElementNumberCheck(m_energyBinLowerEdges));
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::ParameterElementNumberCheck(FloatVector &parameter) const
{
    FloatVector::const_iterator binEdgesIter = std::unique(parameter.begin(), parameter.end());
    if ((parameter.size() != m_nEnergyBins) || (parameter.end() != binEdgesIter))
    {
        std::cout << "PhotonReconstructionAlgorithm::ParameterElementNumberCheck - PDF energy binning issue." << std::endl;
        return STATUS_CODE_INVALID_PARAMETER;
    }
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::GetNSglBkgEvts(const TiXmlHandle xmlHandle, const std::string &nSignalEventsStr, const std::string &nBackgroundEventsStr)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadVectorOfValues(xmlHandle, nSignalEventsStr, m_nSignalEvents));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadVectorOfValues(xmlHandle, nBackgroundEventsStr, m_nBackgroundEvents));
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::FillPDFVarLikelihoodPDFMapParameters(const TiXmlHandle xmlHandle, const PDFVar pdfVar, const std::string &nBinStr, 
    const int nBinDefault, const std::string &lowValueStr, const float lowValueDefault, const std::string &highValueStr, const float highValueDefault)
{
    LikelihoodPDFObject &likelihoodPDFObject(m_pdfVarLikelihoodPDFMap.find(pdfVar)->second);
    
    likelihoodPDFObject.m_nBins = nBinDefault;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        nBinStr, likelihoodPDFObject.m_nBins));
    
    likelihoodPDFObject.m_lowValue = lowValueDefault;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        lowValueStr, likelihoodPDFObject.m_lowValue));

    likelihoodPDFObject.m_highValue = highValueDefault;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        highValueStr, likelihoodPDFObject.m_highValue));
        
    return STATUS_CODE_SUCCESS;
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
    return pHistogram->GetBinContent(pHistogram->GetBinNumber(value));
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PhotonReconstructionAlgorithm::CreatePhotonsForTraining(const Cluster *const pCluster, const ShowerProfilePlugin::ShowerPeakList &showersPhoton)
{
    // ATTN showersCharged or showersPhoton could empty
    if (showersPhoton.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->DeleteCluster(pCluster));
        return STATUS_CODE_SUCCESS;
    }
    std::string originalClusterListName, peakClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->InitialiseFragmentation(pCluster, originalClusterListName, peakClusterListName));    
    bool usedCluster(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateClustersAndTrainPhotonID(showersPhoton, pCluster->GetElectromagneticEnergy()));  
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->EndFragmentation(usedCluster, originalClusterListName, peakClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->DeleteCluster(pCluster));
    
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::CreateClustersAndTrainPhotonID(const ShowerProfilePlugin::ShowerPeakList &showersPhoton, const float wholeClusuterEnergy)
{
for (unsigned int iPeak = 0, iPeakEnd = showersPhoton.size(); iPeak < iPeakEnd; ++iPeak)
    {
        const ShowerProfilePlugin::ShowerPeak &showerPeak(showersPhoton[iPeak]);
        const Cluster *pPeakCluster = NULL;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateCluster(showerPeak, pPeakCluster));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->TrainPhotonID(showerPeak, pPeakCluster, wholeClusuterEnergy));
    }
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::TrainPhotonID(const ShowerProfilePlugin::ShowerPeak &showerPeak, const Cluster *const pPeakCluster, const float wholeClusuterEnergy)
{
    PDFVarFloatMap pdfVarFloatMap;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CalculateForPhotonID(showerPeak, pPeakCluster, wholeClusuterEnergy, pdfVarFloatMap));
    // ATTN when training the likelihood file, only write xml file. Do not read xml file
    if (this->PassPhotonQualityCut(pPeakCluster->GetElectromagneticEnergy(), pdfVarFloatMap))
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FillPdfHistograms(pPeakCluster, pdfVarFloatMap));
    }
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::FillPdfHistograms(const pandora::Cluster *const pCluster, const PDFVarFloatMap &pdfVarFloatMap)
{
    const MCParticle *pMCParticle(NULL);
    try
    {
        pMCParticle = MCParticleHelper::GetMainMCParticle(pCluster);
    }
    catch (StatusCodeException &)
    {
    }

    const unsigned int energyBin(this->GetEnergyBin(pCluster->GetElectromagneticEnergy()));

    if ((NULL != pMCParticle) && (PHOTON == pMCParticle->GetParticleId()))
    {
        for (PDFVarLikelihoodPDFMap::const_iterator iter = m_pdfVarLikelihoodPDFMap.begin(), iterEnd = m_pdfVarLikelihoodPDFMap.end(); iter != iterEnd; ++iter)
        {
            const LikelihoodPDFObject &likelihoodPDFObject((*iter).second);
            const PDFVar &pdfVar((*iter).first);
            
            likelihoodPDFObject.m_pSignalPDF[energyBin]->Fill(pdfVarFloatMap.find(pdfVar)->second);
        }
        m_nSignalEvents[energyBin]++;
    }
    else
    {
        for (PDFVarLikelihoodPDFMap::const_iterator iter = m_pdfVarLikelihoodPDFMap.begin(), iterEnd = m_pdfVarLikelihoodPDFMap.end(); iter != iterEnd; ++iter)
        {
            const LikelihoodPDFObject &likelihoodPDFObject((*iter).second);
            const PDFVar &pdfVar((*iter).first);
            
            likelihoodPDFObject.m_pBackgroundPDF[energyBin]->Fill(pdfVarFloatMap.find(pdfVar)->second);
        }
        m_nBackgroundEvents[energyBin]++;
    }
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PhotonReconstructionAlgorithm::NormalizeHistogram(Histogram *const pHistogram) const
{
    // ATTN normalise underflow and overflow bin
    const float cumulativeSum(pHistogram->GetCumulativeSum(pHistogram->GetUnderflowBinNumber(), pHistogram->GetOverflowBinNumber()));
    if (std::fabs(cumulativeSum) < std::numeric_limits<float>::epsilon())
    {
        std::cout << "PhotonReconstructionAlgorithm: Cannot normalize empty histogram " << std::endl;
        return;
    }
    pHistogram->Scale(1.f / cumulativeSum);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PhotonReconstructionAlgorithm::WriteString(TiXmlDocument &xmlDocument, const std::string nameStr, const std::string valueStr)
{
    TiXmlElement *const pStringElement = new TiXmlElement(nameStr);
    pStringElement->LinkEndChild(new TiXmlText(valueStr));
    xmlDocument.LinkEndChild(pStringElement);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PhotonReconstructionAlgorithm::NormalizeAndWriteHistograms()
{
    std::string energyBinLowerEdgesString, nSignalEventsString, nBackgroundEventsString;
    
    for (PDFVarLikelihoodPDFMap::iterator iter = m_pdfVarLikelihoodPDFMap.begin(), iterEnd = m_pdfVarLikelihoodPDFMap.end(); iter != iterEnd; ++iter)
    {
        LikelihoodPDFObject &likelihoodPDFObject((*iter).second);
        for (unsigned int energyBin = 0; energyBin < m_nEnergyBins; ++energyBin)
        {
            this->NormalizeHistogram(likelihoodPDFObject.m_pSignalPDF[energyBin]);
            this->NormalizeHistogram(likelihoodPDFObject.m_pBackgroundPDF[energyBin]);
        }
    }
    for (unsigned int energyBin = 0; energyBin < m_nEnergyBins; ++energyBin)
    {
        energyBinLowerEdgesString += TypeToString(m_energyBinLowerEdges[energyBin]) + " ";
        nSignalEventsString += TypeToString(m_nSignalEvents[energyBin]) + " ";
        nBackgroundEventsString += TypeToString(m_nBackgroundEvents[energyBin]) + " ";
    }
    
    TiXmlDocument xmlDocument;    
    this->WriteString(xmlDocument, "EnergyBinLowerEdges", energyBinLowerEdgesString);
    this->WriteString(xmlDocument, "NSignalEvents", nSignalEventsString);
    this->WriteString(xmlDocument, "NBackgroundEvents", nBackgroundEventsString);
    
    for (PDFVarLikelihoodPDFMap::iterator iter = m_pdfVarLikelihoodPDFMap.begin(), iterEnd = m_pdfVarLikelihoodPDFMap.end(); iter != iterEnd; ++iter)
    {
        LikelihoodPDFObject &likelihoodPDFObject((*iter).second);
        for (unsigned int energyBin = 0; energyBin < m_nEnergyBins; ++energyBin)
        {
            likelihoodPDFObject.m_pSignalPDF[energyBin]->WriteToXml(&xmlDocument, "PhotonSig" + likelihoodPDFObject.m_pdfVarName + "_" + TypeToString(energyBin));
            likelihoodPDFObject.m_pBackgroundPDF[energyBin]->WriteToXml(&xmlDocument, "PhotonBkg" + likelihoodPDFObject.m_pdfVarName + "_" + TypeToString(energyBin));
        }
    }
    xmlDocument.SaveFile(m_histogramFile);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PhotonReconstructionAlgorithm::DrawHistograms() const
{
    for (unsigned int energyBin = 0; energyBin < m_nEnergyBins; ++energyBin)
    {
        std::cout << "PDF EnergyBin " << energyBin << std::endl;
        for (PDFVarLikelihoodPDFMap::const_iterator iter = m_pdfVarLikelihoodPDFMap.begin(), iterEnd = m_pdfVarLikelihoodPDFMap.end(); iter != iterEnd; ++iter)
        {
            const LikelihoodPDFObject &likelihoodPDFObject((*iter).second);    
            std::cout << likelihoodPDFObject.m_pdfVarName << ", Signal, Background " << std::endl;
            PANDORA_MONITORING_API(DrawPandoraHistogram(this->GetPandora(), *likelihoodPDFObject.m_pSignalPDF[energyBin]));
            PANDORA_MONITORING_API(DrawPandoraHistogram(this->GetPandora(), *likelihoodPDFObject.m_pBackgroundPDF[energyBin]));
        }
    }
}

} // namespace lc_content
