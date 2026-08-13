/**
 *  @file   PandoraSDK/src/Geometry/LArTPC.cc
 *
 *  @brief  Implementation of the lar tpc class.
 *
 *  $Log: $
 */

#include "Geometry/LArTPC.h"

#include <limits>

namespace pandora
{

LArTPC::LArTPC(const object_creation::Geometry::LArTPC::Parameters &inputParameters) :
    m_larTPCVolumeId(inputParameters.m_larTPCVolumeId.Get()),
    m_centerX(inputParameters.m_centerX.Get()),
    m_centerY(inputParameters.m_centerY.Get()),
    m_centerZ(inputParameters.m_centerZ.Get()),
    m_widthX(inputParameters.m_widthX.Get()),
    m_widthY(inputParameters.m_widthY.Get()),
    m_widthZ(inputParameters.m_widthZ.Get()),
    m_wirePitchU(inputParameters.m_wirePitchU.Get()),
    m_wirePitchV(inputParameters.m_wirePitchV.Get()),
    m_wirePitchW(inputParameters.m_wirePitchW.Get()),
    m_wireAngleU(inputParameters.m_wireAngleU.Get()),
    m_wireAngleV(inputParameters.m_wireAngleV.Get()),
    m_wireAngleW(inputParameters.m_wireAngleW.Get()),
    m_sigmaUVW(inputParameters.m_sigmaUVW.Get()),
    m_isDriftInPositiveX(inputParameters.m_isDriftInPositiveX.Get())
{
    for (const auto &volumeParams : inputParameters.m_readoutVolumeParametersVector)
    {
        LArReadoutUnit::ReadoutUnits readoutUnits;

        for (const auto &unitParams : volumeParams.m_readoutUnitParametersVector)
        {
            LArReadoutChannel::ReadoutChannels readoutChannels;

            for (const auto &channelParams : unitParams.m_channelParametersVector)
                readoutChannels.emplace_back(channelParams.m_id, channelParams.m_channelIntervalArray);

            readoutUnits.emplace_back(unitParams.m_id, unitParams.m_view, unitParams.m_referenceCoordinate, unitParams.m_pitch, readoutChannels);
        }

        m_readoutVolumes.emplace(volumeParams.m_id, LArReadoutVolume(volumeParams.m_id, volumeParams.m_center, volumeParams.m_size, readoutUnits));
    }

    for (const auto &[_, volume] : m_readoutVolumes)
    {
        volume.SetParent(this);
        volume.Finalize();
    }
    this->FinalizeReadoutVolumeNeighbours();
}

//------------------------------------------------------------------------------------------------------------------------------------------

LArTPC::~LArTPC()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LArTPC::FinalizeReadoutVolumeNeighbours() const
{
    auto overlaps = [](float cA, float wA, float cB, float wB)
    {
        return std::fabs(cA - cB) < 0.5f * (wA + wB);
    };

    for (const auto &entryA : m_readoutVolumes)
    {
        const LArReadoutVolume &volA(entryA.second);
        const float centerA[3]{volA.GetCenter().GetX(), volA.GetCenter().GetY(), volA.GetCenter().GetZ()};
        const float sizeA[3]{volA.GetSize().GetX(), volA.GetSize().GetY(), volA.GetSize().GetZ()};

        // For each axis where we are checking for "adjacency" check that the other two axes overlap
        for (unsigned int axis = 0; axis < 3; ++axis)
        {
            const unsigned int perp1((axis + 1) % 3), perp2((axis + 2) % 3);
            const LArReadoutVolume *pNearestPlus(nullptr), *pNearestMinus(nullptr);
            float bestPlus(std::numeric_limits<float>::max()), bestMinus(std::numeric_limits<float>::max());

            for (const auto &entryB : m_readoutVolumes)
            {
                if (entryB.first == entryA.first)
                    continue;

                const LArReadoutVolume &volB(entryB.second);
                const float centerB[3]{volB.GetCenter().GetX(), volB.GetCenter().GetY(), volB.GetCenter().GetZ()};
                const float sizeB[3]{volB.GetSize().GetX(), volB.GetSize().GetY(), volB.GetSize().GetZ()};

                if (!overlaps(centerA[perp1], sizeA[perp1], centerB[perp1], sizeB[perp1]) ||
                    !overlaps(centerA[perp2], sizeA[perp2], centerB[perp2], sizeB[perp2]))
                    continue;

                const float delta(centerB[axis] - centerA[axis]);

                if (delta > 0.f && delta < bestPlus)
                {
                    bestPlus = delta;
                    pNearestPlus  = &volB;
                }
                else if (delta < 0.f && -delta < bestMinus)
                {
                    bestMinus = -delta;
                    pNearestMinus = &volB;
                }
            }

            static const ReadoutVolumeNeighbour plusDirs[3]{ReadoutVolumeNeighbour::PLUS_X, ReadoutVolumeNeighbour::PLUS_Y, ReadoutVolumeNeighbour::PLUS_Z};
            static const ReadoutVolumeNeighbour minusDirs[3]{ReadoutVolumeNeighbour::MINUS_X, ReadoutVolumeNeighbour::MINUS_Y, ReadoutVolumeNeighbour::MINUS_Z};

            volA.SetNeighbour(plusDirs[axis], pNearestPlus);
            volA.SetNeighbour(minusDirs[axis], pNearestMinus);
        }
    }
}

} // namespace pandora
