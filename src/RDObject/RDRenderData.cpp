// =====================================================================================
// 
//       Filename:  RDRenderData.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  01/04/11 20:37:23
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Huang Jianchao (), hjc0720@gmail.com
//        Company:  
// 
// =====================================================================================

#include "RDRenderData.h"
#include "RDObject.h"
#include "RDSceneRenderData.h"
#include "RDNode.h"
#include "mac_define.h"
#include "RDScene.h"

RDRenderData::RDRenderData(const std::string& name,RDNode& node,const RDSceneRenderData& SceneRenderData)
        :m_RenderBuffer(0,0)
     ,m_ItemBuffer(0,0)
     ,m_strName(name)
     ,m_nChangeLevel(RDRender_New)
     ,m_nRenderChangeLevel(RDRender_New)
     ,m_Node(node)
     ,m_pPrivateData(0)
      ,m_pCurSection(0)
      ,m_SceneRenderData(SceneRenderData)
         ,m_nStoryTime(0)
      ,m_vScale(1,1,1)
{
    if(m_Node.GetObject())
        m_Node.GetObject()->CreateRenderData(*this);
    m_pParent = NULL;
}

RDRenderData::~RDRenderData()
{
    if(m_Node.GetObject())
        m_Node.GetObject()->ReleaseRenderData(*this);
    SAFE_DELETE(m_pPrivateData);
}

float RDRenderData::GetSceneScale()const
{ 
    return m_SceneRenderData.GetSceneScale();
}

RenderManager *RDRenderData::getRenderManager() const
{
    return m_SceneRenderData.getRenderManager();
}

void            RDRenderData::setChildChangeLevel(RDRenderChangeLevel nLevel)
{
    m_nChangeLevel = nLevel > m_nChangeLevel ? nLevel : m_nChangeLevel;
    for(size_t i = 0; i < m_Node.GetChildCount();i++)
    {
        RDRenderData* pRender = m_Node.GetChild(i)->GetRenderData(m_strName);
        pRender->setChildChangeLevel(nLevel);
    }
}

void RDRenderData::setChangeLevel(RDRenderChangeLevel nLevel)
{
    m_nChangeLevel = nLevel > m_nChangeLevel ? nLevel : m_nChangeLevel;
}

void RDRenderData::SetRenderChangeLevel(RDRenderChangeLevel nLevel)
{
    m_nRenderChangeLevel = nLevel > m_nRenderChangeLevel ? nLevel : m_nRenderChangeLevel;
}

const RDObject* RDRenderData::GetObject()const
{ 
    return m_Node.GetObject();
}
RDObject* RDRenderData::GetObject()
{ 
    return m_Node.GetObject();
}
void RDRenderData::Lock()const
{
    m_Node.Lock();
}
void RDRenderData::UnLock()const
{
    m_Node.UnLock();
}
bool RDRenderData::IsPlay()const
{
    return m_SceneRenderData.IsPlay();
}
const RDTime& RDRenderData::GetTime()const
{
    return m_SceneRenderData.GetTime() ;
}
const RDStory& RDRenderData::GetCurTrigStory()const
{
    return m_SceneRenderData.GetCurTrigStory();
}

uint RDRenderData::GetSceneWidth()
{
    return m_SceneRenderData.GetWidth();
}
uint RDRenderData::GetSceneHeight()
{
    return m_SceneRenderData.GetHeight();
}
float RDRenderData::GetSceneNowWidth()
{
    return m_SceneRenderData.GetNowWidth();
}
float RDRenderData::GetSceneNowHeight()
{
    return m_SceneRenderData.GetNowHeight();
}
