

namespace CodeAtlas
{
	class CutModularizer
	{
	public:
		typedef Eigen::Triplet<double>		Triplet;
		typedef Eigen::SparseMatrix<double> SparseMatrix;
		typedef Eigen::MatrixXf				MatrixXf;
		typedef Eigen::VectorXf				VectorXf;
		typedef Eigen::MatrixXd				MatrixXd;
		typedef Eigen::VectorXd				VectorXd;

		CutModularizer(float semanticWeight = 0.8):m_semanticWeight(semanticWeight){}

		void						addNodes(const QList<SymbolNode::Ptr>& nodeList){m_nodeList = nodeList;}
		void						addNode(SymbolNode::Ptr& node){m_nodeList.append(node);}
		bool						setDependency(const SparseMatrix& veMat, const VectorXd& edgeWeightList);

		bool						compute();
		int							getComponent(int ithNode);
	private:
		// compute similarity, 
		// minE  element with lower index
		// maxE  element with higher index
		inline float				getSimilarity(int minE, int maxE)
		{
			return m_simMat(minE,maxE) * (1-m_semanticWeight) + m_simMat(maxE,minE) * m_semanticWeight;
		}
		void						computeStructualSim();
		void						computeSemanticSim();
		void						findCluster();

		QList<SymbolNode::Ptr>		m_nodeList;
		SparseMatrix				m_veMat;
		VectorXd					m_edgeWeightList;
		
		// lower half->semantic similarity
		// upper half->structural similarity
		// diagonal -> not use
		MatrixXf					m_simMat;
												
		float						m_semanticWeight;
		std::vector<int>			m_componentList;
	};
}