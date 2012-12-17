/**
 *  @file   PandoraPFANew/Framework/include/Objects/Histograms.h
 * 
 *  @brief  Header file for histogram classes
 * 
 *  $Log: $
 */
#ifndef HISTOGRAMS_H
#define HISTOGRAMS_H 1

#include <map>

namespace pandora
{

class TiXmlDocument;
class TiXmlHandle;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Histogram class
 */
class Histogram
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  nBinsX number of x bins
     *  @param  xLow min binned x value
     *  @param  xHigh max binned x value
     */
    Histogram(const unsigned int nBinsX, const float xLow, const float xHigh);

    /**
     *  @brief  Constructor
     * 
     *  @param  pXmlHandle address of the handle for the xml element describing the histogram
     *  @param  xmlElementName the xml element name for the histogram
     */
    Histogram(const TiXmlHandle *const pXmlHandle, const std::string &xmlElementName);

    /**
     *  @brief  Copy constructor
     * 
     *  @param  rhs the histogram to copy
     */
    Histogram(const Histogram &rhs);

    /**
     *  @brief  Get the number of x bins
     * 
     *  @return The number of x bins
     */
    int GetNBinsX() const;

    /**
     *  @brief  Get the min binned x value
     * 
     *  @return The min binned x value
     */
    float GetXLow() const;

    /**
     *  @brief  Get the max binned x value
     * 
     *  @return The max binned x value
     */
    float GetXHigh() const;

    /**
     *  @brief  Get the x bin width
     * 
     *  @return The x bin width
     */
    float GetXBinWidth() const;

    /**
     *  @brief  Get the content of a specified bin
     * 
     *  @param  binX the specified bin number
     * 
     *  @return The content of the specified bin
     */
    float GetBinContent(const int binX) const;

    /**
     *  @brief  Get the cumulative sum of bin entries in the histogram (excluding overflow and underflow bins)
     * 
     *  @return The cumulative sum
     */
    float GetCumulativeSum() const;

    /**
     *  @brief  Get the cumulative sum of bin entries in a specified range of the histogram (excluding overflow and underflow bins)
     * 
     *  @param  xLowBin bin at start of specified x range
     *  @param  xHighBin bin at end of specified x range
     * 
     *  @return The cumulative sum
     */
    float GetCumulativeSum(const int xLowBin, const int xHighBin) const;

    /**
     *  @brief  Get the maximum value in the histogram and the corresponding bin number (excludes overflow and underflow bins)
     * 
     *  @param  maximumValue to receive the maximum value
     *  @param  maximumBinX to receive the coordinate of the bin containing the maximum value
     */
    void GetMaximum(float &maximumValue, int &maximumBinX) const;

    /**
     *  @brief  Get the maximum value in a specified range of the histogram and the corresponding bin numbers (excludes overflow
     *          and underflow bins)
     * 
     *  @param  xLowBin bin at start of specified x range
     *  @param  xHighBin bin at end of specified x range
     *  @param  maximumValue to receive the maximum value
     *  @param  maximumBinX to receive the coordinate of the bin containing the maximum value
     */
    void GetMaximum(const int xLowBin, const int xHighBin, float &maximumValue, int &maximumBinX) const;

    /**
     *  @brief  Get the mean x value of entries in the histogram (excluding overflow and underflow bins)
     * 
     *  @return The mean x value
     */
    float GetMeanX() const;

    /**
     *  @brief  Get the mean x value of entries in a specified range of the histogram (excluding overflow and underflow bins)
     * 
     *  @param  xLowBin bin at start of specified x range
     *  @param  xHighBin bin at end of specified x range
     * 
     *  @return The mean x value
     */
    float GetMeanX(const int xLowBin, const int xHighBin) const;

    /**
     *  @brief  Get the standard deviation of entries in the histogram (excluding overflow and underflow bins)
     * 
     *  @return The standard deviation of x entries
     */
    float GetStandardDeviationX() const;

    /**
     *  @brief  Get the standard deviation of x entries in a specified range of the histogram (excluding overflow and underflow bins)
     * 
     *  @param  xLowBin bin at start of specified x range
     *  @param  xHighBin bin at end of specified x range
     * 
     *  @return The standard deviation of x entries
     */
    float GetStandardDeviationX(const int xLowBin, const int xHighBin) const;

    /**
     *  @brief  Set the contents of a specified bin
     * 
     *  @param  binX the specified bin number
     *  @param  value the new value for the specified bin
     */
    void SetBinContent(const int binX, const float value);

    /**
     *  @brief  Add an entry to the histogram
     * 
     *  @param  valueX the value for the entry
     *  @param  weight the weight associated with this entry
     */
    void Fill(const float valueX, const float weight = 1.f);

    /**
     *  @brief  Scale contents of all histogram bins by a specified factor
     * 
     *  @param  scaleFactor the scale factor
     */
    void Scale(const float scaleFactor);

    /**
     *  @brief  Write the histogram to an xml document
     * 
     *  @param  pTiXmlDocument address of the xml document
     *  @param  xmlElementName the xml element name for the histogram
     */
    void WriteToXml(TiXmlDocument *pTiXmlDocument, const std::string &xmlElementName) const;

private:
    typedef std::map<int, float> HistogramMap;

    HistogramMap        m_histogramMap;         ///< The histogram map

    int                 m_nBinsX;               ///< The number of x bins
    float               m_xLow;                 ///< The min binned x value
    float               m_xHigh;                ///< The max binned x value
    float               m_xBinWidth;            ///< The x bin width
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  TwoDHistogram class
 */
class TwoDHistogram
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  nBinsX number of x bins
     *  @param  xLow min binned x value
     *  @param  xHigh max binned x value
     *  @param  nBinsY number of y bins
     *  @param  yLow min binned y value
     *  @param  yHigh max binned y value
     */
    TwoDHistogram(const unsigned int nBinsX, const float xLow, const float xHigh, const unsigned int nBinsY, const float yLow,
        const float yHigh);

    /**
     *  @brief  Constructor
     * 
     *  @param  pXmlHandle address of the handle for the xml element describing the histogram
     *  @param  xmlElementName the xml element name for the histogram
     */
    TwoDHistogram(const TiXmlHandle *const pXmlHandle, const std::string &xmlElementName);

    /**
     *  @brief  Copy constructor
     * 
     *  @param  rhs the histogram to copy
     */
    TwoDHistogram(const TwoDHistogram &rhs);

    /**
     *  @brief  Get the number of x bins
     * 
     *  @return The number of x bins
     */
    int GetNBinsX() const;

    /**
     *  @brief  Get the min binned x value
     * 
     *  @return The min binned x value
     */
    float GetXLow() const;

    /**
     *  @brief  Get the max binned x value
     * 
     *  @return The max binned x value
     */
    float GetXHigh() const;

    /**
     *  @brief  Get the x bin width
     * 
     *  @return The x bin width
     */
    float GetXBinWidth() const;

    /**
     *  @brief  Get the number of y bins
     * 
     *  @return The number of y bins
     */
    int GetNBinsY() const;

    /**
     *  @brief  Get the min binned y value
     * 
     *  @return The min binned y value
     */
    float GetYLow() const;

    /**
     *  @brief  Get the max binned y value
     * 
     *  @return The max binned y value
     */
    float GetYHigh() const;

    /**
     *  @brief  Get the y bin width
     * 
     *  @return The y bin width
     */
    float GetYBinWidth() const;

    /**
     *  @brief  Get the content of a specified bin
     * 
     *  @param  binX the specified x bin number
     *  @param  binY the specified y bin number
     * 
     *  @return The content of the specified bin
     */
    float GetBinContent(const int binX, const int binY) const;

    /**
     *  @brief  Get the cumulative sum of bin entries in the histogram (excluding overflow and underflow bins)
     * 
     *  @return The cumulative sum
     */
    float GetCumulativeSum() const;

    /**
     *  @brief  Get the cumulative sum of bin entries in a specified range of the histogram (excluding overflow and underflow bins)
     * 
     *  @param  xLowBin bin at start of specified x range
     *  @param  xHighBin bin at end of specified x range
     *  @param  yLowBin bin at start of specified y range
     *  @param  yHighBin bin at end of specified y range
     * 
     *  @return The cumulative sum
     */
    float GetCumulativeSum(const int xLowBin, const int xHighBin, const int yLowBin, const int yHighBin) const;

    /**
     *  @brief  Get the maximum value in the histogram and the corresponding bin numbers (excludes overflow and underflow bins)
     * 
     *  @param  maximumValue to receive the maximum value
     *  @param  maximumBinX to receive the x coordinate of the bin containing the maximum value
     *  @param  maximumBinY to receive the y coordinate of the bin containing the maximum value
     */
    void GetMaximum(float &maximumValue, int &maximumBinX, int &maximumBinY) const;

    /**
     *  @brief  Get the maximum value in a specified range of the histogram and the corresponding bin numbers (excludes overflow
     *          and underflow bins)
     * 
     *  @param  xLowBin bin at start of specified x range
     *  @param  xHighBin bin at end of specified x range
     *  @param  yLowBin bin at start of specified y range
     *  @param  yHighBin bin at end of specified y range
     *  @param  maximumValue to receive the maximum value
     *  @param  maximumBinX to receive the x coordinate of the bin containing the maximum value
     *  @param  maximumBinY to receive the y coordinate of the bin containing the maximum value
     */
    void GetMaximum(const int xLowBin, const int xHighBin, const int yLowBin, const int yHighBin, float &maximumValue, int &maximumBinX,
        int &maximumBinY) const;

    /**
     *  @brief  Get the mean x value of entries in the histogram (excluding overflow and underflow bins)
     * 
     *  @return The mean x value
     */
    float GetMeanX() const;

    /**
     *  @brief  Get the mean x value of entries in a specified range of the histogram (excluding overflow and underflow bins)
     * 
     *  @param  xLowBin bin at start of specified x range
     *  @param  xHighBin bin at end of specified x range
     *  @param  yLowBin bin at start of specified y range
     *  @param  yHighBin bin at end of specified y range
     * 
     *  @return The mean x value
     */
    float GetMeanX(const int xLowBin, const int xHighBin, const int yLowBin, const int yHighBin) const;

    /**
     *  @brief  Get the standard deviation of x entries in the histogram (excluding overflow and underflow bins)
     * 
     *  @return The standard deviation of x entries
     */
    float GetStandardDeviationX() const;

    /**
     *  @brief  Get the standard deviation of x entries in a specified range of the histogram (excluding overflow and underflow bins)
     * 
     *  @param  xLowBin bin at start of specified x range
     *  @param  xHighBin bin at end of specified x range
     *  @param  yLowBin bin at start of specified y range
     *  @param  yHighBin bin at end of specified y range
     * 
     *  @return The standard deviation of x entries
     */
    float GetStandardDeviationX(const int xLowBin, const int xHighBin, const int yLowBin, const int yHighBin) const;

    /**
     *  @brief  Get the mean y value of entries in the histogram (excluding overflow and underflow bins)
     * 
     *  @return The mean y value
     */
    float GetMeanY() const;

    /**
     *  @brief  Get the mean y value of entries in a specified range of the histogram (excluding overflow and underflow bins)
     * 
     *  @param  xLowBin bin at start of specified x range
     *  @param  xHighBin bin at end of specified x range
     *  @param  yLowBin bin at start of specified y range
     *  @param  yHighBin bin at end of specified y range
     * 
     *  @return The mean y value
     */
    float GetMeanY(const int xLowBin, const int xHighBin, const int yLowBin, const int yHighBin) const;

    /**
     *  @brief  Get the standard deviation of y entries in the histogram (excluding overflow and underflow bins)
     * 
     *  @return The standard deviation of y entries
     */
    float GetStandardDeviationY() const;

    /**
     *  @brief  Get the standard deviation of y entries in a specified range of the histogram (excluding overflow and underflow bins)
     * 
     *  @param  xLowBin bin at start of specified x range
     *  @param  xHighBin bin at end of specified x range
     *  @param  yLowBin bin at start of specified y range
     *  @param  yHighBin bin at end of specified y range
     * 
     *  @return The standard deviation of y entries
     */
    float GetStandardDeviationY(const int xLowBin, const int xHighBin, const int yLowBin, const int yHighBin) const;

    /**
     *  @brief  Set the contents of a specified bin
     * 
     *  @param  binX the specified x bin number
     *  @param  binY the specified y bin number
     *  @param  value the new value for the specified bin
     */
    void SetBinContent(const int binX, const int binY, const float value);

    /**
     *  @brief  Add an entry to the histogram
     * 
     *  @param  valueX the x value for the entry
     *  @param  valueY the y value for the entry
     *  @param  weight the weight associated with this entry
     */
    void Fill(const float valueX, const float valueY, const float weight = 1.f);

    /**
     *  @brief  Scale contents of all histogram bins by a specified factor
     * 
     *  @param  scaleFactor the scale factor
     */
    void Scale(const float scaleFactor);

    /**
     *  @brief  Write the histogram to an xml document
     * 
     *  @param  pTiXmlDocument address of the xml document
     *  @param  xmlElementName the xml element name for the histogram
     */
    void WriteToXml(TiXmlDocument *pTiXmlDocument, const std::string &xmlElementName) const;

private:
    typedef std::map<int, float> HistogramMap;
    typedef std::map<int, HistogramMap> TwoDHistogramMap;

    TwoDHistogramMap    m_xyHistogramMap;       ///< The x->y->value 2d histogram map
    TwoDHistogramMap    m_yxHistogramMap;       ///< The y->x->value 2d histogram map

    int                 m_nBinsX;               ///< The number of x bins
    float               m_xLow;                 ///< The min binned x value
    float               m_xHigh;                ///< The max binned x value
    float               m_xBinWidth;            ///< The x bin width

    int                 m_nBinsY;               ///< The number of y bins
    float               m_yLow;                 ///< The min binned y value
    float               m_yHigh;                ///< The max binned y value
    float               m_yBinWidth;            ///< The y bin width
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline int Histogram::GetNBinsX() const
{
    return m_nBinsX;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Histogram::GetXLow() const
{
    return m_xLow;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Histogram::GetXHigh() const
{
    return m_xHigh;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Histogram::GetXBinWidth() const
{
    return m_xBinWidth;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Histogram::GetCumulativeSum() const
{
    return this->GetCumulativeSum(0, m_nBinsX);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void Histogram::GetMaximum(float &maximumValue, int &maximumBinX) const
{
    return this->GetMaximum(0, m_nBinsX, maximumValue, maximumBinX);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Histogram::GetMeanX() const
{
    return this->GetMeanX(0, m_nBinsX);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Histogram::GetStandardDeviationX() const
{
    return this->GetStandardDeviationX(0, m_nBinsX);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline int TwoDHistogram::GetNBinsX() const
{
    return m_nBinsX;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetXLow() const
{
    return m_xLow;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetXHigh() const
{
    return m_xHigh;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetXBinWidth() const
{
    return m_xBinWidth;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline int TwoDHistogram::GetNBinsY() const
{
    return m_nBinsY;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetYLow() const
{
    return m_yLow;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetYHigh() const
{
    return m_yHigh;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetYBinWidth() const
{
    return m_yBinWidth;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetCumulativeSum() const
{
    return this->GetCumulativeSum(0, m_nBinsX, 0, m_nBinsY);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void TwoDHistogram::GetMaximum(float &maximumValue, int &maximumBinX, int &maximumBinY) const
{
    return this->GetMaximum(0, m_nBinsX, 0, m_nBinsY, maximumValue, maximumBinX, maximumBinY);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetMeanX() const
{
    return this->GetMeanX(0, m_nBinsX, 0, m_nBinsY);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetStandardDeviationX() const
{
    return this->GetStandardDeviationX(0, m_nBinsX, 0, m_nBinsY);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetMeanY() const
{
    return this->GetMeanY(0, m_nBinsX, 0, m_nBinsY);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetStandardDeviationY() const
{
    return this->GetStandardDeviationY(0, m_nBinsX, 0, m_nBinsY);
}

} // namespace pandora

#endif // #ifndef HISTOGRAMS_H
