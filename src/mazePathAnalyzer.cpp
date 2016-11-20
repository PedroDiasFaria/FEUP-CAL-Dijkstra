#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include "Graph.h"
#include "graphviewer.h"
#include "mazeElement.h"

int TAM_I;
int TAM_J;
int partNumber = 1;
int gameOption = -1;
int option = 1;
bool heroInTheMaze = true;
int debugOption = 0;
int partOfAlgorithm = 0;

// Maze Characters positions //
mazeElement hero;
mazeElement sword;
mazeElement mazeExit;
vector<mazeElement> dragons;

using namespace std;

// Gets the filename to read from the console //
char* getName(){

	char* input = (char*)malloc(256);

	cout << "File name?" << endl ;
	cin.getline(input, 256);

	return input;
}

// Stores the the info of the maze elements (H,E,D & S) in variables to use later //
void parseCharacterPosition(char charElement, int y, int x){
	mazeElement temp;
	stringstream ss;
	switch( charElement ){
	case 'H': ss << "H(" << x << ',' << y << ")";
	hero.setX(x);
	hero.setY(y);
	hero.setIdentifier(ss.str());
	break;
	case 'D': ss << "D(" << x << ',' << y << ")";
	temp.setX(x);
	temp.setY(y);
	temp.setIdentifier(ss.str());
	dragons.push_back(temp);
	break;
	case 'S': ss << "S(" << x << ',' << y << ")";
	mazeExit.setX(x);
	mazeExit.setY(y);
	mazeExit.setIdentifier(ss.str());
	break;
	case 'E': ss << "E(" << x << ',' << y << ")";
	sword.setX(x);
	sword.setY(y);
	sword.setIdentifier(ss.str());
	break;
	default : break;
	}
}

// Parses the file from wich we are reading the maze //
char** parseFile(char* fileName){

	ifstream mazeFile;
	mazeFile.open(fileName);
	char** mazeMatrix;

	if(!mazeFile.is_open()){
		cout << "Unable to open " << fileName << ". Terminating!" << endl;
		exit(0);
	}else{
		cout << "Parsing file " << fileName << " ..." << endl;

		string line;
		int lineNr = 0;
		mazeMatrix = (char**)malloc(sizeof(char*));

		while ( getline (mazeFile,line) )
		{
			mazeMatrix = (char**)realloc(mazeMatrix, (lineNr +1 ) * sizeof(char*));
			mazeMatrix[lineNr] = (char*)malloc(line.length() * sizeof(char));

			for(int i = 0; i < line.length(); i++){
				mazeMatrix[lineNr][i] = line.at(i);
				parseCharacterPosition(line.at(i), lineNr, i);
			}

			lineNr++;
		}
		TAM_I = line.length();
		TAM_J = lineNr;
		mazeFile.close();
		cout << "Parsing end " << " ..." << endl;
		return mazeMatrix;
	}
}

void printMaze(char** maze){
	cout << "************************" << endl;
	int i, j;
	for(i = 0; i < TAM_I; i++){
		for(j = 0; j <TAM_J; j++){
			cout << maze[j][i] ;
		}
		cout << endl;
	}
	cout << "************************" << endl;
}

// Adds all vertexes to the graph and to the graphviewer //
void addVertexToGraph(char** maze, Graph<string> &mazeGraph, GraphViewer &gv){

	for(int i = 0; i < TAM_I; i++){
		for(int j = 0; j < TAM_J ; j++){
			if(maze[j][i] != 'X'){// if it's not a wall
				stringstream vertexName;
				vertexName << maze[j][i] << "(" << i << "," << j << ")";

				if(debugOption)
					cout << "Added Vertex " << vertexName.str() << endl;

				mazeGraph.addVertex(vertexName.str());

				int nodeId = 100*i + j;
				gv.addNode(nodeId, i*64, j*64);
				gv.setVertexLabel(nodeId, vertexName.str());

			}
		}
	}

}

// Creates and adds the edges to the graph and the graphviewer //
// Option 1 - ignores the dragons //
// Option 2 - considers the dragons //
void addEdgesToGraph(char** maze, Graph<string> &mazeGraph, int option, GraphViewer &gv){
	int edgeId = 0;
	for(int i = 0; i < TAM_I; i++){
		for(int j = 0; j < TAM_J ; j++){
			if(maze[j][i] != 'X' && maze[j][i] != 'S'){// if it's not a wall
				stringstream vertexName1;
				stringstream vertexName2;

				// Without dragons //
				if(option == 1 ){
					if(maze[j][i] != 'D'){
						if(maze[j][i+1] == '_' || maze[j][i+1] == 'H' || maze[j][i+1] == 'E'){
							vertexName1 << maze[j][i] << "(" << i << "," << j << ")";
							vertexName2 << maze[j][i+1] << "(" << i+1 << "," << j << ")";

							mazeGraph.addEdge(vertexName1.str(), vertexName2.str(), 1);
							mazeGraph.addEdge(vertexName2.str(), vertexName1.str(), 1);

							int nodeId1 = 100*i + j;
							int nodeId2 = 100*(i+1) + j;
							gv.addEdge(edgeId,nodeId1,nodeId2,EdgeType::UNDIRECTED);
							edgeId++;

							vertexName1.str("");
							vertexName2.str("");
						}
						if(maze[j+1][i] == '_' || maze[j+1][i] == 'H' || maze[j+1][i] == 'E'){
							vertexName1 << maze[j][i] << "(" << i << "," << j << ")";
							vertexName2 << maze[j+1][i] << "(" << i << "," << j+1 << ")";

							mazeGraph.addEdge(vertexName1.str(), vertexName2.str(), 1);
							mazeGraph.addEdge(vertexName2.str(), vertexName1.str(), 1);

							int nodeId1 = 100*i + j;
							int nodeId2 = 100*i + j+1;
							gv.addEdge(edgeId,nodeId1,nodeId2,EdgeType::UNDIRECTED);
							edgeId++;

							vertexName1.str("");
							vertexName2.str("");
						}
					}

				}else{ // With dragons //
					if(maze[j][i+1] == '_' || maze[j][i+1] == 'H' || maze[j][i+1] == 'E' || maze[j][i+1] == 'D'){
						vertexName1 << maze[j][i] << "(" << i << "," << j << ")";
						vertexName2 << maze[j][i+1] << "(" << i+1 << "," << j << ")";

						mazeGraph.addEdge(vertexName1.str(), vertexName2.str(), 1);
						mazeGraph.addEdge(vertexName2.str(), vertexName1.str(), 1);

						int nodeId1 = 100*i + j;
						int nodeId2 = 100*(i+1) + j;
						gv.addEdge(edgeId,nodeId1,nodeId2,EdgeType::UNDIRECTED);
						edgeId++;

						vertexName1.str("");
						vertexName2.str("");
					}
					if(maze[j+1][i] == '_' || maze[j+1][i] == 'H' || maze[j+1][i] == 'E' || maze[j+1][i] == 'D'){
						vertexName1 << maze[j][i] << "(" << i << "," << j << ")";
						vertexName2 << maze[j+1][i] << "(" << i << "," << j+1 << ")";

						mazeGraph.addEdge(vertexName1.str(), vertexName2.str(), 1);
						mazeGraph.addEdge(vertexName2.str(), vertexName1.str(), 1);

						int nodeId1 = 100*i + j;
						int nodeId2 = 100*i + j+1;
						gv.addEdge(edgeId,nodeId1,nodeId2,EdgeType::UNDIRECTED);
						edgeId++;

						vertexName1.str("");
						vertexName2.str("");
					}
				}
			}else if(maze[j][i] ==  'S'){ // Process the exit edge connection with the graph //
				stringstream vertexName1;
				stringstream vertexName2;

				// If the exits is in the first line //
				if(j == 0){
					vertexName1 << maze[j][i] << "(" << i << "," << j << ")";
					vertexName2 << maze[j+1][i] << "(" << i << "," << j+1 << ")";

					mazeGraph.addEdge(vertexName1.str(), vertexName2.str(), 1);
					mazeGraph.addEdge(vertexName2.str(), vertexName1.str(), 1);

					int nodeId1 = 100*i + j;
					int nodeId2 = 100*i + j+1;
					gv.addEdge(edgeId,nodeId1,nodeId2,EdgeType::UNDIRECTED);
					edgeId++;

				}else if(j == TAM_J -1){ // If the exit is the last line //
					vertexName1 << maze[j][i] << "(" << i << "," << j << ")";
					vertexName2 << maze[j-1][i] << "(" << i << "," << j-1 << ")";

					mazeGraph.addEdge(vertexName1.str(), vertexName2.str(), 1);
					mazeGraph.addEdge(vertexName2.str(), vertexName1.str(), 1);

					int nodeId1 = 100*i + j;
					int nodeId2 = 100*i + j-1;
					gv.addEdge(edgeId,nodeId1,nodeId2,EdgeType::UNDIRECTED);
					edgeId++;

				}else if(i == 0){ // If the exit is in the first column //
					vertexName1 << maze[j][i] << "(" << i << "," << j << ")";
					vertexName2 << maze[j][i+1] << "(" << i+1 << "," << j << ")";

					mazeGraph.addEdge(vertexName1.str(), vertexName2.str(), 1);
					mazeGraph.addEdge(vertexName2.str(), vertexName1.str(), 1);

					int nodeId1 = 100*i + j;
					int nodeId2 = 100*(i+1) + j;
					gv.addEdge(edgeId,nodeId1,nodeId2,EdgeType::UNDIRECTED);
					edgeId++;

				}else if(i == TAM_I -1){ // If the exit is in the last column //
					vertexName1 << maze[j][i] << "(" << i << "," << j << ")";
					vertexName2 << maze[j][i-1] << "(" << i-1 << "," << j << ")";

					mazeGraph.addEdge(vertexName1.str(), vertexName2.str(), 1);
					mazeGraph.addEdge(vertexName2.str(), vertexName1.str(), 1);

					int nodeId1 = 100*i + j;
					int nodeId2 = 100*(i-1) + j;
					gv.addEdge(edgeId,nodeId1,nodeId2,EdgeType::UNDIRECTED);
					edgeId++;

				}
			}
		}
	}

}

// Prints the vertex set in the graph //
void printVertex(Graph<string> graph){
	cout << "Vertexes" << endl;

	vector< Vertex<string>* > vertices = graph.getVertexSet();

	for (unsigned int i = 0; i < vertices.size(); i++){
		cout << vertices[i]->getInfo() << endl;
	}

	cout << "End printVertexes" << endl;
}


int calculateDragonToKill(Graph<string> mazeGraph){
	int sizeToBeat = 999;
	int dragonNumber = 0;
	mazeGraph.dijkstraShortestPath(hero.getIdentifier());
	vector<string> path;
	for(unsigned int i = 0; i < dragons.size(); i++){
		cout << "Dragon" <<dragons.at(i).getIdentifier() << endl;
		path = mazeGraph.getPath(hero.getIdentifier(),dragons.at(i).getIdentifier());
		if(debugOption){
			for( int j = 0; j < path.size(); j++){
				cout << "PATH: " << path[j] << endl;
			}
		}
		if(path.size() < sizeToBeat){
			cout << "Dragonnumber " << i << endl;
			sizeToBeat = path.size();
			dragonNumber = i;
		}
	}
	cout << "DRAGON : " << dragonNumber << endl;
	return dragonNumber;
}

// Process the graph and applies the Dijkstra //
vector<string> processGraphPart(Graph<string> mazeGraph, char** mazeMatrix){

	// Apply Dijkstra to the maze Graph //
	mazeGraph.dijkstraShortestPath(hero.getIdentifier());
	vector<string> path;
	stringstream ss;
	int dragonIndex;

	switch(partNumber){
	case 1 :
		// Part 1 - Hero finds the sword //
		partOfAlgorithm = 1;
		path = mazeGraph.getPath(hero.getIdentifier(),sword.getIdentifier());
		if(path.at(0) != sword.getIdentifier()){
			// Change matrix elements after hero catches sword //
			mazeMatrix[hero.getY()][hero.getX()] = '_';
			hero.setX(sword.getX()); hero.setY(sword.getY());
			mazeMatrix[hero.getY()][hero.getX()] = 'H';
			ss << "H(" << hero.getX() << ',' << hero.getY() << ")";
			hero.setIdentifier(ss.str());
			option = 2;
			partNumber = 2;
		}else{
			cout << "Maze impossible to solve" << endl;
			exit(-1);
		}
		break;
	case 2 :
		// Part 2 - Hero kills the dragons //
		partOfAlgorithm = 2;
		dragonIndex = calculateDragonToKill(mazeGraph);
		path = mazeGraph.getPath(hero.getIdentifier(),dragons.at(dragonIndex).getIdentifier());
		// Change matrix elements after hero kills the dragons //
		mazeMatrix[hero.getY()][hero.getX()] = '_';
		hero.setX(dragons.at(dragonIndex).getX()); hero.setY(dragons.at(dragonIndex).getY());
		mazeMatrix[hero.getY()][hero.getX()] = 'H';
		ss << "H(" << hero.getX() << ',' << hero.getY() << ")";
		hero.setIdentifier(ss.str());
		dragons.erase(dragons.begin()+dragonIndex);
		if(dragons.empty())
			partNumber = 3;
		break;
	case 3 :
		// Part 3 - Hero finds the exit //
		partOfAlgorithm = 3;
		path = mazeGraph.getPath(hero.getIdentifier(),mazeExit.getIdentifier());
		heroInTheMaze = false;
		break;
	default : break;
	}

	ss.str("");
	for(unsigned int i = 0; i < path.size(); i++) {
		ss << path[i] << " ";
	}

	cout << "Shortest path in part " << partOfAlgorithm << " : " << ss.str() << endl;
	cout << "Total distance : " << path.size() << endl;

	return path;
}

// Gets the x and y of the coordinate in the path string //
int pathGetCoord(string path, int coord){
	stringstream stringStream(path);
	stringstream ss;
	string line;
	vector<string> wordVector;
	while(std::getline(stringStream, line))
	{
		std::size_t prev = 0, pos;
		while ((pos = line.find_first_of("(),", prev)) != std::string::npos)
		{
			if (pos > prev)
				wordVector.push_back(line.substr(prev, pos-prev));
			prev = pos+1;
		}
		if (prev < line.length())
			wordVector.push_back(line.substr(prev, std::string::npos));
	}

	ss << wordVector[coord];
	int id;
	istringstream ( ss.str() ) >> id;
	return id;
}

void moveDragons(char** mazeMatrix){
	int dragonDirection;
	stringstream ss;
	for(int i=0;i<dragons.size();i++){
		bool dragonMoveNotValid = true;
		ss.str("");
		//loop to guarantee that the dragon movement dont overlay the 'X' marks on the maze
		while(dragonMoveNotValid == true){
			//randomly calculates the next movement of the dragon
			dragonDirection = rand() % 5; // || 0-do not move || 1-up || 2-down || 3-left || 4-right

			switch(dragonDirection){
			//do not move
			case 0 : dragonMoveNotValid = false;
			break;
			//up
			case 1 : if(mazeMatrix[dragons.at(i).getY()-1][dragons.at(i).getX()] == '_')
				dragonMoveNotValid = false;
			break;
			//down
			case 2: if(mazeMatrix[dragons.at(i).getY()+1][dragons.at(i).getX()] == '_')
				dragonMoveNotValid = false;
			break;
			//left
			case 3: if(mazeMatrix[dragons.at(i).getY()][dragons.at(i).getX()-1] == '_')
				dragonMoveNotValid = false;
			break;
			//right
			case 4: if(mazeMatrix[dragons.at(i).getY()][dragons.at(i).getX()+1] == '_')
				dragonMoveNotValid = false;
			break;
			}

		}

		//cycle to move the dragon
		switch(dragonDirection){
		//do not move
		case 0 : break;
		//up
		case 1 :mazeMatrix[dragons.at(i).getY()][dragons.at(i).getX()] = '_';
		dragons.at(i).setY(dragons.at(i).getY()-1);
		mazeMatrix[dragons.at(i).getY()][dragons.at(i).getX()] = 'D';
		ss << "D(" << dragons.at(i).getX() << ',' << dragons.at(i).getY() << ")";
		dragons.at(i).setIdentifier(ss.str());
		break;
		//down
		case 2: mazeMatrix[dragons.at(i).getY()][dragons.at(i).getX()] = '_';
		dragons.at(i).setY(dragons.at(i).getY()+1);
		mazeMatrix[dragons.at(i).getY()][dragons.at(i).getX()] = 'D';
		ss << "D(" << dragons.at(i).getX() << ',' << dragons.at(i).getY() << ")";
		dragons.at(i).setIdentifier(ss.str());
		break;
		//left
		case 3: mazeMatrix[dragons.at(i).getY()][dragons.at(i).getX()] = '_';
		dragons.at(i).setX(dragons.at(i).getX()-1);
		mazeMatrix[dragons.at(i).getY()][dragons.at(i).getX()] = 'D';
		ss << "D(" << dragons.at(i).getX() << ',' << dragons.at(i).getY() << ")";
		dragons.at(i).setIdentifier(ss.str());
		break;
		//right
		case 4: mazeMatrix[dragons.at(i).getY()][dragons.at(i).getX()] = '_';
		dragons.at(i).setX(dragons.at(i).getX()+1);
		mazeMatrix[dragons.at(i).getY()][dragons.at(i).getX()] = 'D';
		ss << "D(" << dragons.at(i).getX() << ',' << dragons.at(i).getY() << ")";
		dragons.at(i).setIdentifier(ss.str());
		break;
		}
	}
}

vector<string> processGraphWithRandomDragons(Graph<string> mazeGraph, char** mazeMatrix){

	// Apply Dijkstra to the maze Graph //
	mazeGraph.dijkstraShortestPath(hero.getIdentifier());
	vector<string> path;
	stringstream ss;
	int dragonIndex;
	int heroNextX, heroNextY;

	switch(partNumber){
	case 1 :
		// Part 1 - Hero is trying to find the sword //
		path = mazeGraph.getPath(hero.getIdentifier(),sword.getIdentifier());
		cout << path[0] << endl;
		if(path.at(0) != sword.getIdentifier()){
			heroNextX = pathGetCoord(path[1],1);
			heroNextY = pathGetCoord(path[1],2);
			// Change the hero position //
			mazeMatrix[hero.getY()][hero.getX()] = '_';
			hero.setX(heroNextX); hero.setY(heroNextY);
			mazeMatrix[hero.getY()][hero.getX()] = 'H';
			ss << "H(" << hero.getX() << ',' << hero.getY() << ")";
			hero.setIdentifier(ss.str());
			if( hero.getX() == sword.getX() && hero.getY() == sword.getY()){ // Hero is in the sword location //
				option = 2;
				partNumber = 2;
			}
		}else{
			cout << "Impossible to compute path to sword this turn" << endl;
			path[0] = hero.getIdentifier();
		}
		break;
	case 2 :
		// Part 2 - Hero is trying to kill the dragons //
		printVertex(mazeGraph);
		dragonIndex = calculateDragonToKill(mazeGraph);
		path = mazeGraph.getPath(hero.getIdentifier(),dragons.at(dragonIndex).getIdentifier());
		heroNextX = pathGetCoord(path[1],1);
		heroNextY = pathGetCoord(path[1],2);
		// Change the hero position //
		mazeMatrix[hero.getY()][hero.getX()] = '_';
		hero.setX(heroNextX); hero.setY(heroNextY);
		mazeMatrix[hero.getY()][hero.getX()] = 'H';
		ss << "H(" << hero.getX() << ',' << hero.getY() << ")";
		hero.setIdentifier(ss.str());
		if( hero.getX() == dragons.at(dragonIndex).getX() && hero.getY() == dragons.at(dragonIndex).getY()){ // Hero is at the dragon position //
			dragons.erase(dragons.begin()+dragonIndex);
		}
		if(dragons.empty())
			partNumber = 3;
		break;
	case 3 :
		// Part 3 - Hero finds the exit //
		path = mazeGraph.getPath(hero.getIdentifier(),mazeExit.getIdentifier());
		heroInTheMaze = false;
		break;
	default : break;
	}

	ss.str("");
	for(unsigned int i = 0; i < path.size(); i++) {
		ss << path[i] << " ";
	}

	cout << "Shortest path in part " << partNumber << " : " << ss.str() << endl;
	cout << "Total distance : " << path.size() << endl;

	return path;
}


// Parses the string of the path and converts it to the id of the edge on the graphviewer //
int processIdOfVertex(string pathName){
	stringstream stringStream(pathName);
	stringstream xString;
	stringstream yString;
	int x, y;
	string line;
	vector<string> wordVector;
	while(std::getline(stringStream, line))
	{
		std::size_t prev = 0, pos;
		while ((pos = line.find_first_of("(),", prev)) != std::string::npos)
		{
			if (pos > prev)
				wordVector.push_back(line.substr(prev, pos-prev));
			prev = pos+1;
		}
		if (prev < line.length())
			wordVector.push_back(line.substr(prev, std::string::npos));
	}
	xString << wordVector[1];
	yString << wordVector[2];

	int id;
	istringstream(xString.str()) >> x;
	istringstream(yString.str()) >> y;

	id = x*100 + y;
	return id;
}

// Colors the path in the graphviewer in blue. Start position in green and final position in red //
void colorPath(GraphViewer &gv, vector<string> path){
	for(unsigned int i = 0; i < path.size(); i++) {
		if(i == 0)
			gv.setVertexColor(processIdOfVertex(path[i]), "green");
		else
			if(i == path.size()-1)
				gv.setVertexColor(processIdOfVertex(path[i]), "red");
			else
				gv.setVertexColor(processIdOfVertex(path[i]), "blue");
	}

	for(unsigned int i = 0; i < dragons.size(); i++){
		gv.setVertexColor(processIdOfVertex(dragons.at(i).getIdentifier()), "black");
	}
}

// Displays the graphviewer //
void displayGraph(char** mazeMatrix){
	stringstream ss;
	vector<string> path;
	// GraphViewer //
	GraphViewer *gv = new GraphViewer(640, 640, false);
	gv->createWindow(1000, 1000);

	// Graph //
	Graph<string> mazeGraph;

	// Add vertex to graphs and graph viewer //
	addVertexToGraph(mazeMatrix, mazeGraph, *gv);
	gv->rearrange();

	if(debugOption)
		printVertex(mazeGraph);

	// Add edges to graph and graph viewer //
	addEdgesToGraph(mazeMatrix, mazeGraph, option, *gv);

	if(gameOption == 1){
		// Process the actual part of the graph //
		path = processGraphPart(mazeGraph, mazeMatrix);
	}else{
		path = processGraphWithRandomDragons(mazeGraph, mazeMatrix);
	}
	// Color the path //
	colorPath(*gv, path);
	moveDragons(mazeMatrix);
	if(gameOption == 2){
		sleep(5);
	}
	else{
		//For testing each step
		cout << "Enter 'x' to continue" << endl;
		char stop[1];
		stop[0] = 'z';
		while(stop[0] != 'x' )
			cin.getline(stop, 2);
	}
		gv->closeWindow();
	}

	int main(){
		srand(time(0));
		char* fileName = getName();


		cout << "-------------" << endl;
		cout << "Debug? Yes(1)/No(0)" << endl;
		cin >> debugOption;

		while(strlen(fileName) < 5){
			cout << "Please insert valid name" << endl;
			fileName = getName();
		}

		char** mazeMatrix;
		mazeMatrix = parseFile(fileName);

		if(debugOption)
			printMaze(mazeMatrix);

		while(gameOption!= 1 && gameOption!= 2){
			cout << "-------------" << endl;
			cout << "Select dragon movement:" << endl;
			cout << "(1) - No movement" << endl;
			cout << "(2) - Random movement" << endl;
			cout << "-------------" << endl;

			cin >> gameOption;
		}

		while(heroInTheMaze){
			displayGraph(mazeMatrix);
		}

		cout << "Maze Exit found." << endl;
		cout << " _______ _            ______           _ _ " << endl;
		cout << "|__   __| |          |  ____|         | | |" << endl;
		cout << "   | |  | |__   ___  | |__   _ __   __| | |" << endl;
		cout << "   | |  | '_ \\ / _ \\ |  __| | '_ \\ / _` | |" << endl;
		cout << "   | |  | | | |  __/ | |____| | | | (_| |_|" << endl;
		cout << "   |_|  |_| |_|\\___| |______|_| |_|\\__,_(_)" << endl;
		return 1;
	}
