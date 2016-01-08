/**
 *  @file   LCContent/src/LCParticleId/PhotonSplittingAlgorithm.cc
 *
 *  @brief  Implementation of the photon splitting algorithm
 *
 *  $Log: $
 */
#include "Pandora/AlgorithmHeaders.h"
#include "LCHelpers/ClusterHelper.h"
#include "LCHelpers/SortingHelper.h"

#include "LCParticleId/PhotonSplittingAlgorithm.h"

using namespace pandora;

namespace lc_content
{

PhotonSplittingAlgorithm::PhotonSplittingAlgorithm():
    m_maxSearchLayer(9),
    m_parallelDistanceCut(100.f),
    m_minTrackClusterCosAngle(0.f),
    m_maxDistanceToTrackCut(20000.f),
    m_transProfileEcalOnly(true),
    m_transProfileMaxLayer(0),
    m_minClusterEnergy1(10.f),
    m_minDaughterEnergy1(1.f),
    m_minClusterEnergy2(10.f),
    m_minDaughterEnergy2(5.f),
    m_minClusterEnergy3(20.f),
    m_minDaughterEnergy3(10.f),
    m_maxNPeaks(5)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

PhotonSplittingAlgorithm::~PhotonSplittingAlgorithm()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonSplittingAlgorithm::Run()
{
    // ATTN Implicit assumption that individual physical layers in the ECAL will always correspond to individual pseudo layers
    // Also ECAL BARRAL has same layer as ECAL ENDCAP and ecal is the first inner detector
    if (m_transProfileEcalOnly && m_transProfileMaxLayer <= 0 )
        m_transProfileMaxLayer = PandoraContentApi::GetPlugins(*this)->GetPseudoLayerPlugin()->GetPseudoLayerAtIp() + 
            PandoraContentApi::GetGeometry(*this)->GetSubDetector(ECAL_BARREL).GetNLayers();
            
    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pTrackList));

    TrackVector trackVector(pTrackList->begin(), pTrackList->end());
    std::sort(trackVector.begin(), trackVector.end(), lc_content::SortingHelper::SortTracksByEnergy);

    const ClusterList *pPhotonClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pPhotonClusterList));

    const ShowerProfilePlugin *const pShowerProfilePlugin(PandoraContentApi::GetPlugins(*this)->GetShowerProfilePlugin());
    ClusterVector clusterVector(pPhotonClusterList->begin(), pPhotonClusterList->end());
    for (ClusterVector::const_iterator iter = clusterVector.begin(), iterEnd = clusterVector.end(); iter != iterEnd; ++iter)
    {
        const Cluster *const pCluster = *iter;
        if (pCluster->GetParticleIdFlag() != PHOTON) continue;
        
        int nCloseTrack(0);
        for (TrackVector::const_iterator trackIter = trackVector.begin(), trackIterEnd = trackVector.end(); trackIter != trackIterEnd; ++trackIter)
        {
            const Track *const pTrack = *trackIter;
            float trackClusterDistance(std::numeric_limits<float>::max());
            if (STATUS_CODE_SUCCESS == lc_content::ClusterHelper::GetTrackClusterDistance(pTrack, pCluster, m_maxSearchLayer, m_parallelDistanceCut,
                m_minTrackClusterCosAngle, trackClusterDistance))
            {
                if (trackClusterDistance < m_maxDistanceToTrackCut)
                    ++nCloseTrack;
            }
        }

        ShowerProfilePlugin::ShowerPeakList showersPhoton;
        pShowerProfilePlugin->CalculateTransverseProfile(pCluster, m_transProfileMaxLayer, showersPhoton, true);
        
        bool split(false);
        float cutOffE(m_minClusterEnergy3), cutOffE2(m_minDaughterEnergy3);
        if (nCloseTrack == 0)
        {
            cutOffE = m_minClusterEnergy1;
            cutOffE2 = m_minDaughterEnergy1;
        }
        else if (nCloseTrack == 1)
        {
            cutOffE = m_minClusterEnergy2;
            cutOffE2 = m_minDaughterEnergy2;
        }
        if (pCluster->GetElectromagneticEnergy() > cutOffE && showersPhoton.size() < m_maxNPeaks)
        {
            int energyCounter(0);
            for (ShowerProfilePlugin::ShowerPeakList::const_iterator jIter = showersPhoton.begin(), jIterEnd = showersPhoton.end(); jIter != jIterEnd; ++jIter)
            {
                const ShowerProfilePlugin::ShowerPeak &showerPeak(*jIter);
                if (showerPeak.GetPeakEnergy() > cutOffE2   )
                    ++energyCounter;
            }
            if (energyCounter > 1)
                split = true;
        }
        if (showersPhoton.size() > 1 && split ){
            ClusterList tempClusterList;
            tempClusterList.insert(pCluster);
            std::string originalClusterListName, tempClusterListName;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::InitializeFragmentation(*this, tempClusterList,
                originalClusterListName, tempClusterListName));
            for (int iPeak = 0, iPeakEnd =showersPhoton.size(); iPeak!=iPeakEnd; ++iPeak)
            {
                const ShowerProfilePlugin::ShowerPeak &showerPeak(showersPhoton[iPeak]);
                PandoraContentApi::Cluster::Parameters parameters;
                parameters.m_caloHitList = showerPeak.GetPeakCaloHitList();
                if (0 == iPeak)
                    parameters.m_isolatedCaloHitList = pCluster->GetIsolatedCaloHitList();
                const Cluster *pNewCluster = NULL;
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, parameters, pNewCluster));
                PandoraContentApi::Cluster::Metadata metadata;
                metadata.m_particleId = PHOTON;
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AlterMetadata(*this, pNewCluster, metadata));
            }
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::EndFragmentation(*this, tempClusterListName,
                originalClusterListName));
        }
    }
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonSplittingAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=,XmlHelper::ReadValue(xmlHandle,
    "MaxSearchLayer", m_maxSearchLayer));
    
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=,XmlHelper::ReadValue(xmlHandle,
    "ParallelDistanceCut", m_parallelDistanceCut));
    
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=,XmlHelper::ReadValue(xmlHandle,
    "MinTrackClusterCosAngle", m_minTrackClusterCosAngle));
    
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=,XmlHelper::ReadValue(xmlHandle,
    "MaxDistanceToTrackCut", m_maxDistanceToTrackCut));
    
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=,XmlHelper::ReadValue(xmlHandle,
    "TransProfileEcalOnly", m_transProfileEcalOnly));
    
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=,XmlHelper::ReadValue(xmlHandle,
    "TransProfileMaxLayer", m_transProfileMaxLayer));
    
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=,XmlHelper::ReadValue(xmlHandle,
    "MinClusterEnergy1", m_minClusterEnergy1));
    
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=,XmlHelper::ReadValue(xmlHandle,
    "MinDaughterEnergy1", m_minDaughterEnergy1));
    
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=,XmlHelper::ReadValue(xmlHandle,
    "MinClusterEnergy2", m_minClusterEnergy2));
    
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=,XmlHelper::ReadValue(xmlHandle,
    "MinDaughterEnergy2", m_minDaughterEnergy2));
    
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=,XmlHelper::ReadValue(xmlHandle,
    "MinClusterEnergy3", m_minClusterEnergy3));
    
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=,XmlHelper::ReadValue(xmlHandle,
    "MinDaughterEnergy3", m_minDaughterEnergy3));
    
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=,XmlHelper::ReadValue(xmlHandle,
    "MaxNPeaks", m_maxNPeaks));
    
    return STATUS_CODE_SUCCESS;
}
}
