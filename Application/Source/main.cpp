#include "Dwarframe.h"

class Game : public Dwarframe::GameManager
{
public:
	Game() = default;
	virtual ~Game() = default;
};

Dwarframe::GameManager* Dwarframe::CreateGameManager()
{
	int32_t Var1 = 3;
	double Var2 = 2.0;
	bool Var3 = false;

	auto Lambda = [Var1, Var4 = Var2, Var3]() -> float {};

	return new Game();
}

