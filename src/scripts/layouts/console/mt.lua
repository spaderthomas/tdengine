local box_a = tdengine.find_entity_by_id(3)
local box_b = tdengine.find_entity_by_id(4)

print(box_a.tdengine.persist)
print(box_b.tdengine.persist)

box_a.tdengine.persist = true

print(box_a.tdengine.persist)
print(box_b.tdengine.persist)
