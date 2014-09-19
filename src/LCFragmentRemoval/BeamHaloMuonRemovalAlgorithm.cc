/**
 *  @file   LCContent/src/LCFragmentRemoval/BeamHaloMuonRemovalAlgorithm.cc
 * 
 *  @brief  Implementation of the beam halo muon removal algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCFragmentRemoval/BeamHaloMuonRemovalAlgorithm.h"

#include <cstdlib>

using namespace pandora;

namespace lc_content
{

BeamHaloMuonRemovalAlgorithm::BeamHaloMuonRemovalAlgorithm() :
    m_monitoring(false),
    m_displayRetainedClusters(false),
    m_displayRejectedClusters(false)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BeamHaloMuonRemovalAlgorithm::Run()
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

    for (ClusterList::const_iterator iter = pClusterList->begin(); iter != pClusterList->end();)
    {
        Cluster *pCluster = *iter;
        iter++;

        if (this->IsBeamHaloMuon(pCluster))
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=,  PandoraContentApi::Delete(*this, pCluster));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool BeamHaloMuonRemovalAlgorithm::IsBeamHaloMuonInertia(pandora::Cluster * pCluster) const
{
    if (!pCluster->GetAssociatedTrackList().empty())
        return false;

    float zmin(std::numeric_limits<float>::max()), zmax(-std::numeric_limits<float>::max());
    float sumEnergy(0.f), sumEnergyX(0.f), sumEnergyY(0.f), sumEnergyZ(0.f);
    float inertiaX(0.f), inertiaY(0.f), inertiaZ(0.f);

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const CaloHit *pCaloHit = *hitIter;

            if (pCaloHit->GetHitType() != MUON)
            {
                const float energy(pCaloHit->GetHadronicEnergy());
                const CartesianVector &hitPosition(pCaloHit->GetPositionVector());

                sumEnergy  += energy;
                sumEnergyX += energy * hitPosition.GetX();
                sumEnergyY += energy * hitPosition.GetY();
                sumEnergyZ += energy * hitPosition.GetZ();

                if (hitPosition.GetZ() > zmax)
                    zmax = hitPosition.GetZ();

                if (hitPosition.GetZ() < zmin)
                    zmin = hitPosition.GetZ();
            }
        }
    }

    if (sumEnergy < std::numeric_limits<float>::epsilon())
    {
        std::cout << " BeamHaloMuonRemovalAlgorithm: Cluster with negligible energy " << std::endl;
        return false;
        //throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }

    const float zExtent(zmax - zmin);

    if (zExtent < 100.f)
        return false;

    const float meanX(sumEnergyX / sumEnergy);
    const float meanY(sumEnergyY / sumEnergy);
    const float meanZ(sumEnergyZ / sumEnergy);
    const float cosTheta(std::fabs(meanZ) / std::sqrt(meanX * meanX + meanY * meanY + meanZ * meanZ));

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const CaloHit *pCaloHit = *hitIter;

            if (pCaloHit->GetHitType() != MUON)
            {
                const float energy(pCaloHit->GetHadronicEnergy());
                const CartesianVector &hitPosition(pCaloHit->GetPositionVector());

                const float dx(hitPosition.GetX() - meanX);
                const float dy(hitPosition.GetY() - meanY);
                const float dz(hitPosition.GetZ() - meanZ);

                inertiaX += energy * (dy * dy + dz * dz);
                inertiaY += energy * (dz * dz + dx * dx);
                inertiaZ += energy * (dx * dx + dy * dy);
            }
        }
    }

    inertiaX /= sumEnergy;
    inertiaY /= sumEnergy;
    inertiaZ /= sumEnergy;

    const float ratioZ(inertiaZ / (inertiaX + inertiaY + inertiaZ));
    const float inertiaCut((zExtent < 250.f) ? 0.01f : 0.02f + (1.f - cosTheta) * 0.2f);

    return (ratioZ < inertiaCut);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool BeamHaloMuonRemovalAlgorithm::IsBeamHaloMuon(pandora::Cluster * pCluster) const
{
    if (!pCluster->GetAssociatedTrackList().empty())
        return false;

    const bool isBeamHaloMuonInertia(this->IsBeamHaloMuonInertia(pCluster));
    bool isBeamHaloMuon(isBeamHaloMuonInertia);

    float sumEnergy(0.f), sumEnergyX(0.f), sumEnergyY(0.f), sumEnergyZ(0.f);
    float xmin(std::numeric_limits<float>::max()), xmax(-std::numeric_limits<float>::max());
    float ymin(std::numeric_limits<float>::max()), ymax(-std::numeric_limits<float>::max());
    float zmin(std::numeric_limits<float>::max()), zmax(-std::numeric_limits<float>::max());

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const CaloHit *pCaloHit = *hitIter;

            if (pCaloHit->GetHitType() != MUON)
            {
                const float energy(pCaloHit->GetHadronicEnergy());
                const CartesianVector &hitPosition(pCaloHit->GetPositionVector());

                sumEnergy  += energy;
                sumEnergyX += energy * hitPosition.GetX();
                sumEnergyY += energy * hitPosition.GetY();
                sumEnergyZ += energy * hitPosition.GetZ();

                if (hitPosition.GetX() > xmax)
                    xmax = hitPosition.GetX();

                if (hitPosition.GetX() < xmin)
                    xmin = hitPosition.GetX();

                if (hitPosition.GetY() > ymax)
                    ymax = hitPosition.GetY();

                if (hitPosition.GetY() < ymin)
                    ymin = hitPosition.GetY();

                if (hitPosition.GetZ() > zmax)
                    zmax = hitPosition.GetZ();

                if (hitPosition.GetZ() < zmin)
                    zmin = hitPosition.GetZ();
            }
        }
    }

    if (sumEnergy < std::numeric_limits<float>::epsilon())
    {
        std::cout << " BeamHaloMuonRemovalAlgorithm: Cluster with negligible energy " << std::endl;
        return false;
        //throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }

    const float meanZ(sumEnergyZ / sumEnergy);
    const float zExtent(zmax - zmin);

    if (zExtent < 500.f && !isBeamHaloMuonInertia)
        return false;

    const int nBinsX(static_cast<int>((xmax - xmin) / 30.f) + 1);
    const int nBinsY(static_cast<int>((ymax - ymin) / 30.f) + 1);
    TwoDHistogram twoDHistogram_XY(nBinsX, xmin - 15.f, xmax + 15.f, nBinsY, ymin - 15.f, ymax + 15.f);

    float sumdZ(0.f);

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const CaloHit *pCaloHit = *hitIter;

            if (pCaloHit->GetHitType() != pandora::MUON)
            {
                const float energy(pCaloHit->GetHadronicEnergy());
                const CartesianVector &hitPosition(pCaloHit->GetPositionVector());

                twoDHistogram_XY.Fill(hitPosition.GetX(), hitPosition.GetY(), 1.f);

                const float dz(hitPosition.GetZ() - meanZ);
                sumdZ += energy * dz * dz;
            }
        }
    }

    const float rmsZ(std::sqrt(sumdZ / sumEnergy));

    // Identify peak in x-y
    float maximumValue(0.f); int maximumYBin(-1), maximumXBin(-1);
    twoDHistogram_XY.GetMaximum(maximumValue, maximumXBin, maximumYBin);

    int nFilledBins(0), nHighBins(0), nLowBins(0), nSurround(0);
    float nSumSurround(0.f);

    for (int iX = 0; iX < nBinsX; ++iX)
    {
        for (int iY = 0; iY < nBinsY; ++iY)
        {
            const float contentXY(twoDHistogram_XY.GetBinContent(iX, iY));

            if (contentXY > 1)
                nFilledBins++;

            if (contentXY > 3)
                nLowBins++;

            if (contentXY > 9)
                nHighBins++;

            if ((std::abs(iX - maximumXBin) < 2) && (std::abs(iY - maximumYBin) < 2) && (iX != maximumXBin || iY != maximumYBin))
            {
                if (contentXY >= 10)
                    nSurround++;

                nSumSurround += contentXY;
            }
        }
    }

    if (m_monitoring)
    {
        std::cout << " zExtent = " << zExtent << " rmsZ = " << rmsZ << std::endl;
        std::cout << " Maximum value " << maximumValue << std::endl;
        std::cout << " InertiaBasedCut " << isBeamHaloMuonInertia << std::endl;
        std::cout << " nFilledBins " << nFilledBins << " nLowBins " << nLowBins << " nHighBins " << nHighBins << " from " << nBinsX * nBinsY << std::endl; 
        std::cout << " Isolation count " << nSurround << " hits = " << nSumSurround << std::endl;
    }

    if ((maximumValue > 10.f) && (nFilledBins <= 15) && (nLowBins <= 10) && (nHighBins <= 5) && (nBinsX * nBinsY <= 600))
    {
        isBeamHaloMuon = true;
    }
    else if ((maximumValue > 20.f) && (maximumValue < 100.f) && (nFilledBins <= 25) && (nLowBins <= 15) && (nHighBins <= 10) && (nBinsX * nBinsY <= 250))
    {
        isBeamHaloMuon = true;
    }

    // Isolation veto
    if (isBeamHaloMuon)
    {
        const bool isolationVeto((nSurround > 1) && (zExtent < 1000.f) && (nBinsX * nBinsY > 100));

        if (isolationVeto)
        {
            if (m_monitoring)
                std::cout << " Veto beam halo muon cluster ID" << std::endl;

            isBeamHaloMuon = false;
        }
    }

    if (m_monitoring)
    {
        std::cout << " IsBeamHaloMuon : " << isBeamHaloMuon << std::endl;

        if ((isBeamHaloMuon && m_displayRejectedClusters) || (!isBeamHaloMuon && m_displayRetainedClusters))
        {
            ClusterList clusterList;
            clusterList.insert(pCluster);
            PANDORA_MONITORING_API(VisualizeClusters(this->GetPandora(), &clusterList, "HaloClusters", AUTO));
            PANDORA_MONITORING_API(ViewEvent(this->GetPandora()));
            PANDORA_MONITORING_API(DrawPandoraHistogram(this->GetPandora(), twoDHistogram_XY, "colz"));
        }
    }

    return isBeamHaloMuon;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BeamHaloMuonRemovalAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Monitoring", m_monitoring));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DisplayRetainedClusters", m_displayRetainedClusters));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DisplayRejectedClusters", m_displayRejectedClusters));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
