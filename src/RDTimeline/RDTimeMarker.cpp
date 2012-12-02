/*
 * =====================================================================================
 *
 *       Filename:  RDTimeMarker.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年11月28日 07时07分59秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  HuanJianChao (), hjc0720@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "RDTimeMarker.h"
#include <QPainter>
#include <QWidget>
/*
 *--------------------------------------------------------------------------------------
 *       Class:  RDTimeMarker
 *      Method:  RDTimeMarker
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
RDTimeMarker::RDTimeMarker (int nHeight,int nViewHeight,double dScale)
    :m_tri(3)
     ,m_nHeight(nViewHeight)
     ,m_dScale(1 / dScale)
{
    m_tri.setPoints(3, -6, nHeight / 2, 6, nHeight / 2, 0, nHeight);
    SetTime(0);
}  /* -----  end of method RDTimeMarker::RDTimeMarker  (constructor)  ----- */

QRectF RDTimeMarker::boundingRect()const
{
    return QRectF(-6 * m_dScale ,0,12 * m_dScale  , m_nHeight);
}

void RDTimeMarker::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *widget)
{
    painter->setPen(widget->palette().highlight().color());
    painter->drawLine(0,0,0,m_nHeight);

    qreal fScale = painter->worldTransform().m11();
    painter->scale(m_dScale,1);
    painter->setBrush(widget->palette().highlight());
    painter->drawPolygon(m_tri);
    qDebug() << "ScalePt :" << m_dScale << "real Scale :" << fScale; 
    qDebug() << "paint rect" << "boundingRect" << boundingRect();
}
