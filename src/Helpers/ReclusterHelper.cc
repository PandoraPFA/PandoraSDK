/**
 *  @file   PandoraPFANew/Framework/src/Helpers/ReclusterHelper.cc
 * 
 *  @brief  Implementation of the recluster helper class.
 * 
 *  $Log: $
 */

#include "Helpers/ReclusterHelper.h"
#include "Helpers/XmlHelper.h"

#include "Objects/Cluster.h"
#include "Objects/Track.h"

#include "Pandora/PandoraSettings.h"

#include <cmath>
#include <limits>

namespace pandora
{

unsigned int ReclusterHelper::m_nReclusteringProcesses = 0;
ReclusterHelper::ReclusterMonitoringMap ReclusterHelper::m_reclusterMonitoringMap;
ReclusterHelper::ProcessIdToTrackListMap ReclusterHelper::m_processIdToTrackListMap;

//------------------------------------------------------------------------------------------------------------------------------------------

float ReclusterHelper::GetTrackClusterCompatibility(const Cluster *const pCluster, const TrackList &trackList)
{
    float trackEnergySum(0.);

    for (TrackList::const_iterator trackIter = trackList.begin(), trackIterEnd = trackList.end(); trackIter != trackIterEnd; ++trackIter)
        trackEnergySum += (*trackIter)->GetEnergyAtDca();

    static const float hadronicEnergyResolution(PandoraSettings::GetHadronicEnergyResolution());

    if ((0. == trackEnergySum) || (0. == hadronicEnergyResolution))
        throw StatusCodeException(STATUS_CODE_FAILURE);

    const float sigmaE(hadronicEnergyResolution * trackEnergySum / std::sqrt(trackEnergySum));
    const float chi((pCluster->GetTrackComparisonEnergy() - trackEnergySum) / sigmaE);

    return chi;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float ReclusterHelper::GetTrackClusterCompatibility(const float clusterEnergy, const float trackEnergy)
{
    static const float hadronicEnergyResolution(PandoraSettings::GetHadronicEnergyResolution());

    if ((0. == trackEnergy) || (0. == hadronicEnergyResolution))
        throw StatusCodeException(STATUS_CODE_FAILURE);

    const float sigmaE(hadronicEnergyResolution * trackEnergy / std::sqrt(trackEnergy));
    const float chi((clusterEnergy - trackEnergy) / sigmaE);

    return chi;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ReclusterHelper::ExtractReclusterResults(const ClusterList *const pReclusterCandidatesList, ReclusterResult &reclusterResult)
{
    unsigned int nExcessTrackAssociations(0);
    float chi(0.), chi2(0.), dof(0.), unassociatedEnergy(0.), minTrackAssociationEnergy(std::numeric_limits<float>::max());

    for (ClusterList::const_iterator iter = pReclusterCandidatesList->begin(), iterEnd = pReclusterCandidatesList->end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        const TrackList &trackList(pCluster->GetAssociatedTrackList());
        const unsigned int nTrackAssociations(trackList.size());
        const float clusterEnergy(pCluster->GetHadronicEnergy());

        if (0 == nTrackAssociations)
        {
            unassociatedEnergy += clusterEnergy;
            continue;
        }

        if (clusterEnergy < minTrackAssociationEnergy)
            minTrackAssociationEnergy = clusterEnergy;

        nExcessTrackAssociations += nTrackAssociations - 1;

        const float newChi(ReclusterHelper::GetTrackClusterCompatibility(pCluster, trackList));

        chi2 += newChi * newChi;
        chi += newChi;
        dof += 1.;
    }

    if (0. == dof)
        return STATUS_CODE_FAILURE;

    reclusterResult.SetChi(chi);
    reclusterResult.SetChi2(chi2);
    reclusterResult.SetChiPerDof(chi /= dof);
    reclusterResult.SetChi2PerDof(chi2 /= dof);
    reclusterResult.SetUnassociatedEnergy(unassociatedEnergy);
    reclusterResult.SetMinTrackAssociationEnergy(minTrackAssociationEnergy);
    reclusterResult.SetNExcessTrackAssociations(nExcessTrackAssociations);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ReclusterHelper::InitializeReclusterMonitoring(const TrackList &trackList)
{
    ++m_nReclusteringProcesses;

    for (TrackList::const_iterator iter = trackList.begin(), iterEnd = trackList.end(); iter != iterEnd; ++iter)
    {
        Track *pTrack = *iter;

        const void *pTrackParentAddress(pTrack->GetParentTrackAddress());
        ReclusterMonitoringMap::const_iterator mapIter = m_reclusterMonitoringMap.find(pTrackParentAddress);

        if (m_reclusterMonitoringMap.end() == mapIter)
        {
            Cluster *pCluster = NULL;
            const float clusterEnergy((STATUS_CODE_SUCCESS != pTrack->GetAssociatedCluster(pCluster)) ? 0.f : pCluster->GetTrackComparisonEnergy());

            if (!m_reclusterMonitoringMap.insert(ReclusterMonitoringMap::value_type(pTrackParentAddress, ReclusterChangeLog(clusterEnergy))).second)
                return STATUS_CODE_FAILURE;

            if (!m_processIdToTrackListMap[m_nReclusteringProcesses].insert(pTrack).second)
                return STATUS_CODE_FAILURE;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ReclusterHelper::EndReclusterMonitoring()
{
    if (0 == m_nReclusteringProcesses)
        return STATUS_CODE_NOT_ALLOWED;

    ProcessIdToTrackListMap::iterator processIdIter = m_processIdToTrackListMap.find(m_nReclusteringProcesses);

    if (m_processIdToTrackListMap.end() != processIdIter)
    {
        for (TrackList::const_iterator iter = processIdIter->second.begin(), iterEnd = processIdIter->second.end(); iter != iterEnd; ++iter)
        {
            Track *pTrack = *iter;

            const void *pTrackParentAddress(pTrack->GetParentTrackAddress());
            ReclusterMonitoringMap::iterator mapIter = m_reclusterMonitoringMap.find(pTrackParentAddress);

            if (m_reclusterMonitoringMap.end() == mapIter)
                return STATUS_CODE_FAILURE;

            Cluster *pCluster = NULL;
            const float clusterEnergy((STATUS_CODE_SUCCESS != pTrack->GetAssociatedCluster(pCluster)) ? 0.f : pCluster->GetTrackComparisonEnergy());

            mapIter->second.SetNewEnergyValue(clusterEnergy);
        }
        m_processIdToTrackListMap.erase(processIdIter);
    }
    --m_nReclusteringProcesses;

    return STATUS_CODE_SUCCESS;
}


//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ReclusterHelper::GetReclusterMonitoringResults(const void *pTrackParentAddress, float &netEnergyChange, float &sumModulusEnergyChanges,
    float &sumSquaredEnergyChanges)
{
    netEnergyChange = 0.f;
    sumModulusEnergyChanges = 0.f;
    sumSquaredEnergyChanges = 0.f;

    ReclusterMonitoringMap::iterator mapIter = m_reclusterMonitoringMap.find(pTrackParentAddress);

    if (m_reclusterMonitoringMap.end() != mapIter)
    {
        netEnergyChange = mapIter->second.GetNetEnergyChange();
        sumModulusEnergyChanges = mapIter->second.GetSumModulusEnergyChanges();
        sumSquaredEnergyChanges = mapIter->second.GetSumSquaredEnergyChanges();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ReclusterHelper::ResetReclusterMonitoring()
{
    m_reclusterMonitoringMap.clear();
    m_processIdToTrackListMap.clear();

    if (!m_reclusterMonitoringMap.empty() || !m_processIdToTrackListMap.empty())
        return STATUS_CODE_FAILURE;

    m_nReclusteringProcesses = 0;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ReclusterHelper::ReadSettings(const TiXmlHandle *const /*pXmlHandle*/)
{
    /*TiXmlElement *pXmlElement(pXmlHandle->FirstChild("ReclusterHelper").Element());

    if (NULL != pXmlElement)
    {
        const TiXmlHandle xmlHandle(pXmlElement);
    }*/

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

ReclusterHelper::ReclusterChangeLog::ReclusterChangeLog(const float initialEnergy) :
    m_currentEnergy(initialEnergy),
    m_netEnergyChange(0.f),
    m_sumModulusEnergyChanges(0.f),
    m_sumSquaredEnergyChanges(0.f),
    m_nEnergyChanges(0)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

void ReclusterHelper::ReclusterChangeLog::SetNewEnergyValue(const float newEnergy)
{
    const float energyChange(newEnergy - m_currentEnergy);

    if (std::fabs(energyChange) < std::numeric_limits<float>::epsilon())
        return;

    m_netEnergyChange += energyChange;
    m_sumModulusEnergyChanges += std::fabs(energyChange);
    m_sumSquaredEnergyChanges += (energyChange * energyChange);
    m_currentEnergy = newEnergy;
    ++m_nEnergyChanges;
}

} // namespace pandora
