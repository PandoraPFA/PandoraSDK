/**
 *  @file   LCContent/include/LCPlugins/LCPseudoLayerPlugin.h
 * 
 *  @brief  Header file for the lc pseudo layer plugin class.
 * 
 *  $Log: $
 */
#ifndef LC_PSEUDO_LAYER_PLUGIN_H
#define LC_PSEUDO_LAYER_PLUGIN_H 1

#include "Pandora/PandoraInputTypes.h"

#include "Plugins/PseudoLayerPlugin.h"

namespace lc_content
{

/**
 *  @brief  LCPseudoLayerPlugin class
 */
class LCPseudoLayerPlugin : public pandora::PseudoLayerPlugin
{
public:
    /**
     *  @brief  Default constructor
     */
    LCPseudoLayerPlugin();

private:
    pandora::StatusCode Initialize();
    unsigned int GetPseudoLayer(const pandora::CartesianVector &positionVector) const;
    unsigned int GetPseudoLayerAtIp() const;

    /**
     *  @brief  Get the appropriate pseudolayer for a specified parameters
     * 
     *  @param  rCoordinate the radial coordinate
     *  @param  zCoordinate the z coordinate
     *  @param  rCorrection the barrel/endcap overlap r correction
     *  @param  zCorrection the barrel/endcap overlap z correction
     *  @param  barrelInnerR the barrel inner r coordinate
     *  @param  endCapInnerZ the endcap inner z coordinate
     *  @param  pseudoLayer to receive the appropriate pseudolayer
     */
    pandora::StatusCode GetPseudoLayer(const float rCoordinate, const float zCoordinate, const float rCorrection, const float zCorrection, 
        const float barrelInnerR, const float endCapInnerZ, unsigned int &pseudoLayer) const;

    typedef std::vector<float> LayerPositionList;

    /**
     *  @brief  Find the layer number corresponding to a specified position, via reference to a specified layer position list
     * 
     *  @param  position the specified position
     *  @param  layerPositionList the specified layer position list
     *  @param  layer to receive the layer number
     */
    pandora::StatusCode FindMatchingLayer(const float position, const LayerPositionList &layerPositionList, unsigned int &layer) const;

    /**
     *  @brief  Store all revelevant barrel and endcap layer positions upon initialization
     */
    void StoreLayerPositions();

    /**
     *  @brief  Store subdetector layer positions upon initialization
     * 
     *  @param  subDetector the sub detector
     *  @param  layerParametersList the layer parameters list
     */
    void StoreLayerPositions(const pandora::SubDetector &subDetector, LayerPositionList &LayerPositionList);

    /**
     *  @brief  Store positions of barrel and endcap outer edges upon initialization
     */
    void StoreDetectorOuterEdge();

    /**
     *  @brief  Store sine and cosine of angles used to project hit positions onto polygonal calorimeter surfaces upon initialization
     */
    void StorePolygonAngles();

    /**
     *  @brief  Store all details revelevant to barrel/endcap overlap corrections upon initialization
     */
    void StoreOverlapCorrectionDetails();

    typedef std::vector< std::pair<float, float> > AngleVector;

    /**
     *  @brief  Get the maximum polygon radius, with reference to cached sine/cosine values for relevant polygon angles
     * 
     *  @param  angleVector vector containing cached sine/cosine values
     *  @param  x the cartesian x coordinate
     *  @param  y the cartesian y coordinate
     * 
     *  @return the maximum radius
     */
    float GetMaximumRadius(const AngleVector &angleVector, const float x, const float y) const;

    /**
     *  @brief  Fill a vector with sine/cosine values for relevant polygon angles
     * 
     *  @param  symmetryOrder the polygon symmetry order
     *  @param  phi0 the polygon cylindrical polar phi coordinate
     *  @param  angleVector the vector to fill with sine/cosine values for relevant polygon angles
     */
    void FillAngleVector(const unsigned int symmetryOrder, const float phi0, AngleVector &angleVector) const;

    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    LayerPositionList   m_barrelLayerPositions;     ///< List of barrel layer positions
    LayerPositionList   m_endCapLayerPositions;     ///< List of endcap layer positions
    AngleVector         m_eCalBarrelAngleVector;    ///< The ecal barrel angle vector
    AngleVector         m_muonBarrelAngleVector;    ///< The muon barrel angle vector

    float               m_barrelInnerR;             ///< Barrel inner radius
    float               m_endCapInnerZ;             ///< Endcap inner z position
    float               m_barrelInnerRMuon;         ///< Muon barrel inner radius
    float               m_endCapInnerZMuon;         ///< Muon endcap inner z position

    float               m_rCorrection;              ///< Barrel/endcap overlap r correction
    float               m_zCorrection;              ///< Barrel/endcap overlap z correction
    float               m_rCorrectionMuon;          ///< Muon barrel/endcap overlap r correction
    float               m_zCorrectionMuon;          ///< Muon barrel/endcap overlap z correction

    float               m_barrelEdgeR;              ///< Extremal barrel r coordinate
    float               m_endCapEdgeZ;              ///< Extremal endcap z coordinate
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int LCPseudoLayerPlugin::GetPseudoLayerAtIp() const
{
    const unsigned int pseudoLayerAtIp(this->GetPseudoLayer(pandora::CartesianVector(0.f, 0.f, 0.f)));
    return pseudoLayerAtIp;
}

} // namespace lc_content

#endif // #ifndef LC_PSEUDO_LAYER_PLUGIN_H
