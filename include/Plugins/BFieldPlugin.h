/**
 *  @file   PandoraSDK/include/Plugins/BFieldPlugin.h
 * 
 *  @brief  Header file for the bfield plugin interface class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_BFIELD_PLUGIN_H
#define PANDORA_BFIELD_PLUGIN_H 1

#include "Pandora/PandoraInternal.h"
#include "Pandora/Process.h"

namespace pandora
{

/**
 *  @brief  BFieldPlugin class
 */
class BFieldPlugin : public Process
{
public:
    /**
     *  @brief  Get the bfield value for a specified position vector
     * 
     *  @param  positionVector the specified position
     * 
     *  @return the bfield, units Tesla
     */
    virtual float GetBField(const CartesianVector &positionVector) const = 0;

protected:
    friend class PluginManager;
};

} // namespace pandora

#endif // #ifndef PANDORA_BFIELD_PLUGIN_H
