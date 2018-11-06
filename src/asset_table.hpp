struct Asset { 
	string name; 

	virtual void stub() {};
};

struct {
	vector<Asset*> assets;

	template <typename Asset_Type>
	Asset_Type* get_asset(string name) {
		for (auto asset : assets) {
			Asset_Type* asset_as_type = dynamic_cast<Asset_Type*>(asset);
			if (asset_as_type) {
				if (asset_as_type->name == name) {
					return asset_as_type;
				}
			}
		}

		Asset_Type* new_asset = new Asset_Type;
		new_asset->name = name;
		assets.push_back(new_asset);
		return new_asset;
	}

	template <typename Asset_Type>
	vector<Asset_Type*> get_all() {
		vector<Asset_Type*> all;
		for (auto asset : assets) {
			Asset_Type* asset_as_type = dynamic_cast<Asset_Type*>(asset);
			if (asset_as_type) {
				all.push_back(asset_as_type);
			}
		}

		return all;
	}
} asset_table;
