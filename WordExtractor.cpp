#include "StdAfx.h"
#include "WordExtractor.h"

using namespace CodeAtlas;

QSet<QString> WordExtractor::m_uselessWords;
QRegExp       WordExtractor::m_wordSplitter("[A-Za-z][a-z]*");

WordExtractor::WordExtractor()
{
}

QStringList WordExtractor::normalizeWords( const QString& str )
{
// 	QStringList wordList;
// 	int pos = 0;
// 	while ((pos = m_wordSplitter.indexIn(str, pos)) != -1) {
// 		pos += m_wordSplitter.matchedLength();
// 
// 		QString newWord = m_wordSplitter.cap(0).toLower();
// 		if (!getUselessWords().contains(newWord) && newWord.length() > 1)
// 		{
// 			wordList << newWord;
// 		}
// 	}

//	qDebug() << "input str: " << str;

	int beg = 0;
	int strLength = str.size();
	const QChar* pStr = str.constData();
	const QChar  a('a'), z('z'), A('A'), Z('Z');
	QStringList wList;
	while(1)
	{
		// find word begin
		bool isUpperSeq = false;
		for (; beg < strLength; ++beg)
		{
			const QChar* c = pStr +beg;
			if (*c >= a && *c <= z)
			{
				isUpperSeq = false;
				break;
			}
			else if (*c >= A && *c <= Z)
			{
				isUpperSeq = true;
				break;
			}
		}

		if (beg >= strLength)
			break;

		// find word end
		int end = beg+1;
		if (end < strLength)
		{
			const QChar* c = pStr + end;
			isUpperSeq &= (*c >= A && *c <= Z);
		}
		for (; end < strLength; ++end)
		{
			const QChar* c = pStr + end;

			if (isUpperSeq)
			{
				// find non-upper case letter
				if (!(*c >= A && *c <= Z))
				{
					// c is lower case letter
					if (*c >= a && *c <= z)
						end--;
					// c is '_' or numbers
					break;		
				}
			}
			else
			{
				// camel naming, if c is not lower case, break the word
				if (!(*c >= a && *c <= z))
					break;
			}
		}

		// add to list
		int length = end - beg;
		int minLength = isUpperSeq ? 2 : 3;
		if (length >= minLength)
		{
			QString word(pStr+beg, length);
			if (!getUselessWords().contains(word))
			{
				if (isUpperSeq)
					wList << word;
				else
					wList << word.toLower();
				//qDebug() << word;
			}
		}

		beg = end;
	}

	return wList;
}

QSet<QString>& CodeAtlas::WordExtractor::getUselessWords()
{
	if (m_uselessWords.size() == 0)
	{
		// skip all cpp keywords
		m_uselessWords
			// c++ key words
			<<"asm"		<<"do"		<<"if"		<<"return"	<<"typedef"
			<<"auto"	<<"double"	<<"inline"	<<"short"	<<"typeid"
			<<"bool"	<<"dynamic_cast"		<<"int"		<<"signed"		<<"typename"
			<<"break"	<<"else"	<<"long"	<<"sizeof"	<<"union"
			<<"case"	<<"enum"	<<"mutable"	<<"static"	<<"unsigned"
			<<"catch"	<<"explicit"<<"namespace"			<<"static_cast"	<<"using"
			<<"char"	<<"export"	<<"new"		<<"struct"	<<"virtual"
			<<"class"	<<"extern"	<<"operator"<<"switch"	<<"void"
			<<"const"	<<"false"	<<"private"	<<"template"<<"volatile"
			<<"cast"	<<"float"	<<"protected"			<<"this"	
			<<"continue"			<<"for"		<<"public"	<<"throw"		<<"while"
			<<"default"	<<"friend"	<<"register"<<"true"	<<"wchar_t"
			<<"delete"	<<"goto"	<<"reinterpret"	<<"try"

			// addtional words
			<<"string"  <<"unnamed" <<"get"     << "set";
	}
	return m_uselessWords;
}
void WordExtractor::findOccurrence( const QString& wordStr, const QStringList& textSet, QVector<int>& occurrence )
{
	occurrence.resize(textSet.size());
	occurrence.fill(0);
	if (!wordStr.size())
		return;

	int wordLength = wordStr.size();
	QStringMatcher strMatcher(wordStr);
	const QChar underLine('_');
	for (int ithText = 0; ithText < textSet.size(); ++ithText)
	{
		int pos = 0;
		const QString& text = textSet[ithText];
		for(;(pos = strMatcher.indexIn(text, pos)) != -1; pos += wordLength)
		{
			// try match whold word
			if (pos > 0)
			{
				QChar preChar = text.at(pos-1);
				if (preChar.isLetter() || preChar.isNumber() || preChar == underLine)
					continue;
			}
			int end = pos + wordLength;
			if (end < text.size())
			{
				QChar postChar = text.at(end);
				if (postChar.isLetter() || postChar.isNumber() || postChar == underLine)
					continue;
			}
			occurrence[ithText]++;
		}
	}
}
void CodeAtlas::WordExtractor::findOccurrence( const QString& textStr, const QStringList& wordSet, QHash<QString, int>& occurrence )
{
	occurrence.clear();
	if (wordSet.size() == 0 || textStr.size() == 0)
		return;

	// construct match string, for example: \b(mail|letter|correspondence)\b
	const QStringList::ConstIterator pIter = wordSet.constBegin();
	QString matchStr;
	for (int ithStr = 0; ithStr < wordSet.size(); ++ithStr)
		if (wordSet[ithStr].trimmed().size() != 0)
			matchStr += ("|" + wordSet[ithStr]);
	matchStr.replace(0, 1, "\\b(");
	matchStr += ")\\b";

	// match 
	QRegExp reg(matchStr);
	int pos = 0;
	while ((pos = reg.indexIn(textStr, pos)) != -1) {
		pos += reg.matchedLength();

		QString newWord = reg.cap(0);
		if (!occurrence.contains(newWord))
			occurrence[newWord] = 1;
		else
			occurrence[newWord]++;
	}
}

void CodeAtlas::TFIDFMaker::addDoc( int docID, const QMap<int, float>& words )
{
	Doc& doc = m_docList[docID];
	for (QMap<int, float>::ConstIterator pWord = words.constBegin(); pWord != words.constEnd(); ++pWord)
	{
		DocWord& word = doc.m_wordCount[pWord.key()];
		word.m_wordCount += pWord.value();
	}
}

void CodeAtlas::TFIDFMaker::addWord( int docID, int wordID, float count )
{
	Doc& doc = m_docList[docID];
	DocWord& word = doc.m_wordCount[wordID];
	word.m_wordCount += count;
}

void CodeAtlas::TFIDFMaker::computeTFIDF()
{
	typedef QHash<int, int> WordDocCountMap;		// word id -> doc count
	WordDocCountMap wordDocCountMap;

	// compute tf
	for (DocList::Iterator pDoc = m_docList.begin(); pDoc != m_docList.end(); ++pDoc)
	{
		Doc& doc = pDoc.value();
		float totalCount = 0;

		for (WordList::Iterator pWord = doc.m_wordCount.begin(); pWord != doc.m_wordCount.end(); ++pWord)
		{
			// count word
			totalCount += pWord.value().m_wordCount;

			// count document
			int wordID = pWord.key();
			WordDocCountMap::Iterator pw = wordDocCountMap.find(wordID);
			if (pw != wordDocCountMap.end())
				pw.value()++;
			else
				wordDocCountMap[wordID] = 1;
		}
		for (WordList::Iterator pWord = doc.m_wordCount.begin(); pWord != doc.m_wordCount.end(); ++pWord)
		{
			DocWord& word = pWord.value();
			word.m_docFreq = word.m_wordCount / totalCount;
		}
	}

	// compute tfidf
	const int docCount = m_docList.size();
	for (DocList::Iterator pDoc = m_docList.begin(); pDoc != m_docList.end(); ++pDoc)
	{
		Doc& doc = pDoc.value();

		for (WordList::Iterator pWord = doc.m_wordCount.begin(); pWord != doc.m_wordCount.end(); ++pWord)
		{
			int wordID = pWord.key();
			DocWord& wordItem = pWord.value();

			int wordInDocCount = wordDocCountMap[wordID];
			wordItem.m_tfidf = wordItem.m_docFreq * log(1.0 + docCount / (wordInDocCount));
		}
	}
}

float CodeAtlas::TFIDFMaker::getTFIDF( int docID, int wordID )
{
	DocList::Iterator pDoc = m_docList.find(docID);
	if (m_docList.end() == pDoc)
	{
		return 0;
	}
	WordList::Iterator pWord = pDoc->m_wordCount.find(wordID);
	if (pWord == pDoc->m_wordCount.end())
	{
		return 0;
	}
	return pWord->m_tfidf;
}
