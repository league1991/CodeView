// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <vector>
#include <queue>

#include <qdebug.h>
#include <qmath.h>
#include <QtPlugin> 
#include <QStringList> 
#include <QMenu>
#include <QList>
#include <QQueue>
#include <QStandardItem>
#include <QSharedPointer>
#include <QWeakPointer>
#include <QThread>
#include <QVector2D>
#include <QVector3D>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsBlurEffect>
#include <QStyleOptionGraphicsItem>
#include <QScrollBar>
#include <QStaticText>
#include <QSet>
#include <QTimer>
#include <QFile>
#include <QTime>
#include <qmutex>
#include <QEvent.h>

#include <Eigen/Dense>
#include <Eigen/LU>
#include <Eigen/SVD>
#include <Eigen/SparseCore>

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/planarity/PlanarizationLayout.h>
#include <ogdf/planarity/VariableEmbeddingInserter.h>
#include <ogdf/planarity/FastPlanarSubgraph.h>
#include <ogdf/orthogonal/OrthoLayout.h>
#include <ogdf/planarity/EmbedderMinDepthMaxFaceLayers.h>
#include <ogdf/cluster/ClusterPlanarizationLayout.h>
#include <ogdf/cluster/ClusterOrthoLayout.h>
#include <ogdf/energybased/FMMMLayout.h>
#include <ogdf/energybased/FastMultipoleEmbedder.h>
#include <ogdf/misclayout/CircularLayout.h>

// understand
#include <udb.h>

// macros
#define IS_OUT_OF_RANGE(v, minV, maxV)   ((v) < (minV) || (v) > (maxV))
#define ARRAY_OUT_OF_RANGE(v, size)		 (IS_OUT_OF_RANGE(v,0,(int(size)-1)))

// my files
#include "globalSetting.h"

#include "GlobalUtility.h"
#include "svd/SVDSolver.h"
#include "mds/StressMDSLib.h"
#include "mds/MDSSolver.h"
#include "LaplacianSolver.h"
#include "pcasolver.h"
#include "GraphUtility.h"
#include "ModuleClusterer.h"
#include "GeometryUtility.h"
#include "BSpline.h"


#include "SymbolPath.h"
#include "understandDB.h"
#include "SymbolEdgeAttr.h"
#include "SymbolEdge.h"
#include "WordExtractor.h"
#include "SymbolNodeAttr.h"
#include "SymbolNode.h"
#include "symEdgeIter.h"
#include "symnodeiter.h"
#include "CutModularizer.h"
#include "SymbolModifier.h"
#include "Delaunay/DelaunayRoute.h"
#include "Layouter.h"
#include "ComponentLayouter.h"
#include "ClassLayouter.h"
#include "UIElementLocator.h"
#include "SymbolTreeBuilder.h"
#include "DBManager.h"
#include "UIRenderer.h"
#include "UIItemModel.h"
#include "imagearray.h"
#include "NodeUIItem.h"
#include "EdgeUIItem.h"
#include "CursorUIItem.h"
#include "ProjectUIItem.h"
#include "ClassUIItem.h"
#include "FolderUIItem.h"
#include "VariableUIItem.h"
#include "FunctionUIItem.h"
#include "MathUtility.h"
#include "UIUtility.h"
#include "LodManager.h"
#include "LodMerger.h"
#include "backgroundRenderer.h"
#include "TextProcessor.h"
#include "OverlapSolver.h"
#include "CodeScene.h"
#include "CodeView.h"
#include "MainUI.h"
#include "UIManager.h"
