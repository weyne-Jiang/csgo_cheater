#pragma once
#include "PlayerController.hpp"
#include "DrawHelper.hpp"
#include "Single.hpp"

using namespace DrawHelper;

class Cheater : public Single<Cheater>
{
    friend class Single<Cheater>;
public:
	

private:
	Cheater();
    ~Cheater();

private:
	PlayerController* _pPlayerController;

};

Cheater::Cheater()
{
	cout << "Cheater()\n";
}

Cheater::~Cheater()
{
	cout << "~Cheater()\n";
}

//class Cheater : public Single<Cheater>
//{
//	friend class Single<Cheater>;
//public:
//	Cheater(int haha)
//	{
//		printf("Cheater(int %d)\n", haha);
//	}
//
//private:
//	Cheater() = default;
//	~Cheater() = default;
//};



