class RAS_DummyNode
{
public:
	typedef RAS_DummyNode ParentType;

	void Print(unsigned short level, bool recursive)
	{
	}

	template <class Args>
	void Bind(const Args& args)
	{
	}

	template <class Args>
	void Unbind(const Args& args)
	{
	}

	template <class Args>
	void Execute(const Args& args)
	{
	}

	ParentType *GetParent()
	{
		return NULL;
	}

	template <class NodeType>
	void AddChild(NodeType subNode)
	{
	}
};
