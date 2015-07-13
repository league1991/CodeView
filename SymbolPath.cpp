#include "stdafx.h"

using namespace CodeAtlas;

// the following symbols are used to split type string:
// 1.<   2.>   3.[space]   4.*   5.:	6.[		7.]		8.&
QRegExp				SymbolInfo::m_typeElementSplitter("[<> \\*:\\[\\]&]");
QSet<QString>		SymbolInfo::m_basicTypeWords;

// the same order of ElementType
const QString SymbolInfo::m_elementTypeName[] = {
	"Unknown",

	"Class",
	"Enum",
	"Enumerator",

	"FuncPublic",
	"FuncProtected",
	"FuncPrivate",

	"Namespace",

	"VarPublic",
	"VarProtected",
	"VarPrivate",

	"Signal",
	"SlotPublic",
	"SlotProtected",
	"SlotPrivate",

	"Keyword",
	"Macro",

	"Project",
	"Root",

	"Block",

	"Struct",
	"Typedef",
	"Union",
	"File",
	"Parameter",
	"Folder"
};

QSet<QString> CodeAtlas::SymbolInfo::m_primaryTypeWords;

CodeAtlas::SymbolPath::SymbolPath( QList<SymbolInfo>& symbolPath )
{
}

CodeAtlas::SymbolPath::SymbolPath()
{
}


const QList<SymbolInfo>& CodeAtlas::SymbolPath::getSymbolPath()const
{
	return m_symbolPath;
}

void CodeAtlas::SymbolPath::setSymbolPath( QList<SymbolInfo>& symbolPath )
{
	m_symbolPath = symbolPath;
	rehash();
}

const SymbolInfo* CodeAtlas::SymbolPath::getSymbol( int ithSymbol )const
{
	if (ithSymbol < 0 || ithSymbol >= m_symbolPath.size())
		return NULL;
	return &m_symbolPath[ithSymbol];
}

int CodeAtlas::SymbolPath::getSymbolCount()const
{
	return m_symbolPath.size();
}

SymbolPath&	CodeAtlas::SymbolPath::addChildSymbol( const SymbolInfo& symbol )
{
	m_symbolPath.append(symbol);
	rehash();
	return *this;
}

SymbolPath&	CodeAtlas::SymbolPath::addParentSymbol( const SymbolInfo& symbol )
{
	m_symbolPath.prepend(symbol);
	rehash();
	return *this;
}

void CodeAtlas::SymbolPath::clear()
{
	m_symbolPath.clear(); 
	m_hash = 0;
}

bool CodeAtlas::SymbolPath::operator==( const SymbolPath& p )const
{
	if (this->m_symbolPath.size() != p.m_symbolPath.size())
		return false;
	for (int ithSym = 0; ithSym < m_symbolPath.size(); ++ithSym)
	{
		if (m_symbolPath[ithSym] == p.m_symbolPath[ithSym])
			continue;
		return false;
	}
	return true;
}

QString CodeAtlas::SymbolPath::toString( bool showType /*= false*/, bool showElementType /*= false*/ )const
{
	QString s;
	for (int ithSym = 0; ithSym < m_symbolPath.size(); ++ithSym)
	{
		s += m_symbolPath[ithSym].toString(showType, showElementType);
		if (ithSym < m_symbolPath.size()-1)
			s += " / ";
	}
	return s;
}

SymbolPath&	CodeAtlas::SymbolPath::addParentPath( const SymbolPath& parentPath )
{
	for (int ithSym = parentPath.m_symbolPath.size()-1; ithSym >= 0; --ithSym)
		m_symbolPath.prepend(parentPath.m_symbolPath[ithSym]);
	rehash();
	return *this;
}

SymbolPath&	CodeAtlas::SymbolPath::addChildPath( const SymbolPath& parentPath )
{
	m_symbolPath.append(parentPath.m_symbolPath);
	rehash();
	return *this;
}

const SymbolInfo* CodeAtlas::SymbolPath::getLastProjSymbol() const
{
	const SymbolInfo* info = NULL;
	for (int ithSym = m_symbolPath.size()-1; ithSym >= 0; --ithSym)
		if (m_symbolPath[ithSym].isProject())
			info = &(m_symbolPath[ithSym]);
	return info;
}

unsigned CodeAtlas::SymbolPath::findCommonLength( const SymbolPath& p )const
{
	int comLength = 0;
	int length = qMin(m_symbolPath.size(), p.m_symbolPath.size());
	for (comLength = 0; comLength < length; ++comLength)
		if (!(m_symbolPath[comLength] == p.m_symbolPath[comLength]))
			break;
	return comLength;
}

void CodeAtlas::SymbolPath::getTopLevelItemPath( SymbolPath& path ) const
{
	/*
	path.clear();
	int length      = m_symbolPath.size();
	int functionIdx = length;
	int classIdx    = length;
	for (int i = 0; i < length; ++i)
	{
		const SymbolInfo& info = m_symbolPath[i];

		// add lowest-level class or highest-level function
		if (info.isFunction() && functionIdx == length)
			functionIdx = i;
		else if (info.isClass())
			classIdx = i;
	}

	int validIdx = qMin(functionIdx, classIdx);
	if (validIdx == length)
		return;						// no valid item

	for (int i = 0; i <= validIdx; ++i)
		path.m_symbolPath.append(m_symbolPath[i]);
	path.rehash();*/

	path.clear();
	for (int i = 0; i < m_symbolPath.size(); ++i)
	{
		path.m_symbolPath.append(m_symbolPath[i]);
		if (m_symbolPath[i].isTopLevel())
		{
			break;
		}
	}
	path.rehash();
}
SymbolPath CodeAtlas::SymbolPath::getTopLevelItemPath() const
{
	SymbolPath path;
	for (int i = 0; i < m_symbolPath.size(); ++i)
	{
		path.m_symbolPath.append(m_symbolPath[i]);
		if (m_symbolPath[i].isTopLevel())
		{
			break;
		}
	}
	path.rehash();
	return path;
}
int CodeAtlas::SymbolPath::getLength() const
{
	return getSymbolCount();
}

bool CodeAtlas::SymbolPath::hasPrefix( const SymbolPath& prefix ) const
{
	if (m_symbolPath.size() < prefix.m_symbolPath.size())
	{
		return false;
	}

	for (int i = prefix.m_symbolPath.size()-1; i >= 0; --i)
	{
		if (m_symbolPath[i] != prefix.m_symbolPath[i])
		{
			return false;
		}
	}
	return true;
}

SymbolPath& CodeAtlas::SymbolPath::removeChildSymbol()
{
	if (!m_symbolPath.isEmpty())
	{
		m_symbolPath.removeLast();
		rehash();
	}
	return *this;
}

SymbolPath SymbolPath::getProjectPath() const
{
	SymbolPath path;
	path.m_symbolPath.clear();
	for (int ithSym = 0; ithSym < m_symbolPath.size(); ++ithSym)
	{
		if (m_symbolPath[ithSym].isProject())
			path.m_symbolPath.append(m_symbolPath[ithSym]);
		else
			break;
	}
	path.rehash();
	return path;
}

void SymbolPath::getProjectPath(SymbolPath& path)const
{
	path.m_symbolPath.clear();
	for (int ithSym = 0; ithSym < m_symbolPath.size(); ++ithSym)
	{
		if (m_symbolPath[ithSym].isProject())
			path.m_symbolPath.append(m_symbolPath[ithSym]);
		else
			break;
	}
	path.rehash();
}


void SymbolPath::rehash()
{
	unsigned res = 0;
	for ( int ithSymbol = 0; ithSymbol < m_symbolPath.size(); ++ithSymbol)
	{
		res = (res << 2) ^ m_symbolPath[ithSymbol].hash();
	}
	m_hash = res;
}

const QString & CodeAtlas::SymbolInfo::elementTypeName() const
{
	return m_elementTypeName[elementTypeIdx(m_elementType)]; 
}

bool CodeAtlas::SymbolInfo::isPrimaryType( const QString& type )
{
	if (m_primaryTypeWords.size() == 0)
	{
		initPrimaryTypeWordSet();
	}
	QStringList typeList = type.split(" ", QString::SkipEmptyParts);
	for (int i = 0; i < typeList.size(); ++i)
	{
		if (m_primaryTypeWords.contains(typeList[i]))
			return true;
	}
	return false;
}

void CodeAtlas::SymbolInfo::initPrimaryTypeWordSet()
{
	if (m_primaryTypeWords.size() != 0)
		return;
	m_primaryTypeWords 
		<< "bool"		
		<< "char"		<< "unsigned char"
		<< "wchar_t" 
		<< "short"		<< "unsigned short"
		<< "int"		<< "unsigned int"
		<< "long"		<< "unsigned long"
		<< "float" 
		<< "double"
		<< "void" 

		<< "bool*"		
		<< "char*"		<< "unsigned char*"
		<< "wchar_t*" 
		<< "short*"		<< "unsigned short*"
		<< "int*"		<< "unsigned int*"
		<< "long*"		<< "unsigned long*"
		<< "float*" 
		<< "double*"
		<< "void*" ;
}

int CodeAtlas::SymbolInfo::elementTypeIdx( ElementType type )
{
	unsigned typeTag = type;
	for (int ithTag = 0; ithTag < 32; ++ithTag)
	{
		if ((typeTag >> ithTag) & 0x1)
			return ithTag+1;
	}
	return 0;// "unknown"
}

void CodeAtlas::SymbolInfo::setFlag( bool isSet, unsigned flag )
{
	if (isSet)
	{
		m_flag |= flag;
	}
	else
		m_flag &= (~flag);
}

void CodeAtlas::SymbolInfo::setTopLevel( bool isTop )
{
	setFlag(isTop, TopLevel);
}
