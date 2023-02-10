#include <stdarg.h>
#include <stdio.h>      /* vsnprintf */

#include "enclave.h"
#include "enclave_t.h"  /* print_string */

#include "mbt/Crypto.h"
#include "mbt/Graph.h"
#include "mbt/Node.h"
#include "mbt/MerkleBTree.h"
#include "mbt/NodeHeap.h"
#include "mbt/Tools.h"
#include "mbt/AuthenticationTree.h"


class MyVisitor : public IVisitor
{
public:
	unsigned long m_indexIO;
	unsigned long m_leafIO;

public:
	MyVisitor() : m_indexIO(0), m_leafIO(0) {}

	void visitNode(const INode& n) {
		if (n.isLeaf()) m_leafIO++;
		else m_indexIO++;
	}

	void visitData(const IData& d)
	{
		IShape* pS;
		d.getShape(&pS);
			// do something.
		Point key;
		pS->getCenter(key);
		delete pS;

		////cout << "visitData: key="<< key.m_pCoords[0] << "; id=" << d.getIdentifier() << endl;
		////cout << endl;
		printf("visitData: key=%.0f; id=%ld\n\n", key.m_pCoords[0], d.getIdentifier());
	    // the ID of this data entry is an answer to the query. I will just print it to stdout.
	}

	void visitData(std::vector<const IData*>& v) {}
};

/* 
 * printf: 
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */
void printf(const char *fmt, ...)
{
    char buf[BUFSIZ] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print(buf);
}

int ecall_enclave_sample()
{
    printf("IN ENCLAVE\n");

    double x[1] = {9};
    Point p = Point(x, 1);
    printf("p.dimension=%ld\n", p.m_dimension);
    printf("p.size=%ld\n", sizeof(p));

    //IShape* shape = &p;
    Point* shape = &p;
    printf("shape.dimension=%ld\n", shape->getDimension());
    printf("shape.size=%ld\n", sizeof(shape));
    printf("*shape.size=%ld\n", sizeof(*shape));

    Point p2 = (*shape);
    printf("p2[0]=%f\n", p2.m_pCoords[0]);

	return 0;
}

bool Graph::ecall_loadFromFile(string nodeFilename, string edgeFilename) {
	// open the node file and edge file
	// the files follow format from Lifeifei's data

	// make the graph to the empty state
	this->clear();

	// open the node file and read in node data
	// not including PoIs
	fstream inputFileStream(nodeFilename, ios::in);
	if (inputFileStream.fail() == true) {
		Tools::warning("Node File Opening Error!");
		return false;
	}
	int nodeId;
	double longitude, latitude;
	while (inputFileStream >> nodeId >> longitude >> latitude) {
		Node* n = new Node(nodeId, longitude, latitude);
		this->addNode(n);
	}
	inputFileStream.close();

	// open the edge file and read in edge data
	// including PoIs, so we need to add some additional nodes
	// and split the corresponding edge to two or more
	inputFileStream.open(edgeFilename, ios::in);
	if (inputFileStream.fail() == true) {
		Tools::warning("Edge File Opening Error!");
		return false;
	}
	int edgeCount = 0; // count the number of edges and work as edgeId as well
	int startNodeId, endNodeId, numberOfPoIs;
	double length;
	while (inputFileStream >> startNodeId >> endNodeId >> length >> numberOfPoIs) {
		if (numberOfPoIs == 0) { // if there are no pois on the edge
			Node* startNode = this->getNode(startNodeId);
			Node* endNode = this->getNode(endNodeId);

			// create a new edge
			Edge* e = new Edge(this->nextEdgeId(), startNode, endNode, length);

			// add the edge to the graph
			this->addEdge(e);

			// add the edge to the startNode and endNode
			startNode->addEdge(e);
			endNode->addEdge(e);
		}
		else { // there are pois on the edge
			Node* startNode = this->getNode(startNodeId);
			Node* endNode = this->getNode(endNodeId);

			// read next line
			// this line will contain PoI information
			string line;
			getline(inputFileStream, line);
			getline(inputFileStream, line);
			istringstream iss(line);
			int category;
			double lengthToStartNode;
			// the PoI information length are not increasing
			// so we need to change them into the increasing order
			vector<int> categoryVector;
			categoryVector.clear();
			vector<double> lengthVector;
			lengthVector.clear();
			for (int i = 0; i != numberOfPoIs; i++) {
				iss >> category >> lengthToStartNode;
				categoryVector.push_back(category);
				lengthVector.push_back(lengthToStartNode);
			}
			// use selection sort
			for (int i = 0; i != lengthVector.size() - 1; i++) {
				int mini = i;
				double minLength = lengthVector[i];
				for (int j = i + 1; j != lengthVector.size(); j++) {
					if (lengthVector[j] < minLength) {
						mini = j;
						minLength = lengthVector[j];
					}
				}
				if (mini != i) {
					int tempInt = categoryVector[i];
					categoryVector[i] = categoryVector[mini];
					categoryVector[mini] = tempInt;
					double tempDouble = lengthVector[i];
					lengthVector[i] = lengthVector[mini];
					lengthVector[mini] = tempDouble;
				}
			}
			ostringstream oss;
			for (int i = 0; i != numberOfPoIs; i++) {
				oss << categoryVector[i] << " " << lengthVector[i] << " ";
			}
			iss.str(oss.str());

			Node* lastNode = startNode; // the Node that will act as the start node of the new edge
			double lastLength = 0;
			for (int i = 0; i != numberOfPoIs; i++) {
				iss >> category >> lengthToStartNode;

				// create a new PoI node
				Node* poi = new Node(this->nextNodeId(),
					startNode->getLongitude() + (endNode->getLongitude() - startNode->getLongitude()) * (lengthToStartNode / length),
					startNode->getLatitude() + (endNode->getLatitude() - startNode->getLatitude()) * (lengthToStartNode / length),
					Node::POI);

				// add the new PoI node to the graph
				this->addNode(poi);

				// create a new corresponding edge
				Edge* edge = new Edge(this->nextEdgeId(), lastNode, poi, lengthToStartNode - lastLength);

				// add the edge to the graph
				this->addEdge(edge);

				// add the edge to the lastNode and PoI Node
				lastNode->addEdge(edge);
				poi->addEdge(edge);

				// assign PoI Node to lastNode
				lastNode = poi;

				// assign current length to last length
				lastLength = lengthToStartNode;

			}
			// create the last edge connecting the last poi on the edge and the endNode
			// create the edge
			Edge* lastEdge = new Edge(this->nextEdgeId(), lastNode, endNode, length - lastLength);

			// add the edge to the graph
			this->addEdge(lastEdge);

			// add the edge to the last poi and endNode
			lastNode->addEdge(lastEdge);
			endNode->addEdge(lastEdge);
		}
	}
	inputFileStream.close();

	return true;
}

MerkleBTree::ecall_MerkleBTree(Graph* graph, unsigned int fanout, Node::INDEXMETHOD indexMethod, std::string hashMethod) {
	// some preparation and initialization
	this->graph = graph;
	graph->setTree(this);
	this->fanout = fanout;
	this->root = NULL;
	this->hashMethod = hashMethod;

	// make index on the graph according to the indexMethod
	graph->makeIndex(indexMethod);

	// bulk load data from nodes in the graph
	// The first step is to sort the data entries according to a search key.
	graph->sortNodes();

	// add nodes to the tree
	// use normal insert here
	// should be replaced by bulk load later
	unsigned int numberOfNodes = graph->numberOfNodes();
	for (int i = 0; i != numberOfNodes; i++) {
		this->insert(graph->getNode(i));
	}

	this->initializeDigest();
}

void ecall_addKey(int index, unsigned int key) {
	this->keys.insert(this->keys.begin() + index, key);
}

void ecall_delKey() {
	this->keys.pop_back();
}

Node* ecall_getDataItem(unsigned int index) {
	if (index >= 0 && index < this->dataItems.size()) {
		return this->dataItems[index];
	}
	return NULL;
}

string MerkleBTree::ecall_generateVO(std::vector<Node*>& nodes) {
	set<int> indexSet;
	for (vector<Node*>::const_iterator iter = nodes.begin(); iter != nodes.end(); iter++) {
		indexSet.insert((*iter)->getIndexId());
	}
	return this->generateVO(this->root, indexSet);
}

bool AuthenticationTree::ecall_parseVO(std::string s) {
	if (s.length() == 0) {
		return false;
	}

	// initially the root is NULL
	// the root will be created during the parse of "s"
	this->root = NULL;
	AuthenticationTreeNode* currentTreeNode = NULL;

	unsigned int index = 0;

	char nextChar;
	while (index < s.length()) {
		nextChar = s[index];
		string tempValue = "";
		switch (nextChar) {
		case '[':
			if (currentTreeNode == NULL) {
				currentTreeNode = new AuthenticationTreeNode();
				this->root = currentTreeNode;
			}
			else {
				AuthenticationTreeNode* newNode = new AuthenticationTreeNode(currentTreeNode, false, "");
				currentTreeNode->addChildNode(newNode);
				currentTreeNode = newNode;
			}
			break;
		case ']':
			currentTreeNode = currentTreeNode->getParentNode();
			break;
		case ',':
			break;
		case 'h':
			index += 5;
			if (index >= s.length()) {
				cout << "Authentication Failed.\n";
				return false;
			}
			while (index < s.length() && s[index] != ']') {
				tempValue += s[index];
				index++;
			}
			index--; // the current char is probably ']', so go back one char
			currentTreeNode->setValue(tempValue);
			break;
		case 'n':
			index += 5;
			if (index >= s.length()) {
				cout << "Authentication Failed.\n";
				return false;
			}
			while (index < s.length() && s[index] != ']') {
				tempValue += s[index];
				index++;
			}
			index--; // the current char is probably ']', so go back one char
			currentTreeNode->setValue(tempValue);
			currentTreeNode->setIsNode(true);
			break;
		default:
			Tools::warning("The program should not go here.\n");
			exit(1);
		}
		index++;
	}

	// the authentication tree is initialized
	// next, calculate the digests
	this->calculateDigest();
	return true;
}

