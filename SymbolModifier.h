#pragma once
namespace CodeAtlas
{
	class SymbolTree;
	class SymbolTreeBuilder;

	// virtual base class
	class SymbolModifier
	{
	public:
		typedef QSharedPointer<SymbolModifier> Ptr;
		SymbolModifier():m_tree(NULL), m_builder(NULL){}
		virtual ~SymbolModifier(){}

		// called after the tree is cleared
		virtual void clearCache()=0;

		// set tree before calling modifyTree()
		void prepare(SymbolTree& tree){this->m_tree = &tree;}

		// modify symbol tree
		virtual void modifyTree() = 0;

		// called before next modifier to modify the tree
		virtual void updateDirtyData() = 0;

		void setBuilder(SymbolTreeBuilder* builder);
	protected:
		SymbolTree* m_tree;
		SymbolTreeBuilder* m_builder;
	};

	class WordCollector:public SymbolModifier
	{
	public:
		void			clearCache(){}
		void			modifyTree();
		void			updateDirtyData();
	private:
		typedef Eigen::SparseMatrix<double> SparseMatrix;
		void			collectWords();
		void			buildDocTermMat( QList<SymbolNode::Ptr>& nodeList, SparseMatrix& docTermMat, Eigen::VectorXf& radiusVec );
	};

	// add edge FuzzyDependEdge
	class FuzzyDependBuilder: public SymbolModifier
	{
	public:
		void			clearCache(){}
		void			modifyTree();
		void			updateDirtyData(){}
	private:
		typedef Eigen::Triplet<double>		Triplet;
		typedef Eigen::SparseMatrix<double> SparseMatrix;
		struct ChildPack
		{
			SymbolInfo		m_info;		// child's SymbolInfo
			QString			m_code;		// child's code
			SymbolNode::Ptr m_pNode;	// child's SymbolNode
			int             m_index;	// child's index, 
										// for a project node, it's the order in the globalSymList
										// for a symbol node, it's the order in the child hash map
		};

		// build dependency between symbols under a project 
		void			buildProjDepend(const SymbolNode::Ptr& proj);
		// add dependency edges to children
		QString			collectFuzzyDepend(SymbolNode::Ptr& node);
		// build child depend
		void			buildChildDepend( QMultiHash<QString, ChildPack>& childList , 
											Eigen::SparseMatrix<double>& vtxEdgeMat, 
											Eigen::VectorXd& edgeWeightVec, 
											QList<FuzzyDependAttr::DependPair>& dependPair);
		void			buildSubGraphLevel(const SparseMatrix& veMat, const bool* const validMask, int nNodes, Eigen::VectorXi& level);
	};


	struct DBDependData;
	class GeneralDependBuilder:public SymbolModifier
	{
	public:
		void			clearCache();
		void			modifyTree();
		void			updateDirtyData(){}

	private:
		struct Pair
		{
			Pair(int src, int tar):m_src(src), m_tar(tar){}
			friend uint qHash(const Pair& p);
			bool operator==(const Pair& p)const{return m_src == p.m_src && m_tar == p.m_tar;}

			int m_src, m_tar;
		};
		typedef DependRawDataAttr::DependPath DependPath;

		friend uint qHash(const Pair& p);
		void			buildDependPath();
		void			buildNodeDepend(SymbolNode::Ptr node, 
										SymbolPath& nodePath,
										QList<int>& dependIDList);
		bool			buildDependData(QList<DBDependData>& dependData);

		QList<DependPath>& getDependPath();
	};

	inline uint qHash(const GeneralDependBuilder::Pair& p)
	{
		return (p.m_src << 16) | p.m_tar;
	}


	class Modulizer:public SymbolModifier
	{
	public:
		void			clearCache(){}
		void			modifyTree()
		{
			randomModularize();
		}
		void			updateDirtyData(){}
	private:
		void randomModularize();
	};

	class TopicLODMaker:public SymbolModifier
	{
	public:
		void			clearCache(){}
		void			modifyTree();
		void			updateDirtyData(){}
	private:
	};
}