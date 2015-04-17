/**
 *  @file   PandoraSDK/src/Templates/TemplateAlgorithm.cc
 * 
 *  @brief  Implementation of the template algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "Templates/TemplateAlgorithm.h"

using namespace pandora;

StatusCode TemplateAlgorithm::Run()
{
    // Algorithm code here

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TemplateAlgorithm::ReadSettings(const TiXmlHandle /*xmlHandle*/)
{
    // Read settings from xml file here

    return STATUS_CODE_SUCCESS;
}
