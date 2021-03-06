/*
 * =====================================================================================
 *
 *       Filename:  RDImageCell.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年04月25日 14时50分21秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Huang JianChao (hjc), hjc0720@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  RDIMAGECELL_INC
#define  RDIMAGECELL_INC

#include "RDCell.h"
#include <QIcon>

enum RDImageCellChange
{
    RDImageFileChange,
    RDImageSizeChange,
};

class QLineEdit;
class RDSpinBox;
class QLabel;

class RDImageCell : public RDCell
{
    Q_OBJECT
public:
    static RDImageCell* GetImageCell();
    virtual const RDMd5& GetCellMd5();

    void    SetImageFileName(const QString& fileName);
    QString GetImageFileName()const;

    void    SetImageSize(int nWidth,int nHeight);
    void    SetImageOriginSize(int nWidth,int nHeight);
    int     GetImageWidth()const;
    int     GetImageHeight()const;
protected slots:
    void GetImageFile();
    void OnKeepWidth();
    void OnKeepHeight();
    void OnOriginSize();
    void onImageSizeChange();
protected:
    RDImageCell(QWidget* parent);
protected:
    bool            m_bSetValue;
    QLineEdit*      m_pImageFile;
    QLabel*         m_pResolution;
    RDSpinBox*       m_pWidth;
    RDSpinBox*       m_pHeight;
};
#endif   /* ----- #ifndef RDImageCell_INC  ----- */

