/**
 *  @file   LCContent/src/LCParticleId/MuonReconstructionAlgorithm.cc
 * 
 *  @brief  Implementation of the muon reconstruction algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCParticleId/MuonReconstructionAlgorithm.h"

using namespace pandora;

namespace lc_content
{

MuonReconstructionAlgorithm::MuonReconstructionAlgorithm() :
    m_shouldClusterIsolatedHits(false),
    m_maxClusterCaloHits(30),
    m_minClusterOccupiedLayers(8),
    m_minClusterLayerSpan(8),
    m_nClusterLayersToFit(100),
    m_maxClusterFitChi2(4.f),
    m_maxDistanceToTrack(200.f),
    m_minTrackCandidateEnergy(7.f),
    m_minHelixClusterCosAngle(0.98f),
    m_nExpectedTracksPerCluster(1),
    m_nExpectedParentTracks(1),
    m_minHelixCaloHitCosAngle(0.95f),
    m_region1GenericDistance(3.f),
    m_region2GenericDistance(6.f),
    m_isolatedMinRegion1Hits(1),
    m_isolatedMaxRegion2Hits(0),
    m_maxGenericDistance(6.f),
    m_isolatedMaxGenericDistance(3.f),
    m_replaceCurrentClusterList(false),
    m_replaceCurrentPfoList(false)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MuonReconstructionAlgorithm::Run()
{
    std::string muonClusterListName;
    const ClusterList *pMuonClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<CaloHit>(*this, m_inputMuonCaloHitListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_muonClusteringAlgName, pMuonClusterList, muonClusterListName));

    if (!pMuonClusterList->empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AssociateMuonTracks(pMuonClusterList));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AddCaloHits(pMuonClusterList));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateMuonPfos(pMuonClusterList));
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->TidyLists());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MuonReconstructionAlgorithm::AssociateMuonTracks(const ClusterList *const pMuonClusterList) const
{
    const GeometryManager *const pGeometryManager(PandoraContentApi::GetGeometry(*this));
    const BFieldPlugin *const pBFieldPlugin(PandoraContentApi::GetPlugins(*this)->GetBFieldPlugin());

    const float coilMidPointR(0.5f * (pGeometryManager->GetSubDetector(COIL).GetInnerRCoordinate() + pGeometryManager->GetSubDetector(COIL).GetOuterRCoordinate()));
    const float muonBarrelBField(pBFieldPlugin->GetBField(CartesianVector(pGeometryManager->GetSubDetector(MUON_BARREL).GetInnerRCoordinate(), 0.f, 0.f)));
    const float muonEndCapBField(pBFieldPlugin->GetBField(CartesianVector(0.f, 0.f, std::fabs(pGeometryManager->GetSubDetector(MUON_ENDCAP).GetInnerZCoordinate()))));

    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetList(*this, m_inputTrackListName, pTrackList));

    for (ClusterList::const_iterator iter = pMuonClusterList->begin(), iterEnd = pMuonClusterList->end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        // Simple cuts on cluster properties
        if (pCluster->GetNCaloHits() > m_maxClusterCaloHits)
            continue;

        if (pCluster->GetOrderedCaloHitList().size() < m_minClusterOccupiedLayers)
            continue;

        if ((pCluster->GetOuterPseudoLayer() - pCluster->GetInnerPseudoLayer() + 1) < m_minClusterLayerSpan)
            continue;

        // Get direction of the cluster
        ClusterFitResult clusterFitResult;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, ClusterFitHelper::FitStart(pCluster, m_nClusterLayersToFit, clusterFitResult));

        if (!clusterFitResult.IsFitSuccessful())
            continue;

        const CartesianVector clusterInnerCentroid(pCluster->GetCentroid(pCluster->GetInnerPseudoLayer()));
        const bool isPositiveZ(clusterInnerCentroid.GetZ() > 0.f);

        // Loop over all non-associated tracks in the current track list to find bestTrack
        Track *pBestTrack(NULL);
        float bestTrackEnergy(0.f);
        float bestDistanceToTrack(m_maxDistanceToTrack);

        for (TrackList::const_iterator iterT = pTrackList->begin(), iterTEnd = pTrackList->end(); iterT != iterTEnd; ++iterT)
        {
            Track *pTrack = *iterT;

            // Simple cuts on track properties
            if (pTrack->HasAssociatedCluster() || !pTrack->CanFormPfo())
                continue;

            if (!pTrack->GetDaughterTrackList().empty())
                continue;

            if (pTrack->GetEnergyAtDca() < m_minTrackCandidateEnergy)
                continue;

            if (pTrack->IsProjectedToEndCap() && (pTrack->GetTrackStateAtCalorimeter().GetPosition().GetZ() * clusterInnerCentroid.GetZ() < 0.f))
                continue;

            // Extract track helix fit
            const Helix *const pHelix(pTrack->GetHelixFitAtCalorimeter());

            // Compare cluster and helix directions
            CartesianVector muonEntryPoint(0.f, 0.f, 0.f);
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetMuonEntryPoint(pHelix, isPositiveZ, muonEntryPoint));

            bool isInBarrel(false);
            const float muonEntryR(std::sqrt(muonEntryPoint.GetX() * muonEntryPoint.GetX() + muonEntryPoint.GetY() * muonEntryPoint.GetY()));

            if (muonEntryR > coilMidPointR)
            {
                isInBarrel = true;
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pHelix->GetPointOnCircle(coilMidPointR, pHelix->GetReferencePoint(), muonEntryPoint));
            }

            // Create helix that can be propagated in muon system, outside central detector
            const float externalBField(isInBarrel ? muonBarrelBField : muonEndCapBField);

            const Helix externalHelix(muonEntryPoint, pHelix->GetExtrapolatedMomentum(muonEntryPoint),
                (externalBField < 0.f) ? -pHelix->GetCharge() : pHelix->GetCharge(), std::fabs(externalBField));

            CartesianVector correctedMuonEntryPoint(0.f, 0.f, 0.f);
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetMuonEntryPoint(&externalHelix, isPositiveZ, correctedMuonEntryPoint));

            const CartesianVector helixDirection(externalHelix.GetExtrapolatedMomentum(correctedMuonEntryPoint).GetUnitVector());
            const float helixClusterCosAngle(helixDirection.GetCosOpeningAngle(clusterFitResult.GetDirection()));

            if (helixClusterCosAngle < m_minHelixClusterCosAngle)
                continue;

            // Calculate separation of helix and cluster inner centroid
            CartesianVector helixSeparation(0.f, 0.f, 0.f);
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, externalHelix.GetDistanceToPoint(clusterInnerCentroid, helixSeparation));

            const float distanceToTrack(helixSeparation.GetZ());
 
            if ((distanceToTrack < bestDistanceToTrack) || ((distanceToTrack == bestDistanceToTrack) && (pTrack->GetEnergyAtDca() > bestTrackEnergy)))
            {
                pBestTrack = pTrack;
                bestDistanceToTrack = distanceToTrack;
                bestTrackEnergy = pTrack->GetEnergyAtDca();
            }
        }

        if (NULL != pBestTrack)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, pBestTrack, pCluster));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MuonReconstructionAlgorithm::GetMuonEntryPoint(const Helix *const pHelix, const bool isPositiveZ, CartesianVector &muonEntryPoint) const
{
    const GeometryManager *const pGeometryManager(PandoraContentApi::GetGeometry(*this));
    const float muonEndCapInnerZ(std::fabs(pGeometryManager->GetSubDetector(MUON_ENDCAP).GetInnerZCoordinate()));

    float minGenericTime(std::numeric_limits<float>::max());
    const CartesianVector &referencePoint(pHelix->GetReferencePoint());

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pHelix->GetPointInZ(isPositiveZ ? muonEndCapInnerZ : -muonEndCapInnerZ,
        referencePoint, muonEntryPoint, minGenericTime));

    const SubDetector &muonBarrel(pGeometryManager->GetSubDetector(MUON_BARREL));
    const unsigned int muonBarrelInnerSymmetry(muonBarrel.GetInnerSymmetryOrder());
    const float muonBarrelInnerPhi(muonBarrel.GetInnerPhiCoordinate());
    const float muonBarrelInnerR(muonBarrel.GetInnerRCoordinate());

    if (muonBarrelInnerSymmetry > 0)
    {
        const float pi(std::acos(-1.f));
        const float twopi_n = 2.f * pi / (static_cast<float>(muonBarrelInnerSymmetry));

        for (unsigned int i = 0; i < muonBarrelInnerSymmetry; ++i)
        {
            const float phi(twopi_n * static_cast<float>(i) + muonBarrelInnerPhi);

            CartesianVector barrelEntryPoint(0.f, 0.f, 0.f);
            float genericTime(std::numeric_limits<float>::max());

            const StatusCode statusCode(pHelix->GetPointInXY(muonBarrelInnerR * std::cos(phi), muonBarrelInnerR * std::sin(phi),
                 std::cos(phi + 0.5f * pi), std::sin(phi + 0.5f * pi), referencePoint, barrelEntryPoint, genericTime));

            if ((STATUS_CODE_SUCCESS == statusCode) && (genericTime < minGenericTime))
            {
                minGenericTime = genericTime;
                muonEntryPoint = barrelEntryPoint;
            }
        }
    }
    else
    {
        CartesianVector barrelEntryPoint(0.f, 0.f, 0.f);
        float genericTime(std::numeric_limits<float>::max());

        const StatusCode statusCode(pHelix->GetPointOnCircle(muonBarrelInnerR, referencePoint, barrelEntryPoint, genericTime));

        if ((STATUS_CODE_SUCCESS == statusCode) && (genericTime < minGenericTime))
        {
            minGenericTime = genericTime;
            muonEntryPoint = barrelEntryPoint;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MuonReconstructionAlgorithm::AddCaloHits(const ClusterList *const pMuonClusterList) const
{
    const GeometryManager *const pGeometryManager(PandoraContentApi::GetGeometry(*this));
    const float hCalEndCapInnerR(pGeometryManager->GetSubDetector(HCAL_ENDCAP).GetInnerRCoordinate());
    const float eCalEndCapInnerR(pGeometryManager->GetSubDetector(ECAL_ENDCAP).GetInnerRCoordinate());

    const CaloHitList *pCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetList(*this, m_inputCaloHitListName, pCaloHitList));

    OrderedCaloHitList orderedCaloHitList;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, orderedCaloHitList.Add(*pCaloHitList));

    for (ClusterList::const_iterator clusterIter = pMuonClusterList->begin(), clusterIterEnd = pMuonClusterList->end(); clusterIter != clusterIterEnd; ++clusterIter)
    {
        Cluster *pCluster = *clusterIter;

        // Check track associations
        const TrackList &trackList(pCluster->GetAssociatedTrackList());

        if (trackList.size() != m_nExpectedTracksPerCluster)
            continue;

        Track *pTrack = *(trackList.begin());
        const Helix *const pHelix(pTrack->GetHelixFitAtCalorimeter());

        for (OrderedCaloHitList::const_iterator layerIter = orderedCaloHitList.begin(), layerIterEnd = orderedCaloHitList.end(); layerIter != layerIterEnd; ++layerIter)
        {
            TrackDistanceInfoVector trackDistanceInfoVector;
            unsigned int nHitsInRegion1(0), nHitsInRegion2(0);

            for (CaloHitList::const_iterator hitIter = layerIter->second->begin(), hitIterEnd = layerIter->second->end(); hitIter != hitIterEnd; ++hitIter)
            {
                CaloHit *pCaloHit = *hitIter;

                if ((!m_shouldClusterIsolatedHits && pCaloHit->IsIsolated()) || !PandoraContentApi::IsAvailable(*this, pCaloHit))
                    continue;

                const CartesianVector &caloHitPosition(pCaloHit->GetPositionVector());
                const CartesianVector helixDirection(pHelix->GetExtrapolatedMomentum(caloHitPosition).GetUnitVector());

                if (pCaloHit->GetExpectedDirection().GetCosOpeningAngle(helixDirection) < m_minHelixCaloHitCosAngle)
                    continue;

                if (ENDCAP == pCaloHit->GetHitRegion())
                {
                    CartesianVector intersectionPoint(0.f, 0.f, 0.f);
                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pHelix->GetPointInZ(caloHitPosition.GetZ(), pHelix->GetReferencePoint(), intersectionPoint));

                    const float helixR(std::sqrt(intersectionPoint.GetX() * intersectionPoint.GetX() + intersectionPoint.GetY() * intersectionPoint.GetY()));

                    if ((HCAL == pCaloHit->GetHitType()) && (helixR < hCalEndCapInnerR))
                        continue;

                    if ((ECAL == pCaloHit->GetHitType()) && (helixR < eCalEndCapInnerR))
                        continue;
                }

                CartesianVector helixSeparation(0.f, 0.f, 0.f);
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pHelix->GetDistanceToPoint(caloHitPosition, helixSeparation));

                const float cellLengthScale(pCaloHit->GetCellLengthScale());

                if (cellLengthScale < std::numeric_limits<float>::epsilon())
                    continue;

                const float genericDistance(helixSeparation.GetMagnitude() / cellLengthScale);
                trackDistanceInfoVector.push_back(TrackDistanceInfo(pCaloHit, genericDistance));

                if (genericDistance < m_region1GenericDistance)
                {
                    ++nHitsInRegion1;
                }
                else if (genericDistance < m_region2GenericDistance)
                {
                    ++nHitsInRegion2;
                }
            }

            const bool isIsolated((nHitsInRegion1 >= m_isolatedMinRegion1Hits) && (nHitsInRegion2 <= m_isolatedMaxRegion2Hits));
            std::sort(trackDistanceInfoVector.begin(), trackDistanceInfoVector.end(), MuonReconstructionAlgorithm::SortByDistanceToTrack);

            for (TrackDistanceInfoVector::const_iterator iter = trackDistanceInfoVector.begin(), iterEnd = trackDistanceInfoVector.end(); iter != iterEnd; ++iter)
            {
                if ((iter->second > m_maxGenericDistance) || (isIsolated && (iter->second > m_isolatedMaxGenericDistance)))
                    break;

                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddToCluster(*this, pCluster, iter->first));

                if (!isIsolated)
                    break;
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MuonReconstructionAlgorithm::CreateMuonPfos(const ClusterList *const pMuonClusterList) const
{
    const PfoList *pPfoList = NULL; std::string pfoListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::CreateTemporaryListAndSetCurrent(*this, pPfoList, pfoListName));

    for (ClusterList::const_iterator iter = pMuonClusterList->begin(), iterEnd = pMuonClusterList->end(); iter != iterEnd; ++iter)
    {
        PandoraContentApi::ParticleFlowObject::Parameters pfoParameters;

        Cluster *pCluster = *iter;
        pfoParameters.m_clusterList.insert(pCluster);

        // Consider associated tracks
        const TrackList &trackList(pCluster->GetAssociatedTrackList());

        if (trackList.size() != m_nExpectedTracksPerCluster)
            continue;

        Track *pTrack = *(trackList.begin());
        pfoParameters.m_trackList.insert(pTrack);

        // Examine track relationships
        const TrackList &parentTrackList(pTrack->GetParentTrackList());

        if ((parentTrackList.size() > m_nExpectedParentTracks) || !pTrack->GetDaughterTrackList().empty() || !pTrack->GetSiblingTrackList().empty())
        {
            std::cout << "MuonReconstructionAlgorithm: invalid/unexpected track relationships for muon." << std::endl;
            continue;
        }

        if (!parentTrackList.empty())
        {
            pfoParameters.m_trackList.insert(parentTrackList.begin(), parentTrackList.end());
        }

        pfoParameters.m_charge = pTrack->GetCharge();
        pfoParameters.m_momentum = pTrack->GetMomentumAtDca();
        pfoParameters.m_particleId = (pfoParameters.m_charge.Get() > 0) ? MU_PLUS : MU_MINUS;
        pfoParameters.m_mass = PdgTable::GetParticleMass(pfoParameters.m_particleId.Get());
        pfoParameters.m_energy = std::sqrt(pfoParameters.m_mass.Get() * pfoParameters.m_mass.Get() + pfoParameters.m_momentum.Get().GetMagnitudeSquared());

        ParticleFlowObject *pPfo(NULL);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ParticleFlowObject::Create(*this, pfoParameters, pPfo));
    }

    if (!pMuonClusterList->empty() && !pPfoList->empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList<Cluster>(*this, m_outputMuonClusterListName));

        if (m_replaceCurrentClusterList)
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<Cluster>(*this, m_outputMuonClusterListName));

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList<Pfo>(*this, m_outputMuonPfoListName));

        if (m_replaceCurrentPfoList)
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<Pfo>(*this, m_outputMuonPfoListName));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MuonReconstructionAlgorithm::TidyLists() const
{
    // Make list of all tracks, clusters and calo hits in muon pfos
    TrackList pfoTrackList; CaloHitList pfoCaloHitList; ClusterList pfoClusterList;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetPfoComponents(pfoTrackList, pfoCaloHitList, pfoClusterList));

    // Save the muon-removed track list
    const TrackList *pInputTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetList(*this, m_inputTrackListName, pInputTrackList));

    TrackList outputTrackList(*pInputTrackList);

    for (TrackList::const_iterator iter = pfoTrackList.begin(), iterEnd = pfoTrackList.end(); iter != iterEnd; ++iter)
    {
        outputTrackList.erase(*iter);
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList(*this, outputTrackList, m_outputTrackListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<Track>(*this, m_replacementTrackListName));

    // Save the muon-removed calo hit list
    const CaloHitList *pInputCaloHitList = NULL;
    const CaloHitList *pInputMuonCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetList(*this, m_inputCaloHitListName, pInputCaloHitList));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetList(*this, m_inputMuonCaloHitListName, pInputMuonCaloHitList));

    CaloHitList outputCaloHitList(*pInputCaloHitList);
    CaloHitList outputMuonCaloHitList(*pInputMuonCaloHitList);

    for (CaloHitList::const_iterator iter = pfoCaloHitList.begin(), iterEnd = pfoCaloHitList.end(); iter != iterEnd; ++iter)
    {
        outputCaloHitList.erase(*iter);
        outputMuonCaloHitList.erase(*iter);
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList(*this, outputCaloHitList, m_outputCaloHitListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList(*this, outputMuonCaloHitList, m_outputMuonCaloHitListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<CaloHit>(*this, m_replacementCaloHitListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MuonReconstructionAlgorithm::GetPfoComponents(TrackList &pfoTrackList, CaloHitList &pfoCaloHitList, ClusterList &pfoClusterList) const
{
    const PfoList *pPfoList = NULL;
    const StatusCode statusCode(PandoraContentApi::GetList(*this, m_outputMuonPfoListName, pPfoList));

    if (STATUS_CODE_NOT_INITIALIZED == statusCode)
        return STATUS_CODE_SUCCESS;

    if (STATUS_CODE_SUCCESS != statusCode)
        return statusCode;

    for (PfoList::const_iterator iter = pPfoList->begin(), iterEnd = pPfoList->end(); iter != iterEnd; ++iter)
    {
        ParticleFlowObject *pPfo = *iter;
        const int particleId(pPfo->GetParticleId());

        if ((particleId != MU_MINUS) && (particleId != MU_PLUS))
            return STATUS_CODE_FAILURE;

        pfoTrackList.insert(pPfo->GetTrackList().begin(), pPfo->GetTrackList().end());
        pfoClusterList.insert(pPfo->GetClusterList().begin(), pPfo->GetClusterList().end());
    }

    for (ClusterList::const_iterator iter = pfoClusterList.begin(), iterEnd = pfoClusterList.end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;
        pCluster->GetOrderedCaloHitList().GetCaloHitList(pfoCaloHitList);
        pfoCaloHitList.insert(pCluster->GetIsolatedCaloHitList().begin(), pCluster->GetIsolatedCaloHitList().end());
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MuonReconstructionAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    // Input lists
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "InputTrackListName", m_inputTrackListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "InputCaloHitListName", m_inputCaloHitListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "InputMuonCaloHitListName", m_inputMuonCaloHitListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle,
        "MuonClusterFormation", m_muonClusteringAlgName));

    // Clustering
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldClusterIsolatedHits", m_shouldClusterIsolatedHits));

    // Cluster-track association
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxClusterCaloHits", m_maxClusterCaloHits));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinClusterOccupiedLayers", m_minClusterOccupiedLayers));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinClusterLayerSpan", m_minClusterLayerSpan));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NClusterLayersToFit", m_nClusterLayersToFit));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxClusterFitChi2", m_maxClusterFitChi2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxDistanceToTrack", m_maxDistanceToTrack));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinTrackCandidateEnergy", m_minTrackCandidateEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHelixClusterCosAngle", m_minHelixClusterCosAngle));

    // Addition of ecal/hcal hits
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NExpectedTracksPerCluster", m_nExpectedTracksPerCluster));

    if (0 == m_nExpectedTracksPerCluster)
        return STATUS_CODE_INVALID_PARAMETER;

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NExpectedParentTracks", m_nExpectedParentTracks));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHelixCaloHitCosAngle", m_minHelixCaloHitCosAngle));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Region1GenericDistance", m_region1GenericDistance));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Region2GenericDistance", m_region2GenericDistance));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "IsolatedMinRegion1Hits", m_isolatedMinRegion1Hits));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "IsolatedMaxRegion2Hits", m_isolatedMaxRegion2Hits));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxGenericDistance", m_maxGenericDistance));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "IsolatedMaxGenericDistance", m_isolatedMaxGenericDistance));

    // Output lists
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "OutputTrackListName", m_outputTrackListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "OutputCaloHitListName", m_outputCaloHitListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "OutputMuonCaloHitListName", m_outputMuonCaloHitListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "OutputMuonClusterListName", m_outputMuonClusterListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "OutputMuonPfoListName", m_outputMuonPfoListName));

    // Current list management
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "ReplacementTrackListName", m_replacementTrackListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "ReplacementCaloHitListName", m_replacementCaloHitListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "ReplaceCurrentClusterList", m_replaceCurrentClusterList));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "ReplaceCurrentPfoList", m_replaceCurrentPfoList));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
