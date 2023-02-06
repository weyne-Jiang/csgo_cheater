#pragma once
#include "PlayerController.hpp"
#include "DrawHelper.hpp"
#include "Single.hpp"

using namespace DrawHelper;

//class Cheater : public Singleton<Cheater>
//{
//    friend class Singleton<Cheater>;
//public:
//	
//
//private:
//	Cheater();
//    ~Cheater();
//
//private:
//	PlayerController* _pPlayerController;
//
//};
//
//Cheater::Cheater()
//{
//	cout << "Cheater()\n";
//}
//
//Cheater::~Cheater()
//{
//	cout << "~Cheater()\n";
//}

class Cheater : public Single<Cheater>
{
	friend class Single<Cheater>;
public:


private:
	Cheater() = default;
	~Cheater() = default;
};



