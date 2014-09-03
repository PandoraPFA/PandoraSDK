/**
 *  @file   PandoraSDK/include/Pandora/AlgorithmHeaders.h
 * 
 *  @brief  Grouping of header files for many classes of use in particle flow algorithms
 * 
 *  $Log: $
 */
#ifndef PANDORA_ALGORITHM_HEADERS_H
#define PANDORA_ALGORITHM_HEADERS_H 1

#include "Api/PandoraContentApi.h"

#include "Helpers/ClusterFitHelper.h"
#include "Helpers/MCParticleHelper.h"
#include "Helpers/XmlHelper.h"

#include "Managers/GeometryManager.h"
#include "Managers/PluginManager.h"

#include "Objects/CaloHit.h"
#include "Objects/CartesianVector.h"
#include "Objects/Cluster.h"
#include "Objects/DetectorGap.h"
#include "Objects/Helix.h"
#include "Objects/Histograms.h"
#include "Objects/MCParticle.h"
#include "Objects/OrderedCaloHitList.h"
#include "Objects/ParticleFlowObject.h"
#include "Objects/SubDetector.h"
#include "Objects/Track.h"
#include "Objects/TrackState.h"
#include "Objects/Vertex.h"

#include "Pandora/AlgorithmTool.h"
#include "Pandora/PandoraInternal.h"
#include "Pandora/PandoraSettings.h"

#include "Plugins/BFieldPlugin.h"
#include "Plugins/EnergyCorrectionsPlugin.h"
#include "Plugins/ParticleIdPlugin.h"
#include "Plugins/PseudoLayerPlugin.h"
#include "Plugins/ShowerProfilePlugin.h"

#ifdef MONITORING
#include "PandoraMonitoringApi.h"
#endif

#include <algorithm>
#include <cmath>
#include <limits>

#endif // #ifndef PANDORA_ALGORITHM_HEADERS_H
