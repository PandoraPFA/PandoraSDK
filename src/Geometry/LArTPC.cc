/**
 *  @file   PandoraSDK/src/Geometry/LArTPC.cc
 *
 *  @brief  Implementation of the lar tpc class.
 *
 *  $Log: $
 */

#include "Geometry/LArTPC.h"

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

            readoutUnits.emplace_back(unitParams.m_id, unitParams.m_view, readoutChannels);
        }

        m_readoutVolumes.emplace(volumeParams.m_id, LArReadoutVolume(volumeParams.m_id, volumeParams.m_center, volumeParams.m_size, readoutUnits));
    }

    for (const auto &[_, volume] : m_readoutVolumes)
    {
        volume.SetParent(this);
        volume.Finalize();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

LArTPC::~LArTPC()
{
}

} // namespace pandora
