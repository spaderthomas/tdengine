return {
  ["91c34def-6835-43f5-8580-70d43ca2f492"] = {
    children = {},
    is_entry_point = false,
    kind = "Text",
    text = "so, what's your favorite dead record? (evaluated to true)",
    uuid = "91c34def-6835-43f5-8580-70d43ca2f492",
    who = ""
  },
  ["587e28dc-f91c-4291-99b1-977db504c1db"] = {
    branch_on = "demo:001:heard_about_the_dead",
    children = {
      "91c34def-6835-43f5-8580-70d43ca2f492",
      "ca2f7395-2469-449f-98b7-daad4ad392ab"
    },
    is_entry_point = false,
    kind = "Branch",
    uuid = "587e28dc-f91c-4291-99b1-977db504c1db"
  },
  ["ca2f7395-2469-449f-98b7-daad4ad392ab"] = {
    children = {},
    is_entry_point = false,
    kind = "Text",
    text = "you should check out this band called the dead! (evaluated to false)",
    uuid = "ca2f7395-2469-449f-98b7-daad4ad392ab",
    who = ""
  },
  ["f9b7774d-80d9-4885-a100-b4b2443b2506"] = {
    children = {
      "587e28dc-f91c-4291-99b1-977db504c1db"
    },
    is_entry_point = true,
    kind = "Text",
    text = "sup",
    uuid = "f9b7774d-80d9-4885-a100-b4b2443b2506",
    who = ""
  }
}