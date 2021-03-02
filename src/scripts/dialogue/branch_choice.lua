return {
  ["7d5bf4f0-aacc-499e-ac4b-f7599b39ba73"] = {
    children = {},
    is_entry_point = false,
    kind = "Choice",
    text = "hell no!",
    uuid = "7d5bf4f0-aacc-499e-ac4b-f7599b39ba73"
  },
  ["32d7541b-02b1-470b-9cb1-6570db2bc539"] = {
    branch_on = "demo:009:should_display_choice",
    children = {
      "7d5bf4f0-aacc-499e-ac4b-f7599b39ba73"
    },
    is_entry_point = false,
    kind = "Branch",
    uuid = "32d7541b-02b1-470b-9cb1-6570db2bc539"
  },
  ["57192446-48b7-4495-84a8-6ce8aad2cbf8"] = {
    children = {},
    is_entry_point = false,
    kind = "Choice",
    text = "why yes i will!",
    uuid = "57192446-48b7-4495-84a8-6ce8aad2cbf8"
  },
  ["f1e2138c-79e9-4704-8b5c-05e7d7a103a4"] = {
    children = {
      "57192446-48b7-4495-84a8-6ce8aad2cbf8",
      "32d7541b-02b1-470b-9cb1-6570db2bc539"
    },
    is_entry_point = true,
    kind = "Text",
    text = "choose?",
    uuid = "f1e2138c-79e9-4704-8b5c-05e7d7a103a4",
    who = ""
  }
}