#pragma once
#include "PlayerController.hpp"
#include "DrawHelper.hpp"
#include "Single.hpp"

using namespace DrawHelper;

class Cheater : public Singleton<Cheater>
{
    friend class Singleton<Cheater>;
public:
	

private:
	Cheater();
    ~Cheater();


};

Cheater::Cheater()
{
	cout << "Cheater()\n";
}

Cheater::~Cheater()
{
	cout << "~Cheater()\n";
}


