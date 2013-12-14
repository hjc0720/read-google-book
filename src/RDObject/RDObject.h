// =====================================================================================
// 
//       Filename:  RDObject.h
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  26/03/11 21:47:46
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Huang Jianchao (), hjc0720@gmail.com
//        Company:  
// 
// =====================================================================================

#ifndef  RDOBJECT_INC
#define  RDOBJECT_INC

#include <QMutex>
#include <QString>
#include <QUuid>
#include "HVector4f.h"
#include <list>
#include "mac_define.h"

class RDRenderData;
class RDFileDataStream;
class RDNode;
class RDMd5;

//cann't not change the type order, because the type will to save
class RDObject
{
public:
    RDObject();
    virtual ~RDObject(){}
    void Lock()const;
    void UnLock()const;
    virtual const RDMd5& GetObjMd5()const = 0;
    virtual void CreateRenderData(RDRenderData& RenderData) = 0;
    virtual void ReleaseRenderData(RDRenderData& RenderData) = 0;
    virtual void Render(unsigned long nTime,RDRenderData& RenderData) = 0;
    virtual void CalFrame(const RDTime& nSectionTime,RDRenderData& RenderData) = 0;
    virtual void UpdateBound(const RDTime& nTime,RDRenderData& RenderData) = 0;
    const QUuid& GetObjID()const{return m_ObjID;}
    void SetNode(RDNode* pParent){m_pParent = pParent;} 
    RDNode* GetNode(){return m_pParent;}

    virtual void Serialize(RDFileDataStream& buffer,bool bSave);

    virtual bool HitTest(const float3& vScenePt,const RDNode& pNode,const QString& RDName)const = 0;

protected:
    QUuid  m_ObjID;
    RDNode* m_pParent;
};

#endif   // ----- #ifndef rdobject_INC  -----
