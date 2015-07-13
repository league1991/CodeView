#pragma once

namespace CodeAtlas
{
	class UnderstandDB;
	class UnderstandHandle
	{
	public:
		friend class UnderstandDB;
		friend uint qHash(const UnderstandHandle& hdl);

		UnderstandHandle(){}
		UnderstandHandle(const UnderstandHandle& handle)
		{
			m_uniqueName = handle.m_uniqueName;
		}
		~UnderstandHandle(){}

		bool operator==(const UnderstandHandle& other)const
		{
			return m_uniqueName == other.m_uniqueName;
		}
	private:
		UnderstandHandle(char* uniqueName);
		QByteArray m_uniqueName;
	};
	inline uint qHash(const UnderstandHandle& hdl)
	{
		return qHash(hdl.m_uniqueName);
	}

	enum ReferenceType
	{
		Ref_None    =   0,
		Ref_Unknown = (0x1) << 0,

		Ref_Base    = (0x1) << 1,
		Ref_Call    = (0x1) << 2,
		Ref_Exception=(0x1) << 3,
		Ref_Friend  = (0x1) << 4,
		Ref_Modify  = (0x1) << 5,
		Ref_Override= (0x1) << 6,
		Ref_Set     = (0x1) << 7,
		Ref_Type    = (0x1) << 8,
		Ref_Use     = (0x1) << 9,
	};
	class UnderstandDB
	{
	public:
		UnderstandDB(void);
		~UnderstandDB(void);


		bool open(const char* fileName);
		bool close();

		void					findGlobalSymbols(QList<UnderstandHandle>& symbols);
		SymbolInfo				getSymbolInfo(const UnderstandHandle& handle);
		bool					findChildren(const UnderstandHandle& handle, QList<UnderstandHandle>& children, const char* childrenKind = "~file");
		bool					findDepends( const UnderstandHandle& src, QList<UnderstandHandle>& tarList, QList<unsigned>& typeList);
		bool					findCode(const UnderstandHandle& handle, QString& code)
		{
			//UdbEntity entity = getEntity(handle);
			return false;
		}
		bool					findFilePaths(const UnderstandHandle& handle, QList<QString>& paths);
	private:
		void					initializeDB();
		SymbolInfo::ElementType findEntityType(const UdbEntity& entity);

		static inline char*     getUniqueName(const UnderstandHandle& handle)
		{
			return const_cast<char*>(handle.m_uniqueName.constData());
		}
		static inline UdbEntity getEntity(const UnderstandHandle& handle)
		{
			char* uniqueName = const_cast<char*>(handle.m_uniqueName.constData());
			return udbLookupEntityByUniquename(uniqueName);
		}
		unsigned				getRefType(const UdbReference& refs);
		UdbKindList				m_classKind, 
								m_enumKind, 
								m_enumeratorKind, 
								m_fileKind,
								m_functionKind,
								m_macroKind,
								m_namespaceKind,
								m_objectKind,
								m_parameterKind,
								m_structKind,
								m_typedefKind,
								m_unionKind;


	};



}
