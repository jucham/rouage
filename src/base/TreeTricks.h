#ifndef TREETRICKS_H_
#define TREETRICKS_H_

#include <base/Huffman.h>

static void PrintLatexTree(Node *pn, int depth, int son) {

		static const int MAX_DEPTH = 6;
		static const int LEFT_SON = 1;
		static const int RIGHT_SON = 2;

		if (depth == 0) {
			printf("\\begin{tikzpicture}");

			printf("[");
			printf("lf/.style={draw,thick,fill=blue!20},");
			printf("nd/.style={draw,thick,fill=red!20},");

			float distance = 2.0f * MAX_DEPTH;
			for(int i=1; i<=MAX_DEPTH; i++) {

				printf("level %d/.style={sibling distance=%0.1fcm}", i, distance);
				distance /= 2;

				if (i<MAX_DEPTH) {
					printf(",");
				}
			}
			printf("]");

			printf("\\");
		}


		if (depth > MAX_DEPTH) {
			printf("node[nd]{*}");
			return;
		}

		if (pn->IsLeaf()) {
			printf("node[lf]{%d}", pn->pLiteral->value);

		} else {
			printf("node[nd]{.}");
		}

		if (!pn->IsLeaf()) {
			printf("child{");
			PrintLatexTree(pn->pLS, depth+1, LEFT_SON);
			printf("}");

			printf("child{");
			PrintLatexTree(pn->pRS, depth+1, RIGHT_SON);
			printf("}");
		}

		if (depth == 0) {
			printf(";\n");
			printf("\\end{tikzpicture}\n");
		}

	}

#endif
