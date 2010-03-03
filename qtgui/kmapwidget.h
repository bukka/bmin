/*
 *
 * created date: 2/20/2010
 *
 * Copyright (C) 2009-2010 Jakub Zelenka.
 *
 * Bmin is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * Bmin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with Bmin; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.
 */

#ifndef KMAPWIDGET_H
#define KMAPWIDGET_H

#include "modulewidget.h"
// kernel
#include "outputvalue.h"
#include "kmap.h"

#include <vector>
#include <list>

#include <QWidget>
#include <QGraphicsWidget>
#include <QRectF>
#include <QString>
#include <QFont>

class GUIManager;
class Kernel;
class QPainter;
class QGraphicsScene;
class QGraphicsView;
class QGraphicsGridLayout;
class QStyleOptionGraphicsItem;
class QGraphicsSceneContextMenuEvent;
class QGraphicsSceneMouseEvent;

// Class for head row in k-map
class KMapHeadWidget : public QGraphicsWidget
{
    Q_OBJECT

public:
    enum Direction { VERTICAL, HORIZONTAL, CORNER };
    enum Mode { LINES, BINARY };

    static const int SIZE         = 50;
    static const int FONT_SIZE    = 10;
    static const int LINE_SIZE    = 2;
    static const int LINE_SPACING = 14;


    KMapHeadWidget(Mode m, Direction d, QGraphicsItem *parent = 0);

    void setMode(Mode m);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) = 0;

protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    Mode m_mode;
    Direction m_direction;

    unsigned m_varsCount;
    qreal m_width;
    qreal m_height;

    QFont m_font;

private:
    QAction *m_linesAction;
    QAction *m_binaryAction;

private slots:
    void setLinesMode();
    void setBinaryMode();

signals:
    void modeChanged(KMapHeadWidget::Mode);
};

// Class for showing variables next to description in line mode
class KMapLineVarsWidget : public KMapHeadWidget
{
public:
    KMapLineVarsWidget(std::vector<char> *vars, Direction d, QGraphicsItem *parent = 0);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setVars(std::vector<char> *vars) { m_vars = vars; }

private:
    std::vector<char> *m_vars;
};

// Class for showing variables next to description in line mode
class KMapBinaryVarsWidget : public KMapHeadWidget
{
public:
    KMapBinaryVarsWidget(std::vector<char> *hVars, std::vector<char> *vVars, QGraphicsItem *parent = 0);\

    void setVars(std::vector<char> *hVars, std::vector<char> *vVars);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    std::vector<char> *m_hVars;
    std::vector<char> *m_vVars;

    QString m_hVarsStr;
    QString m_vVarsStr;

    QPoint m_hVarsPoint;
    QPoint m_vVarsPoint;
};


// Class for head description of cells
class KMapDescWidget : public KMapHeadWidget
{
public:
    KMapDescWidget(int code, unsigned vc, qreal size, Mode m, Direction d, QGraphicsItem *parent = 0);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    int m_code;
    QString m_codeStr;
};

// Class for cell in k-map
class KMapCellWidget : public QGraphicsWidget
{
    Q_OBJECT

public:
    static const qreal SIZE    = 35.0;
    static const qreal BORDER  = 2.0;
    static const int FONT_SIZE = 10;
    static const int PADDING   = 4;

    KMapCellWidget(QGraphicsItem *parent = 0);
    void setWalls(bool top, bool bottom, bool left, bool right);
    void setValue(char value);
    bool hasCoverPos(unsigned pos);
    unsigned addCover(KMapCell &cell, unsigned pos = 0);
    void setLastCoverPos(unsigned pos);
    inline void setIdx(int idx) { m_idx = idx; }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    inline qreal getCellWidth() { return m_cellWidth; }
    inline qreal getCellHeight() { return m_cellHeight; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    // walls
    bool m_topWall;
    bool m_bottomWall;
    bool m_leftWall;
    bool m_rightWall;

    qreal m_cellWidth;
    qreal m_cellHeight;
    int m_penSize;
    int m_wallSize;
    int m_cellSize;

    std::list<KMapCell> m_covers;

    // value
    char m_value;
    // value string
    QString m_valueStr;
    // text font
    QFont m_font;
    // cell index
    int m_idx;

signals:
    void cellValueChanged(int, OutputValue &);
};

// Class for k-map grid
class KMapGridWidget : public QGraphicsWidget
{
    Q_OBJECT

public:
    KMapGridWidget(KMap *kmap, KMapHeadWidget::Mode mode, QGraphicsItem *parent = 0);
    ~KMapGridWidget();

    void setMapData(KMap *data);
    void showCovers();

private:
    unsigned m_varsCount;
    unsigned m_rowsCount;
    unsigned m_colsCount;
    KMap *m_kmap;

    std::vector<char> m_vars;
    // selecting mode
    KMapHeadWidget::Mode m_mode;
    // main layout
    QGraphicsGridLayout *m_layout;
    // map of cell widgets
    KMapCellWidget **m_map;
    // head desc widgets
    KMapDescWidget **m_vDesc; // vertical
    KMapDescWidget **m_hDesc; // horizontal
    // head vars description widget
    KMapLineVarsWidget *m_vVars;
    KMapLineVarsWidget *m_hVars;
    KMapBinaryVarsWidget *m_binVars;

public slots:
    void setMode(KMapHeadWidget::Mode mode);
};

// Class for Karnaugh Map
class KMapWidget : public ModuleWidget
{
    Q_OBJECT

public:
    // default head mode
    static const KMapHeadWidget::Mode DEFAULT_MODE = KMapHeadWidget::BINARY;

    KMapWidget(const QString &name, int pos);
    ~KMapWidget();


private:
    GUIManager *m_gm;
    Kernel *m_kernel;
    KMap *m_kmap;
    // array with grid widget (for one variable count one widget)
    KMapGridWidget **m_gridWidgets;

    // actual widget in scene
    QGraphicsItem *m_mainItem;
    // scene
    QGraphicsScene *m_scene;
    // view
    QGraphicsView *m_view;

    bool m_active;
    unsigned m_varsCount;

    KMapHeadWidget::Mode m_mode;

public slots:
    void setActivity(bool active);
    void updateData();
};

#endif // KMAPWIDGET_H
