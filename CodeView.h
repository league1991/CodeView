#pragma once

namespace CodeAtlas
{

class CodeView :public QGraphicsView
{
	Q_OBJECT
public:
	CodeView(QWidget *parent = 0);
	~CodeView(void);

	// ignore message when the scene is locked
	void lockView();
	void unlockView();

	void centerViewWhenNecessary();

	LodManager& getLodManager(){return m_lodManager;}

	void			setDrawNames(bool b){m_drawName = b;}
	void			setDrawWordCloud(bool b){m_drawWordCloud = b;}
signals:
protected:
	virtual void    paintEvent(QPaintEvent *event);
	virtual void	drawBackground ( QPainter * painter, const QRectF & rect );
	virtual void	drawForeground(QPainter *painter, const QRectF &rect);
	void			drawCursorIcon(QPainter* painter);

	virtual void	resizeEvent ( QResizeEvent * event );
	virtual bool	event ( QEvent * event );
	void			wheelEvent(QWheelEvent *event);
	void			computeLod();

	void			drawWordCloud(QPainter *painter);
	void			drawName(QPainter *painter);

	QSize				m_curSize;
	QPointF				m_curTranslate;
	QMutex				m_mutex;
	bool				m_receiveMsg;
	bool				m_alwaysSeeCursor;
	bool				m_drawName;
	bool				m_drawWordCloud;

	OverlapSolver		m_overlapSolver;
	LodManager			m_lodManager;

	static QImage		m_cursorImg;
};

}
