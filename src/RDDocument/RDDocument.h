// =====================================================================================
// 
//       Filename:  RDDocument.h
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  26/03/11 13:57:06
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Huang Jianchao (), hjc0720@gmail.com
//        Company:  
// 
// =====================================================================================
#ifndef  RDDOCUMENT_INC
#define  RDDOCUMENT_INC
#include <vector>
#include <QString>
#include <QUuid>
#include <stack>
#include <QUndoStack>
#include "RDProject.h"
#include "mac_define.h"
#include <mutex>


class RDSceneRenderData;
class RDRenderData;
class RDNode;
class RDScene;
class RDUndoCommand;


enum RDEditItemType
{
    RDEdit_ModifyItem,
    RDEdit_AutoKey,
    RDEdit_NonAutoKey,
};

//RDDocument多线程不安全，不能在多线程中使用，除了部分函数
class RDDocument
{
public:
    RDDocument(bool bCreateNewProj);
    ~RDDocument();
    bool bHavePath();
    void SetProjPath(const QString& path);
    const QString& GetProjPath();
    //save load function
    bool bOpenProj()const{return m_pProject;}
    void SaveProj();
    void SaveProjAs(const QString& filePath);
    void LoadProj(const QString& filePath);
    void CloseProj();

    void SetCurScene(int nSceneIndex);
    RDScene*  GetCurScene(){return m_pProject->GetScene(m_nCurSceneIndex);}

    const RDNode* GetTopNode()const{return m_EditRDStack.top();}
    RDNode* GetTopNode(){return m_EditRDStack.top();}
    void PushTopNode(RDNode* pTopNode){ m_EditRDStack.push(pTopNode);}
    const RDNode* PopTopNode()
    {
        RDNode* pNode = GetTopNode();
        m_EditRDStack.pop();
        return pNode;
    }

    size_t  GetSelItemCount()const{return m_SelItemList.size();}
    RDNode* GetSelItem(int i)const{return m_SelItemList[i];}
    void AddSelItem(RDNode& pSelItem){m_SelItemList.push_back(&pSelItem);}
    void ClearSelItem(){ m_SelItemList.clear();}
    void DelSelItems();
    RDEditItemType GetEditType()const{return m_nEditType;}

    void SetScale(float fScale);

    const RDTime& GetCurTime()const{return m_nCurFrame;}
    void SetCurTime(const RDTime& nFrame){m_nCurFrame = nFrame;}
    void  AddChildNode(RDNode& parent,RDNode& pChild);
    RDNode* GetNode(const QUuid& NodeId);

	//undo redo
    QUndoStack* GetUndoStack(){return &m_UndoStack;}
	void UndoBegin(const QString& text);
	void UndoEnd();
    void AddUndoCommand(RDUndoCommand* cmd);

	//story
	void AddStoryAndTrigger(const std::string& StoryName);
	void TriggerStory(size_t nIndex);
	bool RemoveStoryAndTrigger(size_t nIndex);

protected:
    void SaveProj(RDProject& pProj);
    void SaveProjAs(RDProject& pProj,const QString& filePath);
    void CreateTempProjDir();
    void DeleteTempProjDir();
    void TarProjDir(const QString& strProjPath);
    void UntarProjDir(const QString& strProjPath);
    void ClearRDStack();

protected:
    RDEditItemType  m_nEditType;
    int             m_nCurSceneIndex;
    float           m_fScale;
    RDProject*      m_pProject; 
    RDTime         m_nCurFrame;
    std::stack<RDNode*>   m_EditRDStack;
    std::vector<RDNode*>    m_SelItemList;
    QString         m_strCachePath;//have no '/'
    QUuid           m_DocUUID;
    QUndoStack      m_UndoStack;

    //std::vector<RDProject*> m_ProjList;
};

#endif   // ----- #ifndef rddocument_INC  -----
