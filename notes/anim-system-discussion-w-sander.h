#include <krink/flecs/flecs.h>

static ecs_world_t *world;

struct Position {
  float start; // start time of animation, t=0 is now
  float duration;
};

struct Animation {
  float start; // start time of animation, t=0 is now
  float duration;
};

void SysAnimate(ecs_iter_t *it) {}
void InitAnimate(ecs_iter_t *it);


void nop() {
ECS_COMPONENT(world, Position);
ECS_COMPONENT(world, Animation);

ECS_TAG(world, Animate);
ECS_TAG(world, AnimateTo);
ECS_TAG(world, AnimationFunction);
ECS_TAG(world, EaseIn);
ECS_TAG(world, EaseOut);
/*
[9:55 PM] Swan (Silent Tower Games): Oh yeah... C++ can have empty structs
[9:57 PM] tizilogic: do I understand correctly that you are adding the same tag multiple times to a single entity? nvm...
[9:57 PM] Sander: */
ecs_entity_t anim_1 = ecs_new_id(world);
ecs_set(world, anim_1, Animation, {0, 0.1});
ecs_add_pair(world, anim_1, AnimationFunction, EaseIn);
ecs_set_pair_object(world, anim_1, AnimateTo, Position, {10, 20});

ecs_entity_t anim_2 = ecs_new_id(world);
ecs_set(world, anim_2, Animation, {0.1, 0.1});
ecs_add_pair(world, anim_2, AnimationFunction, EaseOut);
ecs_set_pair_object(world, anim_2, AnimateTo, Position, {20, 30});

/* [9:58 PM] Sander: and then */

ecs_entity_t e = ecs_new_id(world);
ecs_set(world, e, Position, {0, 0});
ecs_add_pair(world, e, Animate, anim_1);
ecs_add_pair(world, e, Animate, anim_2);

/* [10:00 PM] Sander: Then you can create a system like this */

ECS_SYSTEM(world, SysAnimate, EcsOnUpdate, (Animate, *));

/*
[10:00 PM] Sander: and inside the system get the properties for the animation & run it
[10:01 PM] tizilogic: I guess I'd also need a trigger to compute the actual values of the added animations in a OnAdd, right?
[10:02 PM] Sander: yeah
[10:02 PM] Sander: that's true
[10:03 PM] Sander: I think I would do
*/

ECS_TRIGGER(world, InitAnimate, EcsOnAdd, (Animate, *));

}
/*
[10:03 PM] Sander: and then in the implementation store the current value* of the component to be animated
[10:04 PM] tizilogic: makes sense 👍. the use of entities as pairs was the "missing link" in my thinking!! thank you so much 🙏
[10:05 PM] Sander: something like
*/

void InitAnimate(ecs_iter_t *it) {
  // ecs_set_id(world, e, ecs_pair(AnimateFrom, comp_id), comp_value);
}
/*
[10:05 PM] Sander: yw! hope it helps 🙂
[10:06 PM] tizilogic: it sure untangled a knot I had in my head for a few days now
[10:49 PM] tizilogic: @Sander ecs_set_object? I can't find such a macro or function. is that just pseudo code or should there be a macro/function with that name?
[10:51 PM] Sander: oops, ecs_set_pair_object
[10:51 PM] Sander: the difference with ecs_set_pair is that the type you set is determined by the 2nd element of the pair, not the first
[10:51 PM] tizilogic: gotcha
[10:52 PM] tizilogic: thx for clearing that up 👍
[10:54 PM] Sander: this part of the manual describes in more detail how component types & relations work: https://github.com/SanderMertens/flecs/blob/master/docs/Relations.md#relation-components
*/