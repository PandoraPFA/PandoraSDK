/**
 *  @file   PandoraSDK/src/Objects/Histograms.cc
 * 
 *  @brief  Implementation of histogram classes.
 * 
 *  $Log: $
 */

#include "Helpers/XmlHelper.h"

#include "Objects/Histograms.h"

#include "Pandora/StatusCodes.h"

#include "Xml/tinyxml.h"

#include <cmath>
#include <limits>

namespace pandora
{

Histogram::Histogram(const unsigned int nBinsX, const float xLow, const float xHigh) :
    m_nBinsX(nBinsX),
    m_xLow(xLow),
    m_xHigh(xHigh)
{
    if ((0 >= nBinsX) || (xHigh - xLow < std::numeric_limits<float>::epsilon()))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    m_xBinWidth = (xHigh - xLow) / static_cast<float>(nBinsX);

    // ATTN Protect against cast to int wrapping to negative numbers if there are very many bins
    if (static_cast<int>((m_xHigh - m_xLow) / m_xBinWidth) < 0)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

Histogram::Histogram(const TiXmlHandle *const pXmlHandle, const std::string &xmlElementName) :
    m_nBinsX(0),
    m_xLow(0.f),
    m_xHigh(0.f)
{
    TiXmlElement *const pXmlElement(pXmlHandle->FirstChild(xmlElementName).Element());

    if (!pXmlElement)
    {
        std::cout << "Construct Histogram from xml: cannot find xml element with name " << xmlElementName << std::endl;
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);
    }

    const TiXmlHandle xmlHandle(pXmlElement);
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "NBinsX", m_nBinsX));
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "XLow", m_xLow));
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "XHigh", m_xHigh));

    if ((0 >= m_nBinsX) || (m_xHigh - m_xLow < std::numeric_limits<float>::epsilon()))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    m_xBinWidth = (m_xHigh - m_xLow) / static_cast<float>(m_nBinsX);

    FloatVector orderedBinContents;
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadVectorOfValues(xmlHandle, "BinContents", orderedBinContents));

    if (orderedBinContents.size() != static_cast<unsigned int>(m_nBinsX) + 2)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    for (int binX = this->GetUnderflowBinNumber(), endBinX = this->GetOverflowBinNumber(); binX <= endBinX; ++binX)
    {
        const float value(orderedBinContents[binX + 1]);

        if (std::fabs(value) > std::numeric_limits<float>::epsilon())
            m_histogramMap[binX] = value;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

Histogram::Histogram(const Histogram &rhs) :
    m_histogramMap(rhs.m_histogramMap),
    m_nBinsX(rhs.m_nBinsX),
    m_xLow(rhs.m_xLow),
    m_xHigh(rhs.m_xHigh),
    m_xBinWidth(rhs.m_xBinWidth)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

float Histogram::GetBinContent(const int binX) const
{
    HistogramMap::const_iterator iter = m_histogramMap.find(binX);

    if (m_histogramMap.end() == iter)
        return 0.f;

    return iter->second;
}

//------------------------------------------------------------------------------------------------------------------------------------------

int Histogram::GetBinNumber(const float valueX) const
{
    if (m_xHigh < valueX)
    {
        return this->GetOverflowBinNumber();
    }
    else if (m_xLow > valueX)
    {
        return this->GetUnderflowBinNumber();
    }
    else
    {
        return static_cast<int>((valueX - m_xLow) / m_xBinWidth);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

float Histogram::GetCumulativeSum(const int xLowBin, const int xHighBin) const
{
    float sumEntries(0.f);

    for (int xBin = std::max(this->GetUnderflowBinNumber(), xLowBin), xBinEnd = std::min(xHighBin, this->GetOverflowBinNumber()); xBin <= xBinEnd; ++xBin)
    {
        HistogramMap::const_iterator iterX = m_histogramMap.find(xBin);

        if (m_histogramMap.end() == iterX)
            continue;

        sumEntries += iterX->second;
    }

    return sumEntries;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram::GetMaximum(const int xLowBin, const int xHighBin, float &maximumValue, int &maximumBinX) const
{
    maximumValue = 0.f;
    maximumBinX = this->GetUnderflowBinNumber();

    for (int xBin = std::max(this->GetUnderflowBinNumber(), xLowBin), xBinEnd = std::min(xHighBin, this->GetOverflowBinNumber()); xBin <= xBinEnd; ++xBin)
    {
        HistogramMap::const_iterator iterX = m_histogramMap.find(xBin);

        if (m_histogramMap.end() == iterX)
            continue;

        const float binContents(iterX->second);

        if (binContents > maximumValue)
        {
            maximumValue = binContents;
            maximumBinX = iterX->first;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

float Histogram::GetMeanX(const int xLowBin, const int xHighBin) const
{
    float sumEntries(0.f), sumXEntries(0.f);
    const float firstBinCenter(m_xLow + (0.5f * m_xBinWidth));

    for (int xBin = std::max(this->GetMinBinNumber(), xLowBin), xBinEnd = std::min(xHighBin, this->GetMaxBinNumber()); xBin <= xBinEnd; ++xBin)
    {
        HistogramMap::const_iterator iterX = m_histogramMap.find(xBin);

        if (m_histogramMap.end() == iterX)
            continue;

        const float binCenter(firstBinCenter + (m_xBinWidth * static_cast<float>(iterX->first)));
        const float binContents(iterX->second);

        sumEntries += binContents;
        sumXEntries += binContents * binCenter;
    }

    if (std::fabs(sumEntries) < std::numeric_limits<float>::epsilon())
        return 0.f;

    return (sumXEntries / sumEntries);
}

//------------------------------------------------------------------------------------------------------------------------------------------

float Histogram::GetStandardDeviationX(const int xLowBin, const int xHighBin) const
{
    float sumEntries(0.f), sumXEntries(0.f), sumXXEntries(0.f);
    const float firstBinCenter(m_xLow + (0.5f * m_xBinWidth));

    for (int xBin = std::max(this->GetMinBinNumber(), xLowBin), xBinEnd = std::min(xHighBin, this->GetMaxBinNumber()); xBin <= xBinEnd; ++xBin)
    {
        HistogramMap::const_iterator iterX = m_histogramMap.find(xBin);

        if (m_histogramMap.end() == iterX)
            continue;

        const float binCenter(firstBinCenter + (m_xBinWidth * static_cast<float>(iterX->first)));
        const float binContents(iterX->second);

        sumEntries += binContents;
        sumXEntries += binContents * binCenter;
        sumXXEntries += binContents * binCenter * binCenter;
    }

    if (std::fabs(sumEntries) < std::numeric_limits<float>::epsilon())
        return 0.f;

    const float meanX(sumXEntries / sumEntries);
    const float meanXX(sumXXEntries / sumEntries);

    return std::sqrt(meanXX - (meanX * meanX));
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram::SetBinContent(const int binX, const float value)
{
    if ((binX < this->GetUnderflowBinNumber()) || (binX > this->GetOverflowBinNumber()))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    m_histogramMap[binX] = value;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram::Fill(const float valueX, const float weight)
{
    const int binX(this->GetBinNumber(valueX));

    HistogramMap::iterator iter = m_histogramMap.find(binX);

    if (m_histogramMap.end() != iter)
    {
        iter->second += weight;
    }
    else
    {
        if (!m_histogramMap.insert(HistogramMap::value_type(binX, weight)).second)
            throw StatusCodeException(STATUS_CODE_FAILURE);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram::Scale(const float scaleFactor)
{
    for (HistogramMap::value_type &mapEntry : m_histogramMap)
    {
        mapEntry.second = (mapEntry.second * scaleFactor);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram::WriteToXml(TiXmlDocument *const pTiXmlDocument, const std::string &histogramXmlKey) const
{
    TiXmlElement *const pHistogramElement = new TiXmlElement(histogramXmlKey);

    TiXmlElement *const pNBinsXElement = new TiXmlElement("NBinsX");
    pNBinsXElement->LinkEndChild(new TiXmlText(TypeToString(m_nBinsX)));
    pHistogramElement->LinkEndChild(pNBinsXElement);

    TiXmlElement *const pXLowElement = new TiXmlElement("XLow");
    pXLowElement->LinkEndChild(new TiXmlText(TypeToString(m_xLow)));
    pHistogramElement->LinkEndChild(pXLowElement);

    TiXmlElement *const pXHighElement = new TiXmlElement("XHigh");
    pXHighElement->LinkEndChild(new TiXmlText(TypeToString(m_xHigh)));
    pHistogramElement->LinkEndChild(pXHighElement);

    std::string binContentsString;
    for (int binX = this->GetUnderflowBinNumber(), endBinX = this->GetOverflowBinNumber(); binX <= endBinX; ++binX)
    {
        binContentsString += TypeToString(this->GetBinContent(binX)) + " ";
    }

    TiXmlElement *const pBinContentsElement = new TiXmlElement("BinContents");
    pBinContentsElement->LinkEndChild(new TiXmlText(binContentsString));
    pHistogramElement->LinkEndChild(pBinContentsElement);

    pTiXmlDocument->LinkEndChild(pHistogramElement);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

TwoDHistogram::TwoDHistogram(const unsigned int nBinsX, const float xLow, const float xHigh, const unsigned int nBinsY, const float yLow,
        const float yHigh) :
    m_nBinsX(nBinsX),
    m_xLow(xLow),
    m_xHigh(xHigh),
    m_nBinsY(nBinsY),
    m_yLow(yLow),
    m_yHigh(yHigh)
{
    if ((0 >= nBinsX) || (xHigh - xLow < std::numeric_limits<float>::epsilon()) || (0 >= nBinsY) || (yHigh - yLow < std::numeric_limits<float>::epsilon()))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    m_xBinWidth = (xHigh - xLow) / static_cast<float>(nBinsX);
    m_yBinWidth = (yHigh - yLow) / static_cast<float>(nBinsY);

    // ATTN Protect against cast to int wrapping to negative numbers if there are very many bins
    if ((static_cast<int>((m_xHigh - m_xLow) / m_xBinWidth) < 0) || (static_cast<int>((m_yHigh - m_yLow) / m_yBinWidth) < 0))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

TwoDHistogram::TwoDHistogram(const TiXmlHandle *const pXmlHandle, const std::string &xmlElementName) :
    m_nBinsX(0),
    m_xLow(0.f),
    m_xHigh(0.f),
    m_nBinsY(0),
    m_yLow(0.f),
    m_yHigh(0.f)
{
    TiXmlElement *const pXmlElement(pXmlHandle->FirstChild(xmlElementName).Element());

    if (!pXmlElement)
    {
        std::cout << "Construct Histogram from xml: cannot find xml element with name " << xmlElementName << std::endl;
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);
    }

    const TiXmlHandle xmlHandle(pXmlElement);
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "NBinsX", m_nBinsX));
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "XLow", m_xLow));
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "XHigh", m_xHigh));

    if ((0 >= m_nBinsX) || (m_xHigh - m_xLow < std::numeric_limits<float>::epsilon()))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    m_xBinWidth = (m_xHigh - m_xLow) / static_cast<float>(m_nBinsX);

    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "NBinsY", m_nBinsY));
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "YLow", m_yLow));
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "YHigh", m_yHigh));

    if ((0 >= m_nBinsY) || (m_yHigh - m_yLow < std::numeric_limits<float>::epsilon()))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    m_yBinWidth = (m_yHigh - m_yLow) / static_cast<float>(m_nBinsY);

    typedef std::vector<FloatVector> HistogramEntryList;
    HistogramEntryList histogramEntryList;
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::Read2DVectorOfValues(xmlHandle, "BinContents", "Row", histogramEntryList));

    if (histogramEntryList.size() != static_cast<unsigned int>(m_nBinsY) + 2)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    for (int binY = this->GetUnderflowBinNumberY(), endBinY = this->GetOverflowBinNumberY(); binY <= endBinY; ++binY)
    {
        const FloatVector &orderedBinContents(histogramEntryList[binY + 1]);

        if (orderedBinContents.size() != static_cast<unsigned int>(m_nBinsX) + 2)
            throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

        for (int binX = this->GetUnderflowBinNumberX(), endBinX = this->GetOverflowBinNumberX(); binX <= endBinX; ++binX)
        {
            const float value(orderedBinContents[binX + 1]);
            if (std::fabs(value) > std::numeric_limits<float>::epsilon())
            {
                m_yxHistogramMap[binY][binX] = value;
                m_xyHistogramMap[binX][binY] = value;
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

TwoDHistogram::TwoDHistogram(const TwoDHistogram &rhs) :
    m_xyHistogramMap(rhs.m_xyHistogramMap),
    m_yxHistogramMap(rhs.m_yxHistogramMap),
    m_nBinsX(rhs.m_nBinsX),
    m_xLow(rhs.m_xLow),
    m_xHigh(rhs.m_xHigh),
    m_xBinWidth(rhs.m_xBinWidth),
    m_nBinsY(rhs.m_nBinsY),
    m_yLow(rhs.m_yLow),
    m_yHigh(rhs.m_yHigh),
    m_yBinWidth(rhs.m_yBinWidth)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

float TwoDHistogram::GetBinContent(const int binX, const int binY) const
{
    TwoDHistogramMap::const_iterator iterX = m_xyHistogramMap.find(binX);

    if (m_xyHistogramMap.end() == iterX)
        return 0.f;

    HistogramMap::const_iterator iterXY = iterX->second.find(binY);

    if (iterX->second.end() == iterXY)
        return 0.f;

    return iterXY->second;
}
//------------------------------------------------------------------------------------------------------------------------------------------

int TwoDHistogram::GetBinNumberX(const float valueX) const
{
    if (m_xHigh < valueX)
    {
        return this->GetOverflowBinNumberX();
    }
    else if (m_xLow > valueX)
    {
        return this->GetUnderflowBinNumberX();
    }
    else
    {
        return static_cast<int>((valueX - m_xLow) / m_xBinWidth);
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------

int TwoDHistogram::GetBinNumberY(const float valueY) const
{
    if (m_yHigh < valueY)
    {
        return this->GetOverflowBinNumberY();
    }
    else if (m_yLow > valueY)
    {
        return this->GetUnderflowBinNumberY();
    }
    else
    {
        return static_cast<int>((valueY - m_yLow) / m_yBinWidth);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

float TwoDHistogram::GetCumulativeSum(const int xLowBin, const int xHighBin, const int yLowBin, const int yHighBin) const
{
    float sumEntries(0.f);

    for (int yBin = std::max(this->GetUnderflowBinNumberY(), yLowBin), yBinEnd = std::min(yHighBin, this->GetOverflowBinNumberY()); yBin <= yBinEnd; ++yBin)
    {
        TwoDHistogramMap::const_iterator iterY = m_yxHistogramMap.find(yBin);

        if (m_yxHistogramMap.end() == iterY)
            continue;

        for (int xBin = std::max(this->GetUnderflowBinNumberX(), xLowBin), xBinEnd = std::min(xHighBin, this->GetOverflowBinNumberX()); xBin <= xBinEnd; ++xBin)
        {
            HistogramMap::const_iterator iterX = iterY->second.find(xBin);

            if (iterY->second.end() == iterX)
                continue;

            sumEntries += iterX->second;
        }
    }

    return sumEntries;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDHistogram::GetMaximum(const int xLowBin, const int xHighBin, const int yLowBin, const int yHighBin, float &maximumValue,
    int &maximumBinX, int &maximumBinY) const
{
    maximumValue = 0.f;
    maximumBinX = this->GetUnderflowBinNumberX(); maximumBinY = this->GetUnderflowBinNumberY();

    for (int yBin = std::max(this->GetUnderflowBinNumberY(), yLowBin), yBinEnd = std::min(yHighBin, this->GetOverflowBinNumberY()); yBin <= yBinEnd; ++yBin)
    {
        TwoDHistogramMap::const_iterator iterY = m_yxHistogramMap.find(yBin);

        if (m_yxHistogramMap.end() == iterY)
            continue;

        for (int xBin = std::max(this->GetUnderflowBinNumberX(), xLowBin), xBinEnd = std::min(xHighBin, this->GetOverflowBinNumberX()); xBin <= xBinEnd; ++xBin)
        {
            HistogramMap::const_iterator iterX = iterY->second.find(xBin);

            if (iterY->second.end() == iterX)
                continue;

            const float binContents(iterX->second);

            if (binContents > maximumValue)
            {
                maximumValue = binContents;
                maximumBinX = iterX->first;
                maximumBinY = iterY->first;
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

float TwoDHistogram::GetMeanX(const int xLowBin, const int xHighBin, const int yLowBin, const int yHighBin) const
{
    float sumEntries(0.f), sumXEntries(0.f);
    const float firstBinXCenter(m_xLow + (0.5f * m_xBinWidth));

    for (int yBin = std::max(this->GetMinBinNumberY(), yLowBin), yBinEnd = std::min(yHighBin, this->GetMaxBinNumberY()); yBin <= yBinEnd; ++yBin)
    {
        TwoDHistogramMap::const_iterator iterY = m_yxHistogramMap.find(yBin);

        if (m_yxHistogramMap.end() == iterY)
            continue;

        for (int xBin = std::max(this->GetMinBinNumberX(), xLowBin), xBinEnd = std::min(xHighBin, this->GetMaxBinNumberX()); xBin <= xBinEnd; ++xBin)
        {
            HistogramMap::const_iterator iterX = iterY->second.find(xBin);

            if (iterY->second.end() == iterX)
                continue;

            const float binXCenter(firstBinXCenter + (m_xBinWidth * static_cast<float>(iterX->first)));
            const float binContents(iterX->second);

            sumEntries += binContents;
            sumXEntries += binContents * binXCenter;
        }
    }

    if (std::fabs(sumEntries) < std::numeric_limits<float>::epsilon())
        return 0.f;

    return (sumXEntries / sumEntries);
}

//------------------------------------------------------------------------------------------------------------------------------------------

float TwoDHistogram::GetStandardDeviationX(const int xLowBin, const int xHighBin, const int yLowBin, const int yHighBin) const
{
    float sumEntries(0.f), sumXEntries(0.f), sumXXEntries(0.f);
    const float firstBinXCenter(m_xLow + (0.5f * m_xBinWidth));

    for (int yBin = std::max(this->GetMinBinNumberY(), yLowBin), yBinEnd = std::min(yHighBin, this->GetMaxBinNumberY()); yBin <= yBinEnd; ++yBin)
    {
        TwoDHistogramMap::const_iterator iterY = m_yxHistogramMap.find(yBin);

        if (m_yxHistogramMap.end() == iterY)
            continue;

        for (int xBin = std::max(this->GetMinBinNumberX(), xLowBin), xBinEnd = std::min(xHighBin, this->GetMaxBinNumberX()); xBin <= xBinEnd; ++xBin)
        {
            HistogramMap::const_iterator iterX = iterY->second.find(xBin);

            if (iterY->second.end() == iterX)
                continue;

            const float binXCenter(firstBinXCenter + (m_xBinWidth * static_cast<float>(iterX->first)));
            const float binContents(iterX->second);

            sumEntries += binContents;
            sumXEntries += binContents * binXCenter;
            sumXXEntries += binContents * binXCenter * binXCenter;
        }
    }

    if (std::fabs(sumEntries) < std::numeric_limits<float>::epsilon())
        return 0.f;

    const float meanX(sumXEntries / sumEntries);
    const float meanXX(sumXXEntries / sumEntries);

    return std::sqrt(meanXX - (meanX * meanX));
}

//------------------------------------------------------------------------------------------------------------------------------------------

float TwoDHistogram::GetMeanY(const int xLowBin, const int xHighBin, const int yLowBin, const int yHighBin) const
{
    float sumEntries(0.f), sumYEntries(0.f);
    const float firstBinYCenter(m_yLow + (0.5f * m_yBinWidth));

    for (int xBin = std::max(this->GetMinBinNumberX(), xLowBin), xBinEnd = std::min(xHighBin, this->GetMaxBinNumberX()); xBin <= xBinEnd; ++xBin)
    {
        TwoDHistogramMap::const_iterator iterX = m_xyHistogramMap.find(xBin);

        if (m_xyHistogramMap.end() == iterX)
            continue;

        for (int yBin = std::max(this->GetMinBinNumberY(), yLowBin), yBinEnd = std::min(yHighBin, this->GetMaxBinNumberY()); yBin <= yBinEnd; ++yBin)
        {
            HistogramMap::const_iterator iterY = iterX->second.find(yBin);

            if (iterX->second.end() == iterY)
                continue;

            const float binYCenter(firstBinYCenter + (m_yBinWidth * static_cast<float>(iterY->first)));
            const float binContents(iterY->second);

            sumEntries += binContents;
            sumYEntries += binContents * binYCenter;
        }
    }

    if (std::fabs(sumEntries) < std::numeric_limits<float>::epsilon())
        return 0.f;

    return (sumYEntries / sumEntries);
}

//------------------------------------------------------------------------------------------------------------------------------------------

float TwoDHistogram::GetStandardDeviationY(const int xLowBin, const int xHighBin, const int yLowBin, const int yHighBin) const
{
    float sumEntries(0.f), sumYEntries(0.f), sumYYEntries(0.f);
    const float firstBinYCenter(m_yLow + (0.5f * m_yBinWidth));

    for (int xBin = std::max(this->GetMinBinNumberX(), xLowBin), xBinEnd = std::min(xHighBin, this->GetMaxBinNumberX()); xBin <= xBinEnd; ++xBin)
    {
        TwoDHistogramMap::const_iterator iterX = m_xyHistogramMap.find(xBin);

        if (m_xyHistogramMap.end() == iterX)
            continue;

        for (int yBin = std::max(this->GetMinBinNumberY(), yLowBin), yBinEnd = std::min(yHighBin, this->GetMaxBinNumberY()); yBin <= yBinEnd; ++yBin)
        {
            HistogramMap::const_iterator iterY = iterX->second.find(yBin);

            if (iterX->second.end() == iterY)
                continue;

            const float binYCenter(firstBinYCenter + (m_yBinWidth * static_cast<float>(iterY->first)));
            const float binContents(iterY->second);

            sumEntries += binContents;
            sumYEntries += binContents * binYCenter;
            sumYYEntries += binContents * binYCenter * binYCenter;
        }
    }

    if (std::fabs(sumEntries) < std::numeric_limits<float>::epsilon())
        return 0.f;

    const float meanY(sumYEntries / sumEntries);
    const float meanYY(sumYYEntries / sumEntries);

    return std::sqrt(meanYY - (meanY * meanY));
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDHistogram::SetBinContent(const int binX, const int binY, const float value)
{
    if ((binX < this->GetUnderflowBinNumberX()) || (binX > this->GetOverflowBinNumberX()) || (binY < this->GetUnderflowBinNumberY()) || (binY > this->GetOverflowBinNumberY()))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    m_xyHistogramMap[binX][binY] = value;
    m_yxHistogramMap[binY][binX] = value;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDHistogram::Fill(const float valueX, const float valueY, const float weight)
{
    const int binX(this->GetBinNumberX(valueX));
    const int binY(this->GetBinNumberY(valueY));

    HistogramMap &yHistogramMap(m_xyHistogramMap[binX]);
    HistogramMap::iterator iter = yHistogramMap.find(binY);

    if (yHistogramMap.end() != iter)
    {
        iter->second += weight;
    }
    else
    {
        if (!yHistogramMap.insert(HistogramMap::value_type(binY, weight)).second)
            throw StatusCodeException(STATUS_CODE_FAILURE);
    }

    m_yxHistogramMap[binY][binX] = yHistogramMap[binY];
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDHistogram::Scale(const float scaleFactor)
{
    for (TwoDHistogramMap::value_type &mapEntryX : m_xyHistogramMap)
    {
        for (HistogramMap::value_type &mapEntryY : mapEntryX.second)
        {
            mapEntryY.second = (mapEntryY.second * scaleFactor);
        }
    }

    for (TwoDHistogramMap::value_type &mapEntryY : m_yxHistogramMap)
    {
        for (HistogramMap::value_type &mapEntryX : mapEntryY.second)
        {
            mapEntryX.second = (mapEntryX.second * scaleFactor);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDHistogram::WriteToXml(TiXmlDocument *const pTiXmlDocument, const std::string &histogramXmlKey) const
{
    TiXmlElement *const pHistogramElement = new TiXmlElement(histogramXmlKey);

    TiXmlElement *const pNBinsXElement = new TiXmlElement("NBinsX");
    pNBinsXElement->LinkEndChild(new TiXmlText(TypeToString(m_nBinsX)));
    pHistogramElement->LinkEndChild(pNBinsXElement);

    TiXmlElement *const pXLowElement = new TiXmlElement("XLow");
    pXLowElement->LinkEndChild(new TiXmlText(TypeToString(m_xLow)));
    pHistogramElement->LinkEndChild(pXLowElement);

    TiXmlElement *const pXHighElement = new TiXmlElement("XHigh");
    pXHighElement->LinkEndChild(new TiXmlText(TypeToString(m_xHigh)));
    pHistogramElement->LinkEndChild(pXHighElement);

    TiXmlElement *const pNBinsYElement = new TiXmlElement("NBinsY");
    pNBinsYElement->LinkEndChild(new TiXmlText(TypeToString(m_nBinsY)));
    pHistogramElement->LinkEndChild(pNBinsYElement);

    TiXmlElement *const pYLowElement = new TiXmlElement("YLow");
    pYLowElement->LinkEndChild(new TiXmlText(TypeToString(m_yLow)));
    pHistogramElement->LinkEndChild(pYLowElement);

    TiXmlElement *const pYHighElement = new TiXmlElement("YHigh");
    pYHighElement->LinkEndChild(new TiXmlText(TypeToString(m_yHigh)));
    pHistogramElement->LinkEndChild(pYHighElement);

    TiXmlElement *const pBinContentsElement = new TiXmlElement("BinContents");

    for (int binY = this->GetUnderflowBinNumberY(), endBinY = this->GetOverflowBinNumberY(); binY <= endBinY; ++binY)
    {
        std::string binContentsString;

        for (int binX = this->GetUnderflowBinNumberX(), endBinX = this->GetOverflowBinNumberX(); binX <= endBinX; ++binX)
        {
            binContentsString += TypeToString(this->GetBinContent(binX, binY)) + " ";
        }

        TiXmlElement *const pRowElement = new TiXmlElement("Row");
        pRowElement->LinkEndChild(new TiXmlText(binContentsString));
        pBinContentsElement->LinkEndChild(pRowElement);
    }

    pHistogramElement->LinkEndChild(pBinContentsElement);

    pTiXmlDocument->LinkEndChild(pHistogramElement);
}

} // namespace pandora
