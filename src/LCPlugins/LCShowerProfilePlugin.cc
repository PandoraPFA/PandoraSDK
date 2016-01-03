/**
 *  @file   LCContent/src/LCPlugins/LCShowerProfilePlugin.cc
 * 
 *  @brief  Implementation of the lc shower profile plugin class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCPlugins/LCShowerProfilePlugin.h"

using namespace pandora;

namespace lc_content
{

LCShowerProfilePlugin::LCShowerProfilePlugin() :
    m_showerStartMipFraction(0.8f),
    m_showerStartNonMipLayers(2),
    m_longProfileBinWidth(0.5f),
    m_longProfileNBins(100),
    m_longProfileMinCosAngle(0.3f),
    m_longProfileCriticalEnergy(0.08f),
    m_longProfileParameter0(1.25f),
    m_longProfileParameter1(0.5f),
    m_longProfileMaxDifference(0.1f),
    m_transProfileNBins(41),
    m_transProfilePeakThreshold(std::numeric_limits<float>::epsilon()),
    m_transProfileNearbyEnergyRatio(2.f),
    m_transProfileMaxPeaksToFind(3),
    m_transProfilePeakFindingMetric(0),
    m_transProfileMinNBinsCut(2),
    m_transProfileTrackNearbyNSlices(3),
    m_transProfileMinTrackToPeakCut(1.6),
    m_transProfileMinDisTrackMatch(1.6)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCShowerProfilePlugin::CalculateShowerStartLayer(const Cluster *const pCluster, unsigned int &showerStartLayer) const
{
    showerStartLayer = std::numeric_limits<unsigned int>::max();

    const unsigned int innerLayer(pCluster->GetInnerPseudoLayer()), outerLayer(pCluster->GetOuterPseudoLayer());
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    bool foundLastForwardLayer(false);
    unsigned int currentShowerLayers(0), lastForwardLayer(outerLayer);

    // Find two consecutive shower layers
    for (unsigned int iLayer = innerLayer; iLayer <= outerLayer; ++iLayer)
    {
        OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(iLayer);
        const bool isLayerPopulated((orderedCaloHitList.end() != iter) && !iter->second->empty());
        float mipFraction(0.f);

        if (isLayerPopulated)
        {
            unsigned int nMipHits(0);

            for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
            {
                if ((*hitIter)->IsPossibleMip())
                    nMipHits++;
            }

            mipFraction = static_cast<float>(nMipHits) / static_cast<float>(iter->second->size());
        }

        if (mipFraction - m_showerStartMipFraction > std::numeric_limits<float>::epsilon())
        {
            currentShowerLayers = 0;
        }
        else if (++currentShowerLayers >= m_showerStartNonMipLayers)
        {
            if (isLayerPopulated)
                lastForwardLayer = iLayer;

            foundLastForwardLayer = true;
            break;
        }
    }

    if (!foundLastForwardLayer)
    {
        showerStartLayer = outerLayer;
        return;
    }

    unsigned int currentMipLayers(0);
    showerStartLayer = lastForwardLayer;

    // Now go backwards to find two consecutive mip layers
    for (unsigned int iLayer = lastForwardLayer; iLayer >= innerLayer; --iLayer)
    {
        OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(iLayer);
        const bool isLayerPopulated((orderedCaloHitList.end() != iter) && !iter->second->empty());

        if (!isLayerPopulated)
            continue;

        unsigned int nMipHits(0);

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            if ((*hitIter)->IsPossibleMip())
                nMipHits++;
        }

        const float mipFraction(static_cast<float>(nMipHits) / static_cast<float>(iter->second->size()));

        if (mipFraction - m_showerStartMipFraction < std::numeric_limits<float>::epsilon())
        {
            currentMipLayers = 0;
            showerStartLayer = iLayer;
        }
        else if (++currentMipLayers >= m_showerStartNonMipLayers)
        {
            return;
        }

        // Be careful when decrementing unsigned ints
        if (0 == iLayer)
            return;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCShowerProfilePlugin::CalculateLongitudinalProfile(const Cluster *const pCluster, float &profileStart, float &profileDiscrepancy) const
{
    // 1. Construct cluster profile.
    const float clusterEnergy(pCluster->GetElectromagneticEnergy() - pCluster->GetIsolatedElectromagneticEnergy());

    if ((clusterEnergy < std::numeric_limits<float>::epsilon()) || (pCluster->GetNCaloHits() < 1))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    // Extract information from the cluster
    if (this->GetPandora().GetGeometry()->GetHitTypeGranularity(pCluster->GetInnerLayerHitType()) > FINE)
        return;

    const CartesianVector &clusterDirection(pCluster->GetFitToAllHitsResult().IsFitSuccessful() ?
        pCluster->GetFitToAllHitsResult().GetDirection() : pCluster->GetInitialDirection());

    // Examine layers to construct profile
    FloatVector profile(m_longProfileNBins, 0.f);
    float eCalEnergy(0.f), nRadiationLengths(0.f), nRadiationLengthsInLastLayer(0.f);

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());
    const unsigned int innerPseudoLayer(pCluster->GetInnerPseudoLayer());
    const unsigned int firstPseudoLayer(this->GetPandora().GetPlugins()->GetPseudoLayerPlugin()->GetPseudoLayerAtIp());

    for (unsigned int iLayer = innerPseudoLayer, outerPseudoLayer = pCluster->GetOuterPseudoLayer(); iLayer <= outerPseudoLayer; ++iLayer)
    {
        OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(iLayer);

        if ((orderedCaloHitList.end() == iter) || (iter->second->empty()))
        {
            nRadiationLengths += nRadiationLengthsInLastLayer;
            continue;
        }

        // Extract information from calo hits
        bool isFineGranularity(true);
        float energyInLayer(0.f), nRadiationLengthsInLayer(0.f);

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            if (this->GetPandora().GetGeometry()->GetHitTypeGranularity((*hitIter)->GetHitType()) > FINE)
            {
                isFineGranularity = false;
                break;
            }

            energyInLayer += (*hitIter)->GetElectromagneticEnergy();
            const float cosOpeningAngle(std::fabs((*hitIter)->GetCellNormalVector().GetCosOpeningAngle(clusterDirection)));
            nRadiationLengthsInLayer += (*hitIter)->GetNCellRadiationLengths() / std::max(cosOpeningAngle, m_longProfileMinCosAngle);
        }

        if (!isFineGranularity)
            break;

        eCalEnergy += energyInLayer;
        nRadiationLengthsInLayer /= static_cast<float>(iter->second->size());
        nRadiationLengthsInLastLayer = nRadiationLengthsInLayer;
        nRadiationLengths += nRadiationLengthsInLayer;

        // Account for layers before start of cluster
        if (innerPseudoLayer == iLayer)
            nRadiationLengths *= static_cast<float>(innerPseudoLayer + 1 - firstPseudoLayer);

        // Finally, create the profile
        const float endPosition(nRadiationLengths / m_longProfileBinWidth);
        const unsigned int endBin(std::min(static_cast<unsigned int>(endPosition), m_longProfileNBins - 1));

        const float deltaPosition(nRadiationLengthsInLayer / m_longProfileBinWidth);

        const float startPosition(endPosition - deltaPosition);
        const unsigned int startBin(static_cast<unsigned int>(startPosition));

        for (unsigned int iBin = startBin; iBin <= endBin; ++iBin)
        {
            float delta(1.f);

            if (startBin == iBin)
            {
                delta -= startPosition - startBin;
            }
            else if (endBin == iBin)
            {
                delta -= 1.f - endPosition + endBin;
            }

            profile[iBin] += energyInLayer * (delta / deltaPosition);
        }
    }

    const unsigned int profileEndBin(std::min(static_cast<unsigned int>(nRadiationLengths / m_longProfileBinWidth), m_longProfileNBins));

    if ((0 == profileEndBin) || (eCalEnergy < std::numeric_limits<float>::epsilon()))
        throw StatusCodeException(STATUS_CODE_FAILURE);

    // 2. Construct expected cluster profile
    const double a(m_longProfileParameter0 + m_longProfileParameter1 * std::log(clusterEnergy / m_longProfileCriticalEnergy));
    const double gammaA(std::exp(lgamma(a)));

    float t(0.f);
    FloatVector expectedProfile;
    for (unsigned int iBin = 0; iBin < m_longProfileNBins; ++iBin)
    {
        t += m_longProfileBinWidth;
        expectedProfile.push_back(static_cast<float>(clusterEnergy / 2. * std::pow(t / 2.f, static_cast<float>(a - 1.)) *
            std::exp(-t / 2.) * m_longProfileBinWidth / gammaA));
    }

    // 3. Compare the cluster profile with the expected profile
    unsigned int binOffsetAtMinDifference(0);
    float minProfileDifference(std::numeric_limits<float>::max());

    for (unsigned int iBinOffset = 0; iBinOffset < profileEndBin; ++iBinOffset)
    {
        float profileDifference(0.);

        for (unsigned int iBin = 0; iBin < profileEndBin; ++iBin)
        {
            if (iBin < iBinOffset)
            {
                profileDifference += profile[iBin];
            }
            else
            {
                profileDifference += std::fabs(expectedProfile[iBin - iBinOffset] - profile[iBin]);
            }
        }

        if (profileDifference < minProfileDifference)
        {
            minProfileDifference = profileDifference;
            binOffsetAtMinDifference = iBinOffset;
        }

        if (profileDifference - minProfileDifference > m_longProfileMaxDifference)
            break;
    }

    profileStart =  binOffsetAtMinDifference * m_longProfileBinWidth;
    profileDiscrepancy = minProfileDifference / eCalEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCShowerProfilePlugin::CalculateTracklessTransverseProfile(const Cluster *const pCluster, const unsigned int maxPseudoLayer, ShowerPeakList &showerPeakList) const
{
    // ATTN could combine trackless and tracked approach
    TwoDShowerProfile showerProfile;
    ShowerPeakObjectVector showerPeakObjectVector;
    this->CalculateTracklessTransverseShowers(pCluster, maxPseudoLayer, showerProfile, showerPeakObjectVector);
    ShowerPeakList showerPeakListCharge;
    this->ConvertBinsToShowerLists(showerProfile, showerPeakObjectVector, showerPeakList, showerPeakListCharge);
    return;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCShowerProfilePlugin::CalculateTrackNearbyTransverseProfile(const Cluster *const pCluster, const unsigned int maxPseudoLayer, const Track *const pMinTrack, 
    const TrackVector &trackVector, ShowerPeakList &showerPeakListPhoton, ShowerPeakList &showerPeakListCharge) const
{
    // TODO convert maxPseudoLayer into radiation length. Then decide how many slices to consider
    if (m_transProfileTrackNearbyNSlices > maxPseudoLayer || m_transProfileTrackNearbyNSlices == 0)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    const unsigned int pseudoLayerPerSlice(maxPseudoLayer / m_transProfileTrackNearbyNSlices);
    
    // process first slice
    TwoDShowerProfile showerProfileFirst;
    ShowerPeakObjectVector showerPeakObjectVectorFirst;    
    TwoDBinVector trackProjectionVector;
    this->CalculateTrackNearbyTransverseShowers(pCluster, pseudoLayerPerSlice, pMinTrack, trackVector, showerProfileFirst, showerPeakObjectVectorFirst, trackProjectionVector);
    if (m_transProfileTrackNearbyNSlices > 1)
    {
        for (unsigned int nIter = 1, nIterEnd = m_transProfileTrackNearbyNSlices; nIter != nIterEnd; ++nIter)
        {
            if (!this->HasPhotonCandidate(showerPeakObjectVectorFirst))
            {
                return;
            }
            const unsigned int pseudoLayerEnd(nIter == m_transProfileTrackNearbyNSlices - 1 ? maxPseudoLayer : pseudoLayerPerSlice * (nIter + 1));
            
            TwoDShowerProfile showerProfileNext;
            ShowerPeakObjectVector showerPeakObjectVectorNext;
            this->CalculateTracklessTransverseShowers(pCluster, pseudoLayerEnd, showerProfileNext, showerPeakObjectVectorNext);
            this->MarkPeaksCloseToTracks(trackProjectionVector, showerPeakObjectVectorNext);
            this->MatchPeaksInTwoSlices(showerPeakObjectVectorFirst, showerPeakObjectVectorNext);
            showerProfileFirst = showerProfileNext;
            showerPeakObjectVectorFirst = showerPeakObjectVectorNext;
        }
    }
    this->ConvertBinsToShowerLists(showerProfileFirst, showerPeakObjectVectorFirst, showerPeakListPhoton, showerPeakListCharge);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCShowerProfilePlugin::CalculateTracklessTransverseShowers(const Cluster *const pCluster, const unsigned int maxPseudoLayer, TwoDShowerProfile &showerProfile, 
    ShowerPeakObjectVector &showerPeakObjectVector) const
{
    this->InitialiseTransverseProfile(pCluster, maxPseudoLayer, showerProfile);
    this->ProcessShowerProfile(showerProfile, showerPeakObjectVector);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCShowerProfilePlugin::CalculateTrackNearbyTransverseShowers(const Cluster *const pCluster, const unsigned int maxPseudoLayer, const Track *const pMinTrack, 
    const TrackVector &trackVector, TwoDShowerProfile &showerProfile, ShowerPeakObjectVector &showerPeakObjectVector, TwoDBinVector &trackProjectionVector) const
{
    this->InitialiseTransverseProfileWithTracks(pCluster, maxPseudoLayer, pMinTrack, trackVector, showerProfile, trackProjectionVector);
    this->ProcessShowerProfile(showerProfile, showerPeakObjectVector);
    this->MarkPeaksCloseToTracks(trackProjectionVector, showerPeakObjectVector);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCShowerProfilePlugin::CreateEmptyTwoDShowerProfile(TwoDShowerProfile &twoDShowerProfile) const
{
    twoDShowerProfile = TwoDShowerProfile(m_transProfileNBins, ShowerProfile(m_transProfileNBins, ShowerProfileEntry()));
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCShowerProfilePlugin::InitialiseTransverseProfile(const pandora::Cluster *const pCluster, const unsigned int maxPseudoLayer, TwoDShowerProfile &showerProfile) const
{
    CartesianVector innerLayerCentroid(0.f, 0.f, 0.f), uAxis(0.f, 0.f, 0.f), vAxis(0.f, 0.f, 0.f);
    this->CalculateProjectionAxes(pCluster, innerLayerCentroid, uAxis, vAxis);    
    this->InitialiseTwoDShowerProfile(pCluster, maxPseudoLayer, innerLayerCentroid, uAxis, vAxis, showerProfile);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCShowerProfilePlugin::InitialiseTransverseProfileWithTracks(const pandora::Cluster *const pCluster, const unsigned int maxPseudoLayer, const Track *const pMinTrack,
    const TrackVector &trackVector, TwoDShowerProfile &showerProfile, TwoDBinVector &trackProjectionVector) const
{
    CartesianVector innerLayerCentroid(0.f, 0.f, 0.f), uAxis(0.f, 0.f, 0.f), vAxis(0.f, 0.f, 0.f);
    this->CalculateProjectionAxes(pCluster, innerLayerCentroid, uAxis, vAxis, pMinTrack);    
    this->FindTracksProjection(pCluster, trackVector, innerLayerCentroid, uAxis, vAxis, trackProjectionVector);
    this->InitialiseTwoDShowerProfile(pCluster, maxPseudoLayer, innerLayerCentroid, uAxis, vAxis, showerProfile);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCShowerProfilePlugin::CalculateProjectionAxes(const Cluster *const pCluster, CartesianVector &innerLayerCentroid, CartesianVector &uAxis, 
    CartesianVector &vAxis, const Track *const pMinTrack) const
{
    innerLayerCentroid = pMinTrack ? (pMinTrack->GetTrackStateAtCalorimeter()).GetPosition() : pCluster->GetCentroid(pCluster->GetInnerPseudoLayer());
    if ( innerLayerCentroid.GetMagnitudeSquared() < std::numeric_limits<float>::epsilon())
    {
        throw StatusCodeException(STATUS_CODE_FAILURE);
    }
    // ATTN, innerLayerCentroid could be in Z direction only; choose x direction
    if (std::fabs(innerLayerCentroid.GetX())< std::numeric_limits<float>::epsilon() && std::fabs(innerLayerCentroid.GetY()) < std::numeric_limits<float>::epsilon())
    {
        uAxis.SetValues(1.f, 0.f, 0.f);
    }
    else
    {
        uAxis = (CartesianVector(innerLayerCentroid.GetY(), -innerLayerCentroid.GetX(), 0.f).GetUnitVector());
    }    
    vAxis = (uAxis.GetCrossProduct(innerLayerCentroid).GetUnitVector());
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCShowerProfilePlugin::InitialiseTwoDShowerProfile(const Cluster *const pCluster, const unsigned int maxPseudoLayer, const CartesianVector &innerLayerCentroid,
    const CartesianVector &uAxis, const CartesianVector &vAxis, TwoDShowerProfile &showerProfile) const
{
    this->CreateEmptyTwoDShowerProfile(showerProfile);
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());
    const int nOffsetBins(m_transProfileNBins / 2);
    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        if (iter->first > maxPseudoLayer)
            break;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const CaloHit *const pCaloHit = *hitIter;
            const float cellLengthScale(pCaloHit->GetCellLengthScale());

            if (cellLengthScale < std::numeric_limits<float>::epsilon())
                throw StatusCodeException(STATUS_CODE_FAILURE);

            int uBin(0), vBin(0);
            this->FindHitPositionProjection(pCaloHit->GetPositionVector(), innerLayerCentroid, uAxis, vAxis, nOffsetBins, cellLengthScale, uBin, vBin);
            if ((uBin >= 0) && (vBin >= 0) && (uBin < m_transProfileNBins) && (vBin < m_transProfileNBins))
            {
                showerProfile[uBin][vBin].m_energy += pCaloHit->GetElectromagneticEnergy();
                showerProfile[uBin][vBin].m_caloHitList.insert(pCaloHit);
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCShowerProfilePlugin::FindTracksProjection(const Cluster *const pCluster, const TrackVector &trackVector, const CartesianVector &innerLayerCentroid, 
    const CartesianVector &uAxis, const CartesianVector &vAxis, TwoDBinVector &trackProjectionVector) const
{
    // ATTN it is not robust against step change in ECAL
    bool setFlag(false);
    float cellLengthScale(0.f);
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());
    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const CaloHit *const pCaloHit = *hitIter;
            cellLengthScale= (pCaloHit->GetCellLengthScale());
            if (cellLengthScale < std::numeric_limits<float>::epsilon())
                throw StatusCodeException(STATUS_CODE_FAILURE);
            setFlag = true;
            break;
        }
        if (setFlag) break;
    }
    
    const int nOffsetBins(m_transProfileNBins / 2);
    for (TrackVector::const_iterator iter = trackVector.begin(), iterEnd = trackVector.end(); iter != iterEnd; ++iter)
    {
        const Track * pTrack = *iter;
        if (!pTrack->CanFormClusterlessPfo() && !pTrack->CanFormPfo()) continue;
        if (!(pTrack->GetDaughterTrackList()).empty()) continue;
        
        int uBin(0), vBin(0);
        this->FindHitPositionProjection((pTrack->GetTrackStateAtCalorimeter()).GetPosition(), innerLayerCentroid, uAxis, vAxis, nOffsetBins, cellLengthScale, uBin, vBin);
        if ((uBin >= 0) && (vBin >= 0) && (uBin < m_transProfileNBins) && (vBin < m_transProfileNBins))
        {
            trackProjectionVector.push_back(std::make_pair(uBin,vBin));
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCShowerProfilePlugin::MaskLowHeightRegions(TwoDShowerProfile &showerProfile) const
{
    for (int uBin = 0; uBin < m_transProfileNBins; ++uBin)
    {
        for (int vBin = 0; vBin < m_transProfileNBins; ++vBin)
        {
            if (showerProfile[uBin][vBin].m_energy < m_transProfilePeakThreshold)
            {
                showerProfile[uBin][vBin].m_isAvailable = false;
                showerProfile[uBin][vBin].m_potentialPeak = false;
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCShowerProfilePlugin::FindRawPeaksInTwoDShowerProfile(TwoDShowerProfile &showerProfile, ShowerPeakObjectVector &showerPeakObjectVector) const
{
    for (int uBin = 0; uBin < m_transProfileNBins; ++uBin)
    {
        for (int vBin = 0; vBin < m_transProfileNBins; ++vBin)
        {
            if (!showerProfile[uBin][vBin].m_isAvailable)
                continue;
            if (showerProfile[uBin][vBin].m_potentialPeak && this->IsPeak(showerProfile,uBin,vBin))
            {
                ShowerPeakObject showerPeakObject(showerProfile[uBin][vBin].m_energy, uBin, vBin);
                showerPeakObjectVector.push_back(showerPeakObject);
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCShowerProfilePlugin::AssociateBinsToPeaks(const TwoDShowerProfile &showerProfile, ShowerPeakObjectVector &showerPeakObjectVector) const
{
    for (int uBin = 0; uBin < m_transProfileNBins; ++uBin)
    {
        for (int vBin = 0; vBin < m_transProfileNBins; ++vBin)
        {
            if (!showerProfile[uBin][vBin].m_isAvailable)
                continue;
            
            ShowerPeakObject  * bestShowerPeakObject = NULL;
            this->CalculateBestPeakUsingMetric(showerPeakObjectVector, uBin, vBin, bestShowerPeakObject);
            if (bestShowerPeakObject)
            {
                bestShowerPeakObject->m_associatedBins.push_back(std::make_pair(uBin,vBin));
            }
        }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------

void LCShowerProfilePlugin::AssociateBinsToPeaks(const TwoDBinVector &twoDBinVector, ShowerPeakObjectVector &showerPeakObjectVector) const
{
    for (TwoDBinVector::const_iterator iter = twoDBinVector.begin(), iterEnd = twoDBinVector.end(); iter !=iterEnd; ++iter)
    {
        const TwoDBin twoDBin(*iter);
        ShowerPeakObject  * bestShowerPeakObject = NULL;
        this->CalculateBestPeakUsingMetric(showerPeakObjectVector, twoDBin.first, twoDBin.second, bestShowerPeakObject);
        if (bestShowerPeakObject)
        {
            bestShowerPeakObject->m_associatedBins.push_back(twoDBin);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCShowerProfilePlugin::ApplyQualityCutPeakNBin(ShowerPeakObjectVector &showerPeakObjectVector, TwoDBinVector &twoDBinVector) const
{
    for (ShowerPeakObjectVector::iterator iter = showerPeakObjectVector.begin(), iterEnd = showerPeakObjectVector.end(); iter !=iterEnd; ++iter)
    {
        ShowerPeakObject &showerPeakObject(*iter);
        if (!this->PassQualityCutPeakNBin(showerPeakObject))
        {
            twoDBinVector.insert (twoDBinVector.end(), showerPeakObject.m_associatedBins.begin(), showerPeakObject.m_associatedBins.end());
            showerPeakObject.m_isAvailable = false;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool LCShowerProfilePlugin::PassQualityCutPeakNBin(const ShowerPeakObject &showerPeakObject) const
{
    return (showerPeakObject.m_associatedBins.size() > m_transProfileMinNBinsCut);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCShowerProfilePlugin::MarkPeaksCloseToTracks(const TwoDBinVector &trackProjectionVector, ShowerPeakObjectVector &showerPeakObjectVector) const
{
    for (ShowerPeakObjectVector::iterator iter = showerPeakObjectVector.begin(), iterEnd = showerPeakObjectVector.end(); iter !=iterEnd; ++iter)
    {
        ShowerPeakObject &showerPeakObject(*iter);
        if (!showerPeakObject.m_isAvailable)
            continue;
        
        const float minTrackToPeakCut2(m_transProfileMinTrackToPeakCut * m_transProfileMinTrackToPeakCut);
        for (TwoDBinVector::const_iterator tIter = trackProjectionVector.begin(), tIterEnd = trackProjectionVector.end(); tIter!= tIterEnd; ++tIter )
        {
            const TwoDBin  &trackTwoDBin(*tIter);
            const int uBinDistance(showerPeakObject.GetPeakUBin() - trackTwoDBin.first);
            const int vBinDistance(showerPeakObject.GetPeakVBin() - trackTwoDBin.second);
            const int distance2(uBinDistance * uBinDistance + vBinDistance * vBinDistance);
            if (static_cast<float>(distance2) < minTrackToPeakCut2)
            {
                showerPeakObject.m_isPhotonCandidate = false;
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCShowerProfilePlugin::MatchPeaksInTwoSlices(const ShowerPeakObjectVector &showerPeakObjectVectorFirst, ShowerPeakObjectVector &showerPeakObjectVectorNext) const
{
    for (ShowerPeakObjectVector::iterator iter = showerPeakObjectVectorNext.begin(), iterEnd = showerPeakObjectVectorNext.end(); iter !=iterEnd; ++iter)
    {
        ShowerPeakObject &showerPeakObjectNext(*iter);
        if (!showerPeakObjectNext.m_isAvailable || !showerPeakObjectNext.m_isPhotonCandidate)
            continue;
        
        showerPeakObjectNext.m_isPhotonCandidate = false;
        const float minDisTrackMatch2(m_transProfileMinDisTrackMatch * m_transProfileMinDisTrackMatch);
        for (ShowerPeakObjectVector::const_iterator jIter = showerPeakObjectVectorFirst.begin(), jIterEnd = showerPeakObjectVectorFirst.end(); jIter !=jIterEnd; ++jIter)
        {
            const ShowerPeakObject &showerPeakObjectFirst(*jIter);
            if (!showerPeakObjectFirst.m_isAvailable || !showerPeakObjectFirst.m_isPhotonCandidate)
                continue;
            
            const int uBinDistance(showerPeakObjectFirst.GetPeakUBin() - showerPeakObjectNext.GetPeakUBin());
            const int vBinDistance(showerPeakObjectFirst.GetPeakVBin() - showerPeakObjectNext.GetPeakVBin());
            const int distance2(uBinDistance * uBinDistance + vBinDistance * vBinDistance);
            if (distance2 < minDisTrackMatch2 && (showerPeakObjectNext.GetPeakEnergy() > showerPeakObjectFirst.GetPeakEnergy() || showerPeakObjectNext.GetPeakEnergy() == showerPeakObjectFirst.GetPeakEnergy()) )
            {
                showerPeakObjectNext.m_isPhotonCandidate = true;
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCShowerProfilePlugin::ProcessShowerProfile(TwoDShowerProfile &showerProfile, ShowerPeakObjectVector &showerPeakObjectVector) const
{
    this->MaskLowHeightRegions(showerProfile);
    this->FindRawPeaksInTwoDShowerProfile(showerProfile, showerPeakObjectVector);
    this->AssociateBinsToPeaks(showerProfile, showerPeakObjectVector);
    TwoDBinVector twoDBinVector;
    this->ApplyQualityCutPeakNBin(showerPeakObjectVector, twoDBinVector);
    this->AssociateBinsToPeaks(twoDBinVector, showerPeakObjectVector);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCShowerProfilePlugin::FindHitPositionProjection(const CartesianVector &hitPosition, const CartesianVector &innerLayerCentroid, const CartesianVector &uAxis, 
    const CartesianVector &vAxis, const int nOffsetBins, const int cellLengthScale, int &uBin, int &vBin) const
{
    const CartesianVector hitCoordinates((hitPosition - innerLayerCentroid) * (1.f / cellLengthScale));
    const float uValue(hitCoordinates.GetDotProduct(uAxis));
    const float vValue(hitCoordinates.GetDotProduct(vAxis));
    uBin = static_cast<int>(uValue + 0.5f + nOffsetBins);
    vBin = static_cast<int>(vValue + 0.5f + nOffsetBins);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCShowerProfilePlugin::ConvertBinsToShowerLists(const TwoDShowerProfile &showerProfile, const ShowerPeakObjectVector &showerPeakObjectVector, 
   ShowerPeakList &showerPeakListPhoton, ShowerPeakList &showerPeakListCharge) const
{
    //const int nOffsetBins(m_transProfileNBins / 2);
    for (ShowerPeakObjectVector::const_iterator iter = showerPeakObjectVector.begin(), iterEnd = showerPeakObjectVector.end(); iter !=iterEnd; ++iter)
    {
        const ShowerPeakObject  &showerPeakObject(*iter);
        if (!showerPeakObject.m_isAvailable)
            continue;
        
        float peakTotalEnergy(0.f), uBar(0.f), vBar(0.f), uuBar(0.f), vvBar(0.f);
        CaloHitList caloHitList;
        for (TwoDBinVector::const_iterator bIter = showerPeakObject.m_associatedBins.begin(), bIterEnd = showerPeakObject.m_associatedBins.end(); bIter!= bIterEnd; ++bIter )
        {
            const TwoDBin  &twoDBin(*bIter);
            const ShowerProfileEntry &showerProfileEntry(showerProfile[twoDBin.first][twoDBin.second]);
            const float energy(showerProfileEntry.m_energy);
            peakTotalEnergy += energy;
            const float uBinDifference(twoDBin.first - showerPeakObject.GetPeakUBin());
            const float vBinDifference(twoDBin.second - showerPeakObject.GetPeakVBin());
            uBar += uBinDifference * energy;
            vBar += vBinDifference * energy;
            uuBar += uBinDifference * uBinDifference * energy;
            vvBar += vBinDifference * vBinDifference * energy;
            caloHitList.insert(showerProfileEntry.m_caloHitList.begin(), showerProfileEntry.m_caloHitList.end());
        }
        if (peakTotalEnergy < std::numeric_limits<float>::epsilon())
            throw StatusCodeException(STATUS_CODE_FAILURE);
            
        uBar /= peakTotalEnergy;
        vBar /= peakTotalEnergy;
        uuBar /= peakTotalEnergy;
        vvBar /= peakTotalEnergy;
        const float meanSquareDeviation(uuBar + vvBar - uBar * uBar - vBar * vBar);
        const float peakRms((meanSquareDeviation > 0.f) ? std::sqrt(meanSquareDeviation) : std::numeric_limits<float>::max());
        
        const float meanSquareDeviationU(uuBar  - uBar * uBar );
        const float peakRmsU((meanSquareDeviationU > 0.f) ? std::sqrt(meanSquareDeviationU) : std::numeric_limits<float>::max());
        const float meanSquareDeviationV(vvBar  - vBar * vBar );
        const float peakRmsV((meanSquareDeviationV > 0.f) ? std::sqrt(meanSquareDeviationV) : std::numeric_limits<float>::max());
        const float rmsRatio(peakRmsU > peakRmsV ? peakRmsU / peakRmsV : peakRmsV / peakRmsU);
        
        if (showerPeakObject.m_isPhotonCandidate)
        {
            showerPeakListPhoton.push_back(ShowerPeak(peakTotalEnergy, peakRms, rmsRatio, caloHitList));
        }
        else
        {
            showerPeakListCharge.push_back(ShowerPeak(peakTotalEnergy, peakRms, rmsRatio, caloHitList));
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool LCShowerProfilePlugin::IsPeak(TwoDShowerProfile &showerProfile, const int uBin, const int vBin ) const
{
    return ( (uBin<=1 || uBin >= m_transProfileNBins-2 || vBin<= 1 || vBin >= m_transProfileNBins-2) ? this->Check8NeighbourFull(showerProfile,uBin,vBin) : 
        this->Check8NeighbourFast(showerProfile,uBin,vBin) ); 
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool LCShowerProfilePlugin::Check8NeighbourFull(TwoDShowerProfile &showerProfile, const int uBin, const int vBin) const
{
    for (int uBinStart = uBin - 1, uBinEnd = uBin + 1; uBinStart <= uBinEnd; ++uBinStart)
    {
        for (int vBinStart = vBin - 1, vBinEnd = vBin + 1; vBinStart <= vBinEnd; ++vBinStart)
        {
            if (uBinStart == uBin && vBinStart == vBin)
                continue;
            if (uBinStart < 0 || uBinStart > m_transProfileNBins - 1 || vBinStart < 0 || vBinStart > m_transProfileNBins - 1)
                continue;
            if (!showerProfile[uBinStart][vBinStart].m_isAvailable)
                continue;
            
            if (showerProfile[uBinStart][vBinStart].m_energy >showerProfile[uBin][vBin].m_energy)
            {
                showerProfile[uBin][vBin].m_potentialPeak = false;
                return false;
            }
            else
            {
                showerProfile[uBinStart][vBinStart].m_potentialPeak = false;
            }
        }
    }
    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool LCShowerProfilePlugin::Check8NeighbourFast(TwoDShowerProfile &showerProfile, const int uBin, const int vBin) const
{
    for (int uBinStart = uBin - 1, uBinEnd = uBin + 1; uBinStart <= uBinEnd; ++uBinStart)
    {
        for (int vBinStart = vBin - 1, vBinEnd = vBin + 1; vBinStart <= vBinEnd; ++vBinStart)
        {
            if (uBinStart == uBin && vBinStart == vBin)
                continue;
            if (!showerProfile[uBinStart][vBinStart].m_isAvailable)
                continue;
                
            if (showerProfile[uBinStart][vBinStart].m_energy >showerProfile[uBin][vBin].m_energy)
            {
                showerProfile[uBin][vBin].m_potentialPeak = false;
                return false;
            }
            else
            {
                showerProfile[uBinStart][vBinStart].m_potentialPeak = false;
            }
        }
    }
    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float LCShowerProfilePlugin::CalculatePeakFindingMetric(const float distance, const float energy)  const
{
    if (energy < std::numeric_limits<float>::epsilon())
        throw StatusCodeException(STATUS_CODE_FAILURE);
    switch (m_transProfilePeakFindingMetric)
    {
        case 0:
            return distance / std::sqrt(energy);
        case 1:
            return distance;
        case 2:
            return distance / energy;
        case 3:
            return distance / energy / energy;
        default: 
            std::cout << "LCShowerProfilePlugin::CalculatePeakFindingMetric invalid parameter for m_transProfilePeakFindingMetric: " << m_transProfilePeakFindingMetric << std::endl;
            throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCShowerProfilePlugin::CalculateBestPeakUsingMetric(ShowerPeakObjectVector &showerPeakObjectVector, const int uBin, const int vBin, 
    ShowerPeakObject  *&bestShowerPeakObject) const
{
    float minMetric(std::numeric_limits<float>::max());
    for (ShowerPeakObjectVector::iterator iter = showerPeakObjectVector.begin(), iterEnd = showerPeakObjectVector.end(); iter !=iterEnd; ++iter)
    {
        ShowerPeakObject  *const showerPeakObject(&*iter);
        if (!showerPeakObject->m_isAvailable)
            continue;
            
        if (uBin == showerPeakObject->GetPeakUBin() && vBin == showerPeakObject->GetPeakVBin())
        {
            // the candidate is a peak; Ignore it
            bestShowerPeakObject = NULL;
            return;
        }
        
        const int distance2((uBin-showerPeakObject->GetPeakUBin())*(uBin-showerPeakObject->GetPeakUBin()) + (vBin-showerPeakObject->GetPeakVBin())*(vBin-showerPeakObject->GetPeakVBin()));
        const float metric(this->CalculatePeakFindingMetric(std::sqrt(static_cast<float>(distance2)), showerPeakObject->GetPeakEnergy()));
        if( metric<minMetric )
        {
            minMetric = metric;
            bestShowerPeakObject = showerPeakObject;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool LCShowerProfilePlugin::HasPhotonCandidate(const ShowerPeakObjectVector &showerPeakObjectVector) const
{
    for (ShowerPeakObjectVector::const_iterator iter = showerPeakObjectVector.begin(), iterEnd = showerPeakObjectVector.end(); iter !=iterEnd; ++iter)
    {
        const ShowerPeakObject  &showerPeakObject(*iter);
        if (showerPeakObject.m_isAvailable && showerPeakObject.m_isPhotonCandidate)
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode LCShowerProfilePlugin::ReadSettings(const TiXmlHandle xmlHandle)
{
PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                    "ShowerStartMipFraction", m_showerStartMipFraction));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                    "ShowerStartNonMipLayers", m_showerStartNonMipLayers));

    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                   "LongProfileBinWidth", m_longProfileBinWidth));

    if (m_longProfileBinWidth < std::numeric_limits<float>::epsilon())
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                   "LongProfileNBins", m_longProfileNBins));

    if (0 == m_longProfileNBins)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                   "LongProfileMinCosAngle", m_longProfileMinCosAngle));

    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                   "LongProfileCriticalEnergy", m_longProfileCriticalEnergy));

    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                   "LongProfileParameter0", m_longProfileParameter0));

    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                   "LongProfileParameter1", m_longProfileParameter1));

    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                   "LongProfileMaxDifference", m_longProfileMaxDifference));

    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                   "TransProfileNBins", m_transProfileNBins));

    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                   "TransProfilePeakThreshold", m_transProfilePeakThreshold));

    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                   "TransProfileNearbyEnergyRatio", m_transProfileNearbyEnergyRatio));

    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                   "TransProfileMaxPeaksToFind", m_transProfileMaxPeaksToFind));
                                   
    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                   "TransProfilePeakFindingMetric", m_transProfilePeakFindingMetric));
                                
    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                   "TransProfileMinNBinsCut", m_transProfileMinNBinsCut));

    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                   "TransProfileTrackNearbyNSlices", m_transProfileTrackNearbyNSlices));

    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                   "TransProfileMinTrackToPeakCut", m_transProfileMinTrackToPeakCut));
                                   
    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                   "TransProfileMinDisTrackMatch", m_transProfileMinDisTrackMatch));
                                   
    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
