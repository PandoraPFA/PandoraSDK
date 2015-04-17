/**
 *  @file   PandoraSDK/src/Templates/TemplateAlgorithmTool.cc
 * 
 *  @brief  Implementation of the template algorithm tool class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "Templates/TemplateAlgorithmTool.h"

using namespace pandora;

StatusCode TemplateAlgorithmTool::Run()
{
    // Algorithm tool code here

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TemplateAlgorithmTool::ReadSettings(const TiXmlHandle /*xmlHandle*/)
{
    // Read settings from xml file here

    return STATUS_CODE_SUCCESS;
}
