import sys, os

PROJECT_ROOT = os.path.dirname(os.path.realpath(__file__))

HEADER_TEMPLATE = '''struct %(action_name)sAction : Action {
    bool update(float dt) override;
    string kind() override { return "%(action_name)sAction"; };
    void imgui_visualizer() override;
    void init(TableNode* table) override;
};
'''

IMPL_TEMPLATE = '''void %(action_name)sAction::init(TableNode* table) {

}

bool %(action_name)sAction::update(float dt) {
	return false;
}

void %(action_name)sAction::imgui_visualizer() {
	if (ImGui::TreeNode("%(action_name)sAction")) {
		ImGui::TreePop();
	}
}
'''

if __name__ == '__main__':
    args = sys.argv[1:]
    
    which = args[0]
    if which == 'action':
        name = args[1]
        lower_name = name.lower()
        class_name = name + "Action"
        
        header_path = os.path.join(PROJECT_ROOT, 'src', 'actions', lower_name + '.hpp')
        with open(header_path, 'w+') as header:
            header.write(HEADER_TEMPLATE % { 'action_name' : name })

        impl_path = os.path.join(PROJECT_ROOT, 'src', 'actions', lower_name + '_impl.hpp')
        with open(impl_path, 'w+') as impl:
            impl.write(IMPL_TEMPLATE % { 'action_name' : name })

        FACTORY_CANARY = '@NEXT@'
        factory_path = os.path.join(PROJECT_ROOT, 'src', 'task_impl.hpp')
        contents = []
        with open(factory_path, 'r') as factory:
            contents = factory.readlines()
            for index, line in enumerate(contents):
                if FACTORY_CANARY in line:
                    contents.insert(index, '\telse if (kind == "{}") {{\n'.format(class_name))
                    contents.insert(index + 1, "\t\taction = new {}\n".format(class_name))
                    contents.insert(index + 2, "\t}\n")
                    break

        with open(factory_path, 'w') as factory:
            factory.write(''.join(contents))
            
    print("Generated header, impl, and factory for {}".format(class_name))
        
        
            
