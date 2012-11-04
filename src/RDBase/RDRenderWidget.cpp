// =====================================================================================
// 
//       Filename:  RDRenderWidget.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  26/03/11 14:46:36
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Huang Jianchao (), hjc0720@gmail.com
//        Company:  
// 
// =====================================================================================

#include "RDRenderWidget.h"
#include <QSizePolicy>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QtDebug>
#include <QCoreApplication>
#include "RDBuffer.h"
#include "RDDocument.h"
#include "mainwindow.h"
#include "RDSceneRenderData.h"
#include "RDRenderManager.h"
#include <QImage>
#include "RDBaseTool.h"
#include "RDToolManager.h"
#include "RDSpaceConvert.h"
#include <QRectF>
#include "RDEditerManager.h"
#include "RDScene.h"

bool g_bForceUpdate = false;

void RDRenderWidget::setVisible(bool visible)
{
    m_RenderTimer.start();
    QWidget::setVisible(visible);
}
void    RDRenderWidget::OnTime(void* param)
{
    static RenderManager* pRDManager = 0;
    if(!pRDManager)
    {
        pRDManager = new RenderManager;
    }
    RDRenderWidget* pWidget = (RDRenderWidget*)param;
    RDDocument* pDoc = &pWidget->GetDocument();
    static double dStartTime = 0;
    static double oldTime = GetTime();
    dStartTime = GetTime();
    pDoc->Lock();
    RDScene* pScene = pDoc->GetCurScene();
    //if(pScene->GetMaxChangeLevel(DEFAULT_RD) == RDRender_NoChange && pDoc->GetCurTime() == pScene->GetRenderData(DEFAULT_RD)->GetTime()) 
    //{
        //oldTime = dStartTime;
        //pDoc->UnLock();
        //if(g_bForceUpdate)
        //{
            //pWidget->update();
            //g_bForceUpdate = false;
        //}
        //return;
    //}
    pRDManager->SetRenderName(DEFAULT_RD);
    pRDManager->SetScene(pScene);
    pRDManager->SetDstBuffer(pWidget->m_swapChain.GetBackBuffer());
    QPointF pt(pWidget->m_nXOffset,pWidget->m_nYOffset);
    if(!pRDManager->RenderScene(pt,pWidget->m_validRt,pDoc->GetCurTime()))
    {
        oldTime = dStartTime;
        pDoc->UnLock();
        if(g_bForceUpdate)
        {
            pWidget->update();
            g_bForceUpdate = false;
        }
        return;
    }
    qDebug() << "On Time :" << pDoc->GetCurTime();
    pWidget->m_swapChain.SwapChain();

    RDRenderData* pSceneRD = pScene->GetRenderData(DEFAULT_RD);
    pWidget->update(pSceneRD->GetDirty().translated(pt).toAlignedRect());
    pDoc->UnLock();
    //pWidget->update();
    if(dStartTime - oldTime > 20)
        qDebug() << dStartTime - oldTime;
    oldTime = dStartTime;
}

RDRenderWidget::RDRenderWidget(int nWidth, int nHeight,QWidget *parent /*= 0*/)
    :QWidget(parent)
     ,m_nProjWidth(nWidth)
     ,m_nProjHeight(nHeight)
     ,m_nXOffset(0)
     ,m_nYOffset(0)
     ,m_fScale(1)
     //,m_pMainWin(pMain)
     ,m_document(true)
     ,m_RenderTimer(20,RDRenderWidget::OnTime,this)
     ,m_swapChain(nWidth,nHeight)
{
    //setAttribute(Qt::WA_PaintOutsidePaintEvent);
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    setFocusPolicy(Qt::ClickFocus);
    //m_swapChain.GetFrontBuffer()->FillColor(0xff00ff00);
}
RDRenderWidget::~RDRenderWidget()
{
    m_RenderTimer.close();
}

QSize RDRenderWidget::hintSize()const
{
    return QSize(m_nProjWidth,m_nProjHeight);
}

void RDRenderWidget::paintEvent(QPaintEvent* event)
{
    static double startTime = GetTime();
    double nowTime = GetTime();
    qDebug() << nowTime - startTime;
    startTime = nowTime;
    QPainter painter(this);
    m_swapChain.Lock();
    RDBuffer* pBuffer = m_swapChain.GetFrontBuffer();
    QImage* pPixmap = pBuffer->GetBuffer();
    QRectF target(event->rect());
    qDebug() << "target" << target;
    painter.drawImage(target,*pPixmap,target);
    m_swapChain.UnLock();
}


void RDRenderWidget::resizeEvent(QResizeEvent* event)
{
    const QSize& newSize = event->size();

    m_document.Lock();
    g_bForceUpdate = true;
    m_fScale = min(newSize.width() / (float)m_nProjWidth,newSize.height() / (float)m_nProjHeight);
    m_nXOffset = (newSize.width() - GetRealPorjWidth()) / 2;
    m_nYOffset = (newSize.height() - GetRealPorjHeight()) / 2;

    m_validRt.setLeft(0);
    m_validRt.setTop(0);
    m_validRt.setBottom(GetRealPorjHeight());
    m_validRt.setRight(GetRealPorjWidth());
    
    m_swapChain.Resize(newSize.width(),newSize.height());
    m_document.UnLock();
    m_document.SetScale(m_fScale);
}

bool RDRenderWidget::event(QEvent* e)
{
    RDBaseTool* pCurTool = RDToolManager::GetToolManager()->GetCurTool();
    bool bProcess = false;
    switch(e->type())
    {
    case QEvent::MouseMove:
        {
            QMouseEvent* pEvent = dynamic_cast<QMouseEvent*>(e);
            float3 vMousePt(ClientToScene(pEvent->pos()));
            float3 vScenePt;
            RDBufferToScene(vScenePt,vMousePt,-m_nProjWidth / 2.f,m_nProjHeight / 2.f);
            bProcess = pCurTool->OnMouseMove(vScenePt,pEvent->buttons());
        }
        break;
    case QEvent::MouseButtonPress:
        {
            QMouseEvent* pEvent = dynamic_cast<QMouseEvent*>(e);
            float3 vMousePt(ClientToScene(pEvent->pos()));
            float3 vScenePt;
            RDBufferToScene(vScenePt,vMousePt,-m_nProjWidth / 2.f,m_nProjHeight / 2.f);
            qDebug() << "Mouse Press Pt in Scene Buffer" << vMousePt.GetX() << vMousePt.GetY() << vMousePt.GetZ();
            qDebug() << "Mouse Press Pt in Scene" << vScenePt.GetX() << vScenePt.GetY() << vScenePt.GetZ();
            bProcess = pCurTool->OnMousePress(pEvent->buttons() ,vScenePt ) ;
            if(bProcess)
                QWidget::event(e);
        }
        break;
    case QEvent::MouseButtonRelease:
        {
            QMouseEvent* pEvent = dynamic_cast<QMouseEvent*>(e);
            float3 vMousePt(ClientToScene(pEvent->pos()));
            float3 vScenePt;
            RDBufferToScene(vScenePt,vMousePt,-m_nProjWidth / 2.f,m_nProjHeight / 2.f);
            bProcess = pCurTool->OnMouseRelease(pEvent->buttons() ,vScenePt ) ;
        }
        break;
    case QEvent::KeyPress:
        {
            QKeyEvent* pEvent = dynamic_cast<QKeyEvent*>(e);
            bProcess = pCurTool->OnKeyPress(pEvent->key()) ;
        }
        break;
    default:
        return QWidget::event(e);
    }
    return bProcess || QWidget::event(e);
}

float3 RDRenderWidget::ClientToScene(const QPoint& pos)
{
    float3 vRet(pos.x() - m_nXOffset,pos.y() - m_nYOffset,0);
    vRet /= m_fScale;
    return vRet;
}
void RDRenderWidget::keyPressEvent( QKeyEvent * event ) 
{
    qDebug() << "RenderWidget key down";
    if(event->key() == Qt::Key_Delete)
    {
		for(size_t i = 0; i < m_document.GetSelItemCount(); i++)
			emit DelNoded(*m_document.GetSelItem(i));
        m_document.DelSelItems();
        RDEditerManager::GetEditerManager().UpdateProperty(0);
        return;
    }
    QWidget::keyPressEvent(event);
}