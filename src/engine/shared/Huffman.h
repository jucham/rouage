#ifndef HUFFMAN_H_
#define HUFFMAN_H_

#include <cassert>
#include <cstring>
#include <list>
#include <base/Types.h>
#include <base/BitTricks.h>

class Huffman {

public:
	static const int NUM_LITERALS = 256;
	static const int MAX_UNCOMPRESSED_PACKET_SIZE = 4096;
	static const int MAX_COMPRESSED_PACKET_SIZE = 1024;

private:
	static const int MAX_NUM_NODES = 2*NUM_LITERALS-1;
	static const int MAX_COMPRESSED_VALUE_SIZE = NUM_LITERALS-1; // maximum size of compressed value happen when the tree is a comb

	struct Literal {
		byte_t           value; // uncompressed value of literal
		byte_t           compValueRepr[MAX_COMPRESSED_VALUE_SIZE]; // representation of compressed value, each byte is 0 or 1
		int              compValueReprSize;
	};

	struct Node {
		Literal *pLiteral; // the literal if the node is a leaf
		double  weight; // the weight of node, frenquency occurence for a literal node
		Node    *pLS; // pointer to left son
		Node    *pRS; // pointer to right son
		Node    *pPR; // pointer to parent

		bool operator<(const Node &n) const { return weight < n.weight; }
		bool IsLeaf() const { return (pLS == 0 && pRS == 0); }
	};

	struct PackedNode {
		Literal *pLiteral; // the literal if the node is a leaf
		PackedNode *pLS; // pointer to left son
		PackedNode *pRS; // pointer to right son
		PackedNode *pPR; // pointer to parent

		bool IsLeaf() const { return (pLS == 0 && pRS == 0); }
	};

	PackedNode *pLeaves[NUM_LITERALS]; // pointers to leaves of tree (literals)
	PackedNode nodes[MAX_NUM_NODES]; // all the nodes of tree non-literals and literals
	int        nextFreeNodeIndex; // useful when creating the tree

	void         BuildTree(const double literalFrequencies[NUM_LITERALS]);
	void         InitAndSortLiteralNodes(const double literalFrequencies[NUM_LITERALS], std::list<Node>& literalNodes);
	static Node* BuildParentNode(Node *pN1, Node *pN2);
	int          BuildPackedTree(Node *pNode);
	static void  ComputeCompressedValueRepr(Node *pNode);

public:
	Huffman(const double literalFrequencies[NUM_LITERALS]);
	int  Compress(const byte_t *pUncompData, byte_t *pCompData, int numBytes) const;
	int Uncompress(const byte_t *pCompData, byte_t *pUncompData, int numBits) const;

	static double LITERALS_FREQUENCIES[NUM_LITERALS];
};

#endif
