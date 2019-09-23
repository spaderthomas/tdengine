// @spader, 3/18/19
// All of this was used for easy bindings to Lua when I was using that.
// That's not a thing anymore, but I still like the idea of having easy bindings
// that you can use to control stuff. Leaving this here until I figure out what
// to do with it 
tdapi void update_animation(EntityHandle me, float dt);
tdapi int collider_kind(EntityHandle me);
tdapi void draw_entity(EntityHandle me, Render_Flags flags);
tdapi void teleport_entity(EntityHandle me, float x, float y);
tdapi void move_entity(EntityHandle me, bool up, bool down, bool left, bool right);
tdapi void set_animation(EntityHandle me, string wish_name);
tdapi void set_animation_no_reset(EntityHandle me, string wish_name);
tdapi void set_interaction(EntityHandle hero, EntityHandle other);
tdapi bool are_interacting(EntityHandle initiator, EntityHandle receiver);
tdapi void draw_aabb(EntityHandle me);
tdapi void update_task(EntityHandle me, float dt);
tdapi bool are_entities_colliding(EntityHandle a, EntityHandle b);
tdapi void register_potential_collision(EntityHandle me, EntityHandle other);
tdapi void set_state(string var, bool val);
tdapi void camera_follow(EntityHandle entity);
