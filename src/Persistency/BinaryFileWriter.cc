/**
 *  @file   PandoraSDK/src/Persistency/BinaryFileWriter.cc
 * 
 *  @brief  Implementation of the file writer class.
 * 
 *  $Log: $
 */

#include "Api/PandoraContentApi.h"
#include "Api/PandoraContentApiImpl.h"

#include "Objects/CaloHit.h"
#include "Objects/DetectorGap.h"
#include "Objects/MCParticle.h"
#include "Objects/SubDetector.h"
#include "Objects/Track.h"

#include "Persistency/BinaryFileWriter.h"

namespace pandora
{

BinaryFileWriter::BinaryFileWriter(const pandora::Pandora &pandora, const std::string &fileName, const FileMode fileMode) :
    FileWriter(pandora, fileName)
{
    if (APPEND == fileMode)
    {
        m_fileStream.open(fileName.c_str(), std::ios::out | std::ios::in | std::ios::binary | std::ios::ate);

        if (!m_fileStream.is_open())
        {
            m_fileStream.open(fileName.c_str(), std::ios::out | std::ios::binary);
        }
    }
    else if (OVERWRITE == fileMode)
    {
        m_fileStream.open(fileName.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
    }
    else
    {
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }

    if (!m_fileStream.is_open() || !m_fileStream.good())
        throw StatusCodeException(STATUS_CODE_FAILURE);

    m_containerPosition = m_fileStream.tellp();
}

//------------------------------------------------------------------------------------------------------------------------------------------

BinaryFileWriter::~BinaryFileWriter()
{
    m_fileStream.close();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteHeader(const ContainerId containerId)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(PANDORA_FILE_HASH));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(containerId));

    m_containerPosition = m_fileStream.tellp();
    const std::ofstream::pos_type dummyContainerSize(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(dummyContainerSize));

    m_containerId = containerId;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteFooter()
{
    if ((EVENT != m_containerId) && (GEOMETRY != m_containerId))
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable((EVENT == m_containerId) ? EVENT_END : GEOMETRY_END));
    m_containerId = UNKNOWN_CONTAINER;

    const std::ofstream::pos_type containerSize(m_fileStream.tellp() - m_containerPosition);
    m_fileStream.seekp(m_containerPosition, std::ios::beg);

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(containerSize));
    m_fileStream.seekp(0, std::ios::end);

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    m_containerPosition = m_fileStream.tellp();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteSubDetector(const SubDetector *const pSubDetector)
{
    if (GEOMETRY != m_containerId)
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(SUB_DETECTOR));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pSubDetector->GetSubDetectorName()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pSubDetector->GetSubDetectorType()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pSubDetector->GetInnerRCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pSubDetector->GetInnerZCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pSubDetector->GetInnerPhiCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pSubDetector->GetInnerSymmetryOrder()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pSubDetector->GetOuterRCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pSubDetector->GetOuterZCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pSubDetector->GetOuterPhiCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pSubDetector->GetOuterSymmetryOrder()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pSubDetector->IsMirroredInZ()));

    const unsigned int nLayers(pSubDetector->GetNLayers());
    const SubDetector::SubDetectorLayerList &subDetectorLayerList(pSubDetector->GetSubDetectorLayerList());

    if (subDetectorLayerList.size() != nLayers)
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(nLayers));

    for (unsigned int iLayer = 0; iLayer < nLayers; ++iLayer)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(subDetectorLayerList[iLayer].GetClosestDistanceToIp()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(subDetectorLayerList[iLayer].GetNRadiationLengths()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(subDetectorLayerList[iLayer].GetNInteractionLengths()));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteDetectorGap(const DetectorGap *const pDetectorGap)
{
    if (GEOMETRY != m_containerId)
        return STATUS_CODE_FAILURE;

    const BoxGap *pBoxGap = NULL;
    pBoxGap = dynamic_cast<const BoxGap *>(pDetectorGap);

    const ConcentricGap *pConcentricGap = NULL;
    pConcentricGap = dynamic_cast<const ConcentricGap *>(pDetectorGap);

    if (NULL != pBoxGap)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(BOX_GAP));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pBoxGap->GetVertex()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pBoxGap->GetSide1()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pBoxGap->GetSide2()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pBoxGap->GetSide3()));
    }
    else if (NULL != pConcentricGap)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(CONCENTRIC_GAP));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pConcentricGap->GetMinZCoordinate()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pConcentricGap->GetMaxZCoordinate()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pConcentricGap->GetInnerRCoordinate()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pConcentricGap->GetInnerPhiCoordinate()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pConcentricGap->GetInnerSymmetryOrder()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pConcentricGap->GetOuterRCoordinate()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pConcentricGap->GetOuterPhiCoordinate()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pConcentricGap->GetOuterSymmetryOrder()));
    }
    else
    {
        return STATUS_CODE_FAILURE;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteCaloHit(const CaloHit *const pCaloHit)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    const CellGeometry cellGeometry(pCaloHit->GetCellGeometry());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(CALO_HIT));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(cellGeometry));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetPositionVector()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetExpectedDirection()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetCellNormalVector()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetCellThickness()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetNCellRadiationLengths()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetNCellInteractionLengths()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetTime()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetInputEnergy()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetMipEquivalentEnergy()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetElectromagneticEnergy()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetHadronicEnergy()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->IsDigital()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetHitType()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetHitRegion()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetLayer()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->IsInOuterSamplingLayer()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetParentCaloHitAddress()));

    if (RECTANGULAR == cellGeometry)
    {
        const RectangularCaloHit *const pRectangularCaloHit = dynamic_cast<const RectangularCaloHit *>(pCaloHit);

        if (NULL == pRectangularCaloHit)
            return STATUS_CODE_FAILURE;

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pRectangularCaloHit->GetCellSizeU()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pRectangularCaloHit->GetCellSizeV()));
    }
    else if (POINTING == cellGeometry)
    {
        const PointingCaloHit *const pPointingCaloHit = dynamic_cast<const PointingCaloHit *>(pCaloHit);

        if (NULL == pPointingCaloHit)
            return STATUS_CODE_FAILURE;

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pPointingCaloHit->GetCellSizeEta()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pPointingCaloHit->GetCellSizePhi()));
    }
    else
    {
        return STATUS_CODE_FAILURE;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteTrack(const Track *const pTrack)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(TRACK));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->GetD0()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->GetZ0()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->GetParticleId()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->GetCharge()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->GetMass()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->GetMomentumAtDca()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->GetTrackStateAtStart()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->GetTrackStateAtEnd()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->GetTrackStateAtCalorimeter()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->GetTimeAtCalorimeter()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->ReachesCalorimeter()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->IsProjectedToEndCap()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->CanFormPfo()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->CanFormClusterlessPfo()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->GetParentTrackAddress()));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteMCParticle(const MCParticle *const pMCParticle)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(MC_PARTICLE));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pMCParticle->GetEnergy()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pMCParticle->GetMomentum()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pMCParticle->GetVertex()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pMCParticle->GetEndpoint()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pMCParticle->GetParticleId()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pMCParticle->GetMCParticleType()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pMCParticle->GetUid()));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteRelationship(const RelationshipId relationshipId, const void *address1, const void *address2, const float weight)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(RELATIONSHIP));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(relationshipId));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(address1));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(address2));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(weight));

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
