// =====================================================================================
// 
//       Filename:  RDRenderManager.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  12/04/11 21:02:31
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Huang Jianchao (), hjc0720@gmail.com
//        Company:  
// 
// =====================================================================================
#include <QPointF>
#include "RDRenderManager.h"
#include "RDSceneRenderData.h"
#include "RDScene.h"
#include "RDBuffer.h"
#include <QRectF>
//#include "HVector3f.h"

bool RenderManager::RenderScene(const QPointF& offSceneToScreen,const QRectF& rtScene,unsigned long nTime)
{
    if((!m_pDstBuffer) || m_RenderName.isEmpty())
        return false;
    RDRenderData* pSceneRD = m_pScene->GetRenderData(m_RenderName);
    if(m_pScene->GetMaxChangeLevel(m_RenderName) == RDRender_NoChange && nTime == pSceneRD->GetTime())
        return false;
    m_pScene->CalFrame(nTime,m_RenderName);
    m_pScene->Render(nTime,m_RenderName);
    QRectF dst(rtScene);
    dst.translate(offSceneToScreen);
    QRectF src(rtScene);
    src.translate(-rtScene.left(),-rtScene.top());
    m_pDstBuffer->Draw(dst,pSceneRD->m_RenderBuffer,src);
    return true;
}