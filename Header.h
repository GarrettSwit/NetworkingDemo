#include <thread>
#pragma once

class SomeClass {  // arbitrary class example 

public :
	SomeClass();  // constructor
	~SomeClass();	//destructor ___ deletes the new thread that was created for cleanup
	void Init();
	void Draw() { while(!complete){} };
	void Update() { while (!complete) {} };

protected:
	std::thread* DrawThread = nullptr;
	std::thread* UpdateThread = nullptr;

	bool complete = false;

};

SomeClass::SomeClass() {

	Init();
	DrawThread = new std::thread(&SomeClass::Draw, this);

}
SomeClass::~SomeClass() {

	complete = true;
	if (DrawThread) {
		DrawThread->join();
		delete DrawThread;
	}
}

void SomeClass::Init() {

}