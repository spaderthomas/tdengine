// Maps type names to the create method for that type
unordered_map<string, void*> create_methods;
template <typename Entity_Type>
void register_type() {
	create_methods[typeid(Entity_Type).name()] = &Entity_Type::create;
}

void setup_create_method_mapping() {
	register_type<Tree>();
}