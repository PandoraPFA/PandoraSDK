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
    // get initial lists
    // get regions of interests
    // get miminum distance to track
    // divide cluster into 2 categories
    // for clusters far from track, do peak findings
    // for cluster close to track, do peak findings, do other stuff
    // for peak finding results, do likelihood test
    // Obtain current lists for later reference
    
    // ATTN Implicit assumption that individual physical layers in the ECAL will always correspond to individual pseudo layers
    // Also ECAL BARRAL has same layer as ECAL ENDCAP and ecal is the first inner detector
    if (m_transProfileEcalOnly && m_transProfileMaxLayer <= 0 )
        m_transProfileMaxLayer = PandoraContentApi::GetPlugins(*this)->GetPseudoLayerPlugin()->GetPseudoLayerAtIp() + 
            PandoraContentApi::GetGeometry(*this)->GetSubDetector(ECAL_BARREL).GetNLayers();
            
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->InitialiseInputClusterListName());
    
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
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReplaceInputClusterList());
    
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::InitialiseInputClusterListName()
{
    const ClusterList *pInputClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pInputClusterList, m_inputClusterListName));
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
    pShowerProfilePlugin->CalculateTracklessTransverseProfile(pCluster, m_transProfileMaxLayer, showersPhoton, false);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::GetTrackClusterShowerList(const Cluster *const pCluster, const Track *const pMinTrack, 
    const TrackVector trackVector, ShowerProfilePlugin::ShowerPeakList &showersPhoton, ShowerProfilePlugin::ShowerPeakList &showersCharged) const
{
    const ShowerProfilePlugin *const pShowerProfilePlugin(PandoraContentApi::GetPlugins(*this)->GetShowerProfilePlugin());
    pShowerProfilePlugin->CalculateTrackNearbyTransverseProfile(pCluster, m_transProfileMaxLayer, pMinTrack, trackVector, showersPhoton, showersCharged);
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
    HistVarQuantityMap hisVarQuantityMap;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CalculateForPhotonID(showerPeak, pPeakCluster, wholeClusuterEnergy, hisVarQuantityMap));
    if (this->isPhotonFromQuantities(pPeakCluster, hisVarQuantityMap, isFromTrack))
    {
        isPhoton = true;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SetPhotonID(pPeakCluster));
    }
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::CalculateForPhotonID(const ShowerProfilePlugin::ShowerPeak &showerPeak, const Cluster *const pPeakCluster, 
    const float wholeClusuterEnergy, HistVarQuantityMap &hisVarQuantityMap) const
{
    const float peakRMS(showerPeak.GetPeakRms());
    const float rmsRatio(showerPeak.GetRmsRatio());

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

    if ((!hisVarQuantityMap.insert(HistVarQuantityMap::value_type(PEAKRMS, peakRMS)).second) ||
        (!hisVarQuantityMap.insert(HistVarQuantityMap::value_type(RMSRATIO, rmsRatio)).second) ||
        (!hisVarQuantityMap.insert(HistVarQuantityMap::value_type(LONGPROFILESTART, longProfileStart)).second) ||
        (!hisVarQuantityMap.insert(HistVarQuantityMap::value_type(LONGPROFILEDISCREPANCY, longProfileDiscrepancy)).second) ||
        (!hisVarQuantityMap.insert(HistVarQuantityMap::value_type(PEAKENERGYFRACTION, energyFraction)).second) ||
        (!hisVarQuantityMap.insert(HistVarQuantityMap::value_type(MINDISTANCETOTRACK, minDistance)).second))
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonReconstructionAlgorithm::isPhotonFromQuantities(const pandora::Cluster *const pPeakCluster, const HistVarQuantityMap &hisVarQuantityMap, const bool isFromTrack) const
{
    const float clusterEnergy(pPeakCluster->GetElectromagneticEnergy());
    return (this->PassPhotonQualityCut(clusterEnergy, hisVarQuantityMap) &&
        this->PassPhotonMetricCut(this->GetMetricForPhotonID(clusterEnergy, hisVarQuantityMap), clusterEnergy, isFromTrack));
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

bool PhotonReconstructionAlgorithm::PassPhotonQualityCut(const float clusterEnergy, const HistVarQuantityMap &hisVarQuantityMap) const
{
    return ( clusterEnergy > m_minPeakEnergy &&
        hisVarQuantityMap.find(PEAKRMS)->second < m_maxPeakRms && 
        hisVarQuantityMap.find(RMSRATIO)->second < m_maxRmsRatio && 
        hisVarQuantityMap.find(LONGPROFILESTART)->second < m_maxLongProfileStart && 
        hisVarQuantityMap.find(LONGPROFILEDISCREPANCY)->second < m_maxLongProfileDiscrepancy && 
        hisVarQuantityMap.find(MINDISTANCETOTRACK)->second > m_minDistanceToTrackCutLow &&
        hisVarQuantityMap.find(MINDISTANCETOTRACK)->second < m_minDistanceToTrackCutHigh);
}

//------------------------------------------------------------------------------------------------------------------------------------------

float PhotonReconstructionAlgorithm::GetMetricForPhotonID(const float clusterEnergy, const HistVarQuantityMap &hisVarQuantityMap) const
{
    double yes(1.f), no(1.f);
    const unsigned int energyBin(this->GetEnergyBin(clusterEnergy));
    
    for (HistVarSglBkgHistMap::const_iterator iter = m_histVarSglBkgHistMap.begin(), iterEnd = m_histVarSglBkgHistMap.end(); iter != iterEnd; ++iter)
    {
        const HistSglBkgObject &histSglBkgObject((*iter).second);
        const HistVar &histVar((*iter).first);
        
        yes *= this->GetHistogramContent(histSglBkgObject.m_pSglHistogram[energyBin], hisVarQuantityMap.find(histVar)->second);
        no  *= this->GetHistogramContent(histSglBkgObject.m_pBkgHistogram[energyBin], hisVarQuantityMap.find(histVar)->second);
    }
    const double yesWeighted(yes * m_nSignalEvents[energyBin]), noWeighted(no * m_nBackgroundEvents[energyBin]);
    return ((yesWeighted + noWeighted) > std::numeric_limits<double>::epsilon() ? static_cast<float>(yesWeighted / (yesWeighted + noWeighted)) : 0.f);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonReconstructionAlgorithm::PassPhotonMetricCut(const float metric, const float clusterEnergy, const bool /*isFromTrack*/) const
{
    // ATTN isFromTrack is left for future if someone wants to have different cut for cluster close to track or not
    if (clusterEnergy < m_energyCutForPid1)
    {
        if (metric > m_pidCut1)
            return true;
    }else if (clusterEnergy < m_energyCutForPid2)
    {
        if (metric > m_pidCut2 )
            return true;
    }else
    {
        if (metric > m_pidCut3 )
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

StatusCode PhotonReconstructionAlgorithm::ReplaceInputClusterList() const
{
    const std::string replacementListName(m_replaceCurrentClusterList ? m_clusterListName : m_inputClusterListName);
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
    
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->InitialiseHistogramVarObjectMap());
    
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
    
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FillHistogramVarObjectMapParameters(xmlHandle, PEAKRMS, "PeakRmsNBins", 50, 
        "PeakRmsLowValue", 0.f, "PeakRmsHighValue", 5.f));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FillHistogramVarObjectMapParameters(xmlHandle, RMSRATIO, "RmsRatioNBins", 30, 
        "RmsRatioLowValue", 1.f, "RmsRatioHighValue", 3.f));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FillHistogramVarObjectMapParameters(xmlHandle, LONGPROFILESTART, "LongProfileStartNBins", 11, 
        "LongProfileStartLowValue", -0.5f, "LongProfileStartHighValue", 10.5f));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FillHistogramVarObjectMapParameters(xmlHandle, LONGPROFILEDISCREPANCY, "LongProfileDiscrepancyNBins", 42, 
        "LongProfileDiscrepancyLowValue", -0.02f, "LongProfileDiscrepancyHighValue", 0.82f));//52 -0.02 1.02
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FillHistogramVarObjectMapParameters(xmlHandle, PEAKENERGYFRACTION, "PeakEnergyFractionNBins", 52, 
        "PeakEnergyFractionLowValue", -0.02f, "PeakEnergyFractionHighValue", 1.02f));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FillHistogramVarObjectMapParameters(xmlHandle, MINDISTANCETOTRACK, "MinDistanceToTrackNBins", 40, 
        "MinDistanceToTrackLowValue", 0.f, "MinDistanceToTrackHighValue", 20.f));

    for (HistVarSglBkgHistMap::iterator iter = m_histVarSglBkgHistMap.begin(), iterEnd = m_histVarSglBkgHistMap.end(); iter != iterEnd; ++iter)
    {
        HistSglBkgObject &histSglBkgObject((*iter).second);
        
        histSglBkgObject.m_pSglHistogram = new Histogram*[m_nEnergyBins];
        histSglBkgObject.m_pBkgHistogram = new Histogram*[m_nEnergyBins];
        
        for (unsigned int energyBin = 0; energyBin < m_nEnergyBins; ++energyBin)
        {
            histSglBkgObject.m_pSglHistogram[energyBin] = new Histogram(histSglBkgObject.m_nBins, histSglBkgObject.m_lowValue, histSglBkgObject.m_highValue);
            histSglBkgObject.m_pBkgHistogram[energyBin] = new Histogram(histSglBkgObject.m_nBins, histSglBkgObject.m_lowValue, histSglBkgObject.m_highValue);
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
    
    for (HistVarSglBkgHistMap::iterator iter = m_histVarSglBkgHistMap.begin(), iterEnd = m_histVarSglBkgHistMap.end(); iter != iterEnd; ++iter)
    {
        HistSglBkgObject &histSglBkgObject((*iter).second);
        
        histSglBkgObject.m_pSglHistogram = new Histogram*[m_nEnergyBins];
        histSglBkgObject.m_pBkgHistogram = new Histogram*[m_nEnergyBins];
        
        for (unsigned int energyBin = 0; energyBin < m_nEnergyBins; ++energyBin)
        {
            histSglBkgObject.m_pSglHistogram[energyBin] = new Histogram(&pdfXmlHandle, "PhotonSig" + histSglBkgObject.m_histName + "_" + TypeToString(energyBin));
            histSglBkgObject.m_pBkgHistogram[energyBin] = new Histogram(&pdfXmlHandle, "PhotonBkg" + histSglBkgObject.m_histName + "_" + TypeToString(energyBin));
        }
    }
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::InitialiseHistogramVarObjectMap()
{
    if (false == m_histVarSglBkgHistMap.insert(HistVarSglBkgHistMap::value_type(PEAKRMS, HistSglBkgObject("PeakRms"))).second)
        return STATUS_CODE_FAILURE;
    if (false == m_histVarSglBkgHistMap.insert(HistVarSglBkgHistMap::value_type(RMSRATIO, HistSglBkgObject("RmsRatio"))).second)
        return STATUS_CODE_FAILURE;
    if (false == m_histVarSglBkgHistMap.insert(HistVarSglBkgHistMap::value_type(LONGPROFILESTART, HistSglBkgObject("LongProfileStart"))).second)
        return STATUS_CODE_FAILURE;
    if (false == m_histVarSglBkgHistMap.insert(HistVarSglBkgHistMap::value_type(LONGPROFILEDISCREPANCY, HistSglBkgObject("LongProfileDiscrepancy"))).second)
        return STATUS_CODE_FAILURE;
    if (false == m_histVarSglBkgHistMap.insert(HistVarSglBkgHistMap::value_type(PEAKENERGYFRACTION, HistSglBkgObject("PeakEnergyFraction"))).second)
        return STATUS_CODE_FAILURE;
    if (false == m_histVarSglBkgHistMap.insert(HistVarSglBkgHistMap::value_type(MINDISTANCETOTRACK, HistSglBkgObject("MinDistanceToTrack"))).second)
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

StatusCode PhotonReconstructionAlgorithm::FillHistogramVarObjectMapParameters(const TiXmlHandle xmlHandle, const HistVar histVar, const std::string &nBinStr, 
    const int nBinDefault, const std::string &lowValueStr, const float lowValueDefault, const std::string &highValueStr, const float highValueDefault)
{
    HistSglBkgObject &histSglBkgObject(m_histVarSglBkgHistMap.find(histVar)->second);
    
    histSglBkgObject.m_nBins = nBinDefault;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        nBinStr, histSglBkgObject.m_nBins));
    
    histSglBkgObject.m_lowValue = lowValueDefault;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        lowValueStr, histSglBkgObject.m_lowValue));

    histSglBkgObject.m_highValue = highValueDefault;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        highValueStr, histSglBkgObject.m_highValue));
        
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
    HistVarQuantityMap hisVarQuantityMap;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CalculateForPhotonID(showerPeak, pPeakCluster, wholeClusuterEnergy, hisVarQuantityMap));
    // ATTN when training the likelihood file, only write xml file. Do not read xml file
    if (this->PassPhotonQualityCut(pPeakCluster->GetElectromagneticEnergy(), hisVarQuantityMap))
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FillPdfHistograms(pPeakCluster, hisVarQuantityMap));
    }
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::FillPdfHistograms(const pandora::Cluster *const pCluster, const HistVarQuantityMap &hisVarQuantityMap)
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
        for (HistVarSglBkgHistMap::const_iterator iter = m_histVarSglBkgHistMap.begin(), iterEnd = m_histVarSglBkgHistMap.end(); iter != iterEnd; ++iter)
        {
            const HistSglBkgObject &histSglBkgObject((*iter).second);
            const HistVar &histVar((*iter).first);
            
            histSglBkgObject.m_pSglHistogram[energyBin]->Fill(hisVarQuantityMap.find(histVar)->second);
        }
        m_nSignalEvents[energyBin]++;
    }
    else
    {
        for (HistVarSglBkgHistMap::const_iterator iter = m_histVarSglBkgHistMap.begin(), iterEnd = m_histVarSglBkgHistMap.end(); iter != iterEnd; ++iter)
        {
            const HistSglBkgObject &histSglBkgObject((*iter).second);
            const HistVar &histVar((*iter).first);
            
            histSglBkgObject.m_pBkgHistogram[energyBin]->Fill(hisVarQuantityMap.find(histVar)->second);
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
    
    for (HistVarSglBkgHistMap::iterator iter = m_histVarSglBkgHistMap.begin(), iterEnd = m_histVarSglBkgHistMap.end(); iter != iterEnd; ++iter)
    {
        HistSglBkgObject &histSglBkgObject((*iter).second);
        for (unsigned int energyBin = 0; energyBin < m_nEnergyBins; ++energyBin)
        {
            this->NormalizeHistogram(histSglBkgObject.m_pSglHistogram[energyBin]);
            this->NormalizeHistogram(histSglBkgObject.m_pBkgHistogram[energyBin]);
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
    
    for (HistVarSglBkgHistMap::iterator iter = m_histVarSglBkgHistMap.begin(), iterEnd = m_histVarSglBkgHistMap.end(); iter != iterEnd; ++iter)
    {
        HistSglBkgObject &histSglBkgObject((*iter).second);
        for (unsigned int energyBin = 0; energyBin < m_nEnergyBins; ++energyBin)
        {
            histSglBkgObject.m_pSglHistogram[energyBin]->WriteToXml(&xmlDocument, "PhotonSig" + histSglBkgObject.m_histName + "_" + TypeToString(energyBin));
            histSglBkgObject.m_pBkgHistogram[energyBin]->WriteToXml(&xmlDocument, "PhotonBkg" + histSglBkgObject.m_histName + "_" + TypeToString(energyBin));
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
        for (HistVarSglBkgHistMap::const_iterator iter = m_histVarSglBkgHistMap.begin(), iterEnd = m_histVarSglBkgHistMap.end(); iter != iterEnd; ++iter)
        {
            const HistSglBkgObject &histSglBkgObject((*iter).second);    
            std::cout << histSglBkgObject.m_histName << ", Signal, Background " << std::endl;
            PANDORA_MONITORING_API(DrawPandoraHistogram(this->GetPandora(), *histSglBkgObject.m_pSglHistogram[energyBin]));
            PANDORA_MONITORING_API(DrawPandoraHistogram(this->GetPandora(), *histSglBkgObject.m_pBkgHistogram[energyBin]));
        }
    }
}

} // namespace lc_content
