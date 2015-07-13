

namespace CodeAtlas{
class ModuleClusterer
{
public:
protected:
};

class RandomWalkClusterer:public ModuleClusterer
{
public:
	RandomWalkClusterer(float t = 0.01, int maxIter = 10000):m_maxIter(maxIter), m_t(t), m_seed(12345){}

	void			clear();
	bool			setVtxCount(int nVtx);
	bool			addDirectedEdge(int srcVtx, int tarVtx, float weight);
	bool			addUndirectedEdge(int srcVtx, int tarVtx, float weight);
	bool			compute();

	int             getClusterID(int ithVtx);
	void			writeToGephi(char* fileName);
private:
	void			init();
	void			randomWalk();
	void			collectResult();

	struct Vertex
	{
		QHash<int,float> m_inEdge, m_outEdge;
		int			m_clusterID;
	};
	inline unsigned	randInt(int maxVal)
	{
		unsigned r1 = rand();
		//unsigned r2 = rand();
		//return (((r1 << 16) | r2) % maxVal);
		return ((r1) % maxVal);
	}
	inline float    randFloat()
	{
		unsigned r1 = rand();
		return r1 / float(RAND_MAX);
	}

	std::vector<Vertex>	m_vtx;

	int				m_seed;
	int				m_maxIter;
	int				m_nCluster;
	float			m_t;			// temperature
};
}