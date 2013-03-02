/*
 * =====================================================================================
 *
 *       Filename:  RDShaderProgram.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/16/13 16:09:05
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  HuanJianChao (), hjc0720@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "mac_define.h"
# include <GL/gl.h>
#include <QString>
#include <GL/glext.h>
#include "RDRenderDevice.h"

class QString;

class RDShader
{
public:
    RDShader(const QString &code, const QString &shaderName, RDShaderType nType);

    void AddRef(){m_nRef++;}
    bool Release(){
        m_nRef--;
        if(m_nRef == 0) 
        {
            delete this;
            return true;
        }
        return false;
    }
    const QString& ShaderName()const{return m_shaderName;}
    GLuint GetGLShader()const{return m_hShader;}
protected:
    ~RDShader() { 
        glDeleteShader(m_hShader);
    }
protected:
    int         m_nRef;
    GLuint      m_hShader;
    QString     m_shaderName;
};

class RDShaderProgram
{
public:
    RDShaderProgram(const QString& shaderName,RDShader* pVertexShader,RDShader* pGeometryShader,RDShader* pFragmentShader);
    bool Release(){
        m_nRef--;
        if(m_nRef == 0) 
        {
            delete this;
            return true;
        }
        return false;
    }

    void SetShader(RDShader* pShader,RDShaderType nType);
    void link();
    void AddRef(){m_nRef++;}

    void use(){
        glUseProgram(m_hShaderProgram);
    }
    GLint GetUniformLocation( const char *name);
protected:
    ~RDShaderProgram(){glDeleteProgram(m_hShaderProgram);}
protected:
    RDShader*   m_pShader[3];
    int         m_nRef;
    GLuint      m_hShaderProgram;
    QString     m_ProgramName;
};

