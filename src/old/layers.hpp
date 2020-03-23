int iactive_layer = 0;
std::vector<Layer*> all_layers = { &editor, &game, &battle };
std::map<std::string, Layer*> layer_map = {
	{ "editor", &editor },
	{ "game", &game },
	{ "battle", &battle },
};
Layer* active_layer = &editor;
#define EDITOR_IDX   0
#define GAME_IDX     1
#define BATTLE_IDX   2
