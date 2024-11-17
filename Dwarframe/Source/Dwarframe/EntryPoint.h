#ifndef ENTRYPOINT_H
#define ENTRYPOINT_H

extern Dwarframe::GameManager* Dwarframe::CreateGameManager();

int main(int argc, char** argv)
{
	Dwarframe::GameManager* Game = Dwarframe::CreateGameManager();

	Game->Initialize();
	Game->Run();
	Game->Shutdown();

	delete Game;

	return 0;
}

#endif // !ENTRYPOINT_H