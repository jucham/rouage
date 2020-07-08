#include "Huffman.h"

Huffman::Huffman(const double literalFrequencies[NUM_LITERALS]) :
nextFreeNodeIndex(-1)
{
	BuildTree(literalFrequencies);
}

void Huffman::BuildTree(const double literalFrequencies[NUM_LITERALS]) {

	// build a sorted list of literal nodes
	std::list<Node> literalNodes(NUM_LITERALS);
	InitAndSortLiteralNodes(literalFrequencies, literalNodes);

	// build a list of nodes to treat to build huffman tree
	std::list<Node*> toTreatNodes;
	for (std::list<Node>::iterator it = literalNodes.begin(); it != literalNodes.end(); ++it) {
		toTreatNodes.push_back(&(*it));
	}

	// build huffman tree
	Node *pRoot = 0;
	while (toTreatNodes.size() > 1) {

		// get the two lightest node
		Node *pN1(toTreatNodes.front());
		toTreatNodes.pop_front();
		Node *pN2(toTreatNodes.front());
		toTreatNodes.pop_front();

		// create a parent for these two nodes
		Node *pParent = BuildParentNode(pN1, pN2);

		// insert the parent keeping nodes sorted
		bool notInserted = true;
		for (std::list<Node*>::iterator it = toTreatNodes.begin(); it != toTreatNodes.end(); ++it) {
			if ( pParent->weight <= (*it)->weight) {
				notInserted = false;
				it = toTreatNodes.insert(it, pParent);
				pRoot = *it;
				break;
			}
		}

		if (notInserted) {
			toTreatNodes.push_back(pParent);
			pRoot = pParent;
		}
	}

	// build the final tree
	BuildPackedTree(pRoot);
}

void Huffman::InitAndSortLiteralNodes(const double literalFrequencies[NUM_LITERALS], std::list<Node>& literalNodes) {
	int literalIndex = 0;
	for (std::list<Node>::iterator it = literalNodes.begin();
			it != literalNodes.end(); ++it) {
		it->pLiteral = new Literal;
		it->pLiteral->value = literalIndex;
		std::memset(it->pLiteral->compValueRepr, 0, MAX_COMPRESSED_VALUE_SIZE);
		it->weight = literalFrequencies[literalIndex++];
		it->pLS = it->pRS = it->pPR = 0;
	}
	literalNodes.sort();
}

Huffman::Node* Huffman::BuildParentNode(Node *pN1, Node *pN2) {

	Node *pParent = new Node;
	pParent->weight = pN1->weight + pN2->weight;
	pParent->pLiteral = 0;
	pParent->pPR = 0;
	pParent->pLS = pN1;
	pParent->pRS = pN2;
	pParent->pLS->pPR = pParent;
	pParent->pRS->pPR = pParent;

	return pParent;
}

int Huffman::BuildPackedTree(Node* pNode) {

	assert(nextFreeNodeIndex < MAX_NUM_NODES);

	int nodeIndex = ++nextFreeNodeIndex;
	PackedNode *pN = &nodes[nodeIndex];

	// build leaf
	if (pNode->IsLeaf()) {
		ComputeCompressedValueRepr(pNode);
		pN->pLiteral = pNode->pLiteral;
		pN->pLS = pN->pRS = 0;
		pLeaves[pN->pLiteral->value] = pN;
	}
	// build sons
	else {
		int LeftSonIndex = BuildPackedTree(pNode->pLS);
		pN->pLS = &nodes[LeftSonIndex];
		pN->pLS->pPR = pN;
		int RightSonIndex = BuildPackedTree(pNode->pRS);
		pN->pRS = &nodes[RightSonIndex];
		pN->pRS->pPR = pN;
	}

	return nodeIndex;
}

void Huffman::ComputeCompressedValueRepr(Node *pNode) {

	int bitIndex = 0;
	byte_t *pCompValueRepr = pNode->pLiteral->compValueRepr;
	const Node *pCurNode = pNode;

	// path nodes to the root
	while (pCurNode->pPR != 0) {
		const Node *pParent = pCurNode->pPR;
		pCompValueRepr[bitIndex++] = ((pParent->pLS == pCurNode) ? 0 : 1);
		pCurNode = pParent;
	}

	pNode->pLiteral->compValueReprSize = bitIndex;

	// reverse to get the compressed value representation
	int lastBitIndex = pNode->pLiteral->compValueReprSize - 1;
	for(int i=0, j=lastBitIndex; i<j; i++, j--) {
		std::swap(pCompValueRepr[i], pCompValueRepr[j]);
	}
}
int Huffman::Compress(const byte_t *pUncompData, byte_t *pCompData, int numBytes) const {

	assert(numBytes <= MAX_UNCOMPRESSED_PACKET_SIZE);

	byte_t *pPackedBits = pCompData;
	byte_t packedBitsCursor = 0;
	int totalBits = 0;

	for (int i=0; i<numBytes; i++) {

		// get the bits to copy and their number
		byte_t *pBits = pLeaves[pUncompData[i]]->pLiteral->compValueRepr;
		short numBits = pLeaves[pUncompData[i]]->pLiteral->compValueReprSize;

		// copy the bits in a packed shape
		packedBitsCursor = BitTricks::PackAndCopyBits(pBits, &pPackedBits, numBits, packedBitsCursor);
		totalBits += numBits;
	}

	return totalBits;
}

int Huffman::Uncompress(const byte_t *pCompData, byte_t *pUncompData, int numBits) const {

	byte_t compBitsRepr[numBits];
	std::memset(compBitsRepr, 0, numBits);
	byte_t *pCompBitsRepr = &compBitsRepr[0];
	int numCompBytes = (numBits % 8 == 0) ? numBits/8 : numBits/8 + 1;

	// get the compressed bits as an unpacked representation
	for (int i = 0; i < numCompBytes; ++i) {
		for (int j = 0; j < 8; ++j) {
			*pCompBitsRepr |= (*pCompData >> 7-j) & 1;
			++pCompBitsRepr;
		}
		++pCompData;
	}

	pCompBitsRepr = &compBitsRepr[0];
	int numBitsToRead = numBits;
	int numUncompBytes = 0;

	// read all the bits and copy literal value when leaf is found
	while (numBitsToRead > 0) {

		const PackedNode *pN = &nodes[0];

		while (!pN->IsLeaf()) {
			pN = (*pCompBitsRepr == 0) ? pN->pLS : pN->pRS;
			++pCompBitsRepr;
			--numBitsToRead;
		}

		*pUncompData = pN->pLiteral->value;
		++pUncompData;
		++numUncompBytes;
	}

	return numUncompBytes;
}

double Huffman::LITERALS_FREQUENCIES[NUM_LITERALS] = {
		0.194762,
		0.128902,
		0.005829,
		0.008795,
		0.000054,
		0.004028,
		0.000049,
		0.000004,
		0.000043,
		0.004098,
		0.012098,
		0.000041,
		0.000042,
		0.000002,
		0.000003,
		0.000045,
		0.000002,
		0.002463,
		0.003756,
		0.004259,
		0.001482,
		0.000044,
		0.000040,
		0.000042,
		0.000162,
		0.000002,
		0.000241,
		0.000043,
		0.000041,
		0.000003,
		0.036156,
		0.003992,
		0.000070,
		0.004098,
		0.000005,
		0.000001,
		0.000041,
		0.000002,
		0.000003,
		0.000085,
		0.000084,
		0.000161,
		0.000004,
		0.000003,
		0.000241,
		0.000043,
		0.000042,
		0.000042,
		0.004019,
		0.000002,
		0.000042,
		0.016071,
		0.000043,
		0.000042,
		0.000082,
		0.000042,
		0.000041,
		0.000003,
		0.000002,
		0.000003,
		0.000166,
		0.010985,
		0.000116,
		0.000093,
		0.005054,
		0.112291,
		0.040448,
		0.000001,
		0.000079,
		0.000044,
		0.000039,
		0.016110,
		0.000078,
		0.000003,
		0.000119,
		0.000040,
		0.004109,
		0.000001,
		0.000080,
		0.004098,
		0.000119,
		0.004095,
		0.012095,
		0.000002,
		0.004175,
		0.000081,
		0.000679,
		0.000003,
		0.000157,
		0.000044,
		0.000121,
		0.000003,
		0.000083,
		0.000081,
		0.000082,
		0.000117,
		0.000042,
		0.012090,
		0.000042,
		0.004018,
		0.004063,
		0.000080,
		0.028201,
		0.000082,
		0.000119,
		0.000003,
		0.000083,
		0.016070,
		0.000045,
		0.000043,
		0.000044,
		0.000001,
		0.004099,
		0.000002,
		0.000119,
		0.000001,
		0.000042,
		0.000004,
		0.000040,
		0.000041,
		0.000043,
		0.000083,
		0.012177,
		0.000050,
		0.001519,
		0.000002,
		0.000044,
		0.000004,
		0.005258,
		0.000004,
		0.000044,
		0.000044,
		0.000042,
		0.040286,
		0.000083,
		0.000043,
		0.000119,
		0.000121,
		0.000236,
		0.000003,
		0.000083,
		0.000005,
		0.000120,
		0.004023,
		0.000081,
		0.000043,
		0.000044,
		0.000003,
		0.000048,
		0.000002,
		0.000082,
		0.004022,
		0.000080,
		0.024106,
		0.000081,
		0.000004,
		0.000044,
		0.000003,
		0.008080,
		0.000007,
		0.000080,
		0.000005,
		0.000043,
		0.004023,
		0.000043,
		0.000002,
		0.000042,
		0.000008,
		0.000043,
		0.000003,
		0.000043,
		0.000041,
		0.000044,
		0.000004,
		0.016112,
		0.000045,
		0.000121,
		0.000006,
		0.000081,
		0.008075,
		0.002479,
		0.000004,
		0.000044,
		0.000046,
		0.020569,
		0.000007,
		0.000047,
		0.000010,
		0.000047,
		0.000017,
		0.000372,
		0.000003,
		0.000044,
		0.000003,
		0.008117,
		0.000004,
		0.000119,
		0.004058,
		0.000082,
		0.000003,
		0.000043,
		0.000041,
		0.004058,
		0.000003,
		0.008130,
		0.000053,
		0.000042,
		0.000079,
		0.000044,
		0.012330,
		0.000082,
		0.000002,
		0.008114,
		0.000004,
		0.000119,
		0.012132,
		0.000042,
		0.000041,
		0.000203,
		0.000242,
		0.001557,
		0.000003,
		0.000079,
		0.000004,
		0.000043,
		0.012059,
		0.000043,
		0.004021,
		0.000162,
		0.000040,
		0.010512,
		0.000003,
		0.004300,
		0.000081,
		0.000043,
		0.024105,
		0.000046,
		0.000041,
		0.000081,
		0.000004,
		0.000043,
		0.000002,
		0.000043,
		0.000003,
		0.000045,
		0.008114,
		0.000085,
		0.000002,
		0.000042,
		0.000004,
		0.000044,
		0.000003,
		0.000044,
		0.000003,
		0.000203,
		0.000085
};
