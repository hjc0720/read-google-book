//email : hjc0720@gmail.com
//animation software

#include "RDMaterial.h"
#include "mac_define.h"
#include <QRectF>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "RDFileDataStream.h"
#include <fstream>

const int MAT_PARAM_INDEX = 1;
const int MAT_TEX_PARAM_START = 2;

class RDMatShaderCode
{
public:
    RDMatShaderCode(const std::string& filePath){
        if(!LoadCode(filePath))  m_pPath = new std::string(filePath);
        else                    m_pPath = nullptr;
    }
    ~RDMatShaderCode(){ SAFE_DELETE(m_pPath);}
    const std::string& GetStr(){
        if(m_pPath){
            LoadCode(*m_pPath);
            SAFE_DELETE(m_pPath);
        }
        return m_strCode;
    }
protected:
    bool LoadCode(const std::string& filePath){
        QFile shader(filePath.data());
        if(shader.open(QIODevice::ReadOnly)){
            QTextStream stream(&shader);
            m_strCode = stream.readAll().toStdString();
            return true;
        }
        return false;
    }
protected:
    std::string * m_pPath;
    std::string m_strCode;
};

RDMatShaderCode MainMatCode(":/shader/main_ps");
RDMatShaderCode TexMatCode[] = {std::string(":/shader/diffuse_ps")};
RDMatShaderCode ParamMatCode[] = {std::string(":/shader/diffusebuffer")};
RDMatShaderCode LightParamCode(":/shader/lightbuffer");
RDMatShaderCode LightCode(":/shader/light_ps");

RDMatTexture::RDMatTexture()
    :m_bFileTex(true)
{
    InitData();
}

RDMatTexture::RDMatTexture(const std::string& strFileName)
     :m_strFile(strFileName)
    ,m_bFileTex(true)
      ,m_bReleaseTex(true)
{
    InitData();
    RDRenderDevice* pDevice = RDRenderDevice::GetRenderManager();
    m_hTex = pDevice->CreateTexture(strFileName);
}

RDMatTexture::RDMatTexture(const uint* pBuffer,int nWidth,int nHeight)
    :m_bFileTex(false)
     ,m_bReleaseTex(true)
{
    InitData();
    RDRenderDevice* pDevice = RDRenderDevice::GetRenderManager();
    m_hTex = pDevice->CreateTexture(nWidth,nHeight,pBuffer,RDNormal2DTexture);
}

RDMatTexture::RDMatTexture(const RDTexture*  hTex,const QRectF& texBound)
     :m_bReleaseTex(false)
{
    m_hTex = const_cast<RDTexture*>(hTex);
    RDRenderDevice* pDevice = RDRenderDevice::GetRenderManager();
    int nWidth = pDevice->GetTextureWidth(hTex);
    int nHeight = pDevice->GetTextureHeight(hTex);
    m_vTexCenter.x = 0;
    m_vTexCenter.y = 0;

    m_vTexOffset.x = texBound.left() / nWidth;
    m_vTexOffset.y = texBound.top() / nHeight;

    m_vTexScale.x = texBound.width() / nWidth;
    m_vTexScale.y = texBound.height() / nHeight;
    
    m_fRotate = 0;
    m_fAlpha = 1;

    m_pTexParam = nullptr;
}

RDMatTexture::~RDMatTexture()
{
    RDRenderDevice* pDevice = RDRenderDevice::GetRenderManager();
    if(m_bReleaseTex)
        pDevice->ReleaseTexture(m_hTex);
    pDevice->ReleaseUniformBufferObject(m_pTexParam);
}

void        RDMatTexture::InitData()
{
    m_vTexOffset.x = 0;
    m_vTexOffset.y = 0;
    m_vTexCenter.x = 0;
    m_vTexCenter.y = 0;
    m_fRotate = 0;
    m_vTexScale.x = 1;
    m_vTexScale.y = 1;
    m_hTex = nullptr;
    m_pTexParam = nullptr;
    m_fAlpha = 1;
}

void            RDMatTexture::SetParamToDevice(int nIndex,RDRenderDevice* pDevice )
{
    pDevice->SetShaderTexture(nIndex,m_hTex);
    pDevice->SetShaderParam(MAT_TEX_PARAM_START + nIndex,m_pTexParam);
}

void            RDMatTexture::UpdateFrame(RDRenderDevice* pDevice ,const RDTime& )
{
    m_matTex = matrix4x4(m_vTexCenter,HMatrixQ4F_POS) * matrix4x4(m_vTexScale.x,m_vTexScale.y,1,HMatrixQ4F_Scale) 
        * matrix4x4(0,0,m_fRotate,HMatrixQ4F_Rotate) * matrix4x4(m_vTexOffset,HMatrixQ4F_POS); 
    float vec[5 * 4];
    memcpy(vec,m_matTex.data(),4*4*sizeof(float));
    vec[4*4] = m_fAlpha;
    
    if(!m_pTexParam)
        m_pTexParam = pDevice->CreateUniformBufferObject(5*4,vec);
    else
        pDevice->ModifyUniformBufferObject(m_pTexParam,vec);
}
////////////////////////////////////////////////////////////////////////////////
RDMaterial::RDMaterial()
{
    memset(m_MatTexture,0,sizeof(RDMatTexture*) * RDMatTextureCount);
    m_pShader = nullptr;
    m_nNowTime = 0;
    m_nChange = MAT_NEW_CHANGE;
    m_pMatParam = nullptr;
}

RDMaterial::RDMaterial(bool bEnableLight,unsigned int color)
    :m_bEnableLight(bEnableLight)
     ,m_vDiffuse(color)
     ,m_vAmbient(0xffffffff)
     ,m_vSpecular(color)
     ,m_fShine(32)
{
    memset(m_MatTexture,0,sizeof(RDMatTexture*) * RDMatTextureCount);
    m_nNowTime = 0;
    m_nChange = MAT_NEW_CHANGE;
    m_pShader = nullptr;
    m_pMatParam = nullptr;
}

void RDMaterial::AddTex(RDMatTextureType nTexType,const std::string& fileName)
{
    if(m_MatTexture[nTexType])
    {
        if(fileName != m_MatTexture[nTexType]->GetFileName())
            SAFE_DELETE(m_MatTexture[nTexType]);
    }
    if(!m_MatTexture[nTexType])
    {
        m_MatTexture[nTexType] = new RDMatTexture(fileName);
        SetChange(RD_ADD_TEXTURE);
    }
}

void RDMaterial::AddTex(RDMatTextureType nTexType,const uint* pBuffer,int nWidth,int nHeight)
{
    if(m_MatTexture[nTexType])
        SAFE_DELETE(m_MatTexture[nTexType]);

    m_MatTexture[nTexType] = new RDMatTexture(pBuffer,nWidth,nHeight);
    SetChange(RD_ADD_TEXTURE);
}

void RDMaterial::AddTex(RDMatTextureType nTexType,const RDTexture*  hTex,const QRectF& texBound)
{
    if(m_MatTexture[nTexType])
    {
        if(m_MatTexture[nTexType]->GetTex() == hTex)
            return;
        SAFE_DELETE(m_MatTexture[nTexType]);
    }
    m_MatTexture[nTexType] = new RDMatTexture(hTex,texBound);
    SetChange(RD_ADD_TEXTURE);
}

bool RDMaterial::UpdateFrame(const RDTime& time,char ,char ,char )
{
    if(m_nNowTime == time && m_nChange == RD_MAT_NO_CHANGE)
        return false;
    if(CheckChange(RD_Mat_Param))
        GenerateMatParam();
    if(CheckChange(RD_ADD_TEXTURE))
        CreateShader();
    if(CheckChange(RD_ADD_TEXTURE))
    {
        RDRenderDevice* pDevice = RDRenderDevice::GetRenderManager();
        for(int i = 0; i < RDMatTextureCount;i++)
        {
            if(m_MatTexture[i])
                m_MatTexture[i]->UpdateFrame(pDevice,time);
        }
    }
    ClearChange();
    return true;
}

bool RDMaterial::CheckChange( RDMatChangeType nType)
{
    return (m_nChange & nType) != 0;
}

int  RDMaterial::SetChange(int nChangeType)
{
    m_nChange |= nChangeType;
    return m_nChange;
}

const std::string texShaderName[] =
{
    "Normal_",
};

void RDMaterial::CreateShader()
{
    std::string shaderName;
    for(int i = 0; i < RDMatTextureCount; i++)
    {
        if(m_MatTexture[i])
            shaderName += texShaderName[i];
    }
    shaderName += ("ps");
    if(m_strShaderName == shaderName)
        return;

    m_strShaderName = shaderName;
    RDRenderDevice* pDevice = RDRenderDevice::GetRenderManager();
    pDevice->ReleaseShader(m_pShader);
    m_pShader = nullptr;

    GenerateShader();
    m_pShader = pDevice->CreateShader(m_strShader,m_strShaderName,FragmentShader);
}

void RDMaterial::GenerateShader()
{
    m_strShader = MainMatCode.GetStr();
    int nParamIndex = m_strShader.find("//end param");

    if(m_bEnableLight)
        m_strShader.insert(nParamIndex,LightParamCode.GetStr());
    for(int i = 0;i < RDMatTextureCount; i++)
    {
        if(m_MatTexture[i])
            m_strShader.insert(nParamIndex,ParamMatCode[i].GetStr());
        qDebug() << "tex shader:\n"<< ParamMatCode[i].GetStr();
    }

    int nCodeIndex = m_strShader.find("//end code");

    if(m_bEnableLight)
        m_strShader.insert(nCodeIndex,LightCode.GetStr());
    for(int i = 0; i < RDMatTextureCount;i++)
    {
        if(m_MatTexture[i])
            m_strShader.insert(nCodeIndex,TexMatCode[i].GetStr());
    }
    size_t nIndex = m_strShader.find("POINT_COUNT");
    size_t length = strlen("POINT_COUNT");
    while(nIndex != std::string::npos)
    {
        m_strShader.replace(nIndex,length,"1");
        nIndex = m_strShader.find("POINT_COUNT",nIndex);
    }
}

void RDMaterial::SetParamToDevice()
{
    RDRenderDevice* pDevice = RDRenderDevice::GetRenderManager();
    //pDevice->SetShader(m_pShader,FragmentShader);

    pDevice->SetShaderParam(MAT_PARAM_INDEX ,m_pMatParam);
    for(int i = 0; i < RDMatTextureCount; i++)
    {
        if(m_MatTexture[i])
        {
            m_MatTexture[i]->SetParamToDevice(i,pDevice);
        }
    }
}

void    RDMaterial::GenerateMatParam()
{
    size_t nBufferSize = GenerateMatParamBuffer(nullptr);
    char* pBuffer = new char[nBufferSize];
    GenerateMatParamBuffer(pBuffer);
    if(m_pMatParam)
        RDRenderDevice::GetRenderManager()->ModifyUniformBufferObject(m_pMatParam,reinterpret_cast<float*>(pBuffer));
    else
        m_pMatParam = RDRenderDevice::GetRenderManager()->CreateUniformBufferObject(nBufferSize,reinterpret_cast<float*>(pBuffer));
}

size_t  RDMaterial::GenerateMatParamBuffer(char* pBuffer)
{
    if(pBuffer)
    {
        RDSaveData(pBuffer,m_vDiffuse);
        RDSaveData(pBuffer,m_vSpecular);
        RDSaveData(pBuffer,m_vAmbient);
        RDSaveData(pBuffer,m_fShine);
        RDSaveData(pBuffer,1.f);
    }
    return 16 * sizeof(float);
}
