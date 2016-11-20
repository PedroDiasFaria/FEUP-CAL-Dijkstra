#ifndef MAZEELEMENT_H_
#define MAZEELEMENT_H_
using namespace std;

class mazeElement{
	private:
		int x;
		int y;
		string identifier;
	public:
		void setX(int x);
		void setY(int y);
		int getX();
		int getY();
		void setIdentifier(string identifier);
		string getIdentifier();
};

void mazeElement::setX(int x){
	this->x = x;
}

void mazeElement::setY(int y){
	this->y = y;
}

void mazeElement::setIdentifier(string identifier){
	this->identifier = identifier;
}

int mazeElement::getX(){
	return this->x;
}

int mazeElement::getY(){
	return this->y;
}

string mazeElement::getIdentifier(){
	return this->identifier;
}



#endif
