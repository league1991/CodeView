#include "StdAfx.h"
#include "UnderstandDB.h"

using namespace CodeAtlas;

UnderstandDB::UnderstandDB(void)
{
	m_classKind = m_enumKind = m_enumeratorKind = m_fileKind = m_functionKind = m_macroKind = m_namespaceKind = m_objectKind = m_parameterKind = m_structKind = m_typedefKind = m_unionKind = NULL;
}

UnderstandDB::~UnderstandDB(void)
{
}


bool UnderstandDB::open( const char* fileName )
{
	UdbStatus status = udbDbOpen(fileName);
	if (status)  {
		printf ("unable to open  Understand database: %s \n", fileName);
		switch (status)  
		{
		case Udb_statusDBAlreadyOpen:   
			printf("database already open\n");                    break;
		case Udb_statusDBChanged:   
			printf("database has been changed\n");                break;
		case Udb_statusDBCorrupt:  
			printf("database is corrupt\n");                      break;
		case Udb_statusDBOldVersion:  
			printf("database is old version\n");                  break;
		case Udb_statusDBUnknownVersion:
			printf("unknown version\n");                          break;
		case Udb_statusDBUnableOpen:
			printf("unable to locate file\n");                    break;
		case Udb_statusNoApiLicense:
			printf("no Understand license available\n");          break;
		case Udb_statusNoApiLicenseAda:
			printf("no Understand Ada license available\n");      break;
		case Udb_statusNoApiLicenseC:
			printf("no Understand C license available\n");        break;
		case Udb_statusNoApiLicenseFtn:
			printf("no Undertstand Fortran license available\n"); break;
		case Udb_statusNoApiLicenseJava:
			printf("no Understand Java license available\n");     break;
		case Udb_statusNoApiLicenseJovial:
			printf("no Understand Jovial license available\n");   break;
		case Udb_statusNoApiLicensePascal:
			printf("no Understand Pascal license available\n");   break;
		default:
			printf("unable to access database\n");                break;
		}
		return false;
	}
	initializeDB();
	return true;
}

bool CodeAtlas::UnderstandDB::close()
{
	udbDbClose();
	return true;
}

void CodeAtlas::UnderstandDB::findGlobalSymbols( QList<UnderstandHandle>& symbols )
{
	symbols.clear();

	UdbEntity* ents = NULL;
	int nents = 0;

	udbListEntity(&ents, &nents);
	UdbKindList kindList = udbKindParse(
		//"c namespace,"
		"c class ~member ~unknown ~unresolved,"
		"c enum ~member ~unknown ~unresolved,"
		"c function ~member ~unknown ~unresolved,"
		"c macro ~unknown ~unresolved,"
		"c object ~member ~unknown ~unresolved ~local,"
		"c struct ~member ~unknown ~unresolved,"
		"c typedef ~member ~unknown ~unresolved,"
		"c union ~member ~unknown ~unresolved");
	udbListEntityFilter(ents, kindList, &ents, &nents);

	for (int i = 0; i < nents; ++i)
	{
		UdbKind kind = udbEntityKind(ents[i]);
		//printf("%s \t---%s---%s---%s\n", udbKindLongname(kind), udbEntityTypetext(ents[i]),udbEntityNameShort(ents[i]), "");//udbEntityNameUnique(ents[i]));


		UdbReference* refs = NULL, *filterRefs = NULL;
		int nRefs = 0, nFilterRefs = 0;
		udbListReference(ents[i], &refs, &nRefs);
		udbListReferenceFilter(refs, udbKindParse("declarein, definein"), udbKindParse("~file ~namespace"), 0, &filterRefs, &nFilterRefs);
		/*
		for (int j = 0; j < nFilterRefs; ++j)
		{
			printf ("		%s: %s [%s (%d)] \n",
				udbKindLongname ( udbReferenceKind (filterRefs[j])),
				udbEntityNameShort ( udbReferenceEntity (filterRefs[j])),
				udbEntityNameShort ( udbReferenceFile (filterRefs[j])),
				udbReferenceLine (filterRefs[j]) );
		}*/
		if (filterRefs)
		{
			udbListReferenceFree(filterRefs);
		}

		// global symbol
		if (!nFilterRefs)
		{
			int entID = udbEntityId(ents[i]);
			char* uniqueName = udbEntityNameUnique(ents[i]);
			//printf("%s\n",uniqueName);
			symbols.append(UnderstandHandle(uniqueName));
		}
		udbListReferenceFree(refs); 

	}

	udbListEntityFree(ents);
}

void CodeAtlas::UnderstandDB::initializeDB()
{
	m_classKind = udbKindParse("c class");
	m_enumKind  = udbKindParse("c enum");
	m_enumeratorKind= udbKindParse("c enumerator");
	m_fileKind  = udbKindParse("c file");
	m_functionKind = udbKindParse("c function");
	m_macroKind = udbKindParse("c macro");
	m_namespaceKind = udbKindParse("c namespace");
	m_objectKind = udbKindParse("c object");
	m_parameterKind= udbKindParse("c parameter");
	m_structKind = udbKindParse("c struct");
	m_typedefKind = udbKindParse("c typedef");
	m_unionKind  = udbKindParse("c union");
}

SymbolInfo::ElementType CodeAtlas::UnderstandDB::findEntityType( const UdbEntity& entity )
{
	UdbKind entityKind = udbEntityKind(entity);
	if (udbKindLocate(entityKind, m_classKind))
	{
		return SymbolInfo::Class;
	}
	if (udbKindLocate(entityKind, m_enumKind))
	{
		return SymbolInfo::Enum;
	}
	if (udbKindLocate(entityKind, m_enumeratorKind))
	{
		return SymbolInfo::Enumerator;
	}
	if (udbKindLocate(entityKind, m_fileKind))
	{
		return SymbolInfo::File;
	}
	if (udbKindLocate(entityKind, m_functionKind))
	{
		if(udbIsKind(entityKind, "c private member function"))
			return SymbolInfo::FuncPrivate;
		if (udbIsKind(entityKind, "c protected member function"))
			return SymbolInfo::FuncProtected;
		return SymbolInfo::FuncPublic;
	}
	if (udbKindLocate(entityKind, m_macroKind))
	{
		return SymbolInfo::Macro;
	}
	if (udbKindLocate(entityKind, m_namespaceKind))
	{
		return SymbolInfo::Namespace;
	}
	if (udbKindLocate(entityKind, m_objectKind))
	{
		if(udbIsKind(entityKind, "c private member object"))
			return SymbolInfo::VarPrivate;
		if (udbIsKind(entityKind, "c protected member object"))
			return SymbolInfo::VarProtected;
		return SymbolInfo::VarPublic;
	}
	if (udbKindLocate(entityKind, m_parameterKind))
	{
		return SymbolInfo::Parameter;
	}
	if (udbKindLocate(entityKind, m_structKind))
	{
		return SymbolInfo::Struct;
	}
	if (udbKindLocate(entityKind, m_typedefKind))
	{
		return SymbolInfo::Typedef;
	}
	if (udbKindLocate(entityKind, m_unionKind))
	{
		return SymbolInfo::Union;
	}
	return SymbolInfo::Unknown;
}

CodeAtlas::SymbolInfo CodeAtlas::UnderstandDB::getSymbolInfo( const UnderstandHandle& handle )
{
	UdbEntity entity = getEntity(handle);
	char* name = udbEntityNameShort(entity);
	char* type = udbEntityTypetext(entity);
	SymbolInfo::ElementType eleType = findEntityType(entity);
	return SymbolInfo(name, type, eleType);
}

bool CodeAtlas::UnderstandDB::findFilePaths(const UnderstandHandle& handle, QList<QString>& paths)
{
	UdbEntity entity = getEntity(handle);
	if (!entity)
		return false;

	//printf ("[%s] %s\n",udbEntityTypetext(entity), udbEntityNameLong(entity));
	paths.clear();

	//printf ("entity name: %s\n",udbEntityNameLong ( entity));

	UdbReference* refs = NULL, *filteredRefs = NULL;
	int nRefs = 0, nFilteredRefs = 0;
	udbListReference(entity, &refs, &nRefs);
	udbListReferenceFilter(refs, udbKindParse("definein"), NULL, 0, &filteredRefs, &nFilteredRefs);
	if (!filteredRefs)
	{
		udbListReferenceFilter(refs, udbKindParse("definein, declarein"), NULL, 0, &filteredRefs, &nFilteredRefs);
	}
	for (int j = 0; j < nFilteredRefs; ++j)
	{
		
		/*printf ("      %s [%s file: %s (%d)] \n",
			//udbEntityNameLong ( entity),
			udbKindLongname ( udbReferenceKind (filteredRefs[j])),
			udbEntityNameLong ( udbReferenceEntity (filteredRefs[j])),
			udbEntityNameShort ( udbReferenceFile (filteredRefs[j])),
			udbReferenceLine (filteredRefs[j]) );*/

		UdbEntity refEntity = udbReferenceEntity(filteredRefs[j]);
		paths.append(udbEntityNameLong(udbReferenceFile (filteredRefs[j])));
	}
	if (nFilteredRefs && filteredRefs)
	{
		udbListReferenceFree(filteredRefs);
	}
	udbListReferenceFree(refs);
	return true;
}
bool CodeAtlas::UnderstandDB::findChildren( const UnderstandHandle& handle, QList<UnderstandHandle>& children, const char* childrenKind )
{
	UdbEntity entity = getEntity(handle);
	if (!entity)
		return false;

	//printf ("[%s] %s\n",udbEntityTypetext(entity), udbEntityNameLong(entity));

	UdbReference* refs = NULL, *filteredRefs = NULL;
	int nRefs = 0, nFilteredRefs = 0;
	udbListReference(entity, &refs, &nRefs);
	udbListReferenceFilter(refs, udbKindParse("declare, define"), udbKindParse(childrenKind), 1, &filteredRefs, &nFilteredRefs);
	for (int j = 0; j < nFilteredRefs; ++j)
	{
		/*
		printf ("		%s: %s [%s (%d)] \n",
			udbKindLongname ( udbReferenceKind (filteredRefs[j])),
			udbEntityTypetext( udbReferenceEntity (filteredRefs[j])),
			udbEntityNameLong ( udbReferenceEntity (filteredRefs[j])),
			//udbEntityNameShort ( udbReferenceFile (filteredRefs[j])),
			udbReferenceLine (filteredRefs[j]) );*/

		UdbEntity refEntity = udbReferenceEntity(filteredRefs[j]);
		children.append(UnderstandHandle(udbEntityNameUnique(refEntity)));
	}
	if (nFilteredRefs && filteredRefs)
	{
		udbListReferenceFree(filteredRefs);
	}
	udbListReferenceFree(refs);
	return true;
}

bool CodeAtlas::UnderstandDB::findDepends( const UnderstandHandle& src, QList<UnderstandHandle>& tarList, QList<unsigned>& typeList)
{ 
	tarList.clear();
	typeList.clear();

	static QSet<QString> nameSet;

	UdbEntity srcEntity = getEntity(src);
	UdbReference*refs = NULL, *filteredRefs = NULL;
	int nRef = 0, nFilteredRef = 0;

	udbListReference(srcEntity, &refs, &nRef);
	udbListReferenceFilter(refs, 
		udbKindParse("call, base, exception, friend, modify, override, set, use"), udbKindParse("~file"), 0, &filteredRefs, &nFilteredRef);
	for (int i = 0; i< nFilteredRef; ++i)
	{
		UdbEntity tar = udbReferenceEntity(filteredRefs[i]);
		char* uniqueName = udbEntityNameUnique(tar);
		tarList.push_back(UnderstandHandle(uniqueName));

		unsigned type = getRefType(filteredRefs[i]);

		typeList.push_back(type);
// 		printf("src: %s\n", udbEntityNameLong(srcEntity));
// 		printf("tar: %s\n", udbEntityNameLong(tar));
// 		printf("type: %s\n\n", name);
// 		nameSet.insert(QString(name));
	}
	if (nFilteredRef)
		udbListReferenceFree(filteredRefs);
	if (nRef)
		udbListReferenceFree(refs);
	return true;
}

unsigned CodeAtlas::UnderstandDB::getRefType( const UdbReference& refs )
{
	UdbKind kind = udbReferenceKind(refs);
	char* refName = udbKindLongname(kind);
	char* r;
	unsigned type=0;
	if (r = strstr(refName, "Base"))
		type |= Ref_Base;
	if (r = strstr(refName, "Call"))
		type |= Ref_Call;
	if (r = strstr(refName, "Exception"))
		type |= Ref_Exception;
	if (r = strstr(refName, "Friend"))
		type |= Ref_Friend;
	if (r = strstr(refName, "Modify"))
		type |= Ref_Modify;
	if (r = strstr(refName, "Override"))
		type |= Ref_Override;
	if (r = strstr(refName, "Set"))
		type |= Ref_Set;
	if (r = strstr(refName, "Typed"))
		type |= Ref_Type;
	if (r = strstr(refName, "Use"))
		type |= Ref_Use;
	if(type == 0)
		type =  Ref_Unknown;
	return type;
}

CodeAtlas::UnderstandHandle::UnderstandHandle( char* uniqueName )
{
	if (!uniqueName)
	{
		return;
	}
	m_uniqueName = QByteArray(uniqueName, strlen(uniqueName)+1);
}


