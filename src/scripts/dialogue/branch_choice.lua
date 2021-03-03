return {
  ["2bb251aa-e505-4cad-aa8f-71a697d30dd8"] = {
    children = {},
    is_entry_point = false,
    kind = "Choice",
    text = "heard you sell drugs bro",
    uuid = "2bb251aa-e505-4cad-aa8f-71a697d30dd8"
  },
  ["2ff2c419-25b9-4e89-b41c-15f459fc1d03"] = {
    branch_on = "demo:009:should_display_choice",
    children = {
      "2bb251aa-e505-4cad-aa8f-71a697d30dd8"
    },
    is_entry_point = false,
    kind = "Branch",
    uuid = "2ff2c419-25b9-4e89-b41c-15f459fc1d03"
  },
  ["a11f80bb-a8d2-4b8b-a812-caf4ca099955"] = {
    children = {},
    is_entry_point = false,
    kind = "Choice",
    text = "not much man, good to see ya!",
    uuid = "a11f80bb-a8d2-4b8b-a812-caf4ca099955"
  },
  ["b7733ff7-9223-4a8d-9ccb-cb7fb3b525e7"] = {
    children = {
      "a11f80bb-a8d2-4b8b-a812-caf4ca099955",
      "2ff2c419-25b9-4e89-b41c-15f459fc1d03"
    },
    is_entry_point = false,
    kind = "Text",
    text = "hey, what's up?",
    uuid = "b7733ff7-9223-4a8d-9ccb-cb7fb3b525e7",
    who = ""
  }
}