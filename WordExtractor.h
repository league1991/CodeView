#pragma once

namespace CodeAtlas{

// split words and skip useless ones
class WordExtractor
{
public:
	WordExtractor();
	~WordExtractor(){}

	// 1.turn all words to lower case
	// 2.skip keywords
	// 3.skip single letter
	static QStringList			normalizeWords( const QString& str );
	// match whole word
	static void                 findOccurrence( const QString& stringToMatch, 
												const QStringList& matchSet, QHash<QString, int>& occurrence );
	static void					findOccurrence( const QString& wordStr, const QStringList& textSet, QVector<int>& occurrence );
private:
	static QSet<QString>&       getUselessWords();

	static QSet<QString> m_uselessWords;
	static QRegExp       m_wordSplitter;
};


class TFIDFMaker
{
public:
	void						addDoc(int docID, const QMap<int, float>& words);
	void						addWord(int docID, int wordID, float count);

	void						computeTFIDF();
	float						getTFIDF(int docID, int wordID);
private:



	struct DocWord
	{
		DocWord():m_wordCount(0), m_docFreq(0), m_tfidf(0){}
		float m_wordCount;
		float m_docFreq;
		float m_tfidf;
	};
	typedef QHash<int, DocWord> WordList;
	struct Doc
	{
		WordList m_wordCount;
	};
	typedef QHash<int, Doc>     DocList;

	DocList m_docList;			// doc id -> doc data
};
class SymbolData
{
public:
	SymbolData():m_level(0){}
	// merge word vector of another item
	void  mergeWords(const SymbolData& item);

	float getRadius()const;
	float getTotalWordCount()const{return m_totalWordCount;}

	void  computeStatistics();
	void  setLocalPos(const QVector2D& pos)
	{
		m_position2D = pos;
	}
	const QVector2D& getLocalPos()const{return m_position2D;}

	// int-> wordIdx, float->weight
	QMap<int,float> m_wordWeightMap;
	int             m_level;				// dependency level, if projA depends projB, A's level is higher, lowest level is 0

	QSet<SymbolPath>                     m_lowLevelSym;
	QSet<SymbolPath>					 m_highLevelSym;	
private:
	QVector2D							 m_position2D;
	float								 m_totalWordCount;
	float								 m_radius;
};
}