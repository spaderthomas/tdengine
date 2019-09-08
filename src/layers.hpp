int iactive_layer = 0;
vector<Layer*> all_layers = { &editor, &cutscene_thing, &game, &battle};
map<string, Layer*> layer_map = {
	{ "editor", &editor },
	{ "cutscene", &cutscene_thing },
	{ "game", &game },
	{ "battle", &battle },
};
Layer* active_layer = &editor;
#define EDITOR_IDX   0
#define CUTSCENE_IDX 1
#define GAME_IDX     2
#define BATTLE_IDX   3
