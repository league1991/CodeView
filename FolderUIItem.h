#pragma once
namespace CodeAtlas
{
class FolderUIItem :
	public ClassUIItem
{
public:
	FolderUIItem::FolderUIItem( const SymbolNode::WeakPtr& node, UIItem* parent /*= 0*/ );
	~FolderUIItem(void);

	void			paint(QPainter *painter, const QStyleOptionGraphicsItem *option=0, QWidget *widget = 0);
};

}
