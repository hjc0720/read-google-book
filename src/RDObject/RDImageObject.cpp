// =====================================================================================
// 
//       Filename:  RDImageObject.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  26/06/11 18:45:38
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Huang Jianchao (), hjc0720@gmail.com
//        Company:  
// 
// =====================================================================================
#include "RDImageObject.h"
#include "RDImageResource.h"
#include "RDResourceManager.h"
#include "RDRenderData.h"
#include "mac_define.h"
#include <QRectF>
#include "RDNode.h"
#include "RDSpaceConvert.h"
#include <QDebug>
#include "RDFileDataStream.h"
#include "HVector4f.h"
#include "RDRenderDevice.h"
#include "RDModel.h"
#include "RDTexture.h"
#include "RDModelResource.h"
#include "RDCamera.h"

class RDModel;
class RDImagePrivateData: public RDRenderPrivateData
{
public:
    RDImagePrivateData(){
        m_pSegModel = nullptr;
        m_pImage = nullptr;
        m_pVertexShader = nullptr;
        m_pPixelShader = nullptr;
        m_pShaderProgram = nullptr;
    }
public:
    RDModelResource*            m_pSegModel;
    const RDTexture*    m_pImage; 
    RDShader*           m_pVertexShader;    
    RDShader*           m_pPixelShader;    
    RDShaderProgram*    m_pShaderProgram;
    matrix4x4          m_vRenderMatrix;
};

RDImageObject::RDImageObject()
    :RDObject(RDObjectImage)
{
}

RDImageObject::RDImageObject(const QString& fileName)
    :RDObject(RDObjectImage)
{
    RDResourceManager* pResManger = RDResourceManager::GetResourceManager();
    m_Image = pResManger->AddResource(fileName,RDResource_Image)->GetMd5();
}

RDImageObject::RDImageObject(const RDMd5& image)
    :RDObject(RDObjectImage)
     ,m_Image(image)
{
    RDResourceManager* pResManager = RDResourceManager::GetResourceManager();
    pResManager->AddResource(m_Image);
    m_nWidth = 0;
    m_nHeight = 0;
}


RDImageObject::~RDImageObject()
{
    RDResourceManager* pResManager = RDResourceManager::GetResourceManager();
    pResManager->RemoveResource(&m_Image);
}


void RDImageObject::Render(unsigned long ,RDRenderData& RenderData) 
{
    RDImagePrivateData* pPrivateData = dynamic_cast<RDImagePrivateData*>( RenderData.GetPrivateData());
    RDRenderDevice* pDevice = RDRenderDevice::GetRenderManager();
    pDevice->SetShader(pPrivateData->m_pShaderProgram);
    const RDModel* pModel = pPrivateData->m_pSegModel->GetModel();

    pDevice->SetShaderTexture(pPrivateData->m_pShaderProgram,"DiffuseTex",pPrivateData->m_pImage);
    
    matrix4x4 WVP = pPrivateData->m_vRenderMatrix * RenderData.GetMVPMatrix();
    float4 vTemp(-1,1,0,1);
    vTemp *= WVP;
    vTemp.DividW();
    pDevice->SetShaderParam(pPrivateData->m_pShaderProgram,"MVP",WVP);

    for(size_t i = 0; i <  pModel->GetSubsetCount(); i++)
        pModel->DrawSubset(i);
    qDebug() << "image render";
    return;
}
void RDImageObject::CalFrame(const RDTime& ,RDRenderData& RenderData) 
{
    RDImagePrivateData* pPrivateData = dynamic_cast<RDImagePrivateData*>( RenderData.GetPrivateData());
    if(RenderData.GetChangeLevel() >= RDRender_TransChange)
    {
        pPrivateData->m_vRenderMatrix = matrix4x4(m_nWidth / 2,m_nHeight / 2,1,HMatrixQ4F_Scale);
    }
}

bool RDImageObject::HitTest(const float3& vScenePt,const RDNode& pNode,const QString& RDName) const
{
    const RDRenderData* pRenderData = pNode.GetRenderData(RDName);
    const RDImagePrivateData* pPrivateData = dynamic_cast<const RDImagePrivateData*>( pRenderData->GetPrivateData());
    RDCamera* pCamera = pNode.GetCamera(RDName);
    matrix4x4 worldMat = pPrivateData->m_vRenderMatrix * pRenderData->GetGlobalMatrix();
    RDSpaceParam param(&worldMat,&pCamera->GetViewMatrix(RDName),&pCamera->GetEditProjMatrix(RDName),pNode.GetSceneRt(RDName));
    const RDModel* pModel = pPrivateData->m_pSegModel->GetModel();
    float3 vHitPt;

    return pModel->HitTest(vHitPt,vScenePt,param);
}

void RDImageObject::Save(RDFileDataStream& buffer)
{
	RDObject::Save(buffer);
	buffer.writeRawData( (char*)&m_Image,sizeof(RDMd5));
	RDImageResource* pResource = dynamic_cast<RDImageResource*>(RDResourceManager::GetResourceManager()->GetResource(m_Image));
	buffer.SaveResource(pResource->GetPath());
	buffer << m_nWidth;
	buffer << m_nHeight;
}

void RDImageObject::Load(RDFileDataStream& buffer)
{
	qDebug() << "begine load image";
	RDObject::Load(buffer);
	buffer.readRawData( (char*)&m_Image,sizeof(RDMd5));
	buffer >> m_nWidth;
	buffer >> m_nHeight;
    RDResourceManager::GetResourceManager()->AddResource(m_Image);
	qDebug() << "end load image";
}
const RDMd5& RDImageObject::GetObjMd5()const 
{
    static RDMd5 ImageMd5("RDImageObject", -1);
    return ImageMd5;
}

void RDImageObject::UpdateBound(const RDTime& ,RDRenderData& RenderData) 
{
    float3 bufferPos;
    RDSceneToBuffer(bufferPos,RenderData.GetPos() ,-(float)RenderData.GetSceneWidth()/ 2,RenderData.GetSceneHeight()/ 2);
    RenderData.SetBound( QRectF(bufferPos.x(),bufferPos.y(),m_nWidth,m_nHeight));

    RenderData.SetMin(float3(-m_nWidth / 2,-m_nHeight / 2,0));
    RenderData.SetMax(float3(m_nWidth / 2,m_nHeight / 2,0));
}

void RDImageObject::CreateRenderData(RDRenderData& pRD)
{
    RDImagePrivateData* pPrivateData = dynamic_cast<RDImagePrivateData*>( pRD.GetPrivateData());
    if(!pPrivateData)
    {
        pPrivateData = new RDImagePrivateData;
        pRD.SetPrivateData(pPrivateData);
    }
    RDResourceManager* pResManager = RDResourceManager::GetResourceManager();
    pPrivateData->m_pSegModel = dynamic_cast<RDModelResource*>(pResManager->AddModelResource("segment"));
    RDImageResource* pResource = dynamic_cast<RDImageResource*>(pResManager->GetResource(m_Image));
    pPrivateData->m_pImage = pResource->GetBuffer();

    RDRenderDevice* pDevice = RDRenderDevice::GetRenderManager();
    pPrivateData->m_pVertexShader = pDevice->CreateShader(":/shader/model_vs",VertexShader);    
    pPrivateData->m_pPixelShader = pDevice->CreateShader(":/shader/main_ps",FragmentShader);    
    pPrivateData->m_pShaderProgram = pDevice->CreateShaderProgram(pPrivateData->m_pVertexShader,nullptr,pPrivateData->m_pPixelShader);
}
