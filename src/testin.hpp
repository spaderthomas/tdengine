void test() {
	tdmap<const type_info*, Graphic_Component*> map;
	tdmap<const type_info*, pool_handle<any_component>> components;
	pool_handle<Entity> e = Entity::create("wilson");
	auto gc = e->get_component<Graphic_Component>();
	map.map(&typeid(Graphic_Component), gc);
	map.map(&typeid(Graphic_Component), gc);
	map.map(&typeid(Graphic_Component), gc);
	map.map(&typeid(Graphic_Component), gc);
	map.map(&typeid(Graphic_Component), gc);
	map.map(&typeid(Graphic_Component), gc);

	tdvec<int> vec;
	vec.push(0);
	vec.push(1);
	vec.push(2);
	vec.push(3);
	vec.erase(2);
}