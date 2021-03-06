/*
 * =====================================================================================
 *
 *       Filename:  RDCreateObj.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/04/2011 09:05:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include "RDCreateObj.h"
#include "RDImageObject.h"
#include <QString>
#include <map>
#include <QDebug>
#include "jsonhelper.h"

std::map<std::string,pCreateObj>* g_pCreateObj = nullptr;
std::map<std::string,pCreateNode>* g_pCreateNode = nullptr;
void RegisterObj(const std::string &pStr, pCreateObj pFun)
{
    if(!g_pCreateObj)
        g_pCreateObj = new std::map<std::string,pCreateObj>;
    (*g_pCreateObj)[pStr] = pFun;
    qDebug()<<"register obj creator"<<pStr;
}

void UnRegisterObj(const std::string &pStr)
{
    g_pCreateObj->erase(pStr); 
    if(g_pCreateObj->empty())
        SAFE_DELETE(g_pCreateObj);
}

void RegisterNode(const std::string &pStr, pCreateNode pFun)
{
    if(!g_pCreateNode)
        g_pCreateNode= new std::map<std::string,pCreateNode>;
    (*g_pCreateNode)[pStr] = pFun;
    qDebug()<<"register obj node"<<pStr;
}

void UnRegisterNode(const std::string &pStr)
{
    g_pCreateNode->erase(pStr); 
    if(g_pCreateNode->empty())
        SAFE_DELETE(g_pCreateNode);
}

RDObject* CreateObj(const std::string& strType)
{
    qDebug() << "creat Obj" << strType;
    auto it = g_pCreateObj->find(strType);
    if(it == g_pCreateObj->end())
        return nullptr;
    return it->second();
}

RDNode* CreateNode(const std::string& strType)
{
    qDebug() << "creat node" << strType;
    auto it = g_pCreateNode->find(strType);
    if(it == g_pCreateNode->end())
        return nullptr;
    return it->second();
}

