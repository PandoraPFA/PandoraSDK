/**
 *  @file   PandoraSDK/include/Utilities/PseudoLayerPlugin.h
 * 
 *  @brief  Header file for the pseudo layer plugin interface class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_PSEUDO_LAYER_PLUGIN_H
#define PANDORA_PSEUDO_LAYER_PLUGIN_H 1

#include "Pandora/PandoraInternal.h"
#include "Pandora/Process.h"

namespace pandora
{

/**
 *  @brief  PseudoLayerPlugin class
 */
class PseudoLayerPlugin : public Process
{
public:
    /**
     *  @brief  Get the appropriate pseudolayer for a specified position vector
     * 
     *  @param  positionVector the specified position
     * 
     *  @return the appropriate pseudolayer
     */
    virtual unsigned int GetPseudoLayer(const CartesianVector &positionVector) const = 0;

    /**
     *  @brief  Get the pseudolayer assigned to a point at the ip, i.e. the initial offset for pseudolayer values
     *          and the start of the pseudolayer scale
     * 
     *  @return the pseudolayer assigned to a point at the ip
     */
    virtual unsigned int GetPseudoLayerAtIp() const = 0;

protected:
    friend class PluginManager;
};

} // namespace pandora

#endif // #ifndef PANDORA_PSEUDO_LAYER_PLUGIN_H
