#include "eecs485pa6p.h"
using namespace std;

int main(int argc, char *argv[]){
	
	//////////////////////////////////read in from command line////////////////////////////////////////////
	string d_val_string = argv[1];
	string stopping_criteria = argv[2];
	string stopping_val_string = argv[3];
	string infile_string = argv[4];
	string outfile_string = argv[5];
	
	//convert stoppingval to double, convert d_val to double
	double d_val = atof(d_val_string.c_str());
	double stopping_val = atof(stopping_val_string.c_str());

	unordered_map<long int, Node> graph;

	//Create a fstream to read in from infile
	ifstream infile;
	infile.open(infile_string);

	//int numVertices = 0;
	//int numEdges = 0;
	
	//Variables for parsing
	string line;
	string junk;
	//long int docId;
	string word;
	string srcIdStr;
	string destIdStr;
	long int sourceId; //Node that the link is leaving from
	long int destId; //Link pointing to this node
	Node node;

	
	getline(infile, line, '>');
	if (line != "<eecs485_edges") {
		cerr << "Malformed input" << endl;
		exit(1);
	}

	stringstream ss;
	while (getline(infile, line, '>')) { //<eecs485_edge>
		// if ()
		getline(infile, line, '>'); //<eecs485_from
		//get sourceId
		getline(infile, srcIdStr, '<');
		// ss << line;
		// getline(ss, junk, '>');
		// getline(ss, srcIdStr, '<');
		sourceId = stol(srcIdStr);
		//check if has ending tag?

		//get destId
		getline(infile, line, '>'); //</eecs485_from
		getline(infile, line, '>'); //<eecs485_to
		getline(infile, destIdStr, '<');
		// ss << line;
		// getline(ss, junk, '>');
		// getline(ss, destIdStr, '<');
		destId = stol(destIdStr);			
		//check if has ending tag?

		unordered_map<long int, Node>::const_iterator foundSrc = graph.find(sourceId);
		if (foundSrc == graph.end()) { //if don't find key in map already
			graph.insert(pair<long int, Node>(sourceId, node));
		}

		unordered_map<long int, Node>::const_iterator foundDest = graph.find(destId);
		if (foundDest == graph.end()) { //if don't find key in map already
			graph.insert(pair<long int, Node>(destId, node));
		}

		if(sourceId != destId){
			graph[sourceId].outgoing.push_back(destId);
			graph[destId].incoming.push_back(sourceId);
		}

		getline(infile, line, '>'); //</eecs485_to>
		getline(infile, line, '>'); //</eecs485_edge>
	}
	
	// ofstream outfile;
	// outfile.open(outfile_string);

	long int graphSize = graph.size();
	//outfile << graphSize << endl;
	//go back through map and set oldPR
	for (auto& it : graph) {
		it.second.oldPR = (1 / (double)graphSize);
	}







/*

	//Obtain the amount of vertices
	infile >> junk >> numVertices;
	Node node;
	//Reading in the initial vertices/words
	for(int i = 0; i < numVertices; ++i){
		infile >> docId >> word;
		node.word = word;
		graph.insert(pair<long int, Node>(docId, node));
		graph[docId].oldPR = (1 / (double)numVertices); //do later FIXME
	}

	infile >> junk >> numEdges;
	//Reading in the source ids and destination ids
	long int sourceId; //Node that the link is leaving from
	long int destId; //Link pointing to this node
	for(int i = 0; i < numEdges; ++i){
		infile >> sourceId >> destId;
		if(sourceId != destId){
			graph[sourceId].outgoing.push_back(destId);
			graph[destId].incoming.push_back(sourceId);
		}
	}
	
*/











	infile.close();

	vector<long int> sinkNodes(0);
	
	for (auto& it : graph) {
		if (it.second.outgoing.size() == 0) {
			sinkNodes.push_back(it.first);
		}
	}

	bool iterations = false;
	if (stopping_criteria == "-k") {
		iterations = true;
	}	

	// int numIterations = 0;
	if (iterations) {
		for (auto iter = 0; iter < stopping_val; iter++) { 
			//calculate extra sink node page rank 
			double tmpSNPgRk = 0;
			for (auto k = 0; k < sinkNodes.size(); ++k) {
				tmpSNPgRk += (graph[sinkNodes[k]].oldPR / (graphSize - 1));
			}
			// tmpSNPgRk /= (graphSize - 1);
			//for every key/node in the graph
			for (auto& it : graph) {
				double pageRank = (1 - d_val) / graphSize;
				double tmpPgRk = 0;
				//for every node pointing to the given node
				for (auto j = 0; j < it.second.incoming.size(); j++) {
					long int tmpID = it.second.incoming[j];
					if (graph[tmpID].outgoing.size() == 0) { //is a sink node
						tmpPgRk += (graph[tmpID].oldPR / (graphSize - 1));
					} else {
						tmpPgRk += (graph[tmpID].oldPR / graph[tmpID].outgoing.size());
					}
				}
				tmpPgRk += tmpSNPgRk;
				pageRank += (d_val * tmpPgRk);
				it.second.newPR = pageRank;
			}
			//set oldPR to the newPR for all nodes
			for (auto& it : graph) {
				it.second.oldPR = it.second.newPR;
			}
		}
	} else { //converge command line argument
		bool again = true;
		while (again) {
			// numIterations++;
			again = false; //assume this will be last iteration
			//calculate extra sink node page rank 
			double tmpSNPgRk = 0;
			for (auto k = 0; k < sinkNodes.size(); ++k) {
				tmpSNPgRk += (graph[sinkNodes[k]].oldPR / (graphSize - 1));
			}
			//for every key/node in the graph
			for (auto& it : graph) {
				double pageRank = (1 - d_val) / graphSize;
				double tmpPgRk = 0;
				//for every node pointing to the given node
				for (auto j = 0; j < it.second.incoming.size(); j++) {
					long int tmpID = it.second.incoming[j];
					if (graph[tmpID].outgoing.size() == 0) { //is a sink node
						tmpPgRk += (graph[tmpID].oldPR / (graphSize - 1));
					} else {
						tmpPgRk += (graph[tmpID].oldPR / graph[tmpID].outgoing.size());
					}
				}
				tmpPgRk += tmpSNPgRk;
				pageRank += (d_val * tmpPgRk);
				it.second.newPR = pageRank;
			}
			//set oldPR to the newPR for all nodes
			for (auto& it : graph) {
				double diff = abs(it.second.newPR - it.second.oldPR) / it.second.oldPR;
				if (diff > stopping_val) {
					again = true;
				}
				it.second.oldPR = it.second.newPR;
			}
		}
	}

	ofstream outfile;
	outfile.open(outfile_string);

	// outfile << numIterations << "\n";
	double totPageRank = 0;
	for (auto& it : graph) {
		outfile << it.first << ", " << fixed << setprecision(4) << scientific << it.second.oldPR << "\n";
		totPageRank += it.second.oldPR;
	}
	//outfile << "THIS IS THE TOTAL PLEASE BE 1: " << totPageRank << "\n";

	outfile.close();

	return 0;
}