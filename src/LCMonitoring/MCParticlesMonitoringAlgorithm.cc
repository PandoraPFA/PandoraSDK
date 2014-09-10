/**
 *  @file   LCContent/src/LCMonitoring/MCParticlesMonitoringAlgorithm.cc
 * 
 *  @brief  Implementation of an algorithm to monitor the mc particles
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCHelpers/SortingHelper.h"

#include "LCMonitoring/MCParticlesMonitoringAlgorithm.h"

#include <iostream>
#include <iomanip>
#include <assert.h>

using namespace pandora;

namespace lc_content
{

StatusCode MCParticlesMonitoringAlgorithm::Initialize()
{
    m_energy = new FloatVector();
    m_momentumX = new FloatVector();
    m_momentumY = new FloatVector();
    m_momentumZ = new FloatVector();
    m_particleId = new IntVector();
    m_outerRadius = new FloatVector();
    m_innerRadius = new FloatVector();
    m_caloHitEnergy = new FloatVector();
    m_trackEnergy = new FloatVector();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

MCParticlesMonitoringAlgorithm::~MCParticlesMonitoringAlgorithm()
{
    if (!m_monitoringFileName.empty() && !m_treeName.empty())
    {
        PANDORA_MONITORING_API(SaveTree(this->GetPandora(), m_treeName, m_monitoringFileName, "UPDATE" ));
    }

    delete m_energy;
    delete m_momentumX;
    delete m_momentumY;
    delete m_momentumZ;
    delete m_particleId;
    delete m_outerRadius;
    delete m_innerRadius;
    delete m_caloHitEnergy;
    delete m_trackEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCParticlesMonitoringAlgorithm::Run()
{
    const MCParticleList *pMCParticleList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pMCParticleList));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FillListOfUsedMCParticles());
    this->MonitorMCParticleList(*pMCParticleList);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void MCParticlesMonitoringAlgorithm::MonitorMCParticleList(const MCParticleList &mcParticleList)
{
    m_energy->clear();
    m_momentumX->clear();
    m_momentumY->clear();
    m_momentumZ->clear();
    m_particleId->clear();
    m_outerRadius->clear();
    m_innerRadius->clear();
    m_caloHitEnergy->clear();
    m_trackEnergy->clear();

    typedef std::multimap<float, int, std::greater<float> > SortIndex;
    SortIndex sortIndex;

    typedef std::vector<const MCParticle*> MCParticleVector;
    MCParticleVector mcParticleVector;

    int mcParticleNumber = 0;

    for (MCParticleList::const_iterator iter = mcParticleList.begin(), iterEnd = mcParticleList.end(); iter != iterEnd; ++iter)
    {
        const MCParticle *pMCParticle = *iter;
        float caloHitEnergy(0.f), trackEnergy(0.f);

        if (this->TakeMCParticle(pMCParticle, caloHitEnergy, trackEnergy))
        {
            mcParticleVector.push_back(pMCParticle);
            sortIndex.insert(SortIndex::value_type(pMCParticle->GetEnergy(), mcParticleNumber++));

            m_energy->push_back(pMCParticle->GetEnergy());
            m_momentumX->push_back(pMCParticle->GetMomentum().GetX());
            m_momentumY->push_back(pMCParticle->GetMomentum().GetY());
            m_momentumZ->push_back(pMCParticle->GetMomentum().GetZ());
            m_particleId->push_back(pMCParticle->GetParticleId());
            m_outerRadius->push_back(pMCParticle->GetOuterRadius());
            m_innerRadius->push_back(pMCParticle->GetInnerRadius());
            m_caloHitEnergy->push_back(caloHitEnergy);
            m_trackEnergy->push_back(trackEnergy);
        }
    }

    if (m_sort)
    {
        for (SortIndex::iterator iter = sortIndex.begin(), iterEnd = sortIndex.end(); iter != iterEnd; ++iter)
        {
            const int index(iter->second);
            assert(std::fabs(iter->first - m_energy->at(index)) < 0.1);

            m_energy->push_back(m_energy->at(index));
            m_momentumX->push_back(m_momentumX->at(index));
            m_momentumY->push_back(m_momentumY->at(index));
            m_momentumZ->push_back(m_momentumZ->at(index));
            m_particleId->push_back(m_particleId->at(index));
            m_outerRadius->push_back(m_outerRadius->at(index));
            m_innerRadius->push_back(m_innerRadius->at(index));
            m_caloHitEnergy->push_back(m_caloHitEnergy->at(index));
            m_trackEnergy->push_back(m_trackEnergy->at(index));
            mcParticleVector.push_back(mcParticleVector.at(index));
        }

        const size_t sortIndexSize(sortIndex.size());
        m_energy->erase(m_energy->begin(), m_energy->begin() + sortIndexSize);
        m_momentumX->erase(m_momentumX->begin(), m_momentumX->begin() + sortIndexSize);
        m_momentumY->erase(m_momentumY->begin(), m_momentumY->begin() + sortIndexSize);
        m_momentumZ->erase(m_momentumZ->begin(), m_momentumZ->begin() + sortIndexSize);
        m_particleId->erase(m_particleId->begin(), m_particleId->begin() + sortIndexSize);
        m_outerRadius->erase(m_outerRadius->begin(), m_outerRadius->begin() + sortIndexSize);
        m_innerRadius->erase(m_innerRadius->begin(), m_innerRadius->begin() + sortIndexSize);
        m_caloHitEnergy->erase(m_caloHitEnergy->begin(), m_caloHitEnergy->begin() + sortIndexSize);
        m_trackEnergy->erase(m_trackEnergy->begin(), m_trackEnergy->begin()   + sortIndexSize);
        mcParticleVector.erase( mcParticleVector.begin(), mcParticleVector.begin() + sortIndexSize );
    }

    if (m_print)
    {
        std::sort(mcParticleVector.begin(), mcParticleVector.end(), SortingHelper::SortMCParticlesByEnergy);
        int index(0);

        for (MCParticleVector::iterator iter = mcParticleVector.begin(), iterEnd = mcParticleVector.end(); iter != iterEnd; ++iter)
        {
            this->PrintMCParticle(*iter, m_caloHitEnergy->at(index), m_trackEnergy->at(index), std::cout);
            std::cout << std::endl;
            ++index;
        }

        std::cout << "Total number of MCPFOs : " << mcParticleNumber << std::endl;
    }

    if (!m_monitoringFileName.empty() && !m_treeName.empty())
    {
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_treeName, "energy", m_energy));
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_treeName, "pX", m_momentumX));
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_treeName, "pY", m_momentumY));
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_treeName, "pZ", m_momentumZ));
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_treeName, "id", m_particleId));
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_treeName, "rO", m_outerRadius));
        PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_treeName, "rI", m_innerRadius));

        if (m_haveCaloHits)
        {
            PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_treeName, "ECalo", m_caloHitEnergy ));
        }

        if (m_haveTracks)
        {
            PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), m_treeName, "ETrack", m_trackEnergy ));
        }

        PANDORA_MONITORING_API(FillTree(this->GetPandora(), m_treeName));
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void MCParticlesMonitoringAlgorithm::PrintMCParticle(const MCParticle *pMCParticle, float &caloHitEnergy, float &trackEnergy, std::ostream &os)
{
    if (m_indent)
    {
        int printDepth = (int)(pMCParticle->GetOuterRadius() / 100); // this can be changed if the printout doesn't look good
        os << std::setw (printDepth) << " ";
    }

    const CartesianVector &momentum = pMCParticle->GetMomentum();
    const CartesianVector &endPoint = pMCParticle->GetEndpoint();

    float radius(0.f), phi(0.f), theta(0.f);
    endPoint.GetSphericalCoordinates(radius, phi, theta);
    const float eta(-1. * std::log(std::tan(theta / 2.)));

    os << std::fixed << std::setprecision(2)
       << "PID=" << pMCParticle->GetParticleId()
       << " E=" << pMCParticle->GetEnergy()
       << std::fixed << std::setprecision(4)
       << " ETA=" << eta
       << " PHI=" << phi
       << std::fixed << std::setprecision(1)
       << " r_i=" << pMCParticle->GetInnerRadius()
       << " r_o=" << pMCParticle->GetOuterRadius()
       << std::scientific
       << " px=" << momentum.GetX()
       << " py=" << momentum.GetY()
       << " pz=" << momentum.GetZ()
       << std::fixed << std::setprecision(4);

    if (m_haveCaloHits)
        os << " ECalo=" << caloHitEnergy;

    if (m_haveTracks)
        os << " ETrack=" << trackEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCParticlesMonitoringAlgorithm::FillListOfUsedMCParticles()
{
    if (m_clusterListNames.empty())
    {
        if (m_haveCaloHits)
        {
            const CaloHitList *pCaloHitList = NULL;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pCaloHitList));

            for(CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
            {
                try
                {
                    CaloHit *pCaloHit = *iter;
                    const float energy(pCaloHit->GetElectromagneticEnergy());
                    const MCParticle *pMCParticle(MCParticleHelper::GetMainMCParticle(pCaloHit));

                    ConstMCParticleToEnergyMap::iterator mcIter = m_mcParticleToEnergyMap.find(pMCParticle);

                    if (mcIter == m_mcParticleToEnergyMap.end())
                    {
                        m_mcParticleToEnergyMap.insert(ConstMCParticleToEnergyMap::value_type(pMCParticle, std::make_pair(energy, 0.f)));
                    }
                    else
                    {
                        mcIter->second.first += energy;
                    }
                }
                catch (StatusCodeException &)
                {
                }
            }
        }

        if (m_haveTracks)
        {
            const TrackList *pTrackList = NULL;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pTrackList));

            for (TrackList::const_iterator iter = pTrackList->begin(), iterEnd = pTrackList->end(); iter != iterEnd; ++iter)
            {
                try
                {
                    Track *pTrack = *iter;
                    const float energy(pTrack->GetEnergyAtDca());
                    const MCParticle *pMCParticle(pTrack->GetMainMCParticle());

                    ConstMCParticleToEnergyMap::iterator mcIter = m_mcParticleToEnergyMap.find(pMCParticle);

                    if (mcIter == m_mcParticleToEnergyMap.end())
                    {
                        m_mcParticleToEnergyMap.insert(ConstMCParticleToEnergyMap::value_type(pMCParticle, std::make_pair(0.f, energy)));
                    }
                    else
                    {
                        if (mcIter->second.second < energy)
                            mcIter->second.second = energy;
                    }
                }
                catch (StatusCodeException &)
                {
                }
            }
        }
    }
    else
    {
        typedef std::vector<const ClusterList*> ClusterVector;
        ClusterVector clusterListVector;

        for (StringVector::iterator iter = m_clusterListNames.begin(), iterEnd = m_clusterListNames.end(); iter != iterEnd; ++iter)
        {
            const ClusterList *pClusterList = NULL;
            if (STATUS_CODE_SUCCESS == PandoraContentApi::GetList(*this, *iter, pClusterList))
                clusterListVector.push_back(pClusterList);
        }

        for (ClusterVector::const_iterator iter = clusterListVector.begin(), iterEnd = clusterListVector.end(); iter != iterEnd; ++iter)
        {
            const ClusterList *pClusterList = *iter;

            for (ClusterList::const_iterator cIter = pClusterList->begin(), cIterEnd = pClusterList->end(); cIter != cIterEnd; ++cIter)
            {
                const Cluster *pCluster = *cIter;

                if (m_haveCaloHits)
                {
                    CaloHitList caloHitList;
                    pCluster->GetOrderedCaloHitList().GetCaloHitList(caloHitList);

                    for (CaloHitList::const_iterator hIter = caloHitList.begin(), hIterEnd = caloHitList.end(); hIter != hIterEnd; ++hIter)
                    {
                        try
                        {
                            CaloHit *pCaloHit = *hIter;
                            const float energy(pCaloHit->GetElectromagneticEnergy());
                            const MCParticle *pMCParticle(MCParticleHelper::GetMainMCParticle(pCaloHit));

                            ConstMCParticleToEnergyMap::iterator mcIter = m_mcParticleToEnergyMap.find(pMCParticle);

                            if (mcIter == m_mcParticleToEnergyMap.end())
                            {
                                m_mcParticleToEnergyMap.insert(ConstMCParticleToEnergyMap::value_type(pMCParticle, std::make_pair(energy, 0.f)));
                            }
                            else
                            {
                                mcIter->second.first += energy;
                            }
                        }
                        catch (StatusCodeException &)
                        {
                        }
                    }
                }

                if (m_haveTracks)
                {
                    const TrackList &trackList = pCluster->GetAssociatedTrackList();

                    for (TrackList::const_iterator tIter = trackList.begin(), tIterEnd = trackList.end(); tIter != tIterEnd; ++tIter)
                    {
                        try
                        {
                            Track *pTrack = *tIter;
                            const float energy(pTrack->GetEnergyAtDca());
                            const MCParticle *pMCParticle(pTrack->GetMainMCParticle());

                            ConstMCParticleToEnergyMap::iterator mcIter = m_mcParticleToEnergyMap.find(pMCParticle);

                            if (mcIter == m_mcParticleToEnergyMap.end())
                            {
                                m_mcParticleToEnergyMap.insert(ConstMCParticleToEnergyMap::value_type(pMCParticle, std::make_pair(0.f, energy)));
                            }
                            else
                            {
                                if (mcIter->second.second < energy)
                                    mcIter->second.second = energy;
                            }
                        }
                        catch (StatusCodeException &)
                        {
                        }
                    }
                }
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool MCParticlesMonitoringAlgorithm::TakeMCParticle(const MCParticle* pMCParticle, float& caloHitEnergy, float& trackEnergy)
{
    if (m_onlyFinal && !pMCParticle->GetDaughterList().empty())
        return false;

    if (m_excludeRootParticles && pMCParticle->GetParentList().empty())
        return false;

    if (m_onlyRootParticles && !pMCParticle->GetParentList().empty())
        return false;

    ConstMCParticleToEnergyMap::iterator iter = m_mcParticleToEnergyMap.find(pMCParticle);

    if (iter == m_mcParticleToEnergyMap.end())
    {
        caloHitEnergy = 0.f;
        trackEnergy = 0.f;

        if (m_haveCaloHits || m_haveTracks)
        {
            return false;
        }
    }
    else
    {
        caloHitEnergy = iter->second.first;
        trackEnergy = iter->second.second;
        return true;
    }

    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCParticlesMonitoringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MonitoringFileName", m_monitoringFileName));

    m_treeName = "MCMonitoringTree";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TreeName", m_treeName));

    m_print = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Print", m_print));

    m_indent = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Indent", m_indent));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "ClusterListNames", m_clusterListNames));

    // "Final" takes only particles without daughters;
    // "ExcludeRoot" excludes particles which don't have parents;
    // "OnlyRoot" takes only particles which don't have parents;
    // "CalorimeterHits"/"Tracks" takes only particles which have caused Tracks or Calorimeterhits.
    //
    // With current rules for MCPFO selection only particles which cross spherical boundary around the IP are retained.
    // All particles are therefore ROOT-particles, the ExcludeRoot, the OnlyRoot and the Final options are therefore nonsensical.
    StringVector mcParticleSelection;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "Selection", mcParticleSelection));

    m_onlyFinal = false;
    m_haveCaloHits = false;
    m_haveTracks = false;

    m_excludeRootParticles = false;
    m_onlyRootParticles = false;   

    for (StringVector::iterator itStr = mcParticleSelection.begin(), itStrEnd = mcParticleSelection.end(); itStr != itStrEnd; ++itStr)
    {
        std::string currentString = (*itStr);

        if (currentString == "Final")
        {
            m_onlyFinal = true;
        }
        else if (currentString == "CalorimeterHits")
        {
            m_haveCaloHits = true;
        }
        else if (currentString == "Tracks")
        {
            m_haveTracks = true;
        }
        else if (currentString == "ExcludeRoot")
        {
            m_excludeRootParticles = true;
        }
        else if (currentString == "OnlyRoot")
        {
            m_onlyRootParticles = true;
        }
        else
        {
            std::cout << "<Selection> '" << currentString << "' unknown in algorithm 'MCParticlesMonitoring'." << std::endl;
            return STATUS_CODE_NOT_FOUND;
        }
    }

    m_sort = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Sort", m_sort));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
